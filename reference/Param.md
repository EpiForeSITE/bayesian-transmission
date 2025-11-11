# Construct a parameter with a prior, weight and an update flag.

Construct a parameter with a prior, weight and an update flag.

## Usage

``` r
Param(
  init,
  weight = if_else(init == 0, 0, 1),
  update = weight > 0,
  prior = init
)
```

## Arguments

- init:

  the initial value of the parameter.

- weight:

  the weight of the prior.

- update:

  a flag indicating if the parameter shouldbe updated in the MCMC.

- prior:

  mean value of the prior distribution, may be used with weight to fully
  determine prior parameters.

## Value

A list with the following elements:

- `init` the initial value of the parameter.

- `weight` the weight of the prior.

- `update` a flag indicating if the parameter shouldbe updated in the
  MCMC.

- `prior` mean value of the prior distribution, may be used with weight
  to fully determine prior parameters.

## Examples

``` r
# Fully specified parameter.
Param(init = 0, weight = 1, update = TRUE, prior = 0.5)
#> $init
#> [1] 0
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.5
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
# Fixed parameter
# Weight = 0 implies update=FALSE and prior is ignored.
Param(0, 0)
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
# Update parameter that starts at zero.
Param(0, weight =1, update=TRUE)
#> $init
#> [1] 0
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
# Parameters specified at zero implies fixed.
Param(0)
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
```
