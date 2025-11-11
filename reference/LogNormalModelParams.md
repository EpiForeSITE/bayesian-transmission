# Model Parameters for a Log Normal Model

Model Parameters for a Log Normal Model

## Usage

``` r
LogNormalModelParams(
  modname,
  nstates = 2L,
  nmetro = 1L,
  forward = TRUE,
  cheat = FALSE,
  Insitu = NULL,
  SurveillanceTest = SurveillanceTestParams(),
  ClinicalTest = ClinicalTestParams(),
  OutOfUnitInfection = OutOfUnitInfectionParams(),
  InUnit = InUnitParams(),
  Abx = AbxParams(),
  AbxRate = AbxRateParams()
)

LinearAbxModel(..., InUnit = ABXInUnitParams())
```

## Arguments

- modname:

  The name of the model used. Usually specified by specification
  functions.

- nstates:

  The number of states in the model.

- nmetro:

  The number of Metropolis-Hastings steps to take between outputs.

- forward:

  TODO

- cheat:

  TODO

- Insitu:

  In Situ Parameters

- SurveillanceTest:

  Surveillance Testing Parameters

- ClinicalTest:

  Clinical Testing Parameters

- OutOfUnitInfection:

  Out of Unit Infection Parameters

- InUnit:

  In Unit Parameters, should be a list of lists with parameters for the
  acquisition, progression and clearance of the disease.

- Abx:

  Antibiotic Parameters

- AbxRate:

  Antibiotic Rate Parameters

- ...:

  Additional arguments passed to LogNormalModelParams

## Value

A list of parameters for the model.

## Functions

- `LinearAbxModel()`: Linear Antibiotic Model Alias

## Examples

``` r
LogNormalModelParams("LogNormalModel")
#> $modname
#> [1] "LogNormalModel"
#> 
#> $nstates
#> [1] 2
#> 
#> $nmetro
#> [1] 1
#> 
#> $forward
#> [1] TRUE
#> 
#> $cheat
#> [1] FALSE
#> 
#> $Insitu
#> $Insitu$probs
#> [1] 0.9 0.0 0.1
#> 
#> $Insitu$priors
#> [1] 0.9 1.0 0.1
#> 
#> $Insitu$doit
#> [1]  TRUE FALSE  TRUE
#> 
#> 
#> $SurveillanceTest
#> $SurveillanceTest$colonized
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
#> $SurveillanceTest$uncolonized
#> $init
#> [1] 0.8
#> 
#> $update
#> [1] TRUE
#> 
#> $prior
#> [1] 0.8
#> 
#> $weight
#> [1] 1
#> 
#> attr(,"class")
#> [1] "Param"
#> 
#> $SurveillanceTest$latent
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
#> $ClinicalTest
#> $ClinicalTest$uncolonized
#> $ClinicalTest$uncolonized$param
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
#> $ClinicalTest$uncolonized$rate
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
#> $ClinicalTest$colonized
#> $ClinicalTest$colonized$param
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
#> $ClinicalTest$colonized$rate
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
#> $ClinicalTest$latent
#> $ClinicalTest$latent$param
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
#> $ClinicalTest$latent$rate
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
#> 
#> $OutCol
#> $OutCol$acquisition
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
#> $OutCol$clearance
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
#> $OutCol$progression
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
#> $InCol
#> $InCol$acquisition
#> $InCol$acquisition$base
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
#> $InCol$acquisition$time
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
#> $InCol$acquisition$mass
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
#> $InCol$acquisition$freq
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
#> $InCol$acquisition$col_abx
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
#> $InCol$acquisition$suss_abx
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
#> $InCol$acquisition$suss_ever
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
#> $InCol$progression
#> $InCol$progression$rate
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
#> $InCol$progression$abx
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
#> $InCol$progression$ever_abx
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
#> $InCol$clearance
#> $InCol$clearance$rate
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
#> $InCol$clearance$abx
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
#> $InCol$clearance$ever_abx
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
#> 
#> $Abx
#> $Abx$onoff
#> [1] FALSE
#> 
#> $Abx$delay
#> [1] 0
#> 
#> $Abx$life
#> [1] 1
#> 
#> 
#> $AbxRate
#> $AbxRate$uncolonized
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
#> $AbxRate$colonized
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
#> $AbxRate$latent
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
