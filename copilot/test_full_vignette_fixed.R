#!/usr/bin/env Rscript
# Test the full vignette workflow with the fix

library(bayestransmission)
data(simulated.data_sorted)

cat("Testing FULL vignette workflow after fix\n")
cat("==========================================\n\n")

# Create params exactly as in vignette
insitu <- InsituParams(
  probs = c(uncolonized = 0.90, latent = 0.0, colonized = 0.10),
  priors = c(1, 1, 1),
  doit = c(TRUE, FALSE, TRUE)
)

surv <- SurveillanceTestParams(
  colonized = Param(init = 0.8, weight = 1),
  uncolonized = Param(init = 1e-10, weight = 0),
  latent = Param(init = 0.0, weight = 0)
)

params <- LinearAbxModel(
  nstates = 2,
  Insitu = insitu,
  SurveillanceTest = surv
)

cat("Running MCMC (10 burn-in, 20 samples)...\n\n")

results <- runMCMC(
  data = simulated.data_sorted,
  MCMCParameters = list(
    nburn = 10,
    nsims = 20,
    outputparam = TRUE,
    outputfinal = FALSE
  ),
  params,
  verbose = FALSE
)

cat("\nResults:\n")
cat("--------\n")
cat(sprintf("Samples: %d\n", length(results$LogLikelihood)))
cat(sprintf("LL range: [%.2f, %.2f]\n", min(results$LogLikelihood), max(results$LogLikelihood)))
cat(sprintf("Mean LL: %.2f\n", mean(results$LogLikelihood)))
cat(sprintf("Any -Inf? %s\n", any(is.infinite(results$LogLikelihood))))
cat(sprintf("WAIC: %.2f / %.2f\n", results$waic1, results$waic2))

cat("\n")
if (any(is.infinite(results$LogLikelihood))) {
  cat("✗ FAILED: Still has -Inf likelihoods\n")
} else {
  cat("✓ SUCCESS: All likelihoods are FINITE!\n")
  cat("\nComparison to original C++:\n")
  cat("  Original: ~-12000\n")
  cat(sprintf("  Current:  %.0f\n", mean(results$LogLikelihood)))
}
