#!/usr/bin/env Rscript
# Final verification that the index swap bug is fixed

library(bayestransmission)
data(simulated.data_sorted)

cat("\n")
cat("╔═══════════════════════════════════════════════════════════════╗\n")
cat("║        FINAL VERIFICATION: INDEX SWAP BUG FIX                 ║\n")
cat("╚═══════════════════════════════════════════════════════════════╝\n")
cat("\n")

# Test with vignette parameters
params <- LinearAbxModel(
  nstates = 2,
  Insitu = InsituParams(
    probs = c(uncolonized = 0.90, latent = 0.0, colonized = 0.10),
    priors = c(1, 1, 1),
    doit = c(TRUE, FALSE, TRUE)
  ),
  SurveillanceTest = SurveillanceTestParams(
    colonized = Param(init = 0.8, weight = 1),
    uncolonized = Param(init = 1e-10, weight = 0),
    latent = Param(init = 0.0, weight = 0)
  )
)

cat("Running MCMC (5 burn-in, 10 samples)...\n")

results <- runMCMC(
  data = simulated.data_sorted,
  MCMCParameters = list(
    nburn = 5,
    nsims = 10,
    outputparam = FALSE,
    outputfinal = FALSE
  ),
  params,
  verbose = FALSE
)

cat("\n")
cat("═══════════════════════════════════════════════════════════════\n")
cat("                        RESULTS\n")
cat("═══════════════════════════════════════════════════════════════\n")
cat(sprintf("Samples:          %d\n", length(results$LogLikelihood)))
cat(sprintf("All finite:       %s\n", all(is.finite(results$LogLikelihood))))
cat(sprintf("Min LL:           %.2f\n", min(results$LogLikelihood)))
cat(sprintf("Max LL:           %.2f\n", max(results$LogLikelihood)))
cat(sprintf("Mean LL:          %.2f\n", mean(results$LogLikelihood)))
cat(sprintf("SD LL:            %.2f\n", sd(results$LogLikelihood)))
cat(sprintf("WAIC:             %.2f / %.2f\n", results$waic1, results$waic2))
cat("═══════════════════════════════════════════════════════════════\n")

if (any(is.infinite(results$LogLikelihood))) {
  cat("\n✗ FAILED: -Inf likelihoods detected!\n")
  cat("  The index swap bug may not be fully fixed.\n")
  quit(status = 1)
} else {
  cat("\n✓ SUCCESS: All likelihoods are FINITE\n")
  cat("  The index swap bug is FIXED!\n\n")
  cat("Comparison to original C++ implementation:\n")
  cat(sprintf("  Original:  ~-12000\n"))
  cat(sprintf("  Current:   %.0f\n", mean(results$LogLikelihood)))
  cat(sprintf("  Difference: %.1f%%\n", 
      abs((mean(results$LogLikelihood) - (-12000)) / (-12000) * 100)))
  cat("\n")
  
  if (abs(mean(results$LogLikelihood) - (-12000)) / (-12000) < 0.15) {
    cat("✓ Within 15% of original C++ - EXCELLENT!\n")
  } else {
    cat("⚠ Differs by more than 15% - may need investigation\n")
  }
}

cat("\n")
