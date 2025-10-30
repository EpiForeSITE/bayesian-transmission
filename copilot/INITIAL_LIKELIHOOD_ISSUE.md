# Issue: Initial Likelihood Discrepancy Between R Package and Original C++

## Summary
The R package produces `-Inf` initial log likelihood with the standard parameter set, while the original C++ implementation produces a finite value (~-12943).

## Expected Behavior (Original C++)
From `original_cpp_with_r_data.txt`:
```
Starting parameters.
...parameters...
LogLike=-12942.9
```

## Actual Behavior (Current R Package)
```r
library(bayestransmission)
params <- LinearAbxModel(nstates = 2, ...)  # Using exact C++ parameters
results <- runMCMC(data = simulated.data, ..., modelParameters = params)
# Initial LogLikelihood: -Inf
```

## Parameters Used (Matching Original C++)
- `nstates = 2`
- `Insitu`: probs=`[0.9, 0.0, 0.1]`
- `SurveillanceTest`: uncolonized=`0.0`, colonized=`0.8`
- `ClinicalTest` (RandomTest): All fixed at `0.5` prob, `1.0` rate
- `OutOfUnitInfection`: acquisition=`0.001`, clearance=`0.01`
- `InUnit.acquisition`: base=`0.001`, time=`1.0`, mass=`1.0`, freq=`1.0`, others=`1.0`
- `InUnit.clearance`: rate=`0.01`, abx=`1.0`, ever_abx=`1.0`
- `Abx`: onoff=`0`, delay=`0.0`, life=`2.0`
- `AbxRate`: All fixed at `1.0`

## Investigation Notes

1. **Clamping is working**: The logit clamping code correctly clamps `1.0` → `0.9999999999`

2. **Data sorting is correct**: Validation confirms `simulated.data` is properly sorted by patient then time

3. **Parameter output matches**: The printed parameter values match the C++ output (except we output extra test parameters when Clinical ≠ Surveillance)

4. **Tests pass**: The test suite passes because tests expect and handle `-Inf` initial likelihoods

5. **WAIC still calculates**: Despite `-Inf` likelihoods, WAIC values are finite and reasonable

## Possible Causes

1. **Episode initialization**: The way initial episodes are assigned may differ between implementations

2. **Likelihood calculation order**: Perhaps the C++ calculates likelihood after some initial episode sampling, while R calculates it before

3. **Hidden state initialization**: The initial hidden states (colonization status) may be set differently

4. **Numerical precision**: Though unlikely to cause -12943 → -Inf

## Impact

- **Low priority for users**: MCMC still runs, WAIC calculates, tests pass
- **Confusing for validation**: Makes it harder to verify R package matches C++ behavior
- **Documentation burden**: Requires explaining why initial likelihood is different

## Reproduction

```bash
cd /home/bt
Rscript test_minimal_likelihood.R
```

Expected output: Initial likelihood: -Inf  
C++ output was: -12942.9

## Related Files

- `/home/bt/original_cpp_with_r_data.txt` - Original C++ output
- `/home/bt/inst/original_cpp/simulated.Model` - Original C++ model parameters
- `/home/bt/test_minimal_likelihood.R` - Minimal reproduction script
- `/home/bt/src/runMCMC.cpp` - Main MCMC entry point
- `/home/bt/src/lognormal_LinearAbxModel.cpp` - Model likelihood calculation

## Recommendation

Investigate the C++ `LinearAbxModel::loglikelihood()` method and compare with how episodes are initialized in `runMCMC()`. The discrepancy likely occurs in how the initial patient states are set up before the first likelihood calculation.
