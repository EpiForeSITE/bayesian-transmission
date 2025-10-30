# System Testing - Summary

## Status: ✅ System class methods are exposed and working

### What's Available

Using `devtools::load_all()`, the following now work:

#### System Creation
```r
sys <- new(CppSystem, facilities, units, times, patients, types)
```

#### Access Methods (return Map/IntMap/List objects)
```r
patients_map <- sys$getPatients()      # Returns Rcpp_CppIntMap
facilities_map <- sys$getFacilities()  # Returns Rcpp_CppIntMap  
units_list <- sys$getUnits()           # Returns Rcpp_CppList
episodes <- sys$getEpisodes(patient)   # Returns Rcpp_CppMap
```

#### Time Methods
```r
start <- sys$startTime()  # Returns double
end <- sys$endTime()      # Returns double
```

#### New Helper Methods
```r
n_episodes <- sys$countEpisodes()      # Returns int
n_events <- sys$countEvents()          # Returns int
counts <- sys$getSystemCounts()        # Returns Rcpp_CppList
```

#### Using size() on Collections
**This is the key point from your question about Map.h**

All collection types have `size()` exposed as a property:
```r
patients_map <- sys$getPatients()
n_patients <- patients_map$size()      # Works! Returns total number of patients

facilities_map <- sys$getFacilities()
n_facilities <- facilities_map$size()  # Works! Returns total number of facilities

units_list <- sys$getUnits()
n_units <- units_list$size()           # Works! Returns total number of units

counts <- sys$getSystemCounts()
n_counts <- counts$size()              # Works! Should return 5 (fac, unit, pat, ep, ev)
```

## Implementation Details

### In System.h (lines ~310-377)
- `countEpisodes()` - iterates through `pepis` map and sums episode counts
- `countEvents()` - iterates through episodes and sums event counts
- `getSystemCounts()` - creates a List with 5 Integer objects containing counts

### In Module-utils.cpp
All utility classes expose `size()`:
```cpp
class_<util::IntMap>("CppIntMap")
    .property("size", &util::IntMap::size)  // Line 20
    
class_<util::List>("CppList")
    .property("size", &util::List::size)    // Line 28
    
class_<util::Map>("CppMap")
    .property("size", &util::Map::size)     // Line 36
```

### In Module-Infect.cpp  
System methods are exposed:
```cpp
class_<System>("CppSystem")
    .method("getFacilities", &System::getFacilities)
    .method("getUnits", &System::getUnits)
    .method("getPatients", &System::getPatients)
    .method("getEpisodes", &System::getEpisodes)
    .method("startTime", &System::startTime)
    .method("endTime", &System::endTime)
    .method("countEpisodes", (int (infect::System::*)() const)&infect::System::countEpisodes)
    .method("countEvents", (int (infect::System::*)() const)&infect::System::countEvents)
    .method("getSystemCounts", (util::List* (infect::System::*)() const)&infect::System::getSystemCounts)
```

## Test File Updates

The test file `tests/testthat/test-system-construction.R` has been updated to:
- Use `sys$getPatients()` instead of `sys$Patients` (method call, not property)
- Use `sys$getFacilities()` instead of `sys$Facilities`
- Use `sys$getUnits()` instead of `sys$Units`
- Use `$size()` on all returned collection objects to get counts

## Important Note on Rcpp Classes

As you correctly pointed out:
- Generator object name: `CppSystem` (used with `new()`)
- Actual R class name: `Rcpp_CppSystem` 
- There is NO class named just `"CppSystem"` - that's the generator object, not a formal S4 class

This is standard Rcpp Module behavior.

## Next Steps

1. Verify all tests pass with `devtools::load_all()`
2. Check that `size()` returns correct counts for all collection types
3. Verify the helper methods return accurate counts
