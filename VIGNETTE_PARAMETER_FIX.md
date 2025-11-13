# Vignette Parameter Fix Summary

## Problem

The vignette was producing infinite log likelihoods due to several
parameter configuration issues.

## Root Causes

1.  **Inconsistent parameter initialization**: Some parameters were not
    explicitly initialized with `init` values
2.  **Wrong antibiotic onoff value**: Used `TRUE` (logical) instead of
    `0` or `1` (numeric)
3.  **Missing parameter field names**: Used `ever` instead of `ever_abx`
    in some places
4.  **Function name inconsistency**: Used `ClinicalTestParams` directly
    instead of being clear about `RandomTestParams`
5.  **Inappropriate default values**: Some rate parameters had values
    that could lead to numerical instability
6.  **Wrong dataset**: Used `simulated.data` instead of
    `simulated.data_sorted`

## Changes Made

### 1. Antibiotic Parameters (`Abx`)

**Before:**

``` r
abx <- AbxParams(onoff = TRUE, delay = 0.3, life = 1)
```

**After:**

``` r
abx <- AbxParams(onoff = 0, delay = 0.0, life = 2.0)
```

- Changed `onoff` from `TRUE` to `0` (numeric, antibiotics off for this
  example)
- Standardized delay to 0.0
- Changed life to 2.0 to match working examples

### 2. Antibiotic Rate Parameters (`AbxRate`)

**Before:**

``` r
abxrate <- AbxRateParams(
  uncolonized = 0.05,  
  colonized = 0.7      
)
```

**After:**

``` r
abxrate <- AbxRateParams(
  uncolonized = Param(init = 1.0, weight = 0),
  colonized = Param(init = 1.0, weight = 0)
)
```

- Made parameters explicit with
  [`Param()`](https://epiforesite.github.io/bayesian-transmission/reference/Param.md)
  calls
- Fixed both parameters (weight = 0) when antibiotics are off
- Set to reasonable default value of 1.0

### 3. Acquisition Parameters

**Before:**

``` r
acquisition <- LinearAbxAcquisitionParams(
    base = Param(0.01),
    time = Param(1, 0),
    mass = Param(1),
    freq = Param(1),
    col_abx = Param(1, 0),
    suss_abx = Param(1, 0),
    suss_ever = Param(1, 0)
)
```

**After:**

``` r
acquisition <- LinearAbxAcquisitionParams(
    base = Param(init = 0.001, weight = 1),
    time = Param(init = 1.0, weight = 0),
    mass = Param(init = 1.0, weight = 1),
    freq = Param(init = 1.0, weight = 1),
    col_abx = Param(init = 1.0, weight = 0),
    suss_abx = Param(init = 1.0, weight = 0),
    suss_ever = Param(init = 1.0, weight = 0)
)
```

- Made all `init` values explicit
- Changed base from 0.01 to 0.001 for better numerical stability

### 4. Progression Parameters

**Before:**

``` r
progression <- ProgressionParams(
    rate = Param(0.01),
    abx  = Param(1, 0),
    ever = Param(1, 0)
)
```

**After:**

``` r
progression <- ProgressionParams(
    rate = Param(init = 0.0, weight = 0),
    abx  = Param(init = 1.0, weight = 0),
    ever_abx = Param(init = 1.0, weight = 0)
)
```

- Changed `ever` to `ever_abx` (correct parameter name)
- Set progression rate to 0 and fixed it (appropriate for 2-state model)
- Made all `init` values explicit

### 5. Clearance Parameters

**Before:**

``` r
clearance <- ClearanceParams(
    rate = Param(0.01),
    abx  = Param(1, 0),
    ever = Param(1, 0)
)
```

**After:**

``` r
clearance <- ClearanceParams(
    rate = Param(init = 0.01, weight = 1),
    abx  = Param(init = 1.0, weight = 0),
    ever_abx = Param(init = 1.0, weight = 0)
)
```

- Changed `ever` to `ever_abx`
- Made all `init` values explicit

### 6. Out of Unit Infection Parameters

**Before:**

``` r
outcol <- OutOfUnitInfectionParams(
  acquisition = 0.1,
  clearance = 0.5
)
```

**After:**

``` r
outcol <- OutOfUnitInfectionParams(
  acquisition = Param(init = 0.001, weight = 1),
  clearance = Param(init = 0.01, weight = 0),
  progression = Param(init = 0.0, weight = 0)
)
```

- Made parameters explicit with
  [`Param()`](https://epiforesite.github.io/bayesian-transmission/reference/Param.md)
  calls
- Added missing `progression` parameter
- Changed acquisition rate from 0.1 to 0.001 for better stability
- Changed clearance rate from 0.5 to 0.01

### 7. Surveillance Test Parameters

**Before:**

``` r
surv <- SurveillanceTestParams(
    uncolonized = Param(0.01, 1),
    colonized = Param(0.9, 1),
    latent = Param(0, 0)
)
```

**After:**

``` r
surv <- SurveillanceTestParams(
    uncolonized = Param(init = 0.0, weight = 0),
    colonized = Param(init = 0.8, weight = 1),
    latent = Param(init = 0.0, weight = 0)
)
```

- Made all `init` values explicit
- Changed uncolonized from 0.01 (updating) to 0.0 (fixed) to avoid
  issues
- Changed colonized initial value from 0.9 to 0.8 to match working
  examples

### 8. Clinical Test Parameters

**Before:**

``` r
clin <- ClinicalTestParams(
    uncolonized = ParamWRate(Param(0.5, 0), rate = Param(1, 0)),
    colonized = ParamWRate(Param(0.5, 0), rate = Param(1, 0)),
    latent = ParamWRate(Param(0, 0), rate = Param(0, 0))
)
```

**After:**

``` r
clin <- RandomTestParams(
    uncolonized = ParamWRate(
      param = Param(init = 0.5, weight = 0), 
      rate = Param(init = 1.0, weight = 0)
    ),
    colonized = ParamWRate(
      param = Param(init = 0.5, weight = 0), 
      rate = Param(init = 1.0, weight = 0)
    ),
    latent = ParamWRate(
      param = Param(init = 0.5, weight = 0), 
      rate = Param(init = 1.0, weight = 0)
    )
)
```

- Changed from `ClinicalTestParams` to `RandomTestParams` for clarity
  (they’re aliases)
- Made all `init` values explicit
- Added `param =` and `rate =` argument names for clarity
- Changed latent from 0.0/0.0 to 0.5/1.0 for consistency

### 9. Dataset Change

**Before:**

``` r
results <- runMCMC(
  data = simulated.data,
  ...
)
```

**After:**

``` r
results <- runMCMC(
  data = simulated.data_sorted,
  ...
)
```

- Changed to use `simulated.data_sorted` which is pre-sorted for the
  algorithm

### 10. Documentation Updates

- Added note about always using explicit `init` values
- Updated comments to clarify parameter behavior
- Added note about antibiotic onoff being numeric (0 or 1)
- Clarified when parameters should be fixed vs. updated

## Verification

These parameters are based on the working test file
`copilot/test_vignette_params.R` which successfully produces finite log
likelihoods.

## Key Takeaways for Users

1.  Always use explicit `init` values in
    [`Param()`](https://epiforesite.github.io/bayesian-transmission/reference/Param.md)
    calls
2.  Antibiotic `onoff` should be numeric (0 or 1), not logical
3.  Use `ever_abx` not `ever` for parameter names
4.  Use `simulated.data_sorted` for examples
5.  Start with conservative (small) rate parameters to avoid numerical
    issues
6.  Fix unnecessary parameters (weight = 0) to reduce computational
    complexity
