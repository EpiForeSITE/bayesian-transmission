#!/usr/bin/env Rscript
# Test if the argument order fix resolves the -Inf initial likelihood

library(bayestransmission)

cat("\n=== Testing Fixed makeHistLink Argument Order ===\n\n")

# Load the data
data("simulated.data", package = "bayestransmission")

# Set up parameters matching original C++ exactly
params <- LogNormalModelParams(
    nstates = 2,
    
    # Insitu parameters
    Insitu = InsituParams(
        Pcol = Param(init = 0.1, weight = 0)
    ),
    
    # Test parameters
    Surveillance = SurveillanceTestParams(
        Sens.Sympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Sens.Asympt = ParamWRate(init = 1.0, weight = 0, rate = 1.0),
        Spec = ParamWRate(init = 1.0, weight = 0, rate = 1.0)
    ),
    
    # Out-of-unit infection
    OutOfUnit = OutOfUnitInfectionParams(
        pInfect = Param(init = 0.0, weight = 0)
    ),
    
    # In-unit acquisition
    InUnit = LinearAbxAcquisitionParams(
        base = Param(init = 0.001, weight = 1),
        time = Param(init = 1.0, weight = 0),
        mass = Param(init = 1.0, weight = 1),
        freq = Param(init = 1.0, weight = 1),
        col_abx = Param(init = 1.0, weight = 0),
        suss_abx = Param(init = 1.0, weight = 0),
        suss_ever = Param(init = 1.0, weight = 0)
    ),
    
    # Progression (not used in 2-state model)
    Progression = ProgressionParams(
        rate = Param(init = 0.0, weight = 0),
        abx = Param(init = 1.0, weight = 0),
        ever_abx = Param(init = 1.0, weight = 0)
    ),
    
    # Clearance
    Clearance = ClearanceParams(
        rate = Param(init = 0.01, weight = 1),
        abx = Param(init = 1.0, weight = 0),
        ever_abx = Param(init = 1.0, weight = 0)
    ),
    
    # Antibiotic effect
    Abx = AbxParams(
        pAbx = Param(init = 0.0, weight = 0)
    ),
    
    # Random testing
    RandomTest = RandomTestParams(
        rate = Param(init = 0.0, weight = 0)
    )
)

cat("Running short MCMC to check initial likelihood...\n")
set.seed(123)

results <- runMCMC(
    data = simulated.data,
    MCMCParameters = list(
        nburn = 5,
        nsims = 5,
        outputparam = TRUE,
        outputfinal = TRUE
    ),
    modelParameters = params,
    verbose = TRUE
)

cat("\n=== Results ===\n")
cat("Log Likelihood values:\n")
print(results$LogLikelihood)

finite_ll <- results$LogLikelihood[is.finite(results$LogLikelihood)]
if(length(finite_ll) > 0) {
    cat("\n✓ SUCCESS: Got finite likelihood values!\n")
    cat("  First finite:", finite_ll[1], "\n")
    cat("  Final:", tail(finite_ll, 1), "\n")
    cat("  Range: [", min(finite_ll), ",", max(finite_ll), "]\n")
} else {
    cat("\n✗ FAILED: All likelihood values are still -Inf\n")
}

cat("\nWAIC values:\n")
cat("  WAIC1:", results$waic1, "\n")
cat("  WAIC2:", results$waic2, "\n")

cat("\n=== Test Complete ===\n")
