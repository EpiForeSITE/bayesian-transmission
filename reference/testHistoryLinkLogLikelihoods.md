# Get Individual HistoryLink Log Likelihoods (Diagnostic Function)

This function creates a model and system history, then returns the log
likelihood contribution from each individual HistoryLink in the system.
Useful for debugging and verifying likelihood calculations.

## Usage

``` r
testHistoryLinkLogLikelihoods(modelParameters)
```

## Arguments

- modelParameters:

  List of model parameters (same format as runMCMC)

## Value

List containing:

- `linkLogLikelihoods` - vector of log likelihoods for each link

- `overallLogLikelihood` - total log likelihood from
  model-\>logLikelihood()

- `numLinks` - number of history links
