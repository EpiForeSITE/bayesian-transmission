# Linear Antibiotic Acquisition Parameters

The model for this acquisition model is given by

## Usage

``` r
LinearAbxAcquisitionParams(
  base = Param(0.001),
  time = Param(1, 0),
  mass = Param(1, 1),
  freq = Param(1, 1),
  col_abx = Param(1, 0),
  suss_abx = Param(1, 0),
  suss_ever = Param(1, 0)
)
```

## Arguments

- base:

  The base rate of acquisition.

- time:

  The time effect on acquisition.

- mass:

  The mass action effect on acquisition.

- freq:

  The frequency effect on acquisition.

- col_abx:

  The effect for colonized on antibiotics.

- suss_abx:

  The effect on susceptible being currently on antibiotics.

- suss_ever:

  The effect on susceptible ever being on antibiotics.

## Value

A list of parameters for acquisition.

## Details

\$\$ P(\mathrm{Acq(t)}) =
\left\[e^{\beta\_\mathrm{time}(t-t_0)}\right\]\\ \left\\e^{\beta_0}
\left\[ \left(\frac{\beta\_\mathrm{freq}}{P(t)}+(1 -
e^{\beta\_\mathrm{freq}})\right) e^{\beta\_\mathrm{mass}}\left(
(N_c(t) - N\_{ca}(t)) + e^{\beta\_\mathrm{col\\abx}}N\_{ca}(t) \right) +
1 - e^{\beta\_\mathrm{mass}} \right\] \right\\\\ \left\[ N_S(t) -
N_E(t) + e^{\beta\_\mathrm{suss\\ever}}\left(\left(E_i(t)-A_i(t)\right)
+A_i(t)e^{\beta\_\mathrm{suss\\abx}}\right) \right\] \$\$

## Examples

``` r
LinearAbxAcquisitionParams()
#> $base
#> $init
#> [1] 0.001
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.001
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $time
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
#> $mass
#> $init
#> [1] 1
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 1
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $freq
#> $init
#> [1] 1
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 1
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $col_abx
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
#> $suss_abx
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
#> $suss_ever
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
