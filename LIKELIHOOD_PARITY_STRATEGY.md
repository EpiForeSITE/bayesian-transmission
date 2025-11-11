# Strategy: R-C++ Log Likelihood Parity

## Problem Statement
The R implementation produces `-Inf` initial log likelihood while the C++ implementation produces finite values (~-12943). This is a **BUG**, not acceptable behavior. Both implementations must produce identical log likelihoods for identical states.

## Root Cause Analysis

### Known Facts from Previous Investigation
1. **Original C++ initial LL**: ~-12942.9 (finite)
2. **R package initial LL**: -Inf (infinite)
3. **Both use same parameters** (verified in VIGNETTE_INVESTIGATION_FINAL.md)
4. **Both use same data** (simulated.data_sorted)
5. **WAIC calculations work** (produce finite values)
6. **Clamping works** (mass=1.0, freq=1.0 get clamped to 1.0-eps)

### Probable Causes
The `-Inf` likelihood can only come from:
1. **log(0)** - Taking log of zero probability
2. **log(negative)** - Taking log of negative value (undefined)
3. **Division by zero** leading to NaN
4. **Incorrect episode initialization** - Different initial hidden states
5. **Event likelihood calculation errors** - Wrong probability calculations
6. **Missing events** - Some events not being counted

## Investigation Strategy

### Phase 1: Identify WHERE -Inf is Generated (1-2 hours)

#### Step 1.1: Add Diagnostic Logging
Create test that prints individual likelihood components:
- Episode initialization contribution
- Each event type contribution (admission, discharge, test, etc.)
- Cumulative likelihood at each step

**Test File**: `tests/testthat/test-likelihood-parity-diagnostic.R`

#### Step 1.2: Compare Component-by-Component
For a MINIMAL example (1 patient, 1 admission, 1 test):
- Track R implementation likelihood calculation
- Track C++ implementation likelihood calculation
- Find first divergence point

#### Step 1.3: Check Episode Initialization
Verify initial hidden states are identical:
- R: Check `results$FinalState` or equivalent
- C++: Check original C++ episode initialization
- Compare initial colonization status assignments

**Test File**: `tests/testthat/test-episode-initialization-parity.R`

### Phase 2: Identify Specific Event Causing -Inf (2-3 hours)

#### Step 2.1: Test Individual Event Types
Create isolated tests for each event likelihood:
- Admission likelihood
- Discharge likelihood  
- Surveillance test positive
- Surveillance test negative
- Clinical test positive
- Clinical test negative
- Antibiotic start/stop
- Isolation start/stop

**Test File**: `tests/testthat/test-individual-event-likelihoods.R`

#### Step 2.2: Test Edge Cases
Check boundary conditions:
- P(test|uncolonized) = 0.0 → log(0) = -Inf
- Rate = 0 events
- Zero time intervals

#### Step 2.3: Identify the Culprit
Binary search through data events:
- Test with first N events
- Find minimum N that produces -Inf
- Identify the specific event

### Phase 3: Fix the Root Cause (2-4 hours)

#### Likely Fix Locations

##### Fix 1: Surveillance Test with P=0
**Problem**: `SurveillanceTest uncolonized = Param(init = 0.0)` 
- If patient is truly uncolonized and tests negative
- Likelihood should be `1 - P(+|uncolonized)` = `1 - 0.0` = `1.0` ✓
- But if patient is uncolonized and tests POSITIVE
- Likelihood is `P(+|uncolonized)` = `0.0` → `log(0)` = `-Inf` ✗

**Solution**: 
- Use minimum probability (e.g., `1e-10`) instead of exact `0.0`
- OR: Check if data has impossible events (positive test on uncolonized patient)
- OR: Handle log(0) specially as very negative but finite value

**Test**: 
```r
test_that("Surveillance test with P=0 doesn't cause -Inf", {
  # Test with uncolonized patient getting positive test
  # Should give very low likelihood, not -Inf
})
```

##### Fix 2: Episode Initialization
**Problem**: Initial hidden states may be impossible given data
- e.g., Patient marked uncolonized but has positive test
- Likelihood of impossible state = 0 → -Inf

**Solution**:
- Verify initial state assignment logic
- Ensure compatibility with first observed events
- May need to initialize from data rather than priors

##### Fix 3: Time Interval Calculations
**Problem**: Zero or negative time intervals
- Rate × time with time=0 or time<0
- Can produce undefined log likelihoods

**Solution**:
- Add epsilon to time intervals
- Validate time ordering

##### Fix 4: Probability Clamping
**Problem**: Probabilities outside [0,1] before clamping
- Values > 1 or < 0 produce NaN in log

**Solution**:
- Ensure clamping happens BEFORE log transform
- Check all acquisition/clearance/progression rate calculations

### Phase 4: Validation (1-2 hours)

#### Step 4.1: Direct Comparison Test
**Test File**: `tests/testthat/test-likelihood-exact-parity.R`

```r
test_that("R and C++ produce identical initial log likelihood", {
  params <- LinearAbxModel(nstates = 2)
  
  # R implementation
  results_r <- runMCMC(
    data = simulated.data_sorted,
    MCMCParameters = list(nburn = 0, nsims = 1, 
                         outputparam = TRUE, outputfinal = TRUE),
    modelParameters = params,
    verbose = FALSE
  )
  
  # Expected value from original C++ (from VIGNETTE_INVESTIGATION_FINAL.md)
  expected_ll <- -12942.9
  tolerance <- 0.1  # Allow tiny floating point differences
  
  expect_true(is.finite(results_r$LogLikelihood[1]))
  expect_equal(results_r$LogLikelihood[1], expected_ll, tolerance = tolerance)
})
```

#### Step 4.2: Regression Test Suite
Create tests that verify:
- No -Inf likelihoods with default parameters
- No -Inf likelihoods with various parameter combinations
- Likelihoods match C++ reference values

#### Step 4.3: Continuous Integration
Add to CI/CD:
```r
# In tests/testthat.R or similar
test_check("bayestransmission", filter = "likelihood-parity")
```

### Phase 5: Documentation (30 min)

#### Update Documentation
- Remove all mentions of "-Inf being expected"
- Document the fix
- Add note about likelihood parity requirement
- Update vignette to show finite likelihoods

## Implementation Plan

### Day 1: Diagnosis (4-6 hours)
1. ✅ Create `test-likelihood-parity-diagnostic.R`
2. ✅ Add logging to track likelihood components
3. ✅ Run minimal example
4. ✅ Identify exact source of -Inf

### Day 2: Fix (4-6 hours)
1. ✅ Implement fix based on diagnosis
2. ✅ Create unit tests for the fix
3. ✅ Verify fix doesn't break existing tests
4. ✅ Test edge cases

### Day 3: Validation (2-4 hours)
1. ✅ Create `test-likelihood-exact-parity.R`
2. ✅ Compare against C++ reference values
3. ✅ Run full test suite
4. ✅ Update documentation and vignette

## Success Criteria

### Must Have
- [ ] `test-likelihood-exact-parity.R` passes
- [ ] R initial LL matches C++ initial LL within tolerance
- [ ] No -Inf likelihoods with standard parameters
- [ ] All existing tests still pass
- [ ] Vignette runs without -Inf warnings

### Should Have
- [ ] Comprehensive test coverage for all event types
- [ ] Clear error messages for invalid configurations
- [ ] Documentation of likelihood calculation

### Nice to Have
- [ ] Performance benchmarks
- [ ] Visual comparison tools
- [ ] Automated C++ comparison in CI

## Files to Create

1. **tests/testthat/test-likelihood-parity-diagnostic.R** - Diagnostic logging
2. **tests/testthat/test-episode-initialization-parity.R** - Episode init checks
3. **tests/testthat/test-individual-event-likelihoods.R** - Per-event tests
4. **tests/testthat/test-likelihood-exact-parity.R** - Final validation
5. **R/likelihood_diagnostics.R** - Helper functions for debugging (optional)
6. **vignettes/likelihood-calculation.Rmd** - Technical documentation (optional)

## Risk Mitigation

### Risks
1. **Fix breaks existing behavior** → Comprehensive regression tests
2. **C++ reference values are wrong** → Validate C++ implementation separately
3. **Multiple sources of -Inf** → Systematic component-by-component testing
4. **Performance regression** → Benchmark before/after

### Rollback Plan
- Keep original code in git history
- Feature flag for new likelihood calculation
- Gradual rollout with testing

## Next Steps

**IMMEDIATE**: Start with Phase 1, Step 1.1 - Create diagnostic test to identify WHERE the -Inf is coming from.

Would you like me to begin implementing the diagnostic test?
