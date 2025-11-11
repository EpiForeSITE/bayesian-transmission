# Progression Parameters

Progression Parameters

## Usage

``` r
ProgressionParams(
  rate = Param(0.01),
  abx = Param(1, 0),
  ever_abx = Param(1, 0)
)
```

## Arguments

- rate:

  Base progression rate

- abx:

  Effect of current antibiotics on progression

- ever_abx:

  Effect of ever having taken antibiotics on progression

## Value

A list of parameters for progression.

## Examples

``` r
ProgressionParams()
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
