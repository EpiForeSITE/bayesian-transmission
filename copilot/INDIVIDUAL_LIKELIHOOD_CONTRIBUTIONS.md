# Individual Event Log Likelihood Contributions

## Summary

Modified Test 2 to compute and display the individual log likelihood contribution for each of the 14 events in the system history. This confirms that all events (including hierarchical start/stop markers) contribute to the total log likelihood.

## Implementation

### New C++ Wrapper Functions

Added to `src/Module-Infect.cpp`:

#### 1. `SystemHistory_getHistoryLinkList_wrapper()`
Returns all HistoryLink objects from the system history chain (via `sNext()` traversal).

```cpp
static SEXP SystemHistory_getHistoryLinkList_wrapper(infect::SystemHistory* hist) {
    // Traverses system history via sNext() and wraps each HistoryLink
    // in an Rcpp_CppHistoryLink reference class
    // Returns: List of wrapped HistoryLink objects
}
```

**Exposed as:** `CppSystemHistory$getHistoryLinkList()`

#### 2. `Model_logLikelihoodLink_wrapper()`
Computes the log likelihood contribution for a single HistoryLink.

```cpp
static double Model_logLikelihoodLink_wrapper(infect::Model* model, infect::HistoryLink* link) {
    // Casts model to UnitLinkedModel (required for HistoryLink-level likelihood)
    // Calls ulm->logLikelihood(link) which includes:
    //   - Gap probabilities (time between events)
    //   - Event-specific probabilities (insitu, admission, test results, etc.)
    // Returns: Log likelihood contribution for this link
}
```

**Exposed as:** `CppModel$logLikelihoodLink(link)`

### R Interface Usage

```r
# Create system and history
sys <- CppSystem$new(...)
hist <- CppSystemHistory$new(sys, model, FALSE)

# Get all history links
links <- hist$getHistoryLinkList()

# Compute likelihood for each link
individual_lls <- vapply(links, function(link) {
  model$logLikelihoodLink(link)
}, numeric(1))

# Sum equals total likelihood
total_ll <- model$logLikelihood(hist)
sum(individual_lls) == total_ll  # TRUE
```

## Test 2 Results

### Input Data
- 4 raw events: start (time 0), admission (time 10), discharge (time 20), stop (time 120)

### Output: 14 History Links

```
Index |      Type | Time | LogLikelihood
------+-----------+------+--------------
    1 |     start |    0 |     0.000000  (System level)
    2 |     start |    0 |     0.000000  (Facility level)
    3 |     start |    0 |     0.000000  (Unit level)
    4 |    insitu |    0 |    -0.105361  (Unit prevalence at start)
    5 |     start |    0 |     0.000000  (Patient level)
    6 | admission |   10 |   -21.791759  (Patient admission from community)
    7 | discharge |   20 |   -40.000000  (Patient discharge)
    8 | discharge |  120 |  -200.000000  (Unit closure discharge)
    9 | admission |    0 |     7.017828  (Unit implied admission)
   10 |      stop |  120 |     0.000000  (Patient level)
   11 | discharge |  120 |     0.000000  (Unit end discharge)
   12 |      stop |  120 |     0.000000  (Unit level)
   13 |      stop |  120 |     0.000000  (Facility level)
   14 |      stop |  120 |     0.000000  (System level)
------+-----------+------+--------------
Sum of individual contributions: -254.879292
Total logLikelihood(hist):       -254.879292
```

### Key Observations

1. **Start/stop events contribute 0** - They define observation windows but don't add probability mass
2. **Insitu event** (Link 4): Contributes prevalence probability at study start
3. **Patient admission** (Link 6): Out-of-unit colonization probability (negative → low prob of colonization)
4. **Patient discharge** (Link 7): Gap probability over 10 time units
5. **Unit implied admission** (Link 9): Positive contribution (gap probability benefit)
6. **Unit closure discharge** (Link 8): Large negative (gap over 100 time units)
7. **Sum equals total**: ✓ Confirms all events are included in likelihood calculation

## Likelihood Computation Breakdown

Each `logLikelihoodLink(link)` call computes:

### 1. Gap Probability (`logProbGap(prev, current)`)
Time-dependent probability for each parameter class:
- **InColParams**: No transmission/progression during gap
- **TestParams**: No tests during gap (or tests consistent with state)
- **AbxParams**: Antibiotic status transitions during gap

### 2. Event Probability (`logProb(link)`)
Event-specific probability based on type:

| Event Type | Parameter Class | Probability Computed |
|------------|----------------|---------------------|
| insitu     | InsituParams   | P(colonization status at start) |
| admission  | OutColParams   | P(acquire colonization out-of-hospital) |
| discharge  | (gap only)     | Gap probabilities |
| test       | TestParams     | P(test result \| infection status) |
| abx events | AbxParams      | P(antibiotic transitions) |
| start/stop | (none)         | 0 (markers only) |

### 3. Total Likelihood
```
logLikelihood(hist) = Σ logLikelihoodLink(link) for all links
```

## Answer to Original Question

> "All 14 would be computed when computing the likelihood correct?"

**Yes, absolutely.** The model computes:
1. **Gap probabilities** between consecutive events
2. **Event probabilities** for each observed event
3. **Hierarchical markers** (start/stop) contribute 0 but define boundaries

The sum of individual contributions exactly equals the total likelihood computed by `model$logLikelihood(hist)`, confirming that all 14 events are included in the calculation.

## Files Modified

1. **src/Module-Infect.cpp**:
   - Added `SystemHistory_getHistoryLinkList_wrapper()` (lines 142-169)
   - Added `Model_logLikelihoodLink_wrapper()` (lines 171-186)
   - Exposed `getHistoryLinkList()` on CppSystemHistory (line 376)
   - Exposed `logLikelihoodLink()` on CppModel (line 316)

2. **tests/testthat/test-event-likelihoods.R**:
   - Rewrote Test 2 to compute individual log likelihood contributions
   - Added assertions to verify sum equals total
   - Added checks for start/stop events contributing 0
   - Displays summary table showing all 14 contributions

## Usage in Future Tests

To inspect individual event contributions in any test:

```r
# Create system and history
hist <- CppSystemHistory$new(sys, model, FALSE)

# Get detailed breakdown
links <- hist$getHistoryLinkList()
event_summary <- data.frame(
  Type = vapply(links, function(l) l$Event$Type, character(1)),
  Time = vapply(links, function(l) l$Event$Time, numeric(1)),
  LogLikelihood = vapply(links, function(l) model$logLikelihoodLink(l), numeric(1))
)

print(event_summary)
```

This allows detailed inspection of which events contribute most to the likelihood, useful for debugging parameter estimation or understanding model behavior.
