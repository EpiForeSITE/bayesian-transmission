#!/usr/bin/env Rscript
# Extended test with more burn-in to demonstrate convergence

library(bayestransmission)

cat("=== Extended Vignette Parameter Test ===\n\n")

# Use the same parameters as the vignette
params <- LinearAbxModel(
  nstates = 2,
  nmetro = 10,
  forward = FALSE,
  cheat = FALSE,
  Insitu = InsituParams(
    probs = c(0.90, 0.0, 0.10),
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
      mass = Param(init = 0.9999, weight = 1),
      freq = Param(init = 0.9999, weight = 1),
      col_abx = Param(init = 1.0, weight = 0),
      suss_abx = Param(init = 1.0, weight = 0),
      suss_ever = Param(init = 1.0, weight = 0)
    ),
    progression = ProgressionParams(
      rate = Param(init = 0.0, weight = 0),
      abx  = Param(init = 1.0, weight = 0),
      ever_abx = Param(init = 1.0, weight = 0)
    ),
    clearance = ClearanceParams(
      rate = Param(init = 0.01, weight = 1),
      abx  = Param(init = 1.0, weight = 0),
      ever_abx = Param(init = 1.0, weight = 0)
    )
  ),
  Abx = AbxParams(onoff = 0, delay = 0.0, life = 2.0),
  AbxRate = AbxRateParams(
    uncolonized = Param(init = 1.0, weight = 0),
    colonized = Param(init = 1.0, weight = 0)
  )
)

cat("Running extended MCMC with more burn-in...\n\n")
set.seed(123)

results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(
    nburn = 50,
    nsims = 50,
    outputparam = TRUE,
    outputfinal = FALSE
  ),
  modelParameters = params,
  verbose = FALSE
)

cat("\n=== Results Summary ===\n")
finite_ll <- results$LogLikelihood[is.finite(results$LogLikelihood)]
n_finite <- length(finite_ll)
n_total <- length(results$LogLikelihood)

cat(sprintf("Finite log likelihood values: %d / %d (%.1f%%)\n", 
            n_finite, n_total, 100 * n_finite / n_total))

if(n_finite > 0) {
  cat("\nFinite Log Likelihood Statistics:\n")
  cat("  First finite value:", finite_ll[1], "\n")
  cat("  Last finite value:", tail(finite_ll, 1), "\n")
  cat("  Mean:", mean(finite_ll), "\n")
  cat("  Median:", median(finite_ll), "\n")
  cat("  Range: [", min(finite_ll), ",", max(finite_ll), "]\n")
  
  cat("\n✓ Model successfully found finite likelihood region\n")
} else {
  cat("\n✗ No finite likelihood values found - may need more burn-in\n")
}

cat("\nWAIC values:\n")
cat("  WAIC1:", results$waic1, "\n")
cat("  WAIC2:", results$waic2, "\n")

cat("\n=== Test Complete ===\n")
