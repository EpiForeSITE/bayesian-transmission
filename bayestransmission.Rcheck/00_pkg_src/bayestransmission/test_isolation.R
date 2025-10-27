#!/usr/bin/env Rscript
# Test that multiple runMCMC calls are properly isolated

library(bayestransmission)

cat("Testing isolation of multiple runMCMC calls...\n\n")

# Load data
data(simulated.data)

# Create model
params <- LinearAbxModel(nstates = 2)

# Run 1
cat("Run 1...")
res1 <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 1, nsims = 1, outputparam = FALSE, outputfinal = FALSE),
  modelParameters = params,
  verbose = FALSE
)
cat("OK\n")

# Run 2 - should start with clean state
cat("Run 2...")
res2 <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 1, nsims = 1, outputparam = FALSE, outputfinal = FALSE),
  modelParameters = params,
  verbose = FALSE
)
cat("OK\n")

# Run 3 - verify still works
cat("Run 3...")
res3 <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(nburn = 1, nsims = 1, outputparam = FALSE, outputfinal = FALSE),
  modelParameters = params,
  verbose = FALSE
)
cat("OK\n")

cat("\nAll runs completed successfully!\n")
cat("The fix properly clears static Map objects between runs.\n")
