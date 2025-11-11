# Run Bayesian Transmission MCMC

Run Bayesian Transmission MCMC

## Usage

``` r
runMCMC(data, MCMCParameters, modelParameters, verbose = FALSE)
```

## Arguments

- data:

  Data frame with columns, in order: facility, unit, time, patient, and
  event type.

- MCMCParameters:

  List of MCMC parameters.

- modelParameters:

  List of model parameters, see .

- verbose:

  Print progress messages.

## Value

A list with the following elements:

- `Parameters` the MCMC chain of model parameters

- `LogLikelihood` the log likelihood of the model at each iteration

- `MCMCParameters` the MCMC parameters used

- `ModelParameters` the model parameters used

- `ModelName` the name of the model

- `nstates` the number of states in the model

- `waic1` the WAIC1 estimate

- `waic2` the WAIC2 estimate

- and optionally (if `MCMCParameters$outputfinal` is true) `FinalModel`
  the final model state.
