#!/usr/bin/env Rscript
# Test the vignette parameters to ensure they produce valid likelihoods

library(bayestransmission)

cat("=== Testing Vignette Parameters ===\n\n")

# Set up parameters exactly as in the vignette
abx <- AbxParams(onoff = 0, delay = 0.0, life = 2.0)

abxrate <- AbxRateParams(
  uncolonized = Param(init = 1.0, weight = 0),
  colonized = Param(init = 1.0, weight = 0)
)

acquisition <- LinearAbxAcquisitionParams(
    base = Param(init = 0.001, weight = 1),
    time = Param(init = 1.0, weight = 0),
    mass = Param(init = 1.0, weight = 1),
    freq = Param(init = 1.0, weight = 1),
    col_abx = Param(init = 1.0, weight = 0),
    suss_abx = Param(init = 1.0, weight = 0),
    suss_ever = Param(init = 1.0, weight = 0)
)

progression <- ProgressionParams(
    rate = Param(init = 0.0, weight = 0),
    abx  = Param(init = 1.0, weight = 0),
    ever_abx = Param(init = 1.0, weight = 0)
)

clearance <- ClearanceParams(
    rate = Param(init = 0.01, weight = 1),
    abx  = Param(init = 1.0, weight = 0),
    ever_abx = Param(init = 1.0, weight = 0)
)

inunit <- ABXInUnitParams(
  acquisition = acquisition,
  progression = progression,
  clearance   = clearance
)

outcol <- OutOfUnitInfectionParams(
  acquisition = Param(init = 0.001, weight = 1),
  clearance = Param(init = 0.01, weight = 0),
  progression = Param(init = 0.0, weight = 0)
)

insitu <- InsituParams(
  probs = c(uncolonized = 0.90,
            latent = 0.0,
            colonized = 0.10),
  priors = c(1, 1, 1),
  doit = c(TRUE, FALSE, TRUE)
)

surv <- SurveillanceTestParams(
    uncolonized = Param(init = 0.0, weight = 0),
    latent = Param(init = 0.0, weight = 0),
    colonized = Param(init = 0.8, weight = 1)
)

clin <- RandomTestParams(
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
)

params <- LinearAbxModel(
  nstates = 2,
  nmetro = 10,
  forward = FALSE,
  cheat = FALSE,
  Insitu = insitu,
  SurveillanceTest = surv,
  ClinicalTest = clin,
  OutOfUnitInfection = outcol,
  InUnit = inunit,
  Abx = abx,
  AbxRate = abxrate
)

cat("Parameters created successfully\n\n")

# Test model initialization
cat("Testing model initialization...\n")
model <- newModelExport(params, verbose = TRUE)
cat("Model initialized successfully!\n\n")

# Run a short MCMC to verify it works
cat("Running short MCMC test...\n")
set.seed(42)
results <- runMCMC(
  data = simulated.data_sorted,
  MCMCParameters = list(
    nburn = 5,
    nsims = 10,
    outputparam = TRUE,
    outputfinal = TRUE
  ),
  modelParameters = params,
  verbose = TRUE
)

cat("\n=== Results Summary ===\n")
cat("Initial Log Likelihood:", results$LogLikelihood[1], "\n")
cat("Final Log Likelihood:", tail(results$LogLikelihood, 1), "\n")
cat("Mean Log Likelihood:", mean(results$LogLikelihood), "\n")
cat("WAIC1:", results$waic1, "\n")
cat("WAIC2:", results$waic2, "\n")

# Check for finite values
if(all(is.finite(results$LogLikelihood))) {
  cat("\n✓ All log likelihood values are finite\n")
} else {
  cat("\n✗ Warning: Some log likelihood values are not finite\n")
  cat("Non-finite indices:", which(!is.finite(results$LogLikelihood)), "\n")
}

cat("\n=== Test Complete ===\n")
