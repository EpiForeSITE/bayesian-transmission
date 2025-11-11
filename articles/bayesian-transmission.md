# bayesian-transmission

``` r
library(bayestransmission)
```

## Introduction

This package provides a Bayesian framework for transmission modeling on
an individual patient level. Modeling is conducted through Markov Chain
Monte Carlo (MCMC) methods.This document will explain the basic usage of
the package, specification of parameters, and the output of the model.

## Data Structure

The algorithms expect a longitudinal data set with the following
columns: \* `facility`: The facility where the event occurred. \*
`unit`: The unit within the facility where the event occurred. \*
`time`: The time at which the event occurred. \* `patient`: The patient
involved in the event. \* `type`: The type of event.

The package includes a simulated dataset, `simulated.data`.

``` r
pillar::glimpse(simulated.data)
#> Rows: 8,360
#> Columns: 5
#> $ facility <int> 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1…
#> $ unit     <int> 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1…
#> $ time     <dbl> 0.060978, 0.061978, 1.560978, 2.883323, 2.884323, 1.422631, 1…
#> $ patient  <int> 1, 1, 1, 1, 1, 10, 10, 10, 10, 19, 19, 19, 23, 23, 23, 28, 28…
#> $ type     <int> 0, 1, 10, 1, 3, 0, 1, 1, 3, 0, 1, 3, 0, 1, 3, 0, 1, 1, 3, 0, …
```

There are 12 different types of events that can be specified in the
`type` column. These are, expected numerical codes shown in parentheses:

- Admission (0) and Discharge (3)
- Surveillance Testing Results
  - Negative Test (1)
  - Positive Test (2)
- Clinical Testing Results
  - Negative Test (4)
  - Positive Test (5)
- Generic Testing
  - Negative Test (7)
  - Positive Test (8)
- Antibiotic Use
  - single dose (9)
  - Start (10)
  - Stop (11)
- Isolation Procedures
  - Start (6)
  - Stop (7)

Not all events need to be used in every data set, but the model selected
should reflect the data that is available. Care should be taken to
correctly code the data. The `EventToCode` and `CodeToEvent` functions
can be used to convert between.

``` r
table(CodeToEvent(simulated.data$type))
#> 
#>      abxoff       abxon   admission   discharge negsurvtest possurvtest 
#>         297         725        2183        2183        2749         223
```

## Model Specifiction

### Model Choice

Since the model is implemented in C++ for speed and efficiency, only the
specified models can be used. The currently implemented models are:

- `"LinearAbxModel"`, A Linear model with antibiotic use as a covariate.
- `"MixedModel"`, and
- `"LogNormalModel"`

Model specification and all parameters are controlled through
constructor functions of the same name, or generically through the
[`LogNormalModelParams()`](https://epiforesite.github.io/bayesian-transmission/reference/LogNormalModelParams.md)
function.

For all models there is the choice of either a 2 state (susceptible and
colonized) or 3 state (susceptible, colonized, and recovered or latent)
model. Number of states is set through the `nstates` parameter, and the
number of states in the model overrides what may be specified in any
individual component.

### Parameters

The remainder of the parameters are grouped into the following
categories:

- `Abx`, Antibiotic use,
- `AbxRate`, Antibiotic rates,
- `InUnit`, In unit infection rates,
- `OutOfUnitInfection`, Out of unit infection rates,
- `Insitu`, In situ parameters,
- Testing:
  - `SurveilenceTest`, Surveillance testing,
  - `ClinicalTest`, Clinical testing.

Unless otherwise specified the parameters are all distributed gamma with
specified shape and rate parameters. Each parameter can also be left as
fixed or be sampled at each iteration of the MCMC.

#### Specifying parameters

Parameters for the model may be specified by the
[`Param()`](https://epiforesite.github.io/bayesian-transmission/reference/Param.md)
function. This function takes up to four arguments:

    1. `init`, is the initial value of the parameter.
    2. `weight`, is the weight of the prior distribution in updates.
    3. `update`, a flag of if the parameter should be sampled in the MCMC algorithm. 
        `FALSE` indicates that the parameter should be fixed, and is by default `TRUE` when `weight` is greater than zero.
    4. `prior`, the mean of the prior distribution.  Taken with the weight will fully parameterize the distribution.

``` r
# Fully specified parameter.
Param(init = 0, weight = 1, update = TRUE, prior = 0.5)
# Fixed parameter
# Weight = 0 implies update=FALSE and prior is ignored.
Param(0, 0)
# Update parameter that starts at zero.
Param(0, weight =1, update=TRUE)
# Parameters specified at zero implies fixed.
Param(0)
```

### `Abx` Antibiotic use

Antibiotic use is specified by the `Abx` parameter. This parameter is a
list constructed with the
[`AbxParams()`](https://epiforesite.github.io/bayesian-transmission/reference/AbxParams.md)
function with the following components:

- `onoff`, If antibiotics are being used or not. The two following
  parameters are only used if `onoff` is `TRUE`.
- `delay`, the delay for the antibiotic to take effect.
- `life`, the duration where the antibiotic to be effective.

``` r
abx <- AbxParams(onoff = TRUE, delay = 0.3, life = 1)
```

Currently, all antibiotics are assumed to be equally effective and have
the same duration of effectiveness.

### `AbxRate` Antibiotic rates

The `AbxRate` parameter control the antibiotic administration rates.

``` r
abxrate <- AbxRateParams(
  # Uncolonized patients do use antibiotics but at a low rate.
  uncolonized = 0.05,  
  # Colonized patients use antibiotics at a high rate.
  colonized = 0.7      
)
```

Here since both parameters are non-zero both will be updated. A rate of
zero for either would indicate that group would never be on antibiotics.

### `InUnit` In unit infection rate

Transmission within unit is the main defining characteristic that
differentiates models. For example the linear antibiotic model,
[`LinearAbxModel()`](https://epiforesite.github.io/bayesian-transmission/reference/LogNormalModelParams.md),
is differentiated from the log normal model,
[`LogNormalModelParams()`](https://epiforesite.github.io/bayesian-transmission/reference/LogNormalModelParams.md)
by the use of a
[`ABXInUnitParams()`](https://epiforesite.github.io/bayesian-transmission/reference/InUnitParams.md)
for the `InUnit` argument rather than the `LogNormalInUnitAcquisition()`
which does not take into account antibiotic use. All in unit
transmission is defined in terms of acquisition, progression, and
clearance.

#### Aqcuisition Model

In the base log normal antibiotic model,
`LogNormalABXInUnitParameters()` log acquisition probability is a linear
function.

$$\log\left( P\left( {Acq(t)} \right) \right) = \beta_{0} + \beta_{t}\left( t - t_{0} \right) + \beta_{c}N_{c}(t) + \beta_{ca}N_{ca}(t) + \beta_{A}A_{i}(t) + \beta_{E}E_{i}(t)$$

Where $\beta_{\star}$ represents the coefficient corresponding to the
amounts, $N_{c}(t)$ represent the total number of colonized patients at
time $t$, $N_{ca}(t)$ the number of colonized on antibiotics, and
$A_{i}(t)$ and $E_{i}(t)$ represents if patient $i$ is currently or ever
on antibiotics.

The linear antibiotic (`LinearAbxAcquisitionParams`) takes a more
complicated form for the acquisition model.

$$P\left( {Acq(t)} \right) = \left\lbrack e^{\beta_{time}{(t - t_{0})}} \right\rbrack\left\{ e^{\beta_{0}}\left\lbrack \left( \frac{\beta_{freq}}{P(t)} + \left( 1 - e^{\beta_{freq}} \right) \right)e^{\beta_{mass}}\left( \left( N_{c}(t) - N_{ca}(t) \right) + e^{\beta_{col\_ abx}}N_{ca}(t) \right) + 1 - e^{\beta_{mass}} \right\rbrack \right\}\left\lbrack N_{S}(t) - N_{E}(t) + e^{\beta_{suss\_ ever}}\left( \left( E_{i}(t) - A_{i}(t) \right) + A_{i}(t)e^{\beta_{suss\_ abx}} \right) \right\rbrack$$

``` r
acquisition <- LinearAbxAcquisitionParams(
    base = Param(0.01),     #< Base acquisition rate (Updated)
    time = Param(1, 0),     #< Time effect (Fixed)
    mass = Param(1),        #< Mass Mixing (Updated)
    freq = Param(1),        #< Frequency/Density effect (Updated)
    col_abx = Param(1, 0),  #< Colonized on antibiotics (Fixed)
    suss_abx = Param(1, 0), #< Susceptible on antibiotics (Fixed)
    suss_ever = Param(1, 0) #< Ever on antibiotics (Fixed)  
)
```

#### Progression Model

In the 3 state model there is a latent state and the progression model
controls how patient transition out of the latent state. The base rate
can be affected by currently being on antiboitics or ever being on
antbiotcs.

$$\log\left( P\left( {progression} \right) \right) = \delta_{0} + \delta_{A}A_{i}(t) + \delta_{E}E_{i}(t)$$
the linear antibiotic model is:

$$P\left( {progression} \right) = e^{\delta_{0}}\left\lbrack 1 - E_{i}(t) + e^{\delta_{2}}\left( E_{i}(t) - A_{i}(t) + e^{\delta_{1}}A_{i}(t) \right) \right\rbrack$$

Where here we use $\delta$ for the coefficients, but the notation is the
same.

``` r
progression <- ProgressionParams(
    rate = Param(0.01),     #< Base progression rate (Updated)
    abx  = Param(1, 0),     #< Currently on antibiotics (Fixed)
    ever = Param(1, 0)      #< Ever on antibiotics (Fixed)
)
```

#### Clearance Model

The clearance model is the same as the progression model in both the log
normal and the linear cases, the coefficients however are independent.

``` r
clearance <- ClearanceParams(
    rate = Param(0.01),     #< Base clearance rate (Updated)
    abx  = Param(1, 0),     #< Currently on antibiotics (Fixed)
    ever = Param(1, 0)      #< Ever on antibiotics (Fixed)
)
```

``` r
inunit <- ABXInUnitParams(
  acquisition = acquisition,
  progression = progression,
  clearance   = clearance
)
```

### Out of Unit Importation

The out of unit parameters control the rate at which admissions come in,
and which state they enter in.

$$\log\left( P\left( {state}_{i}\rightarrow{state}_{j} \right)|t \right) = P_{j} - Q_{i,j}e^{- t\sum\limits_{i}r_{i}}$$

``` r
outcol <- OutOfUnitInfectionParams(
  acquisition = 0.1,
  clearance = 0.5
)
```

### In Situ

I’m not sure what these parameters do. It’s a set of gamma distributed
parameters one for each state. The updates and probabilities are not
time dependent.

When updating the rates for each state are sampled from a
$gamma\left( N_{i},1 \right)$ distribution. Then all three are
normalized to sum to 1.

``` r
insitu <- InsituParams(
  # Starting 90/10 split uncolonized to colonized
  # For 2-state model, latent probability is 0
  probs = c(uncolonized = 0.90,
            latent = 0.0,
            colonized = 0.10),
  # Prior values for Bayesian updating  
  priors = c(1, 1, 1),
  # Which states to update (latent is fixed at 0 for 2-state model)
  doit = c(TRUE, FALSE, TRUE)
)
```

### Testing

There are two types of testing, surveillance, which is conducted
routinely at regular intervals such as on admission then every 3 days
after, and clinical, where the testing is precipitated by staff, and
thus the timing is informative.

#### Surveillance Testing

The timing of surveillance testing is assumed to not be informative.
Therefore, surveillance testing is only parameterized in terms of
probability of a positive test given the underlying status. Surveillance
test parameters are updated with a sample from a
$Beta\left( N_{s,1},N_{s,0} \right)$ distribution where $N_{s,1}$ and
$N_{s,0}$ are the number of positive and negative tests respectively for
state $s$.

``` r
surv <- SurveillanceTestParams(
    # Probability of a positive test when uncolonized (false positive rate)
    # Use small non-zero value to avoid -Inf likelihood if data has any false positives
    uncolonized = Param(0.01, 1),
    # Probability of a positive test when colonized (true positive rate/sensitivity)
    colonized = Param(0.9, 1),
    # Latent state (for 2-state model, this is not used but must be specified)
    latent = Param(0, 0)
)
```

#### Clinical Testing

Since clinical testing time is informative, clinical testing is assumed
to be at random within infection stage. The rate of testing within each
stage is sampled from a gamma distribution. Sensitivity/Specificity are
handled the same as surveillance testing and the likelihood is
multiplicative between rate and effectiveness.

``` r
clin <- ClinicalTestParams(
    # Rate of testing when uncolonized
    uncolonized = ParamWRate(Param(0.5, 0), rate = Param(1, 0)),
    # Rate of testing when colonized  
    colonized = ParamWRate(Param(0.5, 0), rate = Param(1, 0)),
    # Latent state (for 2-state model, not used but must be specified)
    latent = ParamWRate(Param(0, 0), rate = Param(0, 0))
)
```

#### All Together

``` r
params <- LinearAbxModel(
  nstates = 2,
  Insitu = insitu,
  SurveillanceTest = surv,
  ClinicalTest = clin,
  OutOfUnitInfection = outcol,
  InUnit = inunit,
  Abx = abx,
  AbxRate = abxrate
)
```

## Running the Model

The model is run through the
[`runMCMC()`](https://epiforesite.github.io/bayesian-transmission/reference/runMCMC.md)
function. This function takes the following arguments:

``` r
results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(
    nburn = 100,
    nsims = 1000,
    outputparam = TRUE,
    outputfinal = TRUE
  ),
  modelParameters = params,
  verbose = FALSE
)
```

## Analyzing MCMC Results

### Converting Parameters to Data Frame

The `results$Parameters` object contains the MCMC chain of all model
parameters. To create trace plots and posterior distributions, we use
the
[`mcmc_to_dataframe()`](https://epiforesite.github.io/bayesian-transmission/reference/mcmc_to_dataframe.md)
function to convert this nested list structure into a tidy data frame
format.

``` r
# Convert parameters to data frame using package function
param_df <- mcmc_to_dataframe(results)

# Display first few rows
head(param_df)
#>   iteration insitu_uncolonized insitu_colonized surv_test_uncol_neg
#> 1         1         0.36352949        0.6364705                   1
#> 2         2         0.08540797        0.9145920                   1
#> 3         3         0.33773365        0.6622664                   1
#> 4         4         0.43523142        0.5647686                   1
#> 5         5         0.69401926        0.3059807                   1
#> 6         6         0.73077686        0.2692231                   1
#>   surv_test_col_neg surv_test_uncol_pos surv_test_col_pos clin_test_uncol
#> 1                 0                   1                 0             0.5
#> 2                 0                   1                 0             0.5
#> 3                 0                   1                 0             0.5
#> 4                 0                   1                 0             0.5
#> 5                 0                   1                 0             0.5
#> 6                 0                   1                 0             0.5
#>   clin_test_col clin_rate_uncol clin_rate_col outunit_acquisition
#> 1             0               1             0          0.04872909
#> 2             0               1             0          0.04872909
#> 3             0               1             0          0.04872909
#> 4             0               1             0          0.04872909
#> 5             0               1             0          0.04872909
#> 6             0               1             0          0.04872909
#>   outunit_clearance inunit_base inunit_time inunit_mass inunit_freq
#> 1         0.0258701 0.002423405           1           1           1
#> 2         0.0258701 0.002697885           1           1           1
#> 3         0.0258701 0.002706596           1           1           1
#> 4         0.0258701 0.002598325           1           1           1
#> 5         0.0258701 0.002510386           1           1           1
#> 6         0.0258701 0.002608298           1           1           1
#>   inunit_colabx inunit_susabx inunit_susever  inunit_clr inunit_clrAbx
#> 1             1             1              1 0.001853747             1
#> 2             1             1              1 0.001824026             1
#> 3             1             1              1 0.001689453             1
#> 4             1             1              1 0.001566023             1
#> 5             1             1              1 0.001663549             1
#> 6             1             1              1 0.001480498             1
#>   inunit_clrEver abxrate_uncolonized abxrate_colonized loglikelihood
#> 1              1             1054791          629771.8          -Inf
#> 2              1             1166696          711553.9          -Inf
#> 3              1             1251557          677976.5          -Inf
#> 4              1             1102664          658573.4          -Inf
#> 5              1             1097877          695118.3          -Inf
#> 6              1             1171478          656579.7          -Inf
```

### Trace Plots

Trace plots show the evolution of parameters across MCMC iterations,
helping to assess convergence.

``` r
library(ggplot2)
library(tidyr)

# Select key parameters for trace plots
trace_params <- param_df[, c("iteration", "insitu_colonized", "surv_test_col_pos", 
                              "outunit_acquisition", "inunit_base", 
                              "abxrate_colonized", "loglikelihood")]

# Convert to long format
trace_long <- pivot_longer(trace_params, 
                           cols = -iteration,
                           names_to = "parameter",
                           values_to = "value")

# Create trace plots
ggplot(trace_long, aes(x = iteration, y = value)) +
  geom_line() +
  facet_wrap(~parameter, scales = "free_y", ncol = 2) +
  theme_minimal() +
  labs(title = "MCMC Trace Plots",
       x = "Iteration",
       y = "Parameter Value")
```

![](bayesian-transmission_files/figure-html/unnamed-chunk-17-1.png)

### Posterior Distributions

Posterior distributions show the estimated distribution of each
parameter after the MCMC sampling.

``` r
# Remove burn-in if needed (here we already set nburn in the MCMC call)
# For demonstration, let's use all samples since nburn=0 was specified

# Create density plots for posterior distributions
ggplot(trace_long, aes(x = value)) +
  geom_density(fill = "steelblue", alpha = 0.5) +
  geom_vline(aes(xintercept = mean(value, na.rm = TRUE)), 
             color = "red", linetype = "dashed") +
  facet_wrap(~parameter, scales = "free", ncol = 2) +
  theme_minimal() +
  labs(title = "Posterior Distributions",
       subtitle = "Red dashed line shows posterior mean",
       x = "Parameter Value",
       y = "Density")
#> Warning: Removed 1000 rows containing non-finite outside the scale range
#> (`stat_density()`).
```

![](bayesian-transmission_files/figure-html/unnamed-chunk-18-1.png)

### Summary Statistics

``` r
# Calculate summary statistics for each parameter
library(dplyr)
#> 
#> Attaching package: 'dplyr'
#> The following objects are masked from 'package:stats':
#> 
#>     filter, lag
#> The following objects are masked from 'package:base':
#> 
#>     intersect, setdiff, setequal, union

summary_stats <- trace_long %>%
  group_by(parameter) %>%
  summarise(
    mean = mean(value, na.rm = TRUE),
    median = median(value, na.rm = TRUE),
    sd = sd(value, na.rm = TRUE),
    q025 = quantile(value, 0.025, na.rm = TRUE),
    q975 = quantile(value, 0.975, na.rm = TRUE),
    .groups = "drop"
  )

print(summary_stats)
#> # A tibble: 6 × 6
#>   parameter                    mean       median        sd       q025       q975
#>   <chr>                       <dbl>        <dbl>     <dbl>      <dbl>      <dbl>
#> 1 abxrate_colonized   659427.            6.59e+5   3.37e+4    5.91e+5    7.26e+5
#> 2 insitu_colonized         0.494         4.82e-1   2.92e-1    2.73e-2    9.78e-1
#> 3 inunit_base              0.000451      3.41e-4   3.69e-4    1.35e-4    1.55e-3
#> 4 loglikelihood         -Inf          -Inf       NaN       -Inf       -Inf      
#> 5 outunit_acquisition      0.00229       1.24e-4   6.09e-3    5.13e-5    1.64e-2
#> 6 surv_test_col_pos        0             0         0          0          0    
```
