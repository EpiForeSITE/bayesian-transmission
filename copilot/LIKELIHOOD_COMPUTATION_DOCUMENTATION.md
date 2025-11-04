# Likelihood Computation Documentation

**Date:** October 30, 2025  
**Purpose:** Comprehensive documentation of how likelihoods are computed for each event type in the bayesian-transmission package  
**Focus:** Actual implementation in code, noting any deviations from expected statistical practices

---

## Table of Contents
1. [Overall Structure](#overall-structure)
2. [Event Types](#event-types)
3. [Individual Event Likelihood Contributions](#individual-event-likelihood-contributions)
4. [Gap Likelihood Contributions](#gap-likelihood-contributions)
5. [Total Likelihood Computation](#total-likelihood-computation)
6. [Notable Implementation Details](#notable-implementation-details)

---

## Overall Structure

The likelihood computation is implemented primarily in `UnitLinkedModel` class (`src/modeling/models_UnitLinkedModel.cpp`).

**Key Architecture:**
- Event-specific parameters are managed by separate parameter classes (InsituParams, OutColParams, InColParams, TestParams, AbxParams)
- Each parameter class implements:
  - `logProb(HistoryLink*)` - log-likelihood contribution of the event
  - `logProbGap(HistoryLink*, HistoryLink*)` - log-likelihood contribution of the time gap between events
  - `count(HistoryLink*)` - increment sufficient statistics for MCMC updates
  - `countGap(HistoryLink*, HistoryLink*)` - increment gap statistics

---

## Event Types

Event types are defined in `src/infect/EventCoding.h`:

### Observed Events
- `admission` (0) - Regular admission
- `negsurvtest` (1) - Negative surveillance test
- `possurvtest` (2) - Positive surveillance test
- `discharge` (3) - Patient discharge
- `negclintest` (4) - Negative clinical test
- `posclintest` (5) - Positive clinical test
- `abxdose` (9) - Antibiotic dose
- `abxon` (10) - Antibiotic starts
- `abxoff` (11) - Antibiotic ends
- `isolon` (12) - Isolation starts
- `isoloff` (13) - Isolation ends

### Inferred Events
- `insitu` (6) - Patient in situ at start
- `insitu0` (17) - In situ, uncolonized
- `insitu1` (18) - In situ, latent
- `insitu2` (19) - In situ, colonized
- `admission0` (14) - Admission, uncolonized
- `admission1` (15) - Admission, latent
- `admission2` (16) - Admission, colonized
- `acquisition` (31) - Colonization acquisition (S→L or S→C)
- `progression` (32) - Disease progression (L→C)
- `clearance` (33) - Colonization clearance (C→S or L→S)

### Marker Events
- `start` (21) - System start marker
- `stop` (23) - System stop marker
- `marker` (22) - General marker

---

## Individual Event Likelihood Contributions

### 1. Insitu Events (insitu, insitu0, insitu1, insitu2)

**Location:** `InsituParams::logProb()` in `src/modeling/models_InsituParams.cpp`

**Computation:**
```cpp
double InsituParams::logProb(infect::HistoryLink *h)
{
    int i = stateIndex(h->getPState()->infectionStatus());
    return ( i >= 0 ? logprobs[i] : 0);
}
```

**Formula:**
```
log L = log(P(infection_status))
```

Where:
- `P(uncolonized)` = probs[0]
- `P(latent)` = probs[1]  (3-state model only)
- `P(colonized)` = probs[2]

**Default Values:**
- 2-state model: P(unc)=0.9, P(col)=0.1
- 3-state model: P(unc)=0.98, P(lat)=0.01, P(col)=0.01

**Statistical Notes:**
- Represents the **prevalence distribution** at the start of observation
- Assumes patients already in the unit are a random sample from this distribution
- Updated via Dirichlet posterior during MCMC: `counts[i] ~ Gamma(prior[i] + observed[i], 1)`

**IMPORTANT DEVIATION:** The implementation does NOT condition on any prior history or time spent in unit before observation started. It's a simple categorical distribution over states.

---

### 2. Admission Events (admission, admission0, admission1, admission2)

**Location:** `OutColParams::logProb()` in `src/modeling/models_OutColParams.cpp`

**Computation:**
```cpp
double OutColParams::logProb(infect::HistoryLink *h)
{
    infect::PatientState *prev = h->pPrev()->getPState();
    double time = h->getEvent()->getTime() - h->pPrev()->getEvent()->getTime();
    infect::PatientState *cur = h->getPState();
    
    int i = stateIndex(prev->infectionStatus());  // previous state
    int j = stateIndex(cur->infectionStatus());   // current state
    return log(prob(i,j,time));
}
```

**Formula:**
```
log L = log(P(state_j | state_i, time_gap))
```

Where `prob(i,j,t)` is the transition probability computed from a continuous-time Markov chain:
```
P(i,j,t) = [exp(Q*t)]_{i,j}
```

`Q` is the rate matrix with off-diagonal elements being transition rates and diagonal elements ensuring rows sum to 0.

**Statistical Notes:**
- Models **out-of-hospital colonization dynamics** between discharge and readmission
- Uses matrix exponential to compute transition probabilities
- For the **first admission** (no previous discharge), uses equilibrium/stationary distribution
- Eigendecomposition is used for efficient computation (code uses complex numbers for eigenvalues)

**CRITICAL IMPLEMENTATION DETAIL:**
```cpp
if (h->pPrev() != 0) {
    // Use time since last discharge
} else {
    // First admission - uses equilibrium probs
}
```

The implementation correctly handles both cases but the time gap is between **this admission and the previous discharge**, not admission-to-admission time.

---

### 3. Surveillance Test Events (negsurvtest, possurvtest)

**Location:** `TestParams::logProb()` in `src/modeling/modeling_TestParams.cpp`

**Computation:**
```cpp
double TestParams::logProb(infect::HistoryLink *h)
{
    int i = stateIndex(h->getPState()->infectionStatus());  // true state
    int j = testResultIndex(h->getEvent()->getType());      // test result (0=neg, 1=pos)
    return ( i < 0 || j < 0 ? 0 : logprobs[i][j] );
}
```

**Formula:**
```
log L = log(P(test_result | true_infection_state))
```

**Test Sensitivity/Specificity Matrix:**
```
                    Test Negative    Test Positive
Uncolonized         1 - sens_unc     sens_unc
Latent              1 - sens_lat     sens_lat
Colonized           1 - sens_col     sens_col
```

**Default Values (from constructor):**
- Uncolonized: sens = 0.0 (specificity = 1.0)
- Latent: sens = 0.0 (if 3-state model)
- Colonized: sens = 0.8

**Statistical Notes:**
- Classic conditional probability given true state
- **Does NOT include prevalence** - that's handled by the augmented infection history
- Assumes test sensitivity/specificity are **state-dependent** but **time-invariant**
- Updated via Beta posteriors during MCMC

**NO DEVIATIONS** - Standard implementation of diagnostic test likelihood.

---

### 4. Clinical Test Events (negclintest, posclintest)

**Computation:** Identical to surveillance tests but uses separate `clintsp` parameter object.

**Note:** Can share parameters with surveillance tests (`clintsp == survtsp`) or be separate.

---

### 5. Transmission Events (acquisition, progression, clearance)

**Location:** `InColParams::logProb()` - abstract, implemented by subclasses like `MassActionICP`

**For MassActionICP (Mass Action In-Colonization Parameters):**

```cpp
double MassActionICP::logProb(infect::HistoryLink *h)
{
    return log(eventRate(...));
}
```

**Formula:**
```
log L = log(rate)
```

Where rate depends on event type:
- **Acquisition**: `λ_acq * I/N` (mass action)
- **Progression**: `λ_prog`
- **Clearance**: `λ_clear`

**CRITICAL NOTE:** These events are only included when `cheating = TRUE`, meaning the true infection status is being used (typically for validation or initial development).

**Statistical Notes:**
- This is the **rate** of the event, not a probability
- Combined with gap probability (see below) to form proper continuous-time process likelihood
- The "mass action" term `I/N` makes acquisition rate proportional to prevalence in the unit

**DEVIATION FROM STANDARD:** Most implementations would condition on the complete infection history, but here the rate is treated as instantaneous and combined with gap probabilities for the full likelihood.

---

### 6. Antibiotic On Event (abxon)

**Location:** `AbxParams::logProb()` in `src/modeling/models_AbxParams.cpp`

**Computation:**
```cpp
double AbxParams::logProb(infect::HistoryLink* h)
{
    if (h->getEvent()->getType() == abxon)
    {
        infect::AbxPatientState *ps = (infect::AbxPatientState *) h->getPState();
        if (ps->onAbx() == 1)
        {
            return log(rates[stateIndex(ps->infectionStatus())]);
        }
    }
    return 0;
}
```

**Formula:**
```
log L = log(λ_abx[infection_state])
```

Only contributes if patient transitions to antibiotic ON state.

**Statistical Notes:**
- Models antibiotic initiation as a Poisson process with **state-dependent rates**
- Rate varies by infection status (uncolonized, latent, colonized)
- Default rates are all 1.0

**CRITICAL IMPLEMENTATION DETAIL:** The check `ps->onAbx() == 1` means this only fires when antibiotic status **changes from OFF to ON**, not for every time point where patient is on antibiotics.

---

### 7. Non-Contributing Events (Event-Specific Likelihood Only)

These events contribute **0 to the EVENT-SPECIFIC likelihood** but **DO contribute GAP likelihoods** (lines 352-361 in models_UnitLinkedModel.cpp):

- `discharge` - No probability associated with discharge timing itself, but gap probabilities before discharge are included
- `abxdose` - Doses don't affect likelihood directly
- `abxoff` - Antibiotic cessation not modeled
- `marker` - Just markers
- `start` - System boundary (returns 0 explicitly, checked before gap calculation)
- `stop` - System boundary (returns 0 explicitly, checked before gap calculation)
- `isolon` - Isolation not currently modeled
- `isoloff` - Isolation not currently modeled

**CRITICAL DISTINCTION:** 
```cpp
double UnitLinkedModel::logLikelihood(infect::HistoryLink *h, int dogap) {
    // Early return for start/stop - no gap OR event likelihood
    switch(h->getEvent()->getType()) {
        case start:
        case stop:
            return 0;
    }
    
    // Gap likelihoods computed for ALL other events (including discharge)
    if (dogap) {
        x += icp->logProbGap(prev,h);     // Transmission gap
        x += survtsp->logProbGap(prev,h); // Test gap
        x += abxp->logProbGap(prev,h);    // Abx gap
    }
    
    // Event-specific likelihood (discharge is in the "break" section = 0)
    switch(h->getEvent()->getType()) {
        case discharge:  // No event-specific contribution
        case abxdose:
        case abxoff:
        case marker:
            break;  // Contributes 0 to event likelihood
    }
}
```

**Example from Test 2:**
```
Discharge at time 20 (gap from admission at time 10):
  Gap contribution: -40.0 (= -Δt × rates for 10 time units)
  Event contribution: 0.0
  Total: -40.0

Discharge at time 120 (gap from previous discharge at time 20):
  Gap contribution: -200.0 (= -Δt × rates for 100 time units)
  Event contribution: 0.0
  Total: -200.0
```

**IMPORTANT:** Discharge events are **NOT ignored** - they contribute substantial negative log-likelihood through gap probabilities. The gap represents the probability that no transmission/test/antibiotic events occurred between the previous event and the discharge.

**NOTABLE:** The discharge **timing** itself is treated as non-informative (no conditional probability based on patient health status), but the **time interval before discharge** contributes to likelihood through gap probabilities. This is a modeling choice that may not match clinical reality where discharge timing depends on health status.

---

## Gap Likelihood Contributions

Between consecutive events, the model includes a "gap" likelihood representing the probability that **no events occurred** during that interval.

### Structure

For each parameter class, `logProbGap(prev, current)` is computed:

```cpp
// In UnitLinkedModel::logLikelihood()
if (dogap) {
    x += icp->logProbGap(prev, h);       // Transmission events
    x += survtsp->logProbGap(prev, h);    // Surveillance tests
    x += clintsp->logProbGap(prev, h);    // Clinical tests (if separate)
    x += abxp->logProbGap(prev, h);       // Antibiotic events
}
```

### 1. Transmission Gap (InColParams)

**Implementation in MassActionICP:**

```cpp
double MassActionICP::logProbGap(infect::HistoryLink *g, infect::HistoryLink *h)
{
    infect::LocationState *s = h->uPrev()->getUState();
    double dt = h->getEvent()->getTime() - g->getEvent()->getTime();
    
    return -dt * (
        s->getSusceptible() * λ_acquisition * (s->getColonized()/s->getTotal()) +
        s->getLatent() * λ_progression +
        s->getColonized() * λ_clearance
    );
}
```

**Formula:**
```
log L_gap = -Δt * Σ(n_state * λ_state)
```

**Statistical Interpretation:**
- This is the survival function for a **competing risks Poisson process**
- Probability of NO transmission events = exp(-total_rate * time)
- Correctly accounts for number of individuals **at risk** in each state

**CRITICAL DETAIL:** Uses `uPrev()->getUState()` which is the **unit state at the previous event**, properly accounting for the population composition during the gap.

---

### 2. Test Gap (TestParams via RandomTestParams)

**Implementation:**

```cpp
double RandomTestParams::logProbGap(infect::HistoryLink *g, infect::HistoryLink *h)
{
    double dt = h->getEvent()->getTime() - g->getEvent()->getTime();
    return -dt * total_test_rate;
}
```

**Formula:**
```
log L_gap = -Δt * λ_test
```

**Statistical Notes:**
- Models surveillance/clinical testing as a **Poisson process** with constant rate
- Probability of no tests = exp(-λ_test * Δt)
- Rate does NOT depend on unit census (everyone equally likely to be tested)

**POTENTIAL ISSUE:** This assumes test rate is **independent of unit state**. In reality, testing rates might increase with prevalence or patient turnover.

---

### 3. Antibiotic Gap (AbxParams)

**Implementation:**

```cpp
double AbxParams::logProbGap(infect::HistoryLink *g, infect::HistoryLink *h)
{
    infect::AbxLocationState *s = (infect::AbxLocationState *) h->uPrev()->getUState();
    double dt = h->getEvent()->getTime() - g->getEvent()->getTime();
    
    return -dt * (
        s->getNoAbxSusceptible() * rates[0] +
        s->getNoAbxLatent() * rates[1] +
        s->getNoAbxColonized() * rates[2]
    );
}
```

**Formula:**
```
log L_gap = -Δt * Σ(n_not_on_abx_in_state * λ_abx[state])
```

**Statistical Notes:**
- Only patients **currently NOT on antibiotics** are at risk of starting them
- Correctly uses `getNoAbxSusceptible()` etc. to count only at-risk individuals
- State-dependent rates allow for clinical targeting (e.g., higher rates for colonized patients)

**IMPLEMENTATION CORRECTNESS:** Properly implements competing risks with heterogeneous population.

---

## Total Likelihood Computation

### System Level

```cpp
double UnitLinkedModel::logLikelihood(infect::SystemHistory *hist)
{
    double xtot = 0;
    for (Map *h = hist->getUnitHeads(); h->hasNext(); )
    {
        double utot = 0;
        for (infect::HistoryLink *l = (infect::HistoryLink *) h->nextValue(); 
             l != 0; 
             l = l->uNext())
        {
            utot += logLikelihood(l);
        }
        xtot += utot;
    }
    return xtot;
}
```

**Structure:**
1. Iterate over all units
2. For each unit, traverse the unit history chain (`uNext()`)
3. Sum log-likelihoods from all history links
4. Return total sum

**Formula:**
```
log L_total = ΣΣ log L(event, gap)
            units links
```

### History Link Level

```cpp
double UnitLinkedModel::logLikelihood(infect::HistoryLink *h, int dogap)
{
    double x = 0;
    infect::HistoryLink *prev = h->uPrev();
    
    // Gap contributions (if dogap=1)
    if (dogap) {
        x += icp->logProbGap(prev, h);
        x += survtsp->logProbGap(prev, h);
        if (clintsp && clintsp != survtsp)
            x += clintsp->logProbGap(prev, h);
        if (abxp != 0)
            x += abxp->logProbGap(prev, h);
    }
    
    // Event contribution (if not hidden)
    if (!h->isHidden()) {
        switch(h->getEvent()->getType()) {
            case insitu: ... : x += isp->logProb(h); break;
            case admission: ... : x += ocp->logProb(h); break;
            case acquisition: ... : x += icp->logProb(h); break;
            case negsurvtest: ... : x += survtsp->logProb(h); break;
            case negclintest: ... : x += clintsp->logProb(h); break;
            case abxon: x += abxp->logProb(h); break;
            case discharge: ... : break;  // Contributes 0
            // ... etc
        }
    }
    
    return x;
}
```

**Key Point:** The `dogap` parameter controls whether gap probabilities are included. It's typically:
- `dogap=1` for regular likelihood computation
- `dogap=0` when computing likelihood for admission/insitu events (gaps already counted in patient history)

---

## Notable Implementation Details

### 1. Conditional Gap Calculation

**From lines 245-256 in models_UnitLinkedModel.cpp:**

```cpp
double UnitLinkedModel::logLikelihood(infect::Patient *pat, infect::HistoryLink *h, int opt)
{
    for (infect::HistoryLink *l = h; l != 0; )
    {
        infect::Event *e = l->getEvent();
        if (e->getPatient() == pat && (e->isAdmission() || e->isInsitu()))
            x += logLikelihood(l, 0);  // dogap=0 for patient's own admission
        else
            x += logLikelihood(l, 1);  // dogap=1 for other events
        
        if (e->getPatient() == pat && e->getType() == discharge)
            l = l->pNext();  // Follow patient chain
        else
            l = l->uNext();  // Follow unit chain
    }
}
```

**Interpretation:**
- When computing likelihood for a patient's admission, **gaps are not counted** (`dogap=0`)
- For other events affecting the patient, gaps **are counted** (`dogap=1`)
- This prevents **double-counting** of gap probabilities

**CRITICAL FOR CORRECTNESS:** This is essential because the out-of-hospital transition probability (in OutColParams) already accounts for the time gap. Including the gap likelihood again would count it twice.

---

### 2. Hidden Events

```cpp
if (h->isHidden())
    return x;  // Return gap likelihood only, skip event likelihood
```

**Purpose:** During MCMC sampling, proposed augmented events may be "hidden" to test different infection histories. Hidden events contribute gap likelihoods but not event likelihoods.

---

### 3. Start/Stop Events

```cpp
case start:
case stop:
    return 0;
```

These are **purely structural markers** with no probabilistic content. They mark system boundaries but don't contribute to likelihood.

---

### 4. Cheating Mode

```cpp
case acquisition:
case progression:
case clearance:
    return cheating ? icp->logProb(h) : 0;
```

When `cheating=0`, transmission events contribute 0 to likelihood even though they're in the augmented history. This allows the model to:
- Run with **latent infection statuses** (normal mode)
- Run with **known infection statuses** for validation (cheating mode)

**DESIGN NOTE:** This is a clever way to switch between full Bayesian inference and "cheat" mode without changing the code structure.

---

### 5. Log Probability Safety

**From InsituParams::set():**

```cpp
const double eps = 1e-300;  // Very small but still > 0
for (int i=0; i<3; i++)
    logprobs[i] = log(std::max(probs[i], eps));
```

**Protection against:** log(0) = -Inf

This ensures that even if a probability is set to exactly 0 (e.g., for 2-state model where latent probability is 0), the log-probability is finite.

**STATISTICAL IMPLICATION:** Effectively puts a floor on all probabilities at 10^-300, which is computationally sensible but means **no probability is truly zero**.

---

## Summary of Deviations and Notable Choices

### 1. Discharge Events: Gap vs Event Likelihood
**Standard Approach:** Many hospital models condition discharge on patient status (healthier patients discharge sooner).

**Implementation:** 
- Discharge **event itself** contributes 0 to likelihood (discharge timing is non-informative)
- However, discharge events **DO contribute gap likelihoods** representing the probability that no other events (transmission, tests, antibiotics) occurred between the previous event and discharge
- This can be a **substantial negative contribution** to total likelihood

**Example:** A discharge at time 20 with previous event at time 10 contributes:
```
log L = -Δt × Σ(rates) = -(20-10) × (transmission_rate + test_rate + abx_rate)
```
If combined rates = 4.0, then log L = -40.0

**Implication:** 
- Model cannot learn about association between infection status and **when** patients are discharged
- Model CAN learn about infection dynamics **during** the hospitalization period up to discharge
- The gap likelihood penalizes long time periods where no events occur (encourages parsimony)

---

### 2. Gap Probabilities Use Unit-Level State
**Standard Approach:** Could use individual-level risk.

**Implementation:** Uses **aggregate unit state** (e.g., total number susceptible) for computing rates.

**Implication:** This is **correct for mass action models** where force of infection depends on prevalence, but assumes **homogeneous mixing** within units.

---

### 3. Test Rates Independent of Unit State
**Standard Approach:** Testing might intensify with outbreak signals.

**Implementation:** Constant Poisson rate for testing, **independent of unit prevalence**.

**Implication:** Cannot model **outbreak-triggered testing** or targeted screening.

---

### 4. Admission Likelihood Uses Time Since Last Discharge
**Standard Approach:** Could model time-to-readmission from any point.

**Implementation:** Uses **previous discharge** as reference point for transition probability.

**Implication:** This is **correct** - the patient's infection history only matters from their last known state (at discharge). Time between discharges is irrelevant.

---

### 5. No Discharge Rate Modeling
**Standard Approach:** Model discharge as competing risk with infection events.

**Implementation:** Discharge times are **observed and fixed**, not modeled probabilistically.

**Implication:** This is a **semi-Markov** process where sojourn times (admission to discharge) are data, not modeled. Simplifies computation but loses inferential power about discharge processes.

---

### 6. Antibiotic Initiation Is State-Dependent
**Standard Approach:** Could be purely random or protocol-driven.

**Implementation:** Different rates for uncolonized/latent/colonized patients, suggesting **clinical targeting**.

**Implication:** Can capture that doctors may be more likely to prescribe antibiotics to visibly sick (colonized) patients.

---

## Conclusion

The implementation represents a **well-structured, theoretically sound** approach to hospital transmission modeling with:

✅ **Correct Poisson process likelihoods** for competing risks  
✅ **Proper matrix exponential** for out-of-hospital transitions  
✅ **Appropriate test sensitivity/specificity** conditional probabilities  
✅ **Careful handling of gap probabilities** to avoid double-counting  
✅ **State-space augmentation** for latent infection statuses  

⚠️ **Notable Modeling Choices:**
- Discharge timing is non-informative
- Testing rates don't depend on outbreak signals
- Homogeneous mixing within units
- All probabilities have a minimum floor (10^-300)

These choices are **reasonable** for many applications but should be considered when interpreting results. The code is clean, well-commented, and follows good software engineering practices for statistical computing.

---

**Document Author:** AI Analysis  
**Last Updated:** October 30, 2025  
**Code Version:** bayesian-transmission main branch
