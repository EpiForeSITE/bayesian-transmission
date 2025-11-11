# Create a new model object

Creates and initializes a model object based on the provided parameters.
This allows direct creation and inspection of model objects without
running MCMC. Returns a list with all model parameter values for
verification.

## Usage

``` r
newModelExport(modelParameters, verbose = FALSE)
```

## Arguments

- modelParameters:

  List of model parameters, including:

  - `modname` Name of the model (e.g., "LogNormalModel",
    "LinearAbxModel", "LinearAbxModel2", "MixedModel")

  - `nstates` Number of states in the model

  - `nmetro` Number of metropolis steps

  - `forward` Forward parameter

  - `cheat` Cheat parameter

- verbose:

  Print progress messages (default: false)

## Value

A list containing the initialized model parameters:

- `Insitu` - In situ parameters

- `SurveillanceTest` - Surveillance test parameters

- `ClinicalTest` - Clinical test parameters

- `OutCol` - Out of unit colonization parameters

- `InCol` - In unit colonization parameters

- `Abx` - Antibiotic parameters
