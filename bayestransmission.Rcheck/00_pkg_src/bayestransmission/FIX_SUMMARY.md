# Fix Summary: NaN Issues in test-runMCMC.R

## Problem
The `test-runMCMC.R` test was failing with NaN values in MCMC parameters and -inf log likelihood values.

## Root Causes Identified and Fixed

### 1. Data Sorting Issue (PRIMARY ROOT CAUSE)
**Problem:** The `simulated.data` was sorted by time, not by patient ID. The C++ `RawEventList` class expects events to be sorted by patient first (then time), as indicated by the `RawEvent::compare()` method in `src/infect_RawEvent.cpp`.

When events were unsorted by patient:
- The `makeAllEpisodes` function would call `makePatientEpisodes` multiple times for the same patient
- Events for a single hospital stay would be split across multiple episodes
- This created synthetic admission/discharge events
- Possurvtest events would be missing from the history
- Missing events led to -inf likelihoods

**Fix:** Sorted `simulated.data` by patient ID, then time:
```r
simulated.data <- simulated.data[order(simulated.data$patient, simulated.data$time), ]
```
Saved the sorted data to `data/simulated.data.rda`.

**Evidence:**
- Original C++ code uses `RawEventList` which extends `SortedList`
- `RawEvent::compare()` compares by patient first: `if (x->pat < pat) return 1;`
- When data is time-sorted, patient 120's admission at row 114 is separated from their possurvtest at row 116 by patient 117's events
- This breaks the episode grouping algorithm

### 2. InsituParams Default Values
**Problem:** `InsituParams()` constructor was setting defaults as `[1/nstates, ..., 1/nstates]` for all nstates values. For a 2-state model, this gave `[0.5, 0.5]` but should be `[0.9, 0, 0.1]` (susceptible, latent, colonized).

**Fix:** Updated `R/constructors.R`:
```r
InsituParams <- function(...) {
  if (nstates == 2) {
    probs <- c(0.9, 0, 0.1)  # [susceptible, latent, colonized]
  } else {
    probs <- rep(1/nstates, nstates)
  }
  ...
}
```

### 3. LinearAbxAcquisitionParams Initialization
**Problem:** Mass and frequency parameters were initialized to 0.5, which after logit transform gave `logit(0.5) = 0`. Also, `base` was 0, which should be > 0.

**Fix:** Updated to match original C++ model file values:
```r
mass = Param(init = 1.0, doit = 1),   # logit(1.0-eps) ≈ 11.5
freq = Param(init = 1.0, doit = 1),   # logit(1.0-eps) ≈ 11.5  
base = Param(init = 0.001)             # log(0.001) ≈ -6.91
```

### 4. Safety Clamping in setWithLogitTransform
**Problem:** When values are exactly 0 or 1, `logit(0) = -inf` and `logit(1) = +inf`.

**Fix:** Added epsilon clamping in `src/lognormal_LogNormalICP.cpp`:
```cpp
void LogNormalICP::setWithLogitTransform() {
    const double eps = 1e-10;
    double m_clamped = std::max(eps, std::min(1.0 - eps, mass->val));
    double f_clamped = std::max(eps, std::min(1.0 - eps, freq->val));
    ...
}
```

### 5. InsituParams log(0) Prevention
**Problem:** For 2-state models, the latent state probability is 0, which causes `log(0) = -inf` in the `logprobs` array.

**Fix:** Clamp probabilities to minimum epsilon value in `src/models_InsituParams.cpp`:
```cpp
void InsituParams::set(double u, double l, double c) {
    double tot = (u+l+c);
    probs[0] = u/tot;
    probs[1] = l/tot;  // For 2-state: l=0
    probs[2] = c/tot;
    const double eps = 1e-300;
    for (int i=0; i<3; i++)
        logprobs[i] = log(std::max(probs[i], eps));  // log(1e-300) ≈ -690.8, not -inf
}
```

This ensures `logprobs[1] = log(1e-300) ≈ -690.8` instead of `-inf` for 2-state models.

## Verification

### Before Fix
- Initial LogLike: -inf
- Possurvtest events missing from history
- Episodes incorrectly constructed (split across multiple episodes)
- makePatientEpisodes called multiple times per patient

### After Fix
- Initial LogLike: -12942.9 (matches original C++ code)
- All possurvtest events present in history  
- Episodes correctly constructed (one episode per hospital stay)
- makePatientEpisodes called once per patient
- **test-runMCMC.R: 23 PASSED, 0 FAILED**

## Files Modified

1. `/home/bt/data/simulated.data.rda` - Sorted by patient then time
2. `/home/bt/R/constructors.R` - Fixed InsituParams and LinearAbxAcquisitionParams defaults
3. `/home/bt/src/lognormal_LogNormalICP.cpp` - Added safety clamping for logit transform
4. `/home/bt/src/models_InsituParams.cpp` - Clamp probabilities to avoid log(0)
5. `/home/bt/src/runMCMC.cpp` - Added data sorting validation with clear error messages

## Technical Details

### RawEvent Comparison Function
From `src/infect_RawEvent.cpp`:
```cpp
int RawEvent::compare(Object const *const e) const {
    RawEvent const *const x = dynamic_cast<RawEvent const *const>(e);
    if (x->pat < pat) return 1;      // Patient ID is PRIMARY sort key
    if (pat < x->pat) return -1;
    if (x->time < time) return 1;    // Time is SECONDARY sort key
    if (time < x->time) return -1;
    ...
}
```

This comparator is used by `SortedList` (parent class of `RawEventList`) to maintain events in patient-first order.

### Episode Construction Algorithm
`makeAllEpisodes` in `src/infect_System.cpp`:
```cpp
void System::makeAllEpisodes(RawEventList *l, stringstream &err) {
    ...
    for (l->init(); l->hasNext(); ) {
        RawEvent *e = (RawEvent *)l->next();
        if (prev == 0 || prev->getPatientId() != e->getPatientId()) {
            // New patient detected - process previous patient's events
            makePatientEpisodes(p, s, pers, units, facilities);
            ...
        }
    }
}
```

This algorithm assumes events are grouped by patient ID. When events are sorted by time only, it incorrectly detects "new patients" mid-stay, causing episode fragmentation.

## Conclusion

The primary issue was **data ordering**. The R package was passing time-sorted data to C++ code that expected patient-sorted data. This cascaded through the episode construction, history building, and likelihood calculation, ultimately producing NaN values in MCMC.

Sorting the data by patient ID (then time) fixes the entire chain of issues and makes the R implementation match the original C++ behavior.
