# Segfault Fix Summary

## Problem
When running the `test-data-validation.R` test twice in the same R session, a segfault occurred with the error:
```
*** caught segfault ***
address 0x21, cause 'memory not mapped'
```

## Root Cause
In `/home/bt/src/runMCMC.cpp` at line 333, there was a critical bug:

```cpp
delete AbxCoding::sysabx;
```

This line was **deleting a static member** `AbxCoding::sysabx`, which is:
- A **static class member** shared across all instances
- Initialized once at program start in `infect_AbxCoding.cpp`
- Used to track antibiotic state across patients

### Why This Caused a Segfault

1. **First run**: 
   - Creates and uses `sysabx` normally
   - At cleanup, **deletes the static member** `sysabx`
   - The pointer still exists but points to freed memory (dangling pointer)

2. **Second run**:
   - Tries to use `sysabx` 
   - The pointer points to freed memory → **segfault at address 0x21**

## Solution
Changed line 333 in `/home/bt/src/runMCMC.cpp` from:
```cpp
delete AbxCoding::sysabx;
```

To:
```cpp
// Don't delete static members - they are shared across all invocations
// Instead, clear them for the next run
if (AbxCoding::sysabx != 0)
    AbxCoding::sysabx->clear();
if (AbxCoding::syseverabx != 0)
    AbxCoding::syseverabx->clear();
```

### Key Changes:
1. **Removed the deletion** of static member `sysabx`
2. **Added clearing** of both `sysabx` and `syseverabx` to reset state between runs
3. **Added null checks** to prevent issues if the pointers are somehow null

## Verification
The fix was tested by running `test-data-validation.R` multiple times:
- ✅ 2 consecutive runs: SUCCESS
- ✅ 5 consecutive runs: SUCCESS
- ✅ Full test suite: PASS (191 passing tests, 1 pre-existing failure unrelated to this fix)

## Technical Details

### Static Members Involved:
- `AbxCoding::sysabx` - Initialized in `infect_AbxCoding.cpp` line 3
- `AbxCoding::syseverabx` - Initialized in `infect_AbxCoding.cpp` line 4

### Usage Locations:
- `infect_AbxPatientState.cpp`: lines 53, 55, 57
- `infect_AbxLocationState.cpp`: lines 149, 150

### Map::clear() Method:
The `clear()` method (defined in `src/util/Map.h` line 227) properly deallocates all MapLinks while maintaining the Map structure itself, making it safe to reuse.

## Design Analysis: Should These Be Static Members?

### The Design Intent
- `sysabx` and `syseverabx` are **global singleton Maps** that track antibiotic status across ALL patients in the system
- Each patient's `AbxPatientState` updates these global maps when patients start/stop antibiotics
- `AbxLocationState` queries these maps to determine antibiotic prevalence in locations

### Why Static Made Sense in Original Code
In the original standalone C++ program (`inst/original_cpp/runMCMC.cc`):
- The program runs **once** as a standalone executable
- There's only **one System** being simulated per program execution
- At line 234, it deletes `AbxCoding::sysabx` just before `main()` exits
- This worked because the program terminates immediately after

### Why Static is Problematic in R Package
In the R package:
- `runMCMC()` can be called **multiple times** in the same R session
- Each call creates a new `System` with potentially overlapping patient IDs
- Leftover state from previous runs could corrupt new simulations
- **Deleting static members causes segfaults on subsequent calls**

### The Correct Solution
**Clear, don't delete** - Reset the global state between runs while keeping the singleton pattern:
```cpp
// Clear the maps to remove all entries from the previous run
if (AbxCoding::sysabx != 0)
    AbxCoding::sysabx->clear();
if (AbxCoding::syseverabx != 0)
    AbxCoding::syseverabx->clear();
```

This approach:
- ✅ Maintains the singleton pattern required by the architecture
- ✅ Properly isolates consecutive `runMCMC()` calls
- ✅ Prevents memory corruption from dangling pointers
- ✅ Handles patient ID overlaps between different datasets

## Best Practice
**Never delete static members in local scope** - they have program lifetime and should only be cleaned up at program termination. If state needs to be reset between uses, use a `clear()` or `reset()` method instead. This is especially critical when porting standalone C++ programs to library/package contexts where functions may be called multiple times.
