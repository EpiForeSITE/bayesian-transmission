# Convert string events to coded events

Convert string events to coded events

## Usage

``` r
EventToCode(x)
```

## Arguments

- x:

  A vector of strings

## Value

A vector of integers

## Examples

``` r
EventToCode(c("admission", "discharge", "postest", "negtest"))
#> [1] 0 3 7 8
EventToCode(c("abxon", "abxoff", "isolon", "isoloff"))
#> [1] 10 11 12 13
```
