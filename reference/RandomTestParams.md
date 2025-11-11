# Random Testing Parameter Set

Random Testing Parameter Set

## Usage

``` r
RandomTestParams(
  uncolonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
  colonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
  latent = ParamWRate(Param(0), Param(0))
)

ClinicalTestParams(
  uncolonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
  colonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
  latent = ParamWRate(Param(0), Param(0))
)
```

## Arguments

- uncolonized:

  Testing when the individual is uncolonized.

- colonized:

  Testing when the individual is colonized.

- latent:

  Testing when the individual is latent.

## Value

list of parameters for random testing.

## Functions

- `ClinicalTestParams()`: Clinical Test Parameters Alias

## Examples

``` r
RandomTestParams()
#> $uncolonized
#> $uncolonized$param
#> $init
#> [1] 0.5
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 0.5
#> 
#> $weight
#> [1] 0
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $uncolonized$rate
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
#> 
#> $colonized
#> $colonized$param
#> $init
#> [1] 0.5
#> 
#> $update
#> [1] FALSE
#> 
#> $prior
#> [1] 0.5
#> 
#> $weight
#> [1] 0
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $colonized$rate
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
#> 
#> $latent
#> $latent$param
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
#> $latent$rate
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
#> 
```
