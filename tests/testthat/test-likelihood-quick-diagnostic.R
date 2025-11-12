test_that("Quick diagnostic: Model initialization check", {
  # This test checks if we can even CREATE a model without errors
  
  params <- LinearAbxModel(
    nstates = 2,
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 0.0, weight = 0),
      latent = Param(init = 0.0, weight = 0),
      colonized = Param(init = 0.8, weight = 1)
    )
  )
  
  expect_true(is.list(params))
  expect_equal(params$modname, "LinearAbxModel")
  
  # Try to create the C++ model object
  model <- newCppModel(params, verbose = FALSE)
  expect_s4_class(model, "Rcpp_CppLinearAbxModel")
  
  cat("\n✓ Model created successfully\n")
})

test_that("Quick diagnostic: Check for P=0 in surveillance test", {
  # The issue is likely here: P(+|uncolonized) = 0.0
  # If ANY patient who is uncolonized gets a positive test, log(0) = -Inf
  
  params_zero <- LinearAbxModel(
    nstates = 2,
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 0.0, weight = 0),  # PROBLEM: Zero probability
      latent = Param(init = 0.0, weight = 0),
      colonized = Param(init = 0.8, weight = 1)
    )
  )
  
  params_small <- LinearAbxModel(
    nstates = 2,
    SurveillanceTest = SurveillanceTestParams(
      uncolonized = Param(init = 1e-10, weight = 0),  # FIX: Small but non-zero
      latent = Param(init = 0.0, weight = 0),
      colonized = Param(init = 0.8, weight = 1)
    )
  )
  
  # Both should create successfully
  model_zero <- newCppModel(params_zero, verbose = FALSE)
  model_small <- newCppModel(params_small, verbose = FALSE)
  
  expect_s4_class(model_zero, "Rcpp_CppLinearAbxModel")
  expect_s4_class(model_small, "Rcpp_CppLinearAbxModel")
  
  cat("\n=== Parameter Comparison ===\n")
  cat("With P=0.0: Model created\n")
  cat("With P=1e-10: Model created\n")
  cat("\nBoth models initialize, but P=0 will cause -Inf likelihood\n")
  cat("if any uncolonized patient tests positive.\n")
})

test_that("Quick diagnostic: Examine simulated.data_sorted for impossible events", {
  data <- simulated.data_sorted
  
  # Get all surveillance test events
  surv_pos <- data[data$type == 2, ]  # Positive surveillance tests
  surv_neg <- data[data$type == 1, ]  # Negative surveillance tests
  
  cat("\n=== Simulated Data Summary ===\n")
  cat("Total events:", nrow(data), "\n")
  cat("Surveillance tests (positive):", nrow(surv_pos), "\n")
  cat("Surveillance tests (negative):", nrow(surv_neg), "\n")
  
  # The key question: When P(+|uncolonized) = 0.0, 
  # can we have positive tests from uncolonized patients?
  
  # We can't know colonization status from data alone,
  # but if initial assignment is random and we have positive tests,
  # some will be assigned to uncolonized patients → -Inf
  
  cat("\nWith P(+|uncolonized) = 0.0:\n")
  cat("- If episode initialization assigns any patient with positive test as uncolonized\n")
  cat("- Then likelihood of that test = 0 → log(0) = -Inf\n")
  cat("\nSOLUTION: Use P(+|uncolonized) >= 1e-10, never exactly 0\n")
  
  expect_gt(nrow(surv_pos), 0, label = "Should have positive surveillance tests in data")
})
