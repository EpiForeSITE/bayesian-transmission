# Clearance Parameters

Clearance Parameters

## Usage

``` r
ClearanceParams(rate = Param(0.01), abx = Param(1, 0), ever_abx = Param(1, 0))
```

## Arguments

- rate:

  base rate of clearance

- abx:

  effect of antibiotics on clearance

- ever_abx:

  effect of ever having taken antibiotics on clearance

## Value

A list of parameters for clearance.

## Examples

``` r
ClearanceParams()
#> $rate
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
#> $abx
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
#> $ever_abx
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
```
