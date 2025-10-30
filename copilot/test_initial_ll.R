#!/usr/bin/env Rscript
# Simple test - just capture the printed initial likelihood

library(bayestransmission)
data("simulated.data", package = "bayestransmission")

cat("\n=== Test Initial Likelihood with Fixed Argument Order ===\n\n")

# Minimal model params
modelParams <- list(
    modname = 'LinearAbxModel2',
    nstates = 2,
    nmetro = 100,
    forward = 1,
    cheat = 0,
    
    Abx = AbxParams(),
    
    Insitu = InsituParams(Pcol = Param(init = 0.1, weight = 0)),
    
    SurveillanceTest = SurveillanceTestParams(
        Sens.Sympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Sens.Asympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Spec = ParamWRate(init = 1.0, weight = 0, rate = 1.0)
    ),
    
    OutOfUnit = OutOfUnitInfectionParams(
        pInfect = Param(init = 0.0, weight = 0)
    ),
    
    InUnit = LinearAbxAcquisitionParams(
        base = Param(init = 0.001, weight = 1),
        time = Param(init = 1.0, weight = 0),
        mass = Param(init = 1.0, weight = 1),
        freq = Param(init = 1.0, weight = 1),
        col_abx = Param(init = 1.0, weight = 0),
        suss_abx = Param(init = 1.0, weight = 0),
        suss_ever = Param(init = 1.0, weight = 0)
    )
)

cat("Running MCMC with verbose=TRUE to see initial likelihood...\n\n")
set.seed(123)

results <- runMCMC(
    data = simulated.data,
    MCMCParameters = list(
        nburn = 2,
        nsims = 2,
        outputparam = TRUE,
        outputfinal = TRUE
    ),
    modelParameters = modelParams,
    verbose = TRUE
)

cat("\n\n=== Post-MCMC Log Likelihoods ===\n")
print(results$LogLikelihood)

finite_ll <- results$LogLikelihood[is.finite(results$LogLikelihood)]
cat("\nFinite values:", length(finite_ll), "out of", length(results$LogLikelihood), "\n")

if(length(finite_ll) > 0) {
    cat("✓ Got finite post-MCMC likelihoods\n")
    cat("  Range: [", min(finite_ll), ",", max(finite_ll), "]\n")
} else {
    cat("✗ All post-MCMC likelihoods are -Inf\n")
}

cat("\n=== Check the 'LogLike=' value printed above ===\n")
cat("If it shows a finite number (not -inf), the fix worked!\n")
cat("The original C++ got LogLike=-12942.9 for this data.\n\n")
