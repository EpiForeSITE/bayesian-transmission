# Quick Fix: Convert System to use shared_ptr

## The Simplest Solution

Instead of fighting with Rcpp's memory management, just use `std::shared_ptr` for the member variables. This is a ~10 line change:

```cpp
// System.h - BEFORE
class System {
private:
    IntMap *pat;
    IntMap *fac;
    Map *pepis;
    
public:
    inline IntMap* getFacilities() {
        fac->init();
        return fac;  // DANGER: R might delete this!
    }
    
    ~System() {
        delete fac;  // Double delete!
        delete pat;
        delete pepis;
    }
};
```

```cpp
// System.h - AFTER
#include <memory>

class System {
private:
    std::shared_ptr<IntMap> pat;
    std::shared_ptr<IntMap> fac;
    std::shared_ptr<Map> pepis;
    
public:
    inline std::shared_ptr<IntMap> getFacilities() {
        fac->init();
        return fac;  // SAFE: Reference count incremented
    }
    
    ~System() = default;  // No manual delete needed!
};
```

## Changes Required

### 1. System.h - Member declarations (3 lines)
```cpp
std::shared_ptr<IntMap> pat;
std::shared_ptr<IntMap> fac;
std::shared_ptr<Map> pepis;
```

### 2. System constructor - Allocation (3 lines changed)
```cpp
// OLD:
pat = new IntMap();
fac = new IntMap();
pepis = new Map();

// NEW:
pat = std::make_shared<IntMap>();
fac = std::make_shared<IntMap>();
pepis = std::make_shared<Map>();
```

### 3. System.h - Getters (3 lines changed)
```cpp
// OLD:
IntMap* getFacilities() { return fac; }
IntMap* getPatients() { return pat; }
Map* getEpisodes(Patient* p) { return pepis->get(p); }

// NEW:
std::shared_ptr<IntMap> getFacilities() { return fac; }
std::shared_ptr<IntMap> getPatients() { return pat; }
std::shared_ptr<Map> getEpisodes(Patient* p) { 
    return std::shared_ptr<Map>((Map*)pepis->get(p), [](Map*){});
}
```

### 4. System destructor - Remove manual deletes
```cpp
// OLD:
~System() {
    // 20 lines of manual delete calls
    delete fac;
    delete pat;
    delete pepis;
}

// NEW:
~System() = default;  // Smart pointers clean up automatically!
```

### 5. Module-Infect.cpp - NO CHANGES NEEDED!
Rcpp handles shared_ptr automatically:
```cpp
class_<System>("CppSystem")
    .method("getFacilities", &System::getFacilities)  // Just works!
    ;
```

## Why This Works

1. **Reference Counting**: `shared_ptr` tracks how many references exist
2. **R holds one**: When R wraps the return value, refcount = 2
3. **System holds one**: System keeps its reference, refcount stays >= 1
4. **R GC runs**: R's reference deleted, refcount = 1
5. **System destroyed**: Last reference gone, refcount = 0, object deleted ONCE

## Implementation Time

- **Change declarations**: 2 minutes
- **Update constructor**: 5 minutes
- **Fix getEpisodes**: 5 minutes  
- **Remove destructor code**: 2 minutes
- **Test**: 5 minutes
- **Total**: ~20 minutes

## The Alternative (What We're Trying Now)

Fighting with custom finalizers and XPtr is complicated because:
- Need to track which pointers are owned vs non-owned
- getEpisodes() sometimes creates new, sometimes returns member
- Easy to make mistakes
- Doesn't solve the root problem

## Recommendation

**Just use shared_ptr.** It's the modern C++ way, it's what smart pointers were designed for, and Rcpp supports it natively.

Would you like me to implement the shared_ptr solution? It's cleaner and safer than trying to work around the ownership issue.
