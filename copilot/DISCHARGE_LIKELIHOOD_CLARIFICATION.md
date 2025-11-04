# Discharge Event Likelihood: Clarification

## Question

The documentation states that "discharge events contribute 0 to likelihood," but Test 2 shows discharge events with substantial non-zero log likelihood contributions:

```
Event  7 | discharge    |   20.0 |     -40.000000
Event  8 | discharge    |  120.0 |    -200.000000
Event 11 | discharge    |  120.0 |       0.000000
```

Why is this?

## Answer

The documentation is **technically correct but incomplete**. Here's what's actually happening:

### Two Components of Likelihood

Each HistoryLink contributes to the total likelihood through **two mechanisms**:

#### 1. Event-Specific Likelihood
This is the probability of the event itself occurring given the current state.

```cpp
switch(h->getEvent()->getType()) {
    case insitu:
        x += isp->logProb(h);  // P(initial infection status)
        break;
    case admission:
        x += ocp->logProb(h);  // P(colonization at admission)
        break;
    case discharge:
        break;  // Contributes 0 - discharge timing is non-informative
}
```

**For discharge:** Event-specific contribution = **0**

#### 2. Gap Likelihood
This is the probability that NO OTHER events occurred during the time interval between the previous event and the current event.

```cpp
if (dogap) {
    x += icp->logProbGap(prev, h);     // No transmission events in gap
    x += survtsp->logProbGap(prev, h); // No test events in gap
    x += abxp->logProbGap(prev, h);    // No antibiotic events in gap
}
```

**For discharge:** Gap contribution = **-Δt × Σ(rates)**

### What Gets Computed for Start/Stop vs Discharge

**Start/Stop events:**
```cpp
// Early return - checked BEFORE gap calculation
case start:
case stop:
    return 0;  // No gap OR event likelihood
```

**Discharge (and other non-informative events):**
```cpp
// Gap IS calculated
if (dogap) {
    x += gap_probabilities;  // This runs for discharge
}

// Then event-specific likelihood
switch(type) {
    case discharge:
        break;  // Event contribution = 0
}

return x;  // Returns gap likelihood only
```

## Example from Test 2

### Discharge at Time 20

```
Previous event: admission at time 10
Time gap: 10 units
Log likelihood: -40.0
```

**Breakdown:**
- Event-specific: 0.0 (discharge timing is non-informative)
- Gap contribution: -40.0
  - Formula: `-Δt × (transmission_rate + test_rate + abx_rate)`
  - Calculation: `-10 × (rate ≈ 4.0) = -40.0`
  
**Interpretation:** This is the log probability that:
- No patients acquired colonization during those 10 time units
- No tests were performed during those 10 time units  
- No patients started antibiotics during those 10 time units

### Discharge at Time 120

```
Previous event: discharge at time 20
Time gap: 100 units
Log likelihood: -200.0
```

**Breakdown:**
- Event-specific: 0.0
- Gap contribution: -200.0
  - Formula: `-100 × (rate ≈ 2.0) = -200.0`
  
The different rate (2.0 vs 4.0) likely reflects different unit states:
- After admission: Patient present, higher combined event rate
- After discharge: Unit may have fewer or no patients, lower event rate

### Discharge at Time 120 (Event 11)

```
Previous event: stop at time 120
Time gap: 0 units
Log likelihood: 0.0
```

**Breakdown:**
- Event-specific: 0.0
- Gap contribution: 0.0 (no time gap)

This discharge is coincident with the system stop marker, so there's no gap.

## Updated Documentation Statement

**OLD (incomplete):**
> "Discharge events contribute 0 to the likelihood"

**NEW (complete):**
> "Discharge events contribute 0 to the **event-specific** likelihood (discharge timing is non-informative), but **DO contribute gap likelihoods** representing the probability that no transmission, testing, or antibiotic events occurred between the previous event and the discharge. This gap contribution can be substantial and negative, especially for long time intervals."

## Implications

### What the Model CAN Learn
- **Transmission dynamics** during hospitalization (via gap probabilities)
- **Testing patterns** (via gap probabilities)
- **Antibiotic usage patterns** (via gap probabilities)
- All of these are influenced by the timing and sequencing of events including discharges

### What the Model CANNOT Learn
- Whether **discharge timing itself** is associated with infection status
- For example, if colonized patients stay longer, the model won't capture this through discharge events
- This would require modeling discharge as a competing risk with infection-status-dependent rates

### Why This Design?

This is a **semi-Markov process** approach where:
- Event **times** are observed (not modeled probabilistically)
- Event **sequences** contribute to likelihood
- **Gaps between events** penalize long intervals without activity

**Advantages:**
- Computationally simpler than modeling discharge as competing risk
- Still captures most transmission dynamics
- Focuses inference on colonization/testing processes rather than hospital operations

**Disadvantages:**
- Cannot detect associations between infection status and length of stay
- Treats all discharge timing as exogenous

## Code References

**Gap calculation:** `src/modeling/models_UnitLinkedModel.cpp` lines 286-298
```cpp
if (dogap) {
    x += icp->logProbGap(prev,h);      // Transmission
    x += survtsp->logProbGap(prev,h);   // Tests
    x += abxp->logProbGap(prev,h);      // Antibiotics
}
```

**Event-specific for discharge:** `src/modeling/models_UnitLinkedModel.cpp` line 347
```cpp
case discharge:
    break;  // No event-specific contribution
```

**Early return for start/stop:** `src/modeling/models_UnitLinkedModel.cpp` lines 273-277
```cpp
switch(h->getEvent()->getType()) {
    case start:
    case stop:
        return 0;  // Skip gap calculation entirely
}
```

## Correction to Original Documentation

The original statement "discharge events contribute 0" has been updated in `LIKELIHOOD_COMPUTATION_DOCUMENTATION.md` to clarify:

1. **Section 3.7** - Now titled "Non-Contributing Events (Event-Specific Likelihood Only)" with detailed explanation of gap vs event contributions
2. **Section 8.1** - Expanded to explain gap likelihood contributions for discharge events with formula and example

The key insight: **"Contribute 0" only refers to the event-specific likelihood, not the total contribution to the likelihood function.**
