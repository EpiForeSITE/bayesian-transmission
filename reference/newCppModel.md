# Create a new C++ model object with parameters

Creates and initializes a C++ model object based on the provided
parameters. This function wraps the underlying C++ model classes
(LogNormalModel, LinearAbxModel, LinearAbxModel2, MixedModel) in
appropriate R reference classes that expose the model's methods and
properties.

## Usage

``` r
newCppModel(modelParameters, verbose = FALSE)
```

## Arguments

- modelParameters:

  List of model parameters created using functions from constructors.R,
  such as:

  - [`LogNormalModelParams()`](https://epiforesite.github.io/bayestransmission/reference/LogNormalModelParams.md) -
    Basic log-normal model

  - [`LinearAbxModel()`](https://epiforesite.github.io/bayestransmission/reference/LogNormalModelParams.md) -
    Linear antibiotic model

  - Or custom parameter lists containing:

    - `modname`: Model name ("LogNormalModel", "LinearAbxModel",
      "LinearAbxModel2", "MixedModel")

    - `nstates`: Number of states (2 or 3)

    - `nmetro`: Number of Metropolis-Hastings steps

    - `forward`: Forward simulation flag

    - `cheat`: Cheat flag for debugging

    - `Insitu`: In situ parameters from
      [`InsituParams()`](https://epiforesite.github.io/bayestransmission/reference/InsituParams.md)

    - `SurveillanceTest`: Surveillance test parameters from
      [`SurveillanceTestParams()`](https://epiforesite.github.io/bayestransmission/reference/SurveillanceTestParams.md)

    - `ClinicalTest`: Clinical test parameters from
      [`ClinicalTestParams()`](https://epiforesite.github.io/bayestransmission/reference/RandomTestParams.md)

    - `OutCol`: Out-of-unit infection parameters from
      [`OutOfUnitInfectionParams()`](https://epiforesite.github.io/bayestransmission/reference/OutOfUnitInfectionParams.md)

    - `InCol`: In-unit parameters from
      [`InUnitParams()`](https://epiforesite.github.io/bayestransmission/reference/InUnitParams.md)
      or
      [`ABXInUnitParams()`](https://epiforesite.github.io/bayestransmission/reference/InUnitParams.md)

    - `Abx`: Antibiotic parameters from
      [`AbxParams()`](https://epiforesite.github.io/bayestransmission/reference/AbxParams.md)

    - `AbxRate`: Antibiotic rate parameters from
      [`AbxRateParams()`](https://epiforesite.github.io/bayestransmission/reference/AbxRateParams.md)

- verbose:

  Logical flag to print progress messages during model creation and
  parameter setup (default: FALSE)

## Value

A reference class object wrapping the C++ model. The specific class
depends on `modelParameters$modname`:

- `CppLogNormalModel` - For "LogNormalModel"

- `CppLinearAbxModel` - For "LinearAbxModel"

- `CppLinearAbxModel2` - For "LinearAbxModel2"

- `CppMixedModel` - For "MixedModel" (if exposed in C++)

All returned objects inherit from `CppBasicModel` and provide access to:

- **Properties:**

  - `InColParams` - In-unit colonization parameters

  - `OutColParams` - Out-of-unit colonization parameters

  - `InsituParams` - In situ parameters

  - `SurveillanceTestParams` - Surveillance test parameters

  - `ClinicalTestParams` - Clinical test parameters

  - `AbxParams` - Antibiotic parameters

- **Methods:**

  - `logLikelihood(hist)` - Calculate log likelihood for a SystemHistory

  - `getHistoryLinkLogLikelihoods(hist)` - Get individual link log
    likelihoods

  - `forwardSimulate(...)` - Perform forward simulation

  - `initEpisodeHistory(...)` - Initialize episode history

  - `sampleEpisodes(...)` - Sample episodes

  - `setAbx(...)` - Set antibiotic parameters

## Details

The function uses the existing `newModel` C++ function to instantiate
the model and configure all parameters, then wraps it in the appropriate
R reference class based on the model type specified in
`modelParameters$modname`.

## See also

- [`LogNormalModelParams()`](https://epiforesite.github.io/bayestransmission/reference/LogNormalModelParams.md)
  for creating model parameters

- [`LinearAbxModel()`](https://epiforesite.github.io/bayestransmission/reference/LogNormalModelParams.md)
  for linear antibiotic model parameters

- [`InsituParams()`](https://epiforesite.github.io/bayestransmission/reference/InsituParams.md),
  [`SurveillanceTestParams()`](https://epiforesite.github.io/bayestransmission/reference/SurveillanceTestParams.md),
  etc. for parameter components

- [`newModelExport()`](https://epiforesite.github.io/bayestransmission/reference/newModelExport.md)
  for extracting parameter values from a model

## Examples

``` r
if (FALSE) { # \dontrun{
# Create a basic log-normal model
params <- LogNormalModelParams("LogNormalModel")
model <- newCppModel(params)

# Access model properties
inColParams <- model$InColParams
insituParams <- model$InsituParams

# Create a linear antibiotic model
params <- LinearAbxModel()
model <- newCppModel(params, verbose = TRUE)

# Get parameter values
inColParams <- model$InColParams
paramValues <- inColParams$values

# Use with a system history for likelihood calculation
# (requires data and SystemHistory object)
# ll <- model$logLikelihood(systemHistory)
} # }
```
