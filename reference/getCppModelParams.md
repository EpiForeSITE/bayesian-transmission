# Extract Model Parameters from C++ Model Object

Convenience function to extract all parameter values from a C++ model
object created with
[`newCppModel()`](https://epiforesite.github.io/bayestransmission/reference/newCppModel.md).
This is essentially a wrapper around accessing the model's parameter
properties.

## Usage

``` r
getCppModelParams(model)
```

## Arguments

- model:

  A C++ model object created with
  [`newCppModel()`](https://epiforesite.github.io/bayestransmission/reference/newCppModel.md)

## Value

A named list containing all model parameter values:

- `Insitu` - In situ parameter values

- `SurveillanceTest` - Surveillance test parameter values

- `ClinicalTest` - Clinical test parameter values

- `OutCol` - Out-of-unit colonization parameter values

- `InCol` - In-unit colonization parameter values

- `Abx` - Antibiotic parameter values (if applicable)

## Examples

``` r
if (FALSE) { # \dontrun{
params <- LinearAbxModel()
model <- newCppModel(params)

# Extract all parameters
all_params <- getCppModelParams(model)
print(all_params$InCol)
} # }
```
