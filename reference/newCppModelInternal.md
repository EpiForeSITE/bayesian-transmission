# Create a new C++ model object wrapped in appropriate reference class

Creates and initializes a C++ model object based on the provided
parameters, then wraps it in the appropriate R reference class that
exposes the model's methods and properties.

## Usage

``` r
newCppModelInternal(modelParameters, verbose = FALSE)
```

## Arguments

- modelParameters:

  List of model parameters (same format as runMCMC)

- verbose:

  Print progress messages (default: false)

## Value

A reference class object wrapping the C++ model:

- CppLogNormalModel - for "LogNormalModel"

- CppLinearAbxModel - for "LinearAbxModel"

- CppLinearAbxModel2 - for "LinearAbxModel2"

- CppMixedModel - for "MixedModel" (note: needs Module exposure)

The returned object provides access to model methods and properties
including:

- InColParams, OutColParams, InsituParams, etc.

- logLikelihood(), getHistoryLinkLogLikelihoods(), etc.
