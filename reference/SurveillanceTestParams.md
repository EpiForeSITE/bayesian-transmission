# Surveillance Test Parameters

Specify the rates of positive tests for each state of the model.

## Usage

``` r
SurveillanceTestParams(
  colonized = Param(0, 0),
  uncolonized = Param(0.8, 1),
  latent = Param(0, 0)
)
```

## Arguments

- colonized:

  Also known as the true positive rate for a two state model.

- uncolonized:

  Also known as the false positive rate for a two state model.

- latent:

  The rate of positive tests when the individual is in the (optional)
  latent state.

## Examples

``` r
SurveillanceTestParams()
#> $colonized
#> $init
#> [1] 0
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 0
#> 
#> $weight
#> [1] 0
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $uncolonized
#> $init
#> [1] 0.8
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.8
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $latent
#> $init
#> [1] 0
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 0
#> 
#> $weight
#> [1] 0
#> 
#> attr(,"class")
#> [1] "Param"
#> 
```
