#!/usr/bin/env Rscript
# Minimal test to isolate the -inf likelihood issue

library(bayestransmission)

cat("=== Minimal Test for -Inf Initial Likelihood ===\n\n")

# Use THE EXACT parameters from original C++ (from simulated.Model file)
params <- LinearAbxModel(
  nstates = 2,
  nmetro = 10,
  forward = FALSE,
  cheat = FALSE,
  Insitu = InsituParams(
    probs = c(0.9, 0.0, 0.1),
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
    latent = Param(init = 0.0, weight = 0),
    colonized = Param(init = 1.0, weight = 0)
  )
)

cat("Testing with simulated.data (unsorted)...\n")
set.seed(2)  # Same seed as original C++

results1 <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 0, nsims = 2, outputparam = TRUE, outputfinal = FALSE),
  modelParameters = params,
  verbose = TRUE
)

cat("\nInitial likelihood:", results1$LogLikelihood[1], "\n")
cat("Expected from C++: -12942.9\n")
cat("Match:", abs(results1$LogLikelihood[1] - (-12942.9)) < 1, "\n")
