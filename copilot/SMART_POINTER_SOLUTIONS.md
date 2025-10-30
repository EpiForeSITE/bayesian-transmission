# Smart Pointer Solutions for Double-Delete Issue

## Problem Analysis

```cpp
// Current code (BROKEN)
class System {
    IntMap *fac;  // System OWNS this
    
    IntMap* getFacilities() { 
        return fac;  // Returns pointer to owned data
    }
    
    ~System() {
        delete fac;  // System deletes it
    }
};
```

When R wraps `getFacilities()` result:
1. R creates `Rcpp_CppIntMap` wrapper around `fac`
2. R's finalizer tries to `delete fac`
3. System destructor tries to `delete fac` 
4. **CRASH: Double deletion**

---

## Solution 1: std::shared_ptr (Best for New Code)

### Advantages
- Automatic reference counting
- Safe sharing between R and C++
- No manual memory management
- Rcpp has good support for shared_ptr

### Implementation

```cpp
// In System.h
#include <memory>

class System {
private:
    std::shared_ptr<IntMap> fac;
    std::shared_ptr<IntMap> pat;
    std::shared_ptr<Map> pepis;
    
public:
    // Constructor
    System() {
        fac = std::make_shared<IntMap>();
        pat = std::make_shared<IntMap>();
        pepis = std::make_shared<Map>();
    }
    
    // Safe getter - returns shared ownership
    std::shared_ptr<IntMap> getFacilities() {
        return fac;  // Reference count incremented
    }
    
    // Destructor is automatic - no delete needed!
    ~System() = default;
};
```

### Rcpp Integration

```cpp
// In Module-Infect.cpp
class_<System>("CppSystem")
    .method("getFacilities", &System::getFacilities)  // Works automatically!
    ;
```

Rcpp knows how to handle `shared_ptr` - when R GCs the object, it decrements refcount. When System is destroyed, it decrements refcount. Object deleted when refcount reaches zero.

### Effort Required
- **High**: Need to change all allocation sites
- ~49+ `new Map/IntMap/List` calls
- All member variables in multiple classes
- **Time: 2-3 days** for System class alone

---

## Solution 2: Non-Owning Wrappers (Minimal Change)

### Keep raw pointers, but return NON-OWNING wrappers

```cpp
// In Module-Infect.cpp - add before init function

// Custom wrapper that tells Rcpp "don't delete this"
Rcpp::XPtr<IntMap> getFacilitiesNonOwning(System* sys) {
    IntMap* fac = sys->getFacilities();
    
    // Create XPtr with no-op deleter
    auto deleter = [](IntMap*) { 
        // Do nothing - System owns it
    };
    
    return Rcpp::XPtr<IntMap>(fac, deleter);
}

void init_Module_infect() {
    class_<System>("CppSystem")
        .method("getFacilities", &getFacilitiesNonOwning)  // Use wrapper
        ;
}
```

### Advantages
- Minimal code change
- Works with existing raw pointers
- Solves double-delete immediately

### Disadvantages
- R can't call methods on XPtr directly
- Need to handle all container accessors
- Still error-prone manual memory management

### Effort Required
- **Low**: Just wrapper functions
- **Time: 2-3 hours**

---

## Solution 3: Return Copies (Simple but Memory Cost)

### Copy data instead of sharing pointers

```cpp
// In System.h
inline IntMap* getFacilitiesCopy() const {
    return fac->copy();  // Return new copy
}
```

### Advantages
- Simple to implement
- R owns the copy, safe to delete
- Original stays with System

### Disadvantages
- **Memory overhead**: Duplicates all data
- **Performance**: Copy operations expensive for large data
- Still need manual memory management

### Effort Required
- **Low**: Add `copy()` calls
- **Time: 1-2 hours**

---

## Solution 4: Intrusive Reference Counting (Like Python)

### Make util::Object itself reference-counted

```cpp
// In util/Object.h
class Object {
private:
    mutable std::atomic<int> refCount{0};
    
public:
    void addRef() const { ++refCount; }
    
    void release() const {
        if (--refCount == 0) {
            delete this;
        }
    }
};

// Smart pointer wrapper
template<typename T>
class RefPtr {
    T* ptr;
    
public:
    RefPtr(T* p) : ptr(p) { 
        if (ptr) ptr->addRef(); 
    }
    
    ~RefPtr() { 
        if (ptr) ptr->release(); 
    }
    
    // Copy constructor increments refcount
    RefPtr(const RefPtr& other) : ptr(other.ptr) {
        if (ptr) ptr->addRef();
    }
};
```

### Advantages
- Single ownership model throughout
- Can mix old and new code gradually
- Good for large refactoring

### Disadvantages
- **Very High Effort**: Changes Object base class
- Affects entire codebase
- Subtle bugs if done wrong

### Effort Required
- **Very High**: Core infrastructure change
- **Time: 1-2 weeks**

---

## Recommended Approach: Hybrid

### Phase 1: Immediate Fix (Today)
Use **Solution 2** (Non-owning wrappers) for exposed methods:

```cpp
// Wrapper that returns reference without ownership transfer
SEXP getFacilitiesSafe(System* sys) {
    IntMap* fac = sys->getFacilities();
    
    // Return as external pointer with "do not delete" marker
    Rcpp::Environment module("package:bayestransmission");
    Rcpp::Function createRef = module["createNonOwningRef"];
    return createRef(Rcpp::XPtr<IntMap>(fac, false));  // false = no finalizer
}
```

### Phase 2: Gradual Migration (Next Week)
Convert System class to use `std::shared_ptr`:

```cpp
class System {
    std::shared_ptr<IntMap> fac;
    std::shared_ptr<IntMap> pat;
    std::shared_ptr<Map> pepis;
};
```

### Phase 3: Full Modernization (Next Month)
- Replace util::Map with std::unordered_map<K, std::shared_ptr<V>>
- Replace util::List with std::vector<std::shared_ptr<Object>>
- Modern C++ throughout

---

## Rcpp Smart Pointer Support

Rcpp DOES support smart pointers out of the box:

```cpp
// This works automatically!
class_<System>("CppSystem")
    .method("getFacilities", 
        [](System* sys) -> std::shared_ptr<IntMap> {
            // Convert raw pointer to shared_ptr
            return std::shared_ptr<IntMap>(
                sys->getFacilities(), 
                [](IntMap*) { /* System still owns it */ }
            );
        })
    ;
```

---

## Decision Matrix

| Solution | Effort | Safety | Performance | Time |
|----------|--------|--------|-------------|------|
| shared_ptr (full) | High | Excellent | Good | 2-3 days |
| XPtr wrappers | Low | Good | Excellent | 2-3 hours |
| Return copies | Low | Good | Poor | 1-2 hours |
| Intrusive refcount | Very High | Excellent | Excellent | 1-2 weeks |

**Recommendation:** 
1. **Now**: Use XPtr wrappers (Solution 2)
2. **Next**: Migrate System to shared_ptr (Solution 1)  
3. **Future**: Full container modernization

---

## Example Implementation: XPtr Wrapper Approach

Let me implement Solution 2 right now since it's the quickest fix:

```cpp
// In Module-Infect.cpp, before init_Module_infect()

// Wrapper that returns non-owning pointer
IntMap* System_getFacilities_safe(System* sys) {
    return sys->getFacilities();
}

// Custom wrap function with no-delete finalizer
namespace Rcpp {
    template <> SEXP wrap(IntMap* const& ptr) {
        // Mark as non-owning by using aliasing constructor
        return XPtr<IntMap>(ptr, false);  // false = no delete
    }
}

void init_Module_infect() {
    class_<System>("CppSystem")
        .method("getFacilities", &System_getFacilities_safe)
        ;
}
```

This tells Rcpp: "Here's a pointer, but DON'T delete it when R GCs the wrapper."
