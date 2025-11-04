# R CMD CHECK Results Summary

**Date**: November 4, 2025  
**Package**: bayestransmission 0.0.0.9000  
**Status**: 2 ERRORS, 3 WARNINGS, 6 NOTES

---

## ERRORS (Must Fix) 🔴

### 1. Test Failure - Missing devtools dependency
**Location**: `tests/testthat/test-segfault-system.r:2:1`  
**Issue**: Test file loads `devtools` but it's not in Suggests  
**Fix**: Either add `devtools` to Suggests in DESCRIPTION, or remove the dependency from test

### 2. Vignette Build Failure - Missing ggplot2
**Location**: `vignettes/bayesian-transmission.Rmd:467-490`  
**Issue**: Vignette uses `ggplot2` and `tidyr` but not declared in DESCRIPTION  
**Fix**: Add to Suggests: `ggplot2`, `tidyr`

---

## WARNINGS (Should Fix) ⚠️

### 1. Executable Files in inst/
**Files**:
- `inst/original_cpp/src/lognormal/testHistLinkLL`
- `inst/original_cpp/src/util/test`

**Fix**: Remove executable bit or move to tools/ directory

### 2. Object Files in Source Package
**Issue**: Multiple `.o` files and `.a` libraries in `src/` and `inst/original_cpp/`  
**Fix**: Add cleanup in `.Rbuildignore` or ensure `make clean` removes them

### 3. GNU Extensions in Makevars
**File**: `src/Makevars`  
**Issue**: Uses `$(wildcard)` which is GNU-specific  
**Impact**: May not work on non-GNU make systems  
**Fix**: Use portable alternatives or document GNU make requirement

---

## NOTES (Nice to Fix) 📝

### 1. CRAN Incoming Feasibility
- **Version format**: 0.0.0.9000 has large components (development version OK)
- **Invalid URLs**: Several academic.oup.com URLs return 403 (common, likely paywall)
- **Suggestion**: Use DOIs instead of direct URLs where possible
- **Tarball size**: 5.3MB (acceptable)

### 2. Object Files in Source
**Already covered in WARNINGS**

### 3. Unable to Verify Current Time
**Issue**: System time sync issue in container  
**Impact**: Minor - just a timestamp check  
**Action**: Ignore (container-specific)

### 4. Non-standard Top-level Files
- `check_output.log` - Should be in `.Rbuildignore`
- `compare_ll.py` - Should be in `.Rbuildignore` or moved to `inst/` or `tools/`

### 5. Unstated Vignette Dependencies
**Already covered in ERRORS** - need ggplot2, tidyr in Suggests

### 6. HTML Manual Check Skipped
**Issue**: No `tidy` command available  
**Impact**: Minor - just validation  
**Action**: Optional install or ignore

---

## Package Size ℹ️
- **Installed size**: 46.1 MB
  - `libs/`: 41.8 MB (compiled code)
  - `original_cpp/`: 3.3 MB

---

## Test Results ✅
- **PASS**: 352 tests
- **FAIL**: 1 test (devtools dependency)
- **SKIP**: 17 tests (intentional)

---

## Priority Action Items

### High Priority (Breaking Issues)
1. Add to DESCRIPTION Suggests: `ggplot2`, `tidyr`, `devtools`
2. Add to `.Rbuildignore`: `check_output.log`, `compare_ll.py`
3. Clean object files and remove executable permissions

### Medium Priority (CRAN Submission)
4. Consider replacing GNU make extensions in `src/Makevars` with portable alternatives
5. Update URLs to use DOIs where available

### Low Priority (Polish)
6. Review and clean copilot/ directory - may want to move to `.Rbuildignore`
7. Consider size optimization if targeting CRAN (46MB is large but acceptable)
