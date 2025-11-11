#!/usr/bin/env Rscript
# Comprehensive test to diagnose the initialization bug causing -Inf likelihood

library(bayestransmission)
data(simulated.data_sorted)

cat(paste(rep("=", 80), collapse=""), "\n")
cat("INITIALIZATION BUG DIAGNOSIS\n")
cat(paste(rep("=", 80), collapse=""), "\n\n")

# Step 1: Verify data has positive tests
cat("STEP 1: Verify data structure\n")
cat("-------------------------------\n")
pos_tests <- simulated.data_sorted[simulated.data_sorted$type == 2, ]
cat(sprintf("Total events: %d\n", nrow(simulated.data_sorted)))
cat(sprintf("Positive surveillance tests (type=2): %d\n", nrow(pos_tests)))
cat(sprintf("Unique patients with positive tests: %d\n", 
    length(unique(pos_tests$patient))))
cat("\n")

# Step 2: Create model with parameters that SHOULD work
cat("STEP 2: Create model with vignette parameters\n")
cat("------------------------------------------------\n")

insitu <- InsituParams(
  probs = c(uncolonized = 0.90, latent = 0.0, colonized = 0.10),
  priors = c(1, 1, 1),
  doit = c(TRUE, FALSE, TRUE)
)

# KEY: Using 1e-10 instead of 0.0 to avoid log(0)
surv <- SurveillanceTestParams(
  colonized = Param(init = 0.8, weight = 1),
  uncolonized = Param(init = 1e-10, weight = 0),
  latent = Param(init = 0.0, weight = 0)
)

params <- LinearAbxModel(
  data = simulated.data_sorted,
  insitu = insitu,
  surveillance.test = surv
)

cat("Model created successfully\n\n")

# Step 3: Get initial likelihood
cat("STEP 3: Check initial log likelihood\n")
cat("--------------------------------------\n")

ll <- params$model$logLikelihood()
cat(sprintf("Initial log likelihood: %.4f\n", ll))

if (is.infinite(ll) && ll < 0) {
  cat("\n*** BUG CONFIRMED: Initial likelihood is -Inf ***\n\n")
  
  cat("EXPECTED: Original C++ gave initial LL ~ -12000\n")
  cat("ACTUAL: R implementation gives -Inf\n\n")
  
  cat("DIAGNOSIS:\n")
  cat("1. The initialization should set all 171 patients with positive tests\n")
  cat("   as 'colonized' to avoid P(positive|uncolonized)=1e-10 giving -Inf\n")
  cat("2. The C++ code has this logic in ConstrainedSimulator::initEpisodeHistory()\n")
  cat("3. But it's either NOT being called or NOT working correctly\n\n")
  
  cat("NEXT STEPS:\n")
  cat("- Check if hist->positives() correctly identifies patients\n")
  cat("- Verify initEpisodeHistory() is being called for each episode\n")
  cat("- Debug why patients with positive tests aren't initialized as colonized\n")
  
} else {
  cat("\n*** SUCCESS: Initial likelihood is FINITE! ***\n\n")
  cat(sprintf("Original C++: ~-12000\n"))
  cat(sprintf("Current R:    %.2f\n", ll))
  
  diff_pct <- abs((ll - (-12000)) / (-12000) * 100)
  if (diff_pct < 5) {
    cat("\n✓ Within 5% of original C++ - EXCELLENT!\n")
  } else if (diff_pct < 20) {
    cat(sprintf("\n⚠ Differs by %.1f%% from original - needs investigation\n", diff_pct))
  } else {
    cat(sprintf("\n✗ Differs by %.1f%% from original - MAJOR DISCREPANCY\n", diff_pct))
  }
}

cat("\n")
cat(paste(rep("=", 80), collapse=""), "\n")
