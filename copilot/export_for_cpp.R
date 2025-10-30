#!/usr/bin/env Rscript
# Export data for original C++ comparison

library(bayestransmission)

cat("Loading simulated data...\n")
data("simulated.data")

# The original C++ expects data in format:
# facility unit time patient type [comment]

cat("Exporting data in original C++ format...\n")

# Sort data as expected
data_sorted <- simulated.data[order(simulated.data$patient, simulated.data$time), ]

# Write to file
write.table(
  data_sorted[, c("facility", "unit", "time", "patient", "type")],
  file = "simulated_data_for_cpp.txt",
  row.names = FALSE,
  col.names = FALSE,
  quote = FALSE
)

cat("Data exported to: simulated_data_for_cpp.txt\n")
cat("Number of events:", nrow(data_sorted), "\n")

# Also create a simple model file for LinearAbxModel2
# Format is described in readModel.h and LogNormalModel.h read() method
cat("\nCreating model parameter file...\n")

model_file <- file("model_params_for_cpp.txt", "w")

# Model name and number of states
cat("LinearAbxModel2 2\n", file = model_file)

# AbxByOnOff
cat("AbxByOnOff 0\n", file = model_file)

# Abx delay
cat("AbxDelay 0.0\n", file = model_file)

# Abx life
cat("AbxLife 1.5\n", file = model_file)

# Blank line
cat("\n", file = model_file)

# Insitu params: probs (for each state) and priors (for each state)
cat("InsituParams\n", file = model_file)
cat("0.90 0.0 0.10\n", file = model_file)  # probs for states
cat("1 1 1\n", file = model_file)  # priors

# Blank line
cat("\n", file = model_file)

# Surveillance test params
cat("SurveillanceTestParams\n", file = model_file)
cat("0.0 0.0 0.8\n", file = model_file)  # P(+|state) for uncolonized, latent, colonized

# Blank line
cat("\n", file = model_file)

# Clinical (Random) test params  
cat("ClinicalTestParams\n", file = model_file)
cat("0.5 0.5\n", file = model_file)  # P(+) for uncolonized, colonized
cat("1.0 1.0\n", file = model_file)  # rates

# Blank line
cat("\n", file = model_file)

# Out of unit (OutCol) params
cat("OutOfUnitParams\n", file = model_file)
cat("0.001 0.01 0.0\n", file = model_file)  # acquisition, clearance, progression

# Blank line
cat("\n", file = model_file)

# In unit (InCol) params - LogNormalICP
cat("InUnitParams\n", file = model_file)
cat("0.001\n", file = model_file)  # base acquisition
cat("1.0\n", file = model_file)    # time effect
cat("0.9999\n", file = model_file) # mass action
cat("0.9999\n", file = model_file) # frequency
cat("1.0\n", file = model_file)    # col_abx
cat("1.0\n", file = model_file)    # suss_abx
cat("1.0\n", file = model_file)    # suss_ever_abx
cat("0.0\n", file = model_file)    # progression rate
cat("1.0\n", file = model_file)    # progression abx effect
cat("1.0\n", file = model_file)    # progression ever abx effect
cat("0.01\n", file = model_file)   # clearance rate
cat("1.0\n", file = model_file)    # clearance abx effect
cat("1.0\n", file = model_file)    # clearance ever abx effect

# Blank line
cat("\n", file = model_file)

# Abx rate params
cat("AbxRateParams\n", file = model_file)
cat("1.0 1.0\n", file = model_file)  # rates for uncolonized, colonized

close(model_file)

cat("Model parameters exported to: model_params_for_cpp.txt\n")
cat("\nDone. Ready to run original C++ code.\n")
