# Event Types and Their Likelihood Contributions

## Overview

This document enumerates all event types in the bayestransmission package and describes how each contributes to the log-likelihood calculation.

## Event Type Categories

### 1. Observed Events (Data Input)
These are events that come directly from the input data.

### 2. Inferred Events (Model-Generated)
These are events that the model infers during history construction.

### 3. Marker Events (Structural)
These are structural markers for episode boundaries.

---

## Complete Event Type Enumeration

Based on `src/infect/EventCoding.h`:

### **Observed Events**

| Event Code | Event Name | Number | Likelihood Component | Description |
|------------|------------|--------|----------------------|-------------|
| `admission` | admission | 0 | **Out-of-Unit Colonization** (`ocp->logProb(h)`) | Patient admission to unit - model assigns colonization state at admission |
| `negsurvtest` | negsurvtest | 1 | **Surveillance Test** (`survtsp->logProb(h)`) | Negative surveillance test result |
| `possurvtest` | possurvtest | 2 | **Surveillance Test** (`survtsp->logProb(h)`) | Positive surveillance test result |
| `discharge` | discharge | 3 | **No contribution** | Patient discharge - marks episode end |
| `negclintest` | negclintest | 4 | **Clinical Test** (`clintsp->logProb(h)`) | Negative clinical test result |
| `posclintest` | posclintest | 5 | **Clinical Test** (`clintsp->logProb(h)`) | Positive clinical test result |
| `abxdose` | abxdose | 9 | **No contribution** | Antibiotic dose administered |
| `abxon` | abxon | 10 | **Antibiotic Effect** (`abxp->logProb(h)`) | Antibiotic treatment started |
| `abxoff` | abxoff | 11 | **No contribution** | Antibiotic treatment stopped |
| `isolon` | isolon | 12 | **No contribution** | Isolation started |
| `isoloff` | isoloff | 13 | **No contribution** | Isolation stopped |
| `postest` | postest | 7 | N/A | Generic positive test (not used in UnitLinkedModel) |
| `negtest` | negtest | 8 | N/A | Generic negative test (not used in UnitLinkedModel) |

### **Inferred Events**

| Event Code | Event Name | Number | Likelihood Component | Description |
|------------|------------|--------|----------------------|-------------|
| `admission0` | admission0 | 14 | **Out-of-Unit Colonization** (`ocp->logProb(h)`) | Patient admitted in state 0 (uncolonized) |
| `admission1` | admission1 | 15 | **Out-of-Unit Colonization** (`ocp->logProb(h)`) | Patient admitted in state 1 |
| `admission2` | admission2 | 16 | **Out-of-Unit Colonization** (`ocp->logProb(h)`) | Patient admitted in state 2 |
| `insitu` | insitu | 6 | **In-situ Colonization** (`isp->logProb(h)`) | Generic in-situ colonization at admission |
| `insitu0` | insitu0 | 17 | **In-situ Colonization** (`isp->logProb(h)`) | Patient colonized in-situ to state 0 at admission |
| `insitu1` | insitu1 | 18 | **In-situ Colonization** (`isp->logProb(h)`) | Patient colonized in-situ to state 1 at admission |
| `insitu2` | insitu2 | 19 | **In-situ Colonization** (`isp->logProb(h)`) | Patient colonized in-situ to state 2 at admission |
| `acquisition` | acquisition | 31 | **In-unit Transmission** (`icp->logProb(h)`) | Patient acquired colonization within unit |
| `progression` | progression | 32 | **In-unit Transmission** (`icp->logProb(h)`) | Patient progressed to higher colonization state |
| `clearance` | clearance | 33 | **In-unit Transmission** (`icp->logProb(h)`) | Patient cleared colonization |

### **Marker Events**

| Event Code | Event Name | Number | Likelihood Component | Description |
|------------|------------|--------|----------------------|-------------|
| `start` | start | 21 | **No contribution** (returns 0) | Start of system/unit history |
| `marker` | marker | 22 | **No contribution** | Generic marker event |
| `stop` | stop | 23 | **No contribution** (returns 0) | End of system/unit history |

### **Special Events**

| Event Code | Event Name | Number | Likelihood Component | Description |
|------------|------------|--------|----------------------|-------------|
| `nullevent` | nullevent | -1 | N/A | Placeholder/null event |
| `error` | error | -999 | N/A | Error indicator |

---

## Likelihood Components Explained

The log-likelihood is computed by `UnitLinkedModel::logLikelihood(HistoryLink *h, int dogap)` and consists of:

### 1. **Gap Contributions** (if `dogap=1`)

Between consecutive events, the model computes the probability of the gap (time interval) where no events occurred:

- **`icp->logProbGap(prev, h)`**: Probability of no transmission events (acquisition/progression/clearance) in gap
- **`survtsp->logProbGap(prev, h)`**: Probability of no surveillance tests in gap
- **`clintsp->logProbGap(prev, h)`**: Probability of no clinical tests in gap (if clinical tests differ from surveillance)
- **`abxp->logProbGap(prev, h)`**: Probability related to antibiotic gaps (if antibiotics modeled)

### 2. **Event-Specific Contributions**

When an event occurs, its likelihood depends on the event type:

#### **A. Out-of-Unit Colonization (`ocp`)**
- **Event types**: `admission`, `admission0`, `admission1`, `admission2`
- **Computes**: Probability patient is in specific colonization state at admission
- **Key parameters**: `OutOfUnitInfectionParams`

#### **B. In-situ Colonization (`isp`)**
- **Event types**: `insitu`, `insitu0`, `insitu1`, `insitu2`
- **Computes**: Probability patient is colonized at admission site (not from outside)
- **Key parameters**: `InsituParams` - probabilities for each state

#### **C. In-unit Transmission (`icp`)**
- **Event types**: `acquisition`, `progression`, `clearance`
- **Computes**: Rate and probability of state transitions within unit
- **Key parameters**: 
  - Acquisition rates (depend on force of infection from colonized patients)
  - Progression rates
  - Clearance rates

#### **D. Surveillance Test (`survtsp`)**
- **Event types**: `negsurvtest`, `possurvtest`
- **Computes**: Probability of test result given patient's colonization state
- **Key parameters**: `SurveillanceTestParams` (sensitivity, specificity)
- **Critical**: Returns **-Inf** if test result contradicts inferred patient state

#### **E. Clinical Test (`clintsp`)**
- **Event types**: `negclintest`, `posclintest`
- **Computes**: Probability of clinical test result given patient's state
- **Key parameters**: `RandomTestParams` or similar (sensitivity, specificity)
- **Critical**: Returns **-Inf** if test result contradicts inferred patient state

#### **F. Antibiotic Effect (`abxp`)**
- **Event types**: `abxon`
- **Computes**: Impact of antibiotic administration on transmission rates
- **Key parameters**: `AbxParams` or `LinearAbxAcquisitionParams`

### 3. **Events with No Direct Contribution**

These events don't add to the likelihood themselves but may affect state:

- `discharge` (3): Marks end of episode
- `abxdose` (9): Tracks antibiotic administration
- `abxoff` (11): Antibiotic cessation
- `marker` (22): Structural marker
- `isolon` (12), `isoloff` (13): Isolation tracking

---

## How Events Cause -Inf Likelihood

Based on the comparison with the original C++ implementation, **test events** are the primary source of impossible configurations:

### **Critical Detection Points**

1. **Surveillance Tests** (`negsurvtest=1`, `possurvtest=2`):
   - If the model infers patient is in state X
   - But test result requires state Y
   - Then `survtsp->logProb(h)` returns `-Inf`

2. **Clinical Tests** (`negclintest=4`, `posclintest=5`):
   - Similar logic to surveillance tests
   - If test result contradicts inferred state → `-Inf`

3. **Admission State** (`admission=0` or `admission0`-`admission2`):
   - If the inferred admission state is incompatible with subsequent observations
   - The out-of-unit colonization probability may be `-Inf` (probability 0)

### **From the C++ vs R Comparison**

The comparison revealed:
- **C++ correctly returns -Inf for 2,176 history links**
- **R package only returns -Inf for 223 history links**
- **1,953 discrepancies** where R gives finite likelihood but C++ gives -Inf

**Most discrepancies involve `EventType=0` (admission)**, suggesting:
- The bug is likely in how admission states are validated against test results
- The R package may not properly check if admission state is compatible with observed tests

---

## Event Type Processing in `needEventType()`

The `UnitLinkedModel::needEventType()` function determines which events the model needs to process:

### **Returns 1** (Always needed):
- `insitu`, `insitu0`, `insitu1`, `insitu2`
- `admission`, `admission0`, `admission1`, `admission2`
- `discharge`
- `negsurvtest`, `possurvtest`
- `negclintest`, `posclintest`
- `abxdose`

### **Returns `cheating`** (Needed only if cheating=1):
- `acquisition`, `progression`, `clearance`
- These are normally hidden/inferred, only used if model is "cheating" (knows true states)

### **Returns 0** (Not needed):
- All other events (markers, null events, etc.)

---

## Summary Table: Likelihood Contribution by Event Type

| Event Type | Code | Contributes to LL? | Component | Can Cause -Inf? |
|------------|------|-------------------|-----------|-----------------|
| **Admissions** | 0, 14-16 | Yes | Out-of-unit | Yes |
| **In-situ** | 6, 17-19 | Yes | In-situ | Possibly |
| **Surveillance tests** | 1, 2 | Yes | Test | **Yes** (common) |
| **Clinical tests** | 4, 5 | Yes | Test | **Yes** (common) |
| **Discharge** | 3 | No | - | No |
| **Antibiotic** | 9, 10, 11 | Partial | Abx / None | No |
| **Transmission** | 31-33 | Yes | In-unit | Possibly |
| **Markers** | 21-23 | No | - | No |
| **Isolation** | 12, 13 | No | - | No |

---

## Key Insight for Bug Fix

The bug causing R to compute finite likelihoods instead of -Inf is most likely in the **test event handling** (surveillance and clinical tests), specifically:

1. **Test probability computation** (`survtsp->logProb(h)`, `clintsp->logProb(h)`)
2. **State validation** when test result contradicts inferred state
3. **Admission state compatibility** with subsequent test observations

The comparison shows the R package is **too lenient** - it's accepting history configurations that should be rejected as impossible.
