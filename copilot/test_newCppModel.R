#!/usr/bin/env Rscript

# Test script for newCppModel function

library(bayestransmission)

cat("=== Testing newCppModel function ===\n\n")

# Test 1: Basic LogNormalModel
cat("Test 1: Creating LogNormalModel\n")
params1 <- LogNormalModelParams("LogNormalModel", nstates = 2)
tryCatch({
  model1 <- newCppModel(params1, verbose = TRUE)
  cat("✓ LogNormalModel created successfully\n")
  cat("  Model class:", class(model1), "\n")
  
  # Try to access properties
  tryCatch({
    insitu <- model1$InsituParams
    cat("  InsituParams accessible:", !is.null(insitu), "\n")
  }, error = function(e) {
    cat("  Warning: Could not access InsituParams:", e$message, "\n")
  })
  
}, error = function(e) {
  cat("✗ Failed to create LogNormalModel:", e$message, "\n")
})

cat("\n")

# Test 2: LinearAbxModel
cat("Test 2: Creating LinearAbxModel\n")
params2 <- LinearAbxModel(nstates = 2)
tryCatch({
  model2 <- newCppModel(params2, verbose = TRUE)
  cat("✓ LinearAbxModel created successfully\n")
  cat("  Model class:", class(model2), "\n")
  
  # Try to get parameters
  tryCatch({
    all_params <- getCppModelParams(model2)
    cat("  Parameters extracted:", length(all_params), "components\n")
    cat("  Components:", paste(names(all_params), collapse=", "), "\n")
  }, error = function(e) {
    cat("  Warning: Could not extract parameters:", e$message, "\n")
  })
  
}, error = function(e) {
  cat("✗ Failed to create LinearAbxModel:", e$message, "\n")
})

cat("\n")

# Test 3: LinearAbxModel2
cat("Test 3: Creating LinearAbxModel2\n")
params3 <- LogNormalModelParams("LinearAbxModel2", nstates = 2)
tryCatch({
  model3 <- newCppModel(params3, verbose = TRUE)
  cat("✓ LinearAbxModel2 created successfully\n")
  cat("  Model class:", class(model3), "\n")
}, error = function(e) {
  cat("✗ Failed to create LinearAbxModel2:", e$message, "\n")
})

cat("\n")

# Test 4: MixedModel
cat("Test 4: Creating MixedModel\n")
params4 <- LogNormalModelParams("MixedModel", nstates = 2)
tryCatch({
  model4 <- newCppModel(params4, verbose = TRUE)
  cat("✓ MixedModel created successfully\n")
  cat("  Model class:", class(model4), "\n")
}, error = function(e) {
  cat("✗ Failed to create MixedModel:", e$message, "\n")
})

cat("\n")

# Test 5: Error handling - invalid model name
cat("Test 5: Error handling - invalid model name\n")
params5 <- LogNormalModelParams("InvalidModel", nstates = 2)
tryCatch({
  model5 <- newCppModel(params5)
  cat("✗ Should have thrown an error\n")
}, error = function(e) {
  cat("✓ Correctly threw error:", e$message, "\n")
})

cat("\n=== Tests completed ===\n")
