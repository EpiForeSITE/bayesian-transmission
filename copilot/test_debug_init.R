#!/usr/bin/env Rscript
# Debug test to understand why we get -Inf

library(bayestransmission)

# Use exact parameters from original C++
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

cat("=== Analyzing Data ===\n")
data(simulated.data)

# Count patients with positive tests
patients_with_pos <- unique(simulated.data$patient[simulated.data$type == 2])  # 2 = possurvtest
cat("Number of patients with positive surveillance tests:", length(patients_with_pos), "\n")

total_patients <- length(unique(simulated.data$patient))
cat("Total patients:", total_patients, "\n")

# Show a few examples
cat("\nFirst few patients with positive tests:\n")
for(p in head(patients_with_pos, 3)) {
  events <- simulated.data[simulated.data$patient == p, ]
  cat(sprintf("  Patient %d: %d events, types: %s\n", 
              p, nrow(events), 
              paste(unique(CodeToEvent(events$type)), collapse=", ")))
}

cat("\n=== Running MCMC ===\n")
set.seed(2)

results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 0, nsims = 1, outputparam = FALSE, outputfinal = FALSE),
  modelParameters = params,
  verbose = TRUE
)

cat("\nInitial likelihood:", results$LogLikelihood[1], "\n")
cat("Expected: -12942.9\n")

if(is.finite(results$LogLikelihood[1])) {
  cat("✓ SUCCESS: Got finite initial likelihood!\n")
  cat("Difference from C++:", abs(results$LogLikelihood[1] - (-12942.9)), "\n")
} else {
  cat("✗ FAILURE: Still getting -Inf\n")
}
