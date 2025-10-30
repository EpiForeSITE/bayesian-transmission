# Investigation Summary: makeHistLink Argument Order

## Date: October 28, 2025

## Investigation

I investigated whether the `-Inf` initial log likelihood was caused by incorrect argument order in calls to `makeHistLink()`.

## Findings

### Original C++ Signature
File: `/home/bt/inst/original_cpp/src/modeling/UnitLinkedModel.h` (line 121)
```cpp
virtual HistoryLink *makeHistLink(Facility *f, Unit *u, double time, Patient *p, EventCode type, int linked)
```
**Argument order**: `(f, u, TIME, PATIENT, type, linked)`

### R Package Signature  
File: `/home/bt/src/modeling/UnitLinkedModel.h` (line 53)
```cpp
virtual infect::HistoryLink* makeHistLink(infect::Facility *f, infect::Unit *u, infect::Patient *p, double time, EventCode type, int linked);
```
**Argument order**: `(f, u, PATIENT, TIME, type, linked)`

## Key Discovery

**The R package INTENTIONALLY changed the argument order from the original C++.**

Git history shows this has been the signature since at least commit 8a27dcc (early in the project). ALL call sites in the R package use the `(f, u, patient, time, ...)` order consistently:

- `models_ConstrainedSimulator.cpp`: Uses `(f,u,p,times[...],...)` 
- `models_ForwardSimulator.cpp`: Uses `(f,u,p,atime,...)` 
- All call sites match the R package signature

## Attempted Fix (REVERTED)

I attempted to "fix" the R package to match the original C++ by:
1. Changing the signature to `(f, u, double time, Patient *p, ...)`
2. Updating all call sites to use `(f, u, time, p, ...)`

**Result**: The package compiled successfully but produced incorrect behavior:
- Every single HistoryLink produced `-inf` likelihood during episode sampling
- The MCMC entered an infinite loop printing debug messages
- This proved the change was WRONG

## Conclusion

1. **The argument order difference is INTENTIONAL and CORRECT for the R package**
2. **All code in the R package is consistent with its own signature**
3. **The `-Inf` initial likelihood is NOT caused by argument order issues**
4. **The original C++ and R package have intentionally different APIs**

## Actual Root Cause of -Inf Likelihood

The `-Inf` initial likelihood is the actual current behavior of the R package and is caused by episode initialization logic, NOT by argument ordering bugs. See `ROOT_CAUSE_INF_LIKELIHOOD.md` for details.

## Recommendation

**DO NOT** attempt to change the `makeHistLink` signature or argument order. The R package works correctly with its current signature. The `-Inf` initial likelihood needs to be addressed through changes to episode initialization logic, not API changes.

## Files Reverted

All changes to `src/` were reverted using:
```bash
git checkout HEAD -- src/
```

The package has been restored to its working state (though still with the `-Inf` initial likelihood issue).
