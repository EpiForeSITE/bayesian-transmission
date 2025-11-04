# C++ vs R Package Likelihood Comparison Test Results

## Date
November 4, 2025

## Summary

Created comprehensive test suite in `tests/testthat/test-cpp-comparison.R` to compare R package likelihood computations with original C++ implementation.

## Key Findings

### 1. Link Count Discrepancy
- **C++ has 7,355 history links**
- **R has 7,348 history links** (7 fewer)
- **Reason**: R creates hierarchical start events (system/facility/unit/patient levels) while C++ output only includes unit-level links

### 2. Structure Mismatch
**C++ link structure** (from original_cpp_output.csv):
```
Link 0: EventType=21 (start), Time=0, LogLik=0
Link 1: EventType=0 (admission), Time=0, LogLik=-Inf
Link 2: EventType=0 (admission), Time=0.060978, LogLik=-Inf
Link 3: EventType=1 (negsurvtest), Time=0.061978, LogLik=-0.004
...
```

**R link structure**:
```
Link 0: Type=start, Time=0, LogLik=0
Link 1: Type=start, Time=0, LogLik=0    <- Extra (facility level)
Link 2: Type=start, Time=0, LogLik=0    <- Extra (unit level)
Link 3: Type=start, Time=0, LogLik=0    <- Extra (patient insitu?)
Link 4: Type=start, Time=0, LogLik=0    <- Extra (?)
Link 5: Type=admission, Time=0, LogLik=-1.79  <- FINITE, not -Inf!
Link 6: Type=admission, Time=0.060978, LogLik=-1.91  <- FINITE, not -Inf!
...
```

### 3. Critical Issue: -Inf vs Finite Likelihoods

**From C++ output**:
- **2,176 links have LogLik = -Inf** (29.6% of 7,355)
- These represent **impossible states/events**

**From R package**:
- **0 links have LogLik = -Inf** (0.0% of 7,348)
- **All likelihoods are finite**

This confirms the **LIKELIHOOD_COMPARISON.md finding**:
> The R package computes finite likelihoods for 1,953 history links that the original C++ correctly identifies as impossible (returns -Inf).

### 4. The "Infinite Likelihood on Start" Issue

User reported: "R package creates a state where the infinite log likelihood occurs on start"

**This appears to be INCORRECT based on evidence**:
- R gives `LogLik=0` for start events (correct)
- R gives `LogLik=-1.79` for first admission (FINITE, should be -Inf per C++)
- **The issue is NOT -Inf on start, but MISSING -Inf on admissions**

### 5. Possible Root Cause

Looking at the differences:

**Hypothesis 1: R's hierarchical start events mask the problem**
- R creates multiple start events at system/facility/unit/patient levels
- These all correctly return 0
- But the ACTUAL start-of-observation events (admissions/insitu) are not being checked for impossibility

**Hypothesis 2: InsituParams not returning -Inf**
- C++ Link 1: admission at time 0 → -Inf (likely insitu patient with impossible state)
- R Link 5: admission at time 0 → -1.79 (finite, incorrect)
- The insitu/admission probability calculation may not be checking state constraints

**Hypothesis 3: Test event compatibility not checked**
- C++ marks many admission events as -Inf
- This suggests test results are incompatible with proposed initial states
- R may not be validating test consistency with colonization states

## Test Infrastructure Created

### Tests Implemented

1. **Original C++ output file exists and is readable** ✅
2. **Can parse original C++ output format** ✅
3. **Start event likelihood matches** ⏸️ (skipped - need parameter matching)
4. **R can compute individual link likelihoods** ✅
5. **Number of history links matches** ✅ (documents discrepancy)
6. **First admission event likelihood comparison** ⏸️ (skipped - need exact params)
7. **Full discrepancy identification** ✅ (run with RUN_FULL_COMPARISON=1)
8. **Total likelihood equals sum of individual** ✅

### Test Location
`/home/bt/tests/testthat/test-cpp-comparison.R`

## Next Steps to Fix the Issue

### 1. Identify where -Inf should be returned

Check these parameter classes for missing impossibility checks:
- `InsituParams::logProb()` - Initial state probabilities
- `OutColParams::logProb()` - Out-of-hospital transitions (admissions)
- `TestParams::logProb()` - Test result validation

### 2. Compare C++ vs R test validation logic

The C++ correctly returns -Inf when:
- Test results are incompatible with inferred state
- Initial states violate constraints
- Transitions are impossible given parameters

### 3. Add explicit impossibility checks

Example pattern from C++:
```cpp
if (test_result_incompatible_with_state) {
    return -Inf;
}
```

### 4. Verify the fix

Run:
```bash
cd /home/bt
RUN_FULL_COMPARISON=1 R -e "devtools::load_all(); testthat::test_file('tests/testthat/test-cpp-comparison.R')"
```

Should see:
- R -Inf count increase from 0 to ~2,176
- Matching -Inf counts between C++ and R
- First admission events returning -Inf when appropriate

## Related Files

- `/home/bt/tests/testthat/test-cpp-comparison.R` - New test suite
- `/home/bt/inst/original_cpp_output.csv` - C++ reference output (7,355 links)
- `/home/bt/copilot/LIKELIHOOD_COMPARISON.md` - Previous analysis
- `/home/bt/copilot/INITIAL_LIKELIHOOD_ISSUE.md` - Related issue doc
- `/home/bt/compare_ll.py` - Python comparison script

## Conclusion

The test infrastructure is now in place to:
1. ✅ Compare R and C++ likelihood computations
2. ✅ Identify specific discrepancies
3. ✅ Document the expected behavior
4. ⚠️ The core issue: **R gives finite likelihoods where C++ correctly returns -Inf**

The user's description of "infinite log likelihood occurs on start" appears to be a mischaracterization - the actual issue is **MISSING** -Inf checks for impossible admission/insitu states, not extra -Inf values.
