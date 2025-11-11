# Specify a random testing parameter with a rate.

Specify a random testing parameter with a rate.

## Usage

``` r
ParamWRate(param = Param(), rate = Param())
```

## Arguments

- param:

  Values for the positive rate of the test.

- rate:

  Values for the rate of the test.

## Value

A list of with param and rate.

## Examples

``` r
ParamWRate(Param(0.5, 0), rate = Param(1, 0))
#> $param
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
#> $rate
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
