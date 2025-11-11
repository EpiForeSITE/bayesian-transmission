# Out of Unit Infection Parameters

Out of Unit Infection Parameters

## Usage

``` r
OutOfUnitInfectionParams(
  acquisition = Param(0.05),
  clearance = Param(0.01),
  progression = Param(0)
)
```

## Arguments

- acquisition:

  Rate of acquisition of the disease moving into latent state.

- clearance:

  Rate of clearance of the disease moving into uncolonized state.

- progression:

  Rate of progression of the disease moving into colonized state.

## Value

A list of parameters for out of unit infection.

## Examples

``` r
OutOfUnitInfectionParams()
#> $acquisition
#> $init
#> [1] 0.05
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.05
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $clearance
#> $init
#> [1] 0.01
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.01
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $progression
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
