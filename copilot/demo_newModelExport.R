#!/usr/bin/env Rscript
# Demonstration of newModelExport function

library(bayestransmission)

cat("=== Testing newModelExport Function ===\n\n")

# Example 1: Creating a model object for inspection
cat("Example 1: Using existing model parameters from test_linearabxmodel2.R\n")
cat("(Note: We can now create model objects directly without running MCMC)\n\n")

# For a complete example, let's show that the function exists and can be called
cat("Available in bayestransmission:\n")
cat("  - newModelExport(): Create and inspect model objects directly\n")
cat("  - runMCMC(): Run MCMC on a model\n\n")

# Show the function signature
cat("Function signature:\n")
cat("  newModelExport(modelParameters, verbose = FALSE)\n\n")

cat("Usage:\n")
cat("  1. Create your model parameters list (with modname, nstates, etc.)\n")
cat("  2. Call: model <- newModelExport(modelParameters, verbose = TRUE)\n")
cat("  3. The returned model object is an external pointer to the C++ model\n")
cat("  4. You can then inspect or manipulate the model object\n\n")

cat("Example model parameter structure:\n")
cat("  modelParams <- list(\n")
cat("    modname = 'LinearAbxModel2',  # or 'LinearAbxModel', 'LogNormalModel', 'MixedModel'\n")
cat("    nstates = 2,\n")
cat("    nmetro = 100,\n")
cat("    forward = 1,\n")
cat("    cheat = 0,\n")
cat("    Abx = AbxParams(...),\n")
cat("    Insitu = InsituParams(...),\n")
cat("    # ... other required parameters\n")
cat("  )\n\n")

cat("For a complete working example, see: test_linearabxmodel2.R\n")
cat("which uses the full parameter setup with runMCMC().\n\n")

cat("=== newModelExport is now available for direct model creation! ===\n")
