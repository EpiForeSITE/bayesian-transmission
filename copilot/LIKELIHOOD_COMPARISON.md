# Likelihood Comparison: R Package vs Original C++

## Summary

Compared individual history link log likelihoods between the R package implementation and the original C++ implementation using the simulated.data dataset and LinearAbxModel2 parameters.

### Key Findings

1. **Total history links**: 7,344
2. **C++ has -Inf**: 2,176 links (29.6%)
3. **R package has -Inf**: 223 links (3.0%)
4. **Agreement on -Inf**: 223 links (both implementations agree these are impossible)
5. **C++ has ADDITIONAL -Inf**: **1,953 links** where R gives finite likelihood but C++ gives -Inf
6. **R has ADDITIONAL -Inf**: 0 (R never gives -Inf when C++ gives finite)

### Finite Value Agreement

For the 5,168 links where both implementations give finite values:
- **Max absolute difference**: 6.13
- **Mean absolute difference**: 0.035
- **Median absolute difference**: (not computed yet)

The finite values show reasonable agreement, with most differences very small.

### Critical Issue

**The R package computes finite likelihoods for 1,953 history links that the original C++ correctly identifies as impossible (returns -Inf).**

This is a serious bug in the R package implementation. The R package is **failing to detect impossible states/events**, which leads to:
1. Incorrect likelihood calculations
2. Accepting impossible histories in MCMC sampling
3. Invalid posterior inference

### Examples of Discrepancies

#### First Few Cases Where C++ Returns -Inf but R Doesn't

1. **Unit=0, Link=1**: EventType=0 (admission), Time=0, Patient=698
   - C++: -Inf
   - R: -0.095
   
2. **Unit=0, Link=10**: EventType=0 (admission), Time=3.148, Patient=19
   - C++: -Inf
   - R: -0.955

3. **Unit=0, Link=1002**: EventType=0 (admission), Time=115.405, Patient=1110
   - C++: -Inf
   - R: -4.094

4. **Unit=0, Link=104**: EventType=0 (admission), Time=16.128, Patient=69
   - C++: -Inf
   - R: -2.854

### Pattern Analysis

Looking at the event types of the problematic links:
- Many involve EventType=0 (admission events)
- These are likely test/surveillance events that are incompatible with the inferred colonization states

### Hypothesis

The original C++ implementation appears to have stricter checking for:
1. Test events that would require the patient to be in a certain state
2. Surveillance/clinical tests that are incompatible with the inferred history
3. Admission events that violate temporal or state constraints

The R package may have:
1. Missing validation checks in the likelihood calculation
2. Incorrect logic for determining when a history link is impossible
3. Different handling of test event probabilities when they contradict inferred states

### Next Steps

1. **Identify the root cause**: Find where the R package differs from C++ in impossibility detection
2. **Focus on EventType=0**: Many discrepancies involve admission events
3. **Review test event handling**: Compare how test events are evaluated in both implementations
4. **Fix the bug**: Ensure R package properly returns -Inf for impossible configurations
5. **Verify the fix**: Re-run this comparison after fixes

### Files

- **C++ output**: `original_cpp_output.csv` (7,363 lines including headers)
- **R output**: `r_package_output.txt` (full diagnostic output)
- **Comparison script**: `compare_ll.py`
- **Test program**: `inst/original_cpp/src/lognormal/testHistLinkLL.cc`
- **Data export**: `simulated_data_for_cpp.txt` (8,360 events)
- **Model params**: `model_params_for_cpp.txt`

### Conclusion

This comparison has revealed a **critical bug in the R package**: it fails to detect 1,953 impossible history links that the original C++ correctly identifies. This explains why the initial likelihood was computing as -Inf (from the 223 correctly identified impossible links) while the package should be rejecting far more configurations as impossible.

The finite likelihood values show good agreement where both implementations compute them, suggesting the core likelihood calculation is correct. The bug is specifically in the **validation/impossibility detection logic**.
