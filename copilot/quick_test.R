#!/usr/bin/env Rscript
# Quick test with simpler parameter setup

library(bayestransmission)
data("simulated.data", package = "bayestransmission")

cat("\n=== Quick Likelihood Test ===\n\n")

# Use the constructor from the vignette
params <- LinearAbxAcquisitionParams(
    base = Param(init = 0.001, weight = 1),
    time = Param(init = 1.0, weight = 0),
    mass = Param(init = 1.0, weight = 1),
    freq = Param(init = 1.0, weight = 1),
    col_abx = Param(init = 1.0, weight = 0),
    suss_abx = Param(init = 1.0, weight = 0),
    suss_ever = Param(init = 1.0, weight = 0)
)

# Create the full model params list
modelParams <- list(
    modname = 'LinearAbxModel2',
    nstates = 2,
    nmetro = 100,
    forward = 1,
    cheat = 0,
    
    Abx = AbxParams(pAbx = Param(init = 0.0, weight = 0)),
    
    Insitu = InsituParams(Pcol = Param(init = 0.1, weight = 0)),
    
    SurveillanceTest = SurveillanceTestParams(
        Sens.Sympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Sens.Asympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Spec = ParamWRate(init = 1.0, weight = 0, rate = 1.0)
    ),
    
    OutOfUnit = OutOfUnitInfectionParams(
        pInfect = Param(init = 0.0, weight = 0)
    ),
    
    InUnit = params
)

cat("Running short MCMC...\n")
set.seed(123)

results <- runMCMC(
    data = simulated.data,
    MCMCParameters = list(
        nburn = 3,
        nsims = 3,
        outputparam = TRUE,
        outputfinal = TRUE
    ),
    modelParameters = modelParams,
    verbose = TRUE
)

cat("\n=== Results ===\n")
cat("Log Likelihood values:\n")
print(results$LogLikelihood)

finite_ll <- results$LogLikelihood[is.finite(results$LogLikelihood)]
cat("\nNumber of finite values:", length(finite_ll), "out of", length(results$LogLikelihood), "\n")

if(length(finite_ll) > 0) {
    cat("\n✓ SUCCESS: Got finite likelihood values!\n")
    cat("  First finite:", finite_ll[1], "\n")
    cat("  Final:", tail(finite_ll, 1), "\n")
} else {
    cat("\n✗ Still getting -Inf\n")
}
