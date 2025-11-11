#!/usr/bin/env Rscript
# Test with EXACT original C++ parameters from inst/original_cpp/simulated.Model

library(bayestransmission)
data(simulated.data_sorted)

cat("Creating model with EXACT original C++ parameters...\n")

# From simulated.Model:
# prob_insitu_uncolonized = 0.9 (weight=1)
# prob_insitu_colonized = 0.1 (weight=1)
insitu <- InsituParams(
  uncolonized = Param(init = 0.9, weight = 1),
  colonized = Param(init = 0.1, weight = 1)
)

# prob_pos_surv_test_when_uncol_offabx = 0 (weight=0 - FIXED)
# prob_pos_surv_test_when_col_offabx = 0.8 (weight=1 - ESTIMATED)
surv <- SurveillanceTestParams(
  uncolonized = Param(init = 0.0, weight = 0),  # EXACT 0.0 like original!
  colonized = Param(init = 0.8, weight = 1)
)

# Clinical test params
clin <- ClinicalTestParams(
  uncolonized = Param(init = 0.5, weight = 0),
  colonized = Param(init = 0.5, weight = 0)
)

# Out of unit acquisition
out <- OutOfUnitInfectionParams(
  acquisition = Param(init = 0.001, weight = 1),
  clearance = Param(init = 0.01, weight = 0)
)

# Acquisition parameters
acq <- AcquisitionParams(
  base = Param(init = 0.001, weight = 1),
  time = Param(init = 1.0, weight = 0),
  dens = Param(init = 1.0, weight = 1),
  freq = Param(init = 1.0, weight = 1),
  colabx = Param(init = 1.0, weight = 0),
  susabx = Param(init = 1.0, weight = 0),
  susever = Param(init = 1.0, weight = 0)
)

# Clearance parameters  
clr <- ClearanceParams(
  base = Param(init = 0.01, weight = 1),
  abx = Param(init = 1.0, weight = 0),
  ever = Param(init = 1.0, weight = 0)
)

# Antibiotic parameters
abx <- AbxParams(onoff = 0, delay = 0.0, life = 2.0)

# Abx rate params
abxrate <- AbxRateParams(
  uncolonized = Param(init = 1.0, weight = 0),
  colonized = Param(init = 1.0, weight = 0)
)

cat("Creating LinearAbxModel2...\n")
model_params <- LinearAbxModel2(
  data = simulated.data_sorted,
  insitu = insitu,
  surveillance.test = surv,
  clinical.test = clin,
  out.of.unit = out,
  acquisition = acq,
  clearance = clr,
  abx = abx,
  abxrate = abxrate
)

cat("\nModel created successfully!\n")
cat("Now getting initial likelihood...\n")

# Get model object
mod <- model_params$model

# Try to get likelihood directly
tryCatch({
  ll <- mod$logLikelihood()
  cat(sprintf("\nInitial log likelihood: %.2f\n", ll))
  
  if (is.infinite(ll) && ll < 0) {
    cat("ERROR: Initial likelihood is -Inf!\n")
    cat("This indicates initialization failed to set colonization states correctly.\n")
  } else {
    cat("SUCCESS: Initial likelihood is finite!\n")
    cat(sprintf("Original C++ first iteration was: -12090.3\n"))
    cat(sprintf("Current R implementation: %.2f\n", ll))
  }
}, error = function(e) {
  cat(sprintf("Error getting likelihood: %s\n", e$message))
})
