# NaN/Inf Debugging Summary

## Issues Found and Fixed

### 1. ✅ InsituParams Defaults for 2-State Models
**Problem**: `InsituParams()` was using `[0.5, 0.5, 0.0]` (3-state defaults) for all models, including 2-state models which should use `[0.9, 0.0, 0.1]` (uncolonized, latent=0, colonized).

**Fix**: Modified `InsituParams()` and `LogNormalModelParams()` in `/home/bt/R/constructors.R`:
- Added `nstates` parameter to `InsituParams()`
- Set defaults: 2-state = `[0.9, 0.0, 0.1]`, 3-state = `[0.98, 0.01, 0.01]`  (matching original C++ defaults)
- `LogNormalModelParams()` now passes `nstates` to create appropriate defaults

**Impact**: Insitu parameters now show correct values: `0.9000000000    0.1000000000` instead of `0.5000000000    0.0000000000`

### 2. ✅ LinearAbxAcquisitionParams Probability Defaults
**Problem**: `mass` and `freq` parameters had defaults of `Param(1)`, causing:
- `logit(1) = inf` transformation error
- `digamma(0) = NaN` in prior calculations when `prival=1, priorn=1` gives `b = (1-1)*1 = 0`

**Fix**: Changed defaults in `LinearAbxAcquisitionParams()`:
```r
mass = Param(0, 0)     # Fixed at 0 (no mass action)
freq = Param(0.005)    # Small probability value
```

**Impact**: Mass now shows `0.0000000001` (clamped epsilon) and freq shows `0.0050000000` instead of `-nan`

### 3. ✅ Logit Transform Safety
**Problem**: `setWithLogitTransform()` didn't handle edge cases where value or prival could be exactly 0 or 1, causing:
- `logit(0)` or `logit(1)` = ±inf
- `digamma(0)` or `trigamma(0)` = NaN

**Fix**: Added clamping in `/home/bt/src/lognormal_LogNormalICP.cpp`:
```cpp
const double eps = 1e-10;
double value_safe = value;
if (value_safe <= eps) value_safe = eps;
if (value_safe >= 1.0 - eps) value_safe = 1.0 - eps;

double prival_safe = prival;
if (prival_safe <= eps) prival_safe = eps;
if (prival_safe >= 1.0 - eps) prival_safe = 1.0 - eps;
```

**Impact**: Prevents NaN from logit/digamma/trigamma calculations

### 4. ✅ InsituParams Log Probability Safety  
**Problem**: `InsituParams::set()` computed `logprobs[i] = log(probs[i])` without protection, causing `log(0) = -inf` for latent state in 2-state models.

**Fix**: Added epsilon in `/home/bt/src/models_InsituParams.cpp`:
```cpp
const double eps = 1e-300;
for (int i=0; i<3; i++)
    logprobs[i] = log(probs[i] + eps);
```

**Impact**: Prevents -inf from log(0), though very large negative number (~-690)

## Remaining Issue

### ⚠️ Initial Log Likelihood = -inf
**Status**: Not yet resolved

**Symptoms**:
- Initial parameter output shows `LogLike=-inf`
- When MCMC starts sampling, get "oldpropprob is nan" error
- Debug shows nalloc=9920 time points for Markov process

**Possible Causes**:
1. Some rate matrix Q contains invalid values (checked - seems OK)
2. Matrix exponential calculation with 9920 points causes numerical instability
3. Some other probability/likelihood component returns 0, causing log(0) = -inf
4. Equilibrium probabilities or initial state probabilities have issues

**Next Steps to Debug**:
1. Add debug output to `model->logLikelihood(hist)` to identify which component returns -inf
2. Check equilibriumProbs() calculation in OutColParams
3. Check if acquisition rate calculations return 0 or negative values
4. Compare with working original C++ execution to see if 9920 links is normal

## Test Results

Component tests (`test-loglikelihood-components.R`): ✅ All pass
- InsituParams now correct for 2-state models
- Acquisition parameters in valid ranges
- No NaN in parameter initialization

Full MCMC test (`test-runMCMC.R`): ❌ Still fails
- Parameters initialize without NaN
- But LogLike=-inf before sampling
- Fails during first sampling iteration with "oldpropprob is nan"

## Files Modified

1. `/home/bt/R/constructors.R`
   - `InsituParams()` - Added nstates parameter and appropriate defaults
   - `LogNormalModelParams()` - Pass nstates to InsituParams
   - `LinearAbxAcquisitionParams()` - Changed mass/freq defaults

2. `/home/bt/src/lognormal_LogNormalICP.cpp`
   - `setWithLogitTransform()` - Added value and prival clamping

3. `/home/bt/src/models_InsituParams.cpp`
   - `InsituParams::set()` - Added epsilon to prevent log(0)

4. `/home/bt/src/models_ConstrainedSimulator.cpp`
   - Added debug output for NaN detection

5. `/home/bt/tests/testthat/test-loglikelihood-components.R` (NEW)
   - Component tests for debugging

6. `/home/bt/src/runMCMC.cpp`
   - Added warning for inf/nan initial log likelihood
