# Run Bayesian Transmission MCMC

Run Bayesian Transmission MCMC

## Usage

``` r
runMCMC(
  data,
  modelParameters,
  nsims,
  nburn = 100L,
  outputparam = TRUE,
  outputfinal = FALSE,
  verbose = FALSE
)
```

## Arguments

- data:

  Data frame with columns, in order: facility, unit, time, patient, and
  event type.

- modelParameters:

  List of model parameters, see .

- nsims:

  Number of MCMC samples to collect after burn-in.

- nburn:

  Number of burn-in iterations.

- outputparam:

  Whether to output parameter values at each iteration.

- outputfinal:

  Whether to output the final model state.

- verbose:

  Print progress messages.

## Value

A list with the following elements:

- `Parameters` the MCMC chain of model parameters (if outputparam=TRUE)

- `LogLikelihood` the log likelihood of the model at each iteration (if
  outputparam=TRUE)

- `MCMCParameters` the MCMC parameters used

- `ModelParameters` the model parameters used

- `ModelName` the name of the model

- `nstates` the number of states in the model

- `waic1` the WAIC1 estimate

- `waic2` the WAIC2 estimate

- and optionally (if outputfinal=TRUE) `FinalModel` the final model
  state.

## Examples

``` r
if (FALSE) { # \dontrun{
  # Minimal example: create parameters and run a very short MCMC
  params <- LinearAbxModel(nstates = 2)
  data(simulated.data_sorted, package = "bayestransmission")
  results <- runMCMC(
    data = simulated.data_sorted,
    modelParameters = params,
    nsims = 1,
    nburn = 0,
    outputparam = TRUE,
    outputfinal = FALSE,
    verbose = FALSE
  )
  str(results)
} # }
```
