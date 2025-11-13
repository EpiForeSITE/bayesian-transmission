# Convert MCMC Parameters to Data Frame

Converts the nested list structure of MCMC parameters from `runMCMC`
output into a tidy data frame format suitable for analysis and
visualization.

## Usage

``` r
mcmc_to_dataframe(mcmc_results)
```

## Arguments

- mcmc_results:

  The results object returned by
  [`runMCMC()`](https://epiforesite.github.io/bayesian-transmission/reference/runMCMC.md).
  Must contain a `Parameters` component and a `LogLikelihood` component.

## Value

A data frame with one row per MCMC iteration containing:

- `iteration`: The iteration number

- `insitu_*`: In-situ probability parameters

- `surv_test_*`: Surveillance test parameters

- `clin_test_*`: Clinical test parameters and rates

- `outunit_*`: Out of unit infection parameters

- `inunit_*`: In unit LinearAbx model parameters (base, time, mass,
  freq, colabx, susabx, susever, clr, clrAbx, clrEver)

- `abxrate_*`: Antibiotic rate parameters

- `loglikelihood`: Log likelihood at each iteration

## Details

The function extracts parameters from the nested list structure and
handles missing values gracefully by inserting `NA` when a parameter is
not present. This is particularly useful for creating trace plots and
posterior distributions.

## Examples

``` r
if (FALSE) { # \dontrun{
results <- runMCMC(data = simulated.data,
                   modelParameters = LinearAbxModel(),
                   nsims = 1000,
                   nburn = 100,
                   outputparam = TRUE,
                   outputfinal = TRUE)
param_df <- mcmc_to_dataframe(results)
head(param_df)
} # }
```
