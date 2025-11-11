#!/usr/bin/env Rscript
# Quick test: Does the vignette code produce -Inf?

library(bayestransmission)
data(simulated.data_sorted)

cat("Creating model params (from vignette)...\n")

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

cat("Running MCMC with verbose=TRUE to see initial likelihood...\n\n")

results <- runMCMC(
  data = simulated.data_sorted,
  MCMCParameters = list(
    nburn = 0,
    nsims = 3,
    outputparam = TRUE,
    outputfinal = FALSE
  ),
  params,  # Pass params directly, not as named argument
  verbose = TRUE
)

cat("\n\nFinal log likelihoods:\n")
print(results$LL)

if (any(is.infinite(results$LL))) {
  cat("\n*** BUG: -Inf likelihood detected! ***\n")
} else {
  cat("\n*** SUCCESS: All likelihoods are finite! ***\n")
  cat(sprintf("Range: [%.2f, %.2f]\n", min(results$LL), max(results$LL)))
  cat("Expected from original C++: ~-12000\n")
}
