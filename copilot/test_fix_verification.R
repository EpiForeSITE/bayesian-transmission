#!/usr/bin/env Rscript
# Test initial likelihood with fixed makeHistLink argument order

library(bayestransmission)
data("simulated.data", package = "bayestransmission")

cat("\n========================================\n")
cat("Testing Fixed makeHistLink Argument Order\n")
cat("========================================\n\n")

# Use the correct constructor from the tests
modelParameters <- LinearAbxModel(
  nstates = 2L,
  nmetro = 10L,
  forward = FALSE,
  cheat = FALSE,
  Insitu = InsituParams(
    probs = c(0.9, 0, 0.1),
    priors = c(1, 1, 1),
    doit = c(TRUE, FALSE, TRUE)
  ),
  SurveillanceTest = SurveillanceTestParams(
    uncolonized = Param(init = 0.0, weight = 0),
    latent = Param(init = 0.0, weight = 0),
    colonized = Param(init = 1.0, weight = 0)  # Fixed at 1.0 to match C++
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
    acquisition = Param(init = 0.0, weight = 0),  # Fixed at 0
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
  Abx = AbxParams(),
  AbxRate = AbxRateParams()
)

cat("Parameters configured to match original C++ test case.\n")
cat("Running MCMC with verbose=TRUE to see initial likelihood...\n\n")

set.seed(42)

results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(
    nburn = 3,
    nsims = 3,
    outputparam = TRUE,
    outputfinal = TRUE
  ),
  modelParameters = modelParameters,
  verbose = TRUE
)

cat("\n\n========================================\n")
cat("RESULTS SUMMARY\n")
cat("========================================\n\n")

cat("Post-MCMC Log Likelihoods:\n")
print(results$LogLikelihood)

finite_ll <- results$LogLikelihood[is.finite(results$LogLikelihood)]
cat("\nFinite likelihood values:", length(finite_ll), "out of", length(results$LogLikelihood), "\n")

if(length(finite_ll) > 0) {
    cat("\n✓ SUCCESS: Got finite likelihood values after MCMC!\n")
    cat("  First finite:", finite_ll[1], "\n")
    cat("  Final:", tail(finite_ll, 1), "\n")
    cat("  Mean:", mean(finite_ll), "\n")
    cat("  Range: [", min(finite_ll), ",", max(finite_ll), "]\n")
} else {
    cat("\n✗ FAILED: All likelihoods are still -Inf\n")
}

cat("\nWAIC values:\n")
cat("  WAIC1:", results$waic1, "\n")
cat("  WAIC2:", results$waic2, "\n")

cat("\n========================================\n")
cat("INTERPRETATION\n")
cat("========================================\n\n")
cat("Look for the 'LogLike=' value printed above in the output.\n")
cat("Original C++ showed: LogLike=-12942.9\n\n")
cat("If the initial LogLike is now finite (not -inf), the fix worked!\n")
cat("The makeHistLink argument order bug has been corrected.\n\n")
