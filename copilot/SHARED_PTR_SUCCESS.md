# Shared Pointer Implementation - SUCCESS

## Problem Solved
Fixed the critical double-deletion segfault that occurred when R's garbage collector and System's destructor both tried to delete the same memory.

## Solution Implemented

### 1. Internal Memory Management (C++)
Converted System class to use `std::shared_ptr` for automatic memory management:

**Modified Files:**
- `src/infect/System.h`: Changed member variables from raw pointers to `std::shared_ptr<IntMap>` and `std::shared_ptr<Map>`
- `src/infect/infect_System.cpp`: Updated constructor to use `std::make_shared<>()`, modified getter methods to return shared_ptr, simplified destructor
- `src/infect/infect.h`: Added `#include <memory>` before namespace declaration
- `src/infect/infect_SystemHistory.cpp`: Updated call sites to use `.get()` on shared_ptr returns

### 2. R Interface Protection (Rcpp Modules)
Prevented R from deleting System-owned objects using the XPtr non-owning pattern:

**Modified Files:**
- `src/Module-Infect.cpp`: Created wrapper functions that explicitly wrap returned pointers with `XPtr<T>(p, false)` and the appropriate reference class

**Key Pattern:**
```cpp
static SEXP System_getFacilities_wrapper(infect::System* sys) {
    util::IntMap* p = sys->getFacilities().get();
    if (p == nullptr) {
        return R_NilValue;
    }
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    return methods_new("Rcpp_CppIntMap", 
                      Rcpp::Named(".object_pointer") = Rcpp::XPtr<util::IntMap>(p, false));
}
```

The critical detail is `XPtr<T>(p, false)` - the `false` parameter tells Rcpp NOT to add a delete finalizer when R garbage collects the object.

## Test Results

✓ System objects can be created  
✓ getFacilities() returns valid Rcpp_CppIntMap object  
✓ getPatients() returns valid Rcpp_CppIntMap object  
✓ Module methods work correctly on returned objects (size, hasNext, init, etc.)  
✓ Objects can be deleted and garbage collected without segfault  
✓ **NO DOUBLE-DELETION ERRORS**

### Test Output:
```
[1] "Created System object"
[1] "Got facilities map"
[1] "Rcpp_CppIntMap"
[1] "Got patients map"
[1] "Rcpp_CppIntMap"
[1] "Facilities size: 2"
[1] "Patients size: 2"
[1] "Facilities map has entries (hasNext = TRUE)"
[1] "Cleaning up..."
          used (Mb) gc trigger (Mb) max used (Mb)
Ncells  880618 47.1    1440942   77  1440942   77
Vcells 1558562 11.9    8388608   64  2621345   20
[1] "✓ TEST PASSED: No segfault! Module methods work correctly!"
```

## Technical Details

### Memory Ownership Model
- **System owns**: The IntMap/Map containers via std::shared_ptr (automatic cleanup)
- **Containers own**: Individual Facility/Patient/Episode objects (manual cleanup in destructor)
- **R gets**: Non-owning raw pointers wrapped in XPtr with false delete flag
- **Result**: Single ownership, no double-deletion, safe memory management

### Shared Pointer Benefits
1. **Automatic lifetime management**: shared_ptr uses reference counting
2. **Safe sharing**: Multiple pointers to same object, deleted only when refcount reaches 0
3. **Exception safety**: RAII pattern ensures cleanup even during exceptions
4. **Custom deleters**: getEpisodes() uses no-op deleter for objects owned by pepis member

### Why This Works
1. System creates shared_ptr in constructor → refcount = 1
2. Getter methods return shared_ptr → refcount temporarily increases
3. Wrapper extracts raw pointer with `.get()` → refcount unchanged
4. R wraps raw pointer with XPtr(p, false) → no delete finalizer added
5. When R object deleted → no action taken on the pointer
6. When System deleted → shared_ptr refcount goes to 0 → object automatically deleted

## Files Changed Summary

1. **src/infect/System.h** - Member variables now std::shared_ptr
2. **src/infect/infect_System.cpp** - Constructor uses make_shared, getters return shared_ptr
3. **src/infect/infect.h** - Added #include <memory>
4. **src/infect/infect_SystemHistory.cpp** - Call sites use .get() on shared_ptr
5. **src/Module-Infect.cpp** - Wrapper functions with XPtr(p, false) pattern

## Next Steps

The segfault is now fixed and the test suite can proceed. The System class now uses modern C++ memory management internally while maintaining safe interoperability with R through Rcpp Modules.

## Pattern for Future Use

When returning System-owned objects to R through Rcpp Modules:

```cpp
// Wrapper function
static SEXP Class_getOwnedObject_wrapper(Class* obj) {
    OwnedType* p = obj->getOwnedObject().get();  // Get raw pointer from shared_ptr
    if (p == nullptr) {
        return R_NilValue;
    }
    Rcpp::Function methods_new = Rcpp::Environment::namespace_env("methods")["new"];
    return methods_new("Rcpp_CppOwnedType",  // Reference class name
                      Rcpp::Named(".object_pointer") = Rcpp::XPtr<OwnedType>(p, false));
                      // false = DO NOT DELETE when R garbage collects
}
```

This pattern can be reused for any similar situation where C++ objects own sub-objects that need to be exposed to R without transfer of ownership.
