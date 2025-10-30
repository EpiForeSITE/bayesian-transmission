# Data Validation: Preventing Unsorted Data Issues

## Problem
The C++ code expects event data to be sorted by patient ID first, then by time within each patient. When data is sorted only by time (chronological order), it causes:
- Episodes to be split incorrectly
- Events to be missing from history
- -inf log likelihoods
- NaN values in MCMC

## Solution: Multi-Layer Validation

### 1. Runtime Validation in `runMCMC()` (Primary Defense)

Added validation in `src/runMCMC.cpp` that checks data ordering BEFORE creating the System:

```cpp
// Validate data is sorted by patient, then time
std::vector<int> patients = as<std::vector<int>>(data[3]);
std::vector<double> times = as<std::vector<double>>(data[2]);

for (size_t i = 1; i < patients.size(); i++) {
    if (patients[i] < patients[i-1]) {
        Rcpp::stop("Data must be sorted by patient ID, then time. "
                  "Row %d has patient %d, but previous row had patient %d. "
                  "Please sort your data: data[order(data$patient, data$time), ]",
                  i+1, patients[i], patients[i-1]);
    }
    if (patients[i] == patients[i-1] && times[i] < times[i-1]) {
        Rcpp::stop("Data must be sorted by patient ID, then time. "
                  "For patient %d, row %d has time %.4f which is before row %d time %.4f. "
                  "Please sort your data: data[order(data$patient, data$time), ]",
                  patients[i], i+1, times[i], i, times[i-1]);
    }
}
```

**Benefits:**
- ✅ Catches the error immediately with a clear, actionable message
- ✅ Prevents invalid System construction
- ✅ Tells user exactly how to fix: `data[order(data$patient, data$time), ]`
- ✅ Low performance overhead (single O(n) pass)

### 2. Documentation Update (Prevention)

Updated the `runMCMC()` documentation in `R/RcppExports.R`:

```r
#' @param data Data frame with columns, in order: facility, unit, time, patient, and event type.
#'   **IMPORTANT**: Data must be sorted by patient ID, then time. 
#'   Use: \code{data <- data[order(data$patient, data$time), ]}
```

### 3. Test Coverage (Regression Prevention)

Created `tests/testthat/test-data-validation.R` with three test cases:

1. **Unsorted by patient** (time-sorted only) → should error
2. **Wrong time order within patient** → should error  
3. **Properly sorted data** → should work

All 3 tests pass ✅

### 4. Data Package Fix (Default Correctness)

The `simulated.data` package dataset is now pre-sorted:
```r
simulated.data <- simulated.data[order(simulated.data$patient, simulated.data$time), ]
```

This ensures examples and tests use correct data by default.

## Error Messages

### Patient Order Violation
```
Error: Data must be sorted by patient ID, then time. 
Row 2 has patient 1, but previous row had patient 698. 
Please sort your data: data[order(data$patient, data$time), ]
```

### Time Order Violation (within patient)
```
Error: Data must be sorted by patient ID, then time. 
For patient 103, row 5 has time 13.8203 which is before row 4 time 14.9996. 
Please sort your data: data[order(data$patient, data$time), ]
```

## Why This Location?

**`runMCMC()` is the best place because:**

1. **User-facing entry point** - Catches errors before they propagate
2. **Clear context** - Error message includes row numbers and values
3. **Actionable** - Tells user exactly how to fix the problem
4. **Performance** - Single O(n) check vs repeated failures in C++ code
5. **R-side fix** - User can easily re-sort in R before retrying

**Alternative locations considered:**

- ❌ `System::System()` constructor - Too low-level, less clear error messages
- ❌ `makeAllEpisodes()` - Too late, harder to diagnose which row is wrong
- ❌ R wrapper function - Would add overhead, users might bypass it
- ❌ RawEventList constructor - C++ side, harder to give helpful R-level errors

## Verification

```r
# This will ERROR with helpful message:
unsorted_data <- data[order(data$time), ]
runMCMC(data = unsorted_data, ...)

# This will WORK:
sorted_data <- data[order(data$patient, data$time), ]
runMCMC(data = sorted_data, ...)
```

## Future Considerations

If performance becomes an issue with large datasets, could add:
- `check_sorting = TRUE` parameter to allow users to skip validation
- Only validate first N rows + spot checks for very large datasets
- Add validation to other entry points (if any are created)

Currently, the O(n) validation is negligible compared to MCMC runtime.
