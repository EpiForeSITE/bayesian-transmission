# ROOT CAUSE: -Inf Likelihood Bug

## Issue
The MCMC consistently produces `-Inf` log likelihoods, making the model unusable.

## Root Cause Analysis

### Discovery Process
1. **Original C++ produces finite likelihoods** (~-12000) with `P_false_positive=0.0`
2. **Current R wrapper produces -Inf** even with `P_false_positive=1e-10`
3. **Verbose output shows**: `Test.P(+|unc) = 0.0000000000` despite R setting it to `1e-10`

### The Bug
The C++ `TestParams` constructor (in `src/modeling/modeling_TestParams.cpp` lines 16-18) sets default values:

```cpp
set(0,0,0,0.5,2);  // uncolonized: value=0
set(1,0,0,0.5,2);  // latent: value=0  
set(2,0.8,1,0.5,2); // colonized: value=0.8
```

**When R parameters with `weight=0` (fixed) are passed, the `init` value is NOT being used to override these C++ defaults.**

Result: Even though R code sets:
```r
SurveillanceTestParams(uncolonized = Param(init = 1e-10, weight = 0))
```

The C++ code uses `0.0` instead of `1e-10`, causing:
- `log(0) = -Inf` when calculating likelihoods
- Complete failure of MCMC

## Why Original C++ Worked

The original C++ **also used P_false_positive=0.0** but avoided -Inf through initialization logic:

1. `Sampler::initializeEpisodes()` calls `hist->positives()` to find all patients with positive tests
2. `ConstrainedSimulator::initEpisodeHistory(eh, haspostest)` is called for each episode
3. **If `haspostest=true`**, it inserts an acquisition event at admission, ensuring patient is colonized
4. This prevents `P(positive|uncolonized)=0` from ever being evaluated (no uncolonized patients have positive tests)

However, this initialization may not be working correctly in the current implementation, OR there's a bug where:
- During MCMC sampling, a patient with a positive test gets proposed as uncolonized
- With P=0, this gives likelihood=0, log-likelihood=-Inf
- This incorrectly propagates to the final result

## The Fix

### Option 1: Ensure R `init` values are passed to C++ (RECOMMENDED)
Modify the parameter passing code to ensure that when `weight=0`, the `init` value from R still overrides the C++ default.

**Location**: Check how `SurveillanceTestParams` are converted and passed to C++, likely in:
- `R/newCppModel.R` or `R/constructors.R`
- C++ binding code in `src/Module-models.cpp` or `src/runMCMC.cpp`

### Option 2: Change C++ defaults
Modify `src/modeling/modeling_TestParams.cpp` line 16 to use a small non-zero default:
```cpp
set(0,1e-10,0,0.5,2);  // uncolonized: value=1e-10 instead of 0
```

### Option 3: Fix initialization logic  
Ensure `initEpisodeHistory()` correctly sets all patients with positive tests as colonized, preventing the likelihood from ever evaluating P(positive|uncolonized).

## Recommendation

**Use Option 1 + Option 3**:
1. Fix parameter passing so R `init` values are respected
2. Verify/fix initialization to ensure no false positives at start
3. Add validation that initial likelihood is finite before running MCMC

## Test Case
```r
surv <- SurveillanceTestParams(
  colonized = Param(init = 0.8, weight = 1),
  uncolonized = Param(init = 1e-10, weight = 0)  # Should use 1e-10, not 0!
)
```

Expected: C++ uses 1e-10  
Actual: C++ uses 0.0 (from default)  
Result: -Inf likelihood

## Files to Modify
1. Parameter passing: `R/newCppModel.R` or bindings in `src/Module-models.cpp`
2. Validation: `src/runMCMC.cpp` - add check that initial LL is finite
3. Documentation: Update vignette to explain why 1e-10 is used instead of 0.0
