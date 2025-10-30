#!/usr/bin/env Rscript
# Test script to examine individual HistoryLink log likelihoods

library(bayestransmission)

cat("Loading simulated data...\n")
data("simulated.data")

cat("\nSetting up model parameters...\n")
params <- LinearAbxModel(
  nstates = 2,
  nmetro = 10,
  forward = TRUE,
  cheat = TRUE,
  Insitu = InsituParams(
    probs = c(0.90, 0.0, 0.10),
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
      mass = Param(init = 0.9999, weight = 1),
      freq = Param(init = 0.9999, weight = 1),
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
    colonized = Param(init = 1.0, weight = 0)
  )
)

cat("\nRunning diagnostic function testHistoryLinkLogLikelihoods...\n")

# Add Data to params
params$Data <- simulated.data
params$verbose <- TRUE

result <- testHistoryLinkLogLikelihoods(params)

cat("\nResult structure:\n")
print(names(result))
print(str(result))

cat("\n=== Getting individual HistoryLink log likelihoods ===\n\n")

# Get the results
link_lls <- result$linkLogLikelihoods

cat("\n=== Summary of HistoryLink Log Likelihoods ===\n")
cat("Total number of links: ", length(link_lls), "\n")
cat("Sum of all link log likelihoods: ", sum(link_lls), "\n")
cat("\nStatistics:\n")
cat("  Min: ", min(link_lls), "\n")
cat("  Max: ", max(link_lls), "\n")
cat("  Mean: ", mean(link_lls), "\n")
cat("  Median: ", median(link_lls), "\n")
cat("  Std Dev: ", sd(link_lls), "\n")

cat("\nNumber of -Inf values: ", sum(is.infinite(link_lls) & link_lls < 0), "\n")
cat("Number of NaN values: ", sum(is.nan(link_lls)), "\n")
cat("Number of NA values: ", sum(is.na(link_lls) & !is.nan(link_lls)), "\n")

# Compare with overall logLikelihood
overall_ll <- result$overallLogLikelihood
cat("\nOverall logLikelihood: ", overall_ll, "\n")
cat("Difference (overall - sum of links): ", overall_ll - sum(link_lls), "\n")

# Show distribution
if (any(is.finite(link_lls))) {
    cat("\nDistribution of finite log likelihoods:\n")
    finite_lls <- link_lls[is.finite(link_lls)]
    print(summary(finite_lls))
}

# Find problematic links
if (any(is.infinite(link_lls) | is.nan(link_lls))) {
    cat("\n=== Problematic Links ===\n")
    prob_idx <- which(is.infinite(link_lls) | is.nan(link_lls))
    cat("Indices of problematic links:", head(prob_idx, 20), "\n")
    if (length(prob_idx) > 20) {
        cat("... and", length(prob_idx) - 20, "more\n")
    }
}

cat("\nDone.\n")
