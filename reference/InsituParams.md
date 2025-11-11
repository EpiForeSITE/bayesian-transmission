# InSitu Parameters

InSitu Parameters

## Usage

``` r
InsituParams(probs = NULL, priors = NULL, doit = NULL, nstates = NULL)
```

## Arguments

- probs:

  The probability of the individual being in each state.

- priors:

  The prior probability of the individual being in each state.

- doit:

  A flag indicating if the rate(s) should be updated in the MCMC.

- nstates:

  The number of states (2 or 3). If NULL, inferred from probs length.
  For 2-state models, uses `c(uncolonized, latent=0, colonized)`. For
  3-state models, uses `c(uncolonized, latent, colonized)`.

## Value

A list of parameters for in situ testing.

## Examples

``` r
InsituParams()
#> $probs
#> [1] 0.9 0.0 0.1
#> 
#> $priors
#> [1] 0.9 1.0 0.1
#> 
#> $doit
#> [1]  TRUE FALSE  TRUE
#> 
InsituParams(nstates = 2)  # c(0.9, 0.0, 0.1)
#> $probs
#> [1] 0.9 0.0 0.1
#> 
#> $priors
#> [1] 0.9 1.0 0.1
#> 
#> $doit
#> [1]  TRUE FALSE  TRUE
#> 
InsituParams(nstates = 3)  # c(0.98, 0.01, 0.01)
#> $probs
#> [1] 0.98 0.01 0.01
#> 
#> $priors
#> [1] 0.98 0.01 0.01
#> 
#> $doit
#> [1] TRUE TRUE TRUE
#> 
```
