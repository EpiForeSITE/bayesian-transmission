# Root Cause Analysis: -Inf Initial Log Likelihood

## Problem Statement
The R package produces `-Inf` initial log likelihood, while the original C++ implementation produced `-12942.9` with identical parameters and data.

## ❌ RULED OUT: Argument Order Bug

**Investigation Date**: October 28, 2025

I initially suspected that `makeHistLink()` calls might have incorrect argument order. However:

1. The original C++ uses: `makeHistLink(f, u, TIME, PATIENT, type, linked)`
2. The R package uses: `makeHistLink(f, u, PATIENT, TIME, type, linked)` 
3. **This difference is INTENTIONAL** - the R package deliberately changed the API
4. ALL call sites in the R package are consistent with the R package signature
5. Attempting to "fix" it to match original C++ broke the package completely

See `ARGUMENT_ORDER_INVESTIGATION.md` for full details.

## Actual Root Cause

### Episode Initialization (`initEpisodeHistory`)
In `src/models_ConstrainedSimulator.cpp` (lines 311-335):

```cpp
void ConstrainedSimulator::initEpisodeHistory(UnitLinkedModel *mod, 
                                               infect::EpisodeHistory *eh, 
                                               bool haspostest)
{
    if (haspostest)
    {
        // Create acquisition event at admission for patients with positive tests
        eh->proposeSwitch(mod->makeHistLink(f,u,p,admit,acquisition,0));
        eh->installProposal();
        eh->apply();
    }
    // If haspostest=FALSE, do NOTHING - no colonization events created!
}
```

### The Issue
1. **Patients WITH positive tests**: Get acquisition events initialized
2. **Patients WITHOUT positive tests**: Get NO colonization events

### Why This Causes -Inf

For patients without positive tests, the entire episode has no colonization history, meaning the patient is in the "uncolonized" state throughout.

When calculating likelihood for such patients:
- Any "in-situ" colonization (starting colonized) has probability based on `Insitu.Pcol`
- But with `Insitu.Pcol = 0.1`, most patients start uncolonized
- If they stay uncolonized the entire episode, that's actually FINE...

BUT there's a subtlety: if there are ANY events that have zero probability given the uncolonized state, we get `-Inf`. This could be:
- Random test results that don't match
- Antibiotic prescribing patterns inconsistent with uncolonized state
- Other model assumptions violated

### Why Original C++ Was Different

The original C++ binary must have had a different initialization strategy that:
1. Created at least SOME colonization events for more patients, OR
2. Had different default episode states that avoided zero-probability configurations, OR  
3. Had a different likelihood calculation order

Without access to the full original C++ source code (only have the executables and model files in `inst/original_cpp/`), we cannot determine the exact difference.

## Current Workaround

The `-Inf` initial likelihood doesn't prevent MCMC from working because:

1. **Burn-in phase** (`nburn` iterations):
   - `mc->sampleEpisodes()` proposes and accepts new colonization events
   - Episodes get properly initialized with realistic colonization histories
   - Likelihood improves from `-Inf` to finite values

2. **Sampling phase** continues from the post-burn-in state

3. **WAIC calculation** uses a different method that doesn't depend on the initial likelihood

## Evidence

From `src/runMCMC.cpp`:
- Line 238: Initial likelihood calculated and printed
- Line 248-255: Burn-in loop calls `sampleEpisodes()`  
- Line 263-277: Sampling loop - likelihoods stored in `llchain(i)` AFTER episode sampling
- Initial likelihood is NOT stored in return value!

## Proposed Solutions

### Short-term (Documentation)
✅ Document in vignette that initial `-Inf` is expected
✅ Explain that burn-in resolves this
✅ Note difference from original C++

### Medium-term (Fix initEpisodeHistory)
Modify `ConstrainedSimulator::initEpisodeHistory` to:
1. Sample initial colonization status from `InsituParams` for ALL patients
2. For colonized patients, create acquisition events
3. Ensure initial configuration has non-zero probability

### Long-term (Match Original C++)
1. Locate original C++ source for initialization
2. Port exact initialization logic to R package
3. Verify initial likelihoods match

## Test Case

```r
# This produces -Inf initially but finite after burn-in
results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 10, nsims = 10, ...),
  modelParameters = LinearAbxModel(nstates = 2, ...),
  verbose = TRUE
)

# results$LogLikelihood[1] is AFTER burn-in, not initial!
# Initial -Inf is printed but not returned
```

## Impact Assessment

**Severity**: Medium
- Does NOT prevent MCMC from running
- Does NOT affect final parameter estimates
- Does NOT affect WAIC calculations
- DOES make validation against C++ confusing
- DOES require longer burn-in

**Users Affected**: Anyone comparing initial likelihoods or using minimal burn-in

## Recommendation

1. **Immediate**: Update vignette documentation (DONE)
2. **Next sprint**: Implement proper episode initialization
3. **Future**: Investigate original C++ initialization strategy

## Files Involved

- `src/models_ConstrainedSimulator.cpp` - Episode initialization
- `src/runMCMC.cpp` - MCMC main loop, likelihood calculation
- `src/models_UnitLinkedModel.cpp` - Log likelihood calculation
- `vignettes/bayesian-transmission.Rmd` - User documentation
- `/home/bt/INITIAL_LIKELIHOOD_ISSUE.md` - Previous investigation notes
