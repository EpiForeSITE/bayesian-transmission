# LinearAbxModel2 Bug Fix Summary

## Issue Discovered
While enhancing test coverage for parameter initialization, we discovered that `LinearAbxModel2` was returning unexpected values for InCol parameters at positions 3 and 4 (mass/density and frequency parameters).

**Observed Behavior:**
- Input: `mass = 0.9123`, `freq = 0.9234`
- LinearAbxModel returned: `0.9123`, `0.9234` ✓ (correct)
- LinearAbxModel2 returned: `0.4771`, `0.4801` ✗ (incorrect)

## Root Cause Analysis

By comparing with the **original C++ implementation** in `inst/original_cpp/`, we discovered that:

### Original C++ Implementation (Gold Standard)

**LinearAbxModel** uses **LinearAbxICP**:
- Header labels: `"LABX.mass.mx"` and `"LABX.freq.mx"`
- Parameter transformation: 
  - Positions 2,3: **logit transform** (`setWithLogitTransform`)
  - Other positions: log transform
- Untransform: `logistic(par[i][j])` for positions 2,3
- Acquisition rate formula: Multiplicative

**LinearAbxModel2** uses **LinearAbxICP2**:
- Header labels: `"LABX.dens"` and `"LABX.freq"` 
- Parameter transformation: 
  - **ALL positions**: **log transform** (`setWithLogTransform`)
- Untransform: `exp(par[i][j])` for all positions
- Acquisition rate formula: Additive (`y = base + c*dens + c*freq/tot`)

### R Package Bug

The R package implementation had:
```cpp
// src/lognormal/lognormal_LinearAbxModel2.cpp (BEFORE FIX)
LinearAbxModel2::LinearAbxModel2(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
{
    InColParams *icp = getInColParams();
    delete icp;
    icp = new LinearAbxICP(nst,nmetro);  // ❌ WRONG - should be LinearAbxICP2
    setInColParams(icp);
}
```

This caused LinearAbxModel2 to incorrectly use LinearAbxICP, which applied logit transforms to positions 2 and 3. The "incorrect" values (0.477, 0.480) were actually `logistic(log(0.9123))` and `logistic(log(0.9234))`.

## Fix Applied

### 1. Fixed LinearAbxModel2 Constructor
**File:** `src/lognormal/lognormal_LinearAbxModel2.cpp`
```cpp
LinearAbxModel2::LinearAbxModel2(int nst, int nmetro, int fw, int ch) : LogNormalModel(nst,nmetro,fw,ch)
{
    // LinearAbxModel2 uses LinearAbxICP2 which has different transformation:
    // - Uses log transform for ALL parameters (not logit for mass/freq)
    // - Different acqRate formula (additive not multiplicative)
    InColParams *icp = getInColParams();
    delete icp;
    icp = new LinearAbxICP2(nst,nmetro);  // ✓ FIXED
    setInColParams(icp);
}
```

### 2. Added LinearAbxICP2 Include
**File:** `src/lognormal/LinearAbxModel2.h`
```cpp
#include "LogNormalModel.h"
#include "LinearAbxICP2.h"  // ✓ Added
```

### 3. Implemented LinearAbxICP2
**File:** `src/lognormal/lognormal_LinearAbxICP2.cpp` (newly created)

Implemented all required methods matching the original C++ implementation:
- Constructor
- `header()` - returns "LABX.dens" and "LABX.freq" 
- `acqRate()` - additive formula
- `set()` - uses log transform for ALL parameters
- `unTransform()` - returns `exp(par[i][j])` for all parameters
- All likelihood methods (logProgressionRate, logClearanceRate, logAcquisitionRate, etc.)

## Test Results

### Before Fix
```
[ FAIL 2 | WARN 0 | SKIP 1 | PASS 76 ]

Failure: InCol position 3 (mass)
  Expected: 0.9123
  Actual: 0.4771

Failure: InCol position 4 (freq)
  Expected: 0.9234
  Actual: 0.4801
```

### After Fix
```
[ FAIL 0 | WARN 0 | SKIP 1 | PASS 78 ]
```

All parameter initialization tests now pass! ✓

## Impact

This fix ensures that `LinearAbxModel2` correctly replicates the behavior of the original C++ implementation. The model now:

1. ✓ Uses log transform for all InCol acquisition parameters (not logit for positions 2,3)
2. ✓ Returns input parameter values correctly via `newModelExport()`
3. ✓ Uses the correct additive acquisition rate formula
4. ✓ Matches the original C++ implementation behavior

## Files Modified

1. `src/lognormal/lognormal_LinearAbxModel2.cpp` - Fixed constructor
2. `src/lognormal/LinearAbxModel2.h` - Added include
3. `src/lognormal/lognormal_LinearAbxICP2.cpp` - Created implementation (NEW)

## Key Lesson

**Always verify against the original C++ implementation (inst/original_cpp/) when discovering unexpected behavior.** The R package implementation should exactly replicate the original C++ behavior, not introduce "improvements" or variations.
