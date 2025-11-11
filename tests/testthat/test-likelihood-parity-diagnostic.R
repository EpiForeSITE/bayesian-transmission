test_that("Diagnostic: Track where -Inf likelihood originates", {
  skip_on_cran()
  
  # Use exact parameters from original C++ that should give LL ~ -12943
  params <- LinearAbxModel(
    nstates = 2,
    nmetro = 10,
    forward = FALSE,
    cheat = FALSE,
    Insitu = InsituParams(
      probs = c(uncolonized = 0.9, latent = 0.0, colonized = 0.1),
      priors = c(1, 1, 1),
      doit = c(TRUE, FALSE, TRUE)
    ),
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 0.0, weight = 0),
      latent = Param(init = 0.0, weight = 0),
      colonized = Param(init = 0.8, weight = 1)
    ),
    ClinicalTest = RandomTestParams(
      uncolonized = ParamWRate(
        param = Param(init = 0.5, weight = 0),
        rate = Param(init = 1.0, weight = 0)
      ),
      latent = ParamWRate(
        param = Param(init = 0.5, weight = 0),
        rate = Param(init = 1.0, weight = 0)
      ),
      colonized = ParamWRate(
        param = Param(init = 0.5, weight = 0),
        rate = Param(init = 1.0, weight = 0)
      )
    ),
    OutOfUnitInfection = OutOfUnitInfectionParams(
      acquisition = Param(init = 0.001, weight = 1),
      clearance = Param(init = 0.01, weight = 0),
      progression = Param(init = 0.0, weight = 0)
    ),
    InUnit = ABXInUnitParams(
      acquisition = LinearAbxAcquisitionParams(
        base = Param(init = 0.001, weight = 1),
        time = Param(init = 1.0, weight = 0),
        mass = Param(init = 1.0, weight = 1),
        freq = Param(init = 1.0, weight = 1),
        col_abx = Param(init = 1.0, weight = 0),
        suss_abx = Param(init = 1.0, weight = 0),
        suss_ever = Param(init = 1.0, weight = 0)
      ),
      clearance = ClearanceParams(
        rate = Param(init = 0.01, weight = 1),
        abx = Param(init = 1.0, weight = 0),
        ever_abx = Param(init = 1.0, weight = 0)
      ),
      progression = ProgressionParams(
        rate = Param(init = 0.0, weight = 0),
        abx = Param(init = 1.0, weight = 0),
        ever_abx = Param(init = 1.0, weight = 0)
      )
    ),
    Abx = AbxParams(onoff = 0, delay = 0.0, life = 2.0),
    AbxRate = AbxRateParams(
      uncolonized = Param(init = 1.0, weight = 0),
      latent = Param(init = 1.0, weight = 0),
      colonized = Param(init = 1.0, weight = 0)
    )
  )
  
  # Test 1: Verify model creation doesn't produce -Inf
  model <- newModelExport(params, verbose = FALSE)
  expect_s4_class(model, "Rcpp_CppModel")
  
  # Test 2: Run minimal MCMC (0 burn-in, 1 iteration) to get initial likelihood
  set.seed(42)
  results <- runMCMC(
    data = simulated.data_sorted,
    MCMCParameters = list(
      nburn = 0,
      nsims = 1,
      outputparam = TRUE,
      outputfinal = TRUE
    ),
    modelParameters = params,
    verbose = TRUE  # Keep verbose to see diagnostic output
  )
  
  # Test 3: Check the initial likelihood
  initial_ll <- results$LogLikelihood[1]
  
  cat("\n=== DIAGNOSTIC RESULTS ===\n")
  cat("Initial Log Likelihood:", initial_ll, "\n")
  cat("Expected (from C++):", -12942.9, "\n")
  cat("Difference:", abs(initial_ll - (-12942.9)), "\n")
  cat("Is -Inf?", is.infinite(initial_ll), "\n")
  cat("Is finite?", is.finite(initial_ll), "\n")
  
  # Test 4: The actual expectation - should be finite and close to C++ value
  expect_finite(initial_ll, 
                info = "Initial log likelihood should be finite, not -Inf")
  
  # Allow some tolerance for floating point differences
  expect_equal(initial_ll, -12942.9, tolerance = 10,
               info = "R likelihood should match C++ likelihood within tolerance")
})

test_that("Diagnostic: Identify which parameter causes -Inf", {
  skip_on_cran()
  
  # Test with progressively more constrained parameters to isolate the issue
  
  # Hypothesis 1: SurveillanceTest with P(+|uncolonized) = 0.0 causes -Inf
  cat("\n--- Testing Hypothesis 1: Surveillance Test P=0 ---\n")
  
  params_h1 <- LinearAbxModel(
    nstates = 2,
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 1e-10, weight = 0),  # Small but non-zero
      latent = Param(init = 0.0, weight = 0),
      colonized = Param(init = 0.8, weight = 1)
    )
  )
  
  results_h1 <- runMCMC(
    data = simulated.data_sorted,
    MCMCParameters = list(nburn = 0, nsims = 1, outputparam = TRUE, outputfinal = TRUE),
    modelParameters = params_h1,
    verbose = FALSE
  )
  
  cat("With P(+|uncolonized) = 1e-10: LL =", results_h1$LogLikelihood[1], "\n")
  cat("Is finite?", is.finite(results_h1$LogLikelihood[1]), "\n")
  
  # Hypothesis 2: Episode initialization issue
  cat("\n--- Testing Hypothesis 2: Episode Initialization ---\n")
  
  # Try with different initial probabilities
  params_h2 <- LinearAbxModel(
    nstates = 2,
    Insitu = InsituParams(
      probs = c(uncolonized = 0.5, latent = 0.0, colonized = 0.5),  # 50/50 split
      priors = c(1, 1, 1),
      doit = c(TRUE, FALSE, TRUE)
    )
  )
  
  results_h2 <- runMCMC(
    data = simulated.data_sorted,
    MCMCParameters = list(nburn = 0, nsims = 1, outputparam = TRUE, outputfinal = TRUE),
    modelParameters = params_h2,
    verbose = FALSE
  )
  
  cat("With 50/50 initial split: LL =", results_h2$LogLikelihood[1], "\n")
  cat("Is finite?", is.finite(results_h2$LogLikelihood[1]), "\n")
  
  # Hypothesis 3: Acquisition rate issue
  cat("\n--- Testing Hypothesis 3: Acquisition Rate ---\n")
  
  params_h3 <- LinearAbxModel(
    nstates = 2,
    InUnit = ABXInUnitParams(
      acquisition = LinearAbxAcquisitionParams(
        base = Param(init = 0.01, weight = 1),  # Higher base rate
        time = Param(init = 1.0, weight = 0),
        mass = Param(init = 1.0, weight = 1),
        freq = Param(init = 1.0, weight = 1),
        col_abx = Param(init = 1.0, weight = 0),
        suss_abx = Param(init = 1.0, weight = 0),
        suss_ever = Param(init = 1.0, weight = 0)
      )
    )
  )
  
  results_h3 <- runMCMC(
    data = simulated.data_sorted,
    MCMCParameters = list(nburn = 0, nsims = 1, outputparam = TRUE, outputfinal = TRUE),
    modelParameters = params_h3,
    verbose = FALSE
  )
  
  cat("With base = 0.01: LL =", results_h3$LogLikelihood[1], "\n")
  cat("Is finite?", is.finite(results_h3$LogLikelihood[1]), "\n")
  
  # Summary
  cat("\n=== HYPOTHESIS TEST SUMMARY ===\n")
  results_summary <- data.frame(
    Hypothesis = c("Original", "P=1e-10", "50/50 init", "base=0.01"),
    LogLikelihood = c(
      NA,  # Will fill from first test
      results_h1$LogLikelihood[1],
      results_h2$LogLikelihood[1],
      results_h3$LogLikelihood[1]
    ),
    IsFinite = c(
      NA,
      is.finite(results_h1$LogLikelihood[1]),
      is.finite(results_h2$LogLikelihood[1]),
      is.finite(results_h3$LogLikelihood[1])
    )
  )
  
  print(results_summary)
  
  # At least ONE configuration should produce finite likelihood
  expect_true(
    any(results_summary$IsFinite, na.rm = TRUE),
    info = "At least one parameter configuration should produce finite likelihood"
  )
})

test_that("Diagnostic: Check data for impossible events", {
  skip_on_cran()
  
  # Check if simulated.data_sorted has any events that could cause -Inf
  # For example: positive test result when surveillance test probability is 0
  
  data <- simulated.data_sorted
  
  cat("\n=== DATA DIAGNOSTICS ===\n")
  cat("Total events:", nrow(data), "\n")
  cat("Event types:\n")
  print(table(CodeToEvent(data$type)))
  
  # Check for surveillance test results
  surv_tests <- data[data$type %in% c(1, 2), ]  # Negative=1, Positive=2
  cat("\nSurveillance tests:", nrow(surv_tests), "\n")
  if (nrow(surv_tests) > 0) {
    cat("  Negative:", sum(surv_tests$type == 1), "\n")
    cat("  Positive:", sum(surv_tests$type == 2), "\n")
  }
  
  # Check for clinical test results
  clin_tests <- data[data$type %in% c(4, 5), ]  # Negative=4, Positive=5
  cat("\nClinical tests:", nrow(clin_tests), "\n")
  if (nrow(clin_tests) > 0) {
    cat("  Negative:", sum(clin_tests$type == 4), "\n")
    cat("  Positive:", sum(clin_tests$type == 5), "\n")
  }
  
  # CRITICAL: If we have P(+|uncolonized) = 0.0 and ANY positive surveillance tests,
  # those tests MUST occur when patient is colonized, or we get -Inf
  
  # This test just documents the data - actual fix will come from analysis
  expect_true(TRUE, info = "Data diagnostics completed")
})
