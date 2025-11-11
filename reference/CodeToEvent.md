# Convert coded events to string events

Convert coded events to string events

## Usage

``` r
CodeToEvent(x)
```

## Arguments

- x:

  A vector of integers

## Value

A vector of strings

## Examples

``` r
CodeToEvent(c(-1:19, 21:23, 31:33, -999))
#>  [1] "nullevent"        "admission"        "negsurvtest"      "possurvtest"     
#>  [5] "discharge"        "negclintest"      "posclintest"      "insitu"          
#>  [9] "postest"          "negtest"          "abxdose"          "abxon"           
#> [13] "abxoff"           "isolon"           "isoloff"          "admission0"      
#> [17] "admission1"       "admission2"       "insitu0"          "insitu1"         
#> [21] "insitu2"          "start"            "marker"           "stop"            
#> [25] "acquisition"      "progression"      "clearance"        "EVENT_TYPE_ERROR"
```
