# Double Deletion Bug Analysis

## Problem

When `sys$getFacilities()` is called:
1. Returns raw pointer `fac` (member of System)
2. R wraps it as `Rcpp_CppIntMap` with automatic finalizer
3. When `fac_map` is garbage collected → calls `delete fac`
4. When `sys` is destroyed → System destructor calls `delete fac` again
5. **SEGFAULT** from double deletion

## Evidence

```cpp
// System.h - returns internal pointer
inline IntMap *getFacilities() {
    if (fac == nullptr) {
        return new IntMap();  // ← NEW IntMap created (orphaned!)
    }
    fac->init();
    return fac;  // ← Returns member pointer (will be double-deleted!)
}

// System.cpp destructor
System::~System() {
    ...
    delete fac;  // ← Tries to delete again!
}
```

## The Problem is Worse Than It Appears

There are **TWO** issues:

### Issue 1: Double Deletion (your discovery)
- `getFacilities()` returns `fac` member pointer
- R takes ownership and deletes it
- System destructor also deletes it

### Issue 2: Memory Leak (additional bug)
- When `fac == nullptr`, creates `new IntMap()`
- Returns orphaned pointer with NO owner
- Neither R nor System tracks it
- Memory leak

## Affected Methods

All these methods have the same problem:
- `System::getFacilities()` - returns `fac`
- `System::getPatients()` - returns `pat`
- `System::getUnits()` - **creates new List** (always leaks!)
- `System::getEpisodes(Patient*)` - complicated case
- `System::getSystemCounts()` - **creates new List** (leaks!)

## Solution Options

### Option A: Mark as Non-Owning (Recommended)
```cpp
// In System.h - add non-owning getters
inline IntMap* getFacilitiesNonOwning() const {
    return fac;
}

// In Module-Infect.cpp
Rcpp::XPtr<IntMap> getFacilitiesSafe(System* sys) {
    IntMap* ptr = sys->getFacilitiesNonOwning();
    // Create XPtr with NO-OP finalizer (won't delete)
    return Rcpp::XPtr<IntMap>(ptr, false);  // false = non-deleting
}

// Then expose the wrapper:
.method("getFacilities", &getFacilitiesSafe)
```

### Option B: Return Copies (Safe but Expensive)
```cpp
inline IntMap* getFacilitiesCopy() const {
    if (fac == nullptr) return new IntMap();
    return fac->copy();  // Returns new owned copy
}
```

### Option C: Use Rcpp Internal References
```cpp
// Wrap in Rcpp::Reference that doesn't own memory
Rcpp::Reference getFacilities() {
    // Returns reference to existing object
    // Rcpp manages lifecycle properly
}
```

### Option D: Fix Ownership Model (Proper Fix)
```cpp
// System should use shared_ptr
class System {
    std::shared_ptr<IntMap> fac;
    std::shared_ptr<IntMap> pat;
    std::shared_ptr<Map> pepis;
    
    std::shared_ptr<IntMap> getFacilities() {
        return fac;  // Shared ownership, ref-counted
    }
};
```

## Immediate Fix (Option A Implementation)

This prevents double-deletion without changing ownership model:

```cpp
// Create wrapper functions in Module-Infect.cpp
static Rcpp::XPtr<util::IntMap> System_getFacilities_safe(infect::System* sys) {
    util::IntMap* ptr = sys->getFacilities();
    // Don't register finalizer - System owns this
    return Rcpp::XPtr<util::IntMap>(ptr, false);
}

static Rcpp::XPtr<util::IntMap> System_getPatients_safe(infect::System* sys) {
    util::IntMap* ptr = sys->getPatients();
    return Rcpp::XPtr<util::IntMap>(ptr, false);
}

// Then in init_Module_infect():
class_<infect::System>("CppSystem")
    ...
    .method("getFacilities", &System_getFacilities_safe)
    .method("getPatients", &System_getPatients_safe)
    ...
```

## Methods That CREATE New Objects (Need Different Fix)

These need to transfer ownership to R:
- `getUnits()` - creates `new List()`
- `getSystemCounts()` - creates `new List()`  
- `getEpisodes()` when null - creates `new Map()`

These SHOULD be deleted by R, so keep default behavior.

## Testing the Fix

```r
# Should NOT crash after fix:
sys <- new(CppSystem, ...)
fac_map <- sys$getFacilities()
rm(fac_map, sys)
gc()  # Should not segfault
```
