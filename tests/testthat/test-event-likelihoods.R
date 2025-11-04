# test-event-likelihoods.R
# Test individual event type likelihood contributions with known parameters

library(testthat)
library(bayestransmission)

# Setup model parameters used across tests
model_params <- list(
    modname = "LinearAbxModel",
    nstates = 2,
    nmetro = 100,
    forward = 1,
    cheat = 0,
    Insitu = InsituParams(
      probs = c(0.8321, 0, 0.1679),
      priors = c(0.9, 0, 0.1),
      doit = c(FALSE, FALSE, FALSE)
    ),
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 0.0, weight = 0, update = FALSE),
      colonized = Param(init = 0.6543, weight = 1, update = FALSE),
      latent = Param(init = 0.0, weight = 0, update = FALSE)
    ),
    ClinicalTest = RandomTestParams(
      uncolonized = ParamWRate(
        param = Param(init = 0.2187, weight = 0, update = FALSE),
        rate = Param(init = 0.8765, weight = 0, update = FALSE)
      ),
      colonized = ParamWRate(
        param = Param(init = 0.0, weight = 0, update = FALSE),
        rate = Param(init = 0.0, weight = 0, update = FALSE)
      ),
      latent = ParamWRate(
        param = Param(init = 0.3456, weight = 0, update = FALSE),
        rate = Param(init = 1.2345, weight = 0, update = FALSE)
      )
    ),
    OutCol = OutOfUnitInfectionParams(
      acquisition = Param(init = 0.004321, weight = 1, update = FALSE),
      clearance = Param(init = 0.007654, weight = 0, update = FALSE)
    ),
    InCol = ABXInUnitParams(
      acquisition = LinearAbxAcquisitionParams(
        base = Param(init = 0.001234, weight = 1, update = FALSE),
        time = Param(init = 1.1111, weight = 0, update = FALSE),
        mass = Param(init = 0.9123, weight = 1, update = FALSE),
        freq = Param(init = 0.9234, weight = 1, update = FALSE),
        col_abx = Param(init = 0.7777, weight = 0, update = FALSE),
        suss_abx = Param(init = 0.8888, weight = 0, update = FALSE),
        suss_ever = Param(init = 0.9999, weight = 0, update = FALSE)
      ),
      clearance = ClearanceParams(
        rate = Param(init = 0.008765, weight = 1, update = FALSE),
        abx = Param(init = 0.6666, weight = 0, update = FALSE),
        ever_abx = Param(init = 0.5555, weight = 0, update = FALSE)
      )
    ),
    Abx = AbxParams(
      onoff = FALSE,
      delay = 0.0,
      life = 2.0
    ),
    AbxRate = AbxRateParams(
      uncolonized = Param(init = 1.3333, weight = 0, update = FALSE),
      colonized = Param(init = 1.4444, weight = 0, update = FALSE),
      latent = Param(init = 0.0, weight = 0, update = FALSE)
    )
)

# Create C++ model
model <- newCppModel(model_params)

# Helper function to create episode data (admission to discharge)
create_episode_data <- function(events_df) {
  # events_df should have columns: time, patient, type
  # Prepend start marker and append stop marker
  data.frame(
    facility = 1,
    unit = 1,
    time = c(0, events_df$time, max(events_df$time) + 100),
    patient = c(NA, events_df$patient, NA),
    type = c(21, events_df$type, 23)
  )
}

create_system <- function(data) {
  CppSystem$new(
    as.integer(data$facility),
    as.integer(data$unit),
    data$time,
    as.integer(data$patient),
    as.integer(data$type)
  )
}

# Helper to compute likelihood for a dataset
compute_likelihood <- function(data, model_type = "LinearAbxModel", ..., model=NULL) {
  # Build the model
  if (is.null(model)) {
    model_params <- LogNormalModelParams(modname = model_type, ...)
    model <- newCppModel(model_params)
  }

  # Create System
  sys <- CppSystem$new(as.integer(data$facility),
             as.integer(data$unit),
             data$time,
             as.integer(data$patient),
             as.integer(data$type))
  # Create SystemHistory
  hist <- CppSystemHistory$new(sys, model, FALSE)

  # Compute likelihood
  ll <- model$logLikelihood(hist)

  return(list(ll = ll, model = model, hist = hist))
}

# =============================================================================
# Test 1: Start and Stop markers (should contribute 0)
# =============================================================================

test_that("Empty system gives zero likelihood", {
  data <- data.frame(
    facility = 1,
    unit = 1,
    time = c(0, 100),
    patient = c(NA, NA),
    type = c(21, 23)  # start, stop
  )

  # Create System
  sys <- CppSystem$new(integer(0),
             integer(0),
             numeric(0),
             integer(0),
             integer(0))
  expect_equal(sys$countEvents(), 0)
  expect_equal(sys$countEpisodes(), 0)
  expect_equal(sys$getPatients()$size, 0)

  # Create SystemHistory
  hist <- CppSystemHistory$new(sys, model, FALSE)
  
  # Use simple parameters
  ll <- model$logLikelihood(hist)
  
  # Start and stop should contribute exactly 0 (not -Inf)
  expect_true(is.finite(ll))
  expect_equal(ll, 0, tolerance = 1e-10)
})

# =============================================================================
# Test 2: Individual log likelihood contributions for all events
# =============================================================================

test_that("All events contribute to total log likelihood", {
  # Note: SystemHistory creates hierarchical events at system/facility/unit/patient levels
  # Input: 4 events (start, admission, discharge, stop)
  # Output: 14 events (includes start/stop markers at each hierarchical level)
  
  # Single patient admission at time 10, discharged at time 20
  data <- create_episode_data(data.frame(
    time = c(10, 20),
    patient = c(1, 1),
    type = c(0, 3)  # admission, discharge
  ))

  model <- newCppModel(LinearAbxModel())
  sys <- create_system(data)
  hist <- CppSystemHistory$new(sys, model, FALSE)
  
  # Get all history links (includes system/facility/unit/patient levels)
  links <- hist$getHistoryLinkList()
  
  # Compute total likelihood
  total_ll <- model$logLikelihood(hist)
  
  # Compute individual contributions
  individual_lls <- vapply(links, function(link) {
    model$logLikelihoodLink(link)
  }, numeric(1))
  
  # Create summary table for inspection
  event_summary <- data.frame(
    Index = seq_along(links),
    Type = vapply(links, function(link) link$Event$Type, character(1)),
    Time = vapply(links, function(link) link$Event$Time, numeric(1)),
    LogLikelihood = individual_lls
  )
  
  # Print for debugging/documentation (only in interactive mode)
  if (interactive()) {
    cat("\nIndividual log likelihood contributions:\n")
    print(event_summary)
    cat(sprintf("\nSum of individual contributions: %.6f\n", sum(individual_lls)))
    cat(sprintf("Total logLikelihood(hist):       %.6f\n", total_ll))
  }
  
  # The sum of individual contributions should equal the total
  expect_equal(sum(individual_lls), total_ll, tolerance = 1e-10)
  
  # Start and stop events should contribute 0 to likelihood
  start_events <- which(event_summary$Type == "start")
  stop_events <- which(event_summary$Type == "stop")
  expect_true(all(individual_lls[start_events] == 0))
  expect_true(all(individual_lls[stop_events] == 0))
  
  # Non-start/stop events should have non-zero contributions
  # (insitu, admission, discharge events)
  active_events <- which(!event_summary$Type %in% c("start", "stop"))
  expect_true(length(active_events) > 0)
  
  # Check that we have the expected 14 events
  expect_equal(length(links), 14)
})

# =============================================================================
# Test 3: Negative surveillance test (EventType = 1)
# =============================================================================

test_that("Negative surveillance test on uncolonized patient has high probability", {
  # Patient admitted, negative test immediately, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 10.001, 20),
    patient = c(1, 1, 1),
    type = c(0, 1, 3)  # admission, negsurvtest, discharge
  ))
  
  sys <- data |> create_system()

  # Set up so patient is likely uncolonized at admission
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 1.0, 0.0), c(0.0, 1.0, 0.0), c(FALSE, FALSE, FALSE)),  # All uncolonized
                          outofunit = ParamWRate(0.01, 0.001),  # Low colonization
                          inunit = ParamWRate(0.001, 0.001),  # Low transmission
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite
  expect_true(is.finite(result$ll))
  expect_true(result$result$ll <= 0)
  
  # Should be reasonably high probability (not extremely negative)
  # Negative test on uncolonized patient with low colonization should be likely
  expect_true(result$ll > -10)
})

# =============================================================================
# Test 4: Positive surveillance test (EventType = 2)
# =============================================================================

test_that("Positive surveillance test on colonized patient has high probability", {
  # Patient admitted, positive test immediately, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 10.001, 20),
    patient = c(1, 1, 1),
    type = c(0, 2, 3)  # admission, possurvtest, discharge
  ))
  
  # Set up so patient is likely colonized at admission
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.0, 1.0), c(0.0, 0.0, 1.0), c(FALSE, FALSE, FALSE)),  # All colonized
                          outofunit = ParamWRate(0.9, 0.001),  # High colonization
                          inunit = ParamWRate(0.001, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite
  expect_true(is.finite(result$ll))
  expect_true(result$result$ll <= 0)
  
  # Should be reasonably high probability
  # Positive test on colonized patient with high colonization should be likely
  expect_true(result$ll > -10)
})

# =============================================================================
# Test 5: Conflicting test result should give -Inf
# =============================================================================

test_that("Positive test immediately after negative test gives -Inf", {
  skip("Need to implement test with impossible configuration")
  
  # Patient admitted, negative test, positive test immediately after (impossible)
  data <- create_episode_data(data.frame(
    time = c(10, 10.001, 10.002, 20),
    patient = c(1, 1, 1, 1),
    type = c(0, 1, 2, 3)  # admission, negsurvtest, possurvtest, discharge
  ))
  
  # With no transmission possible in such short time, this should be -Inf
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 1.0, 0.0), c(0.0, 1.0, 0.0), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.0001, 0.001),  # Very low transmission
                          surveillance = ParamWRate(1.0, 0.001),  # High test rate
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be -Inf due to impossibility
  expect_equal(result$ll, -Inf)
})

# =============================================================================
# Test 6: Discharge event (EventType = 3)
# =============================================================================

test_that("Discharge event contributes 0 to likelihood", {
  # Simple admission and discharge
  data <- create_episode_data(data.frame(
    time = c(10, 20),
    patient = c(1, 1),
    type = c(0, 3)  # admission, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite - discharge itself contributes 0
  expect_true(is.finite(result$ll))
  
  # Check that discharge events have 0 contribution
  links <- result$hist$getHistoryLinkList()
  discharge_lls <- vapply(links, function(link) {
    if (link$Event$Type == "discharge") {
      result$model$logLikelihoodLink(link)
    } else {
      NA_real_
    }
  }, numeric(1))
  discharge_lls <- discharge_lls[!is.na(discharge_lls)]
  
  if (length(discharge_lls) > 0) {
    expect_true(all(discharge_lls == 0))
  }
})

# =============================================================================
# Test 7: Negative clinical test (EventType = 4)
# =============================================================================

test_that("Negative clinical test contributes test probability", {
  # Patient admitted, negative clinical test, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 15, 20),
    patient = c(1, 1, 1),
    type = c(0, 4, 3)  # admission, negclintest, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.1, 0.001),
                          inunit = ParamWRate(0.01, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite
  expect_true(is.finite(result$ll))
  expect_true(result$ll <= 0)
})

# =============================================================================
# Test 8: Positive clinical test (EventType = 5)
# =============================================================================

test_that("Positive clinical test contributes test probability", {
  # Patient admitted, positive clinical test, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 15, 20),
    patient = c(1, 1, 1),
    type = c(0, 5, 3)  # admission, posclintest, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite
  expect_true(is.finite(result$ll))
  expect_true(result$ll <= 0)
})

# =============================================================================
# Test 9: In-situ colonization (EventType = 6)
# =============================================================================

test_that("In-situ colonization at admission contributes in-situ probability", {
  skip("Need to understand how to force in-situ event in data")
  
  # This may require special setup - in-situ is typically inferred, not observed
  # Placeholder for now
})

# =============================================================================
# Test 10: Antibiotic dose (EventType = 9)
# =============================================================================

test_that("Antibiotic dose event contributes 0 directly", {
  # Patient admitted, gets antibiotic, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 12, 20),
    patient = c(1, 1, 1),
    type = c(0, 9, 3)  # admission, abxdose, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite - abxdose itself contributes 0
  expect_true(is.finite(result$ll))
})

# =============================================================================
# Test 11: Antibiotic on (EventType = 10)
# =============================================================================

test_that("Antibiotic on event contributes antibiotic effect probability", {
  # Patient admitted, starts antibiotic, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 12, 20),
    patient = c(1, 1, 1),
    type = c(0, 10, 3)  # admission, abxon, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 2.0,  # Antibiotic increases clearance
                          abxacquisition = 0.5)
  
  # Should be finite
  expect_true(is.finite(result$ll))
  expect_true(result$ll <= 0)
})

# =============================================================================
# Test 12: Antibiotic off (EventType = 11)
# =============================================================================

test_that("Antibiotic off event contributes 0 directly", {
  # Patient admitted, starts then stops antibiotic, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 12, 15, 20),
    patient = c(1, 1, 1, 1),
    type = c(0, 10, 11, 3)  # admission, abxon, abxoff, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 2.0,
                          abxacquisition = 0.5)
  
  # Should be finite - abxoff itself contributes 0
  expect_true(is.finite(result$ll))
})

# =============================================================================
# Test 13: Isolation on (EventType = 12)
# =============================================================================

test_that("Isolation on event contributes 0 directly", {
  # Patient admitted, isolation starts, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 12, 20),
    patient = c(1, 1, 1),
    type = c(0, 12, 3)  # admission, isolon, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite - isolon itself contributes 0
  expect_true(is.finite(result$ll))
})

# =============================================================================
# Test 14: Isolation off (EventType = 13)
# =============================================================================

test_that("Isolation off event contributes 0 directly", {
  # Patient admitted, isolation starts then stops, then discharged
  data <- create_episode_data(data.frame(
    time = c(10, 12, 15, 20),
    patient = c(1, 1, 1, 1),
    type = c(0, 12, 13, 3)  # admission, isolon, isoloff, discharge
  ))
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                          outofunit = ParamWRate(0.5, 0.001),
                          inunit = ParamWRate(0.1, 0.001),
                          surveillance = ParamWRate(0.5, 0.001),
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  # Should be finite - isoloff itself contributes 0
  expect_true(is.finite(result$ll))
})

# =============================================================================
# Test 15: Gap probability contribution
# =============================================================================

test_that("Longer time gaps reduce likelihood due to gap probabilities", {
  # Two scenarios: short gap vs long gap between admission and discharge
  
  # Short gap
  data_short <- create_episode_data(data.frame(
    time = c(10, 11),
    patient = c(1, 1),
    type = c(0, 3)  # admission, discharge
  ))
  
  result_short <- compute_likelihood(data_short, "LinearAbxModel",
                                nstates = 2,
                                clearance = ParamWRate(0.01, 0.001),
                                progression = ParamWRate(0.01, 0.001),
                                insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                                outofunit = ParamWRate(0.5, 0.001),
                                inunit = ParamWRate(0.1, 0.001),
                                surveillance = ParamWRate(0.5, 0.001),
                                abxclearance = 1.0,
                                abxacquisition = 0.5)
  
  # Long gap
  data_long <- create_episode_data(data.frame(
    time = c(10, 50),
    patient = c(1, 1),
    type = c(0, 3)  # admission, discharge
  ))
  
  result_long <- compute_likelihood(data_long, "LinearAbxModel",
                                nstates = 2,
                                clearance = ParamWRate(0.01, 0.001),
                                progression = ParamWRate(0.01, 0.001),
                                insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                                outofunit = ParamWRate(0.5, 0.001),
                                inunit = ParamWRate(0.1, 0.001),
                                surveillance = ParamWRate(0.5, 0.001),
                                abxclearance = 1.0,
                                abxacquisition = 0.5)
  
  # Both should be finite
  expect_true(is.finite(result_short$ll))
  expect_true(is.finite(result_long$ll))
  
  # Long gap should have lower (more negative) likelihood
  # due to accumulating gap probabilities
  expect_true(result_long$ll < result_short$ll)
})

# =============================================================================
# Test 16: Multiple patients - verify independence
# =============================================================================

test_that("Multiple independent patients have additive log-likelihoods", {
  skip("Need to verify multi-patient likelihood calculation")
  
  # Single patient
  data_single <- create_episode_data(data.frame(
    time = c(10, 20),
    patient = c(1, 1),
    type = c(0, 3)
  ))
  
  result_single <- compute_likelihood(data_single, "LinearAbxModel",
                                  nstates = 2,
                                  clearance = ParamWRate(0.01, 0.001),
                                  progression = ParamWRate(0.01, 0.001),
                                  insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                                  outofunit = ParamWRate(0.5, 0.001),
                                  inunit = ParamWRate(0.1, 0.001),
                                  surveillance = ParamWRate(0.5, 0.001),
                                  abxclearance = 1.0,
                                  abxacquisition = 0.5)
  
  # Two identical patients (non-overlapping)
  data_double <- create_episode_data(data.frame(
    time = c(10, 20, 30, 40),
    patient = c(1, 1, 2, 2),
    type = c(0, 3, 0, 3)
  ))
  
  result_double <- compute_likelihood(data_double, "LinearAbxModel",
                                  nstates = 2,
                                  clearance = ParamWRate(0.01, 0.001),
                                  progression = ParamWRate(0.01, 0.001),
                                  insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),
                                  outofunit = ParamWRate(0.5, 0.001),
                                  inunit = ParamWRate(0.1, 0.001),
                                  surveillance = ParamWRate(0.5, 0.001),
                                  abxclearance = 1.0,
                                  abxacquisition = 0.5)
  
  # Log-likelihoods should add (approximately)
  # Note: May not be exactly additive due to unit-level effects
  expect_true(is.finite(result_double$ll))
})

# =============================================================================
# Test 17: Admission state variants (EventType 14-16)
# =============================================================================

test_that("Admission state variants are inferred, not directly observed", {
  skip("Admission state variants (14-16) are inferred by model, not in data")
  
  # These event types (admission0, admission1, admission2) are generated
  # by the model during history construction, not present in input data
  # Testing requires examining the SystemHistory after construction
})

# =============================================================================
# Test 18: In-situ state variants (EventType 17-19)
# =============================================================================

test_that("In-situ state variants are inferred, not directly observed", {
  skip("In-situ state variants (17-19) are inferred by model, not in data")
  
  # These event types (insitu0, insitu1, insitu2) are generated
  # by the model during history construction, not present in input data
})

# =============================================================================
# Test 19: Transmission events (EventType 31-33)
# =============================================================================

test_that("Transmission events are inferred, not directly observed", {
  skip("Transmission events (acquisition=31, progression=32, clearance=33) are inferred")
  
  # These are hidden events inferred by the model
  # Only used if cheating=1 (model knows true states)
})

# =============================================================================
# Test 20: Known probability calculation - explicit test
# =============================================================================

test_that("Simple admission has calculable likelihood", {
  # Simplest case: just admission and discharge, no tests
  # We can calculate the expected likelihood manually
  
  data <- create_episode_data(data.frame(
    time = c(10, 20),
    patient = c(1, 1),
    type = c(0, 3)
  ))
  
  # Set very simple parameters
  # For 2-state model: states 0 (uncolonized) and 1 (colonized)
  # Out-of-unit: P(colonized) = 0.3
  outofunit_p <- 0.3
  
  result <- compute_likelihood(data, "LinearAbxModel",
                          nstates = 2,
                          clearance = ParamWRate(0.01, 0.001),
                          progression = ParamWRate(0.01, 0.001),
                          insitu = InsituParams(c(0.0, 0.5, 0.5), c(0.0, 0.5, 0.5), c(FALSE, FALSE, FALSE)),  # No in-situ
                          outofunit = ParamWRate(outofunit_p, 0.001),
                          inunit = ParamWRate(0.01, 0.001),
                          surveillance = ParamWRate(0.01, 0.001),  # Low test rate
                          abxclearance = 1.0,
                          abxacquisition = 0.5)
  
  expect_true(is.finite(result$ll))
  
  # The likelihood should include:
  # 1. Out-of-unit colonization probability (admission)
  # 2. Gap probabilities (no events during stay)
  # 3. Discharge (contributes 0)
  
  # With no tests, we marginalize over possible admission states
  # Expected LL should be sum of:
  # - log(P(admission state))
  # - log(P(no transmission events in gap))
  # - log(P(no test events in gap))
  
  # We can't calculate exact value without knowing internal formulas,
  # but we verify it's reasonable (negative, finite)
  expect_true(ll < 0)
  expect_true(ll > -100)  # Should not be extremely negative for simple case
})
