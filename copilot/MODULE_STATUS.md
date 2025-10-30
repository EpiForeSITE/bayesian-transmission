# Module Exposure Status

## What We've Done

### 1. Added Helper Methods to System.h ✓
- `countEpisodes()` - counts total episodes across all patients
- `countEvents()` - counts total events across all episodes  
- `getSystemCounts()` - returns a List with comprehensive counts

### 2. Exposed System Methods in Module-Infect.cpp ✓
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

### 3. Exposed Utility Classes in Module-utils.cpp ✓
- `util::IntMap` with `.property("size", ...)` and iteration methods
- `util::List` with `.property("size", ...)` and iteration methods
- `util::Map` with `.property("size", ...)` and iteration methods

### 4. Package Compiles Successfully ✓
```bash
R CMD INSTALL . 
# Result: * DONE (bayestransmission)
```

## Current Problem ❌

**The Rcpp Module is NOT being loaded when the package loads.**

When we try to use the classes:
```r
library(bayestransmission)
getClass('CppSystem')
# Error: "CppSystem" is not a defined class
```

Even trying to load manually doesn't work:
```r
library(Rcpp)
loadModule("BayesianInfectiousDiseaseModelingModule", TRUE)
getClass('CppSystem')
# Error: "CppSystem" is not a defined class
```

## Why `size()` Should Work

Looking at `/home/bt/src/util/Map.h`:
- Line 324: `inline int size() const { return use; }`
- This is already exposed in Module-utils.cpp line 36: `.property("size", &util::Map::size)`

So once the Module loads, the following should work:
```r
sys <- new(CppSystem, facilities, units, times, patients, types)
patients_map <- sys$getPatients()
n_patients <- patients_map$size()  # This will work!
```

## What Needs Investigation

1. **Why isn't the Module being loaded?**
   - R/Module.R has: `Rcpp::loadModule("BayesianInfectiousDiseaseModelingModule", TRUE)`
   - But this doesn't seem to be executing when the package loads

2. **Possible causes:**
   - Missing `.onLoad()` function?
   - NAMESPACE not importing the module classes?
   - Module init functions not being called?
   - Linking issue with the shared library?

3. **Test files updated but can't run:**
   - Updated test-system-construction.R to use method calls: `sys$getPatients()` instead of `sys$Patients`
   - All tests use `$size()` to check counts
   - But tests fail with parse error (appears to be unrelated caching issue)
   - Real issue: `CppSystem` class not found because Module not loaded

## Next Steps

1. Fix the Module loading issue so CppSystem and other classes are available
2. Once that's working, the tests should run
3. The `size()` method is already properly exposed and will work automatically
