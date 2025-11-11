# Antibiotic Administration Rate Parameters

Antibiotic Administration Rate Parameters

## Usage

``` r
AbxRateParams(
  uncolonized = Param(1, 0),
  colonized = Param(1, 0),
  latent = Param(0)
)
```

## Arguments

- uncolonized:

  Rate of antibiotic administration when the individual is uncolonized.

- colonized:

  Rate of antibiotic administration when the individual is colonized.

- latent:

  Rate of antibiotic administration when the individual is latent.

## Value

list of parameters for antibiotic administration.

## Examples

``` r
AbxRateParams()
#> $uncolonized
#> $init
#> [1] 1
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 1
#> 
#> $weight
#> [1] 0
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $colonized
#> $init
#> [1] 1
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 1
#> 
#> $weight
#> [1] 0
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
