## ----include = FALSE----------------------------------------------------------
knitr::opts_chunk$set(
  collapse = TRUE,
  comment = "#>"
)

## ----setup--------------------------------------------------------------------
library(bayestransmission)

## -----------------------------------------------------------------------------
pillar::glimpse(simulated.data)

## -----------------------------------------------------------------------------
table(CodeToEvent(simulated.data$type))

## ----paramexamples, results='hide'--------------------------------------------
# Fully specified parameter.
Param(init = 0, weight = 1, update = TRUE, prior = 0.5)
# Fixed parameter
# Weight = 0 implies update=FALSE and prior is ignored.
Param(0, 0)
# Update parameter that starts at zero.
Param(0, weight =1, update=TRUE)
# Parameters specified at zero implies fixed.
Param(0)

## -----------------------------------------------------------------------------
abx <- AbxParams(onoff = TRUE, delay = 0.3, life = 1)

## -----------------------------------------------------------------------------
abxrate <- AbxRateParams(
  # Uncolonized patients do use antibiotics but at a low rate.
  uncolonized = 0.05,  
  # Colonized patients use antibiotics at a high rate.
  colonized = 0.7      
)

## -----------------------------------------------------------------------------
acquisition <- LinearAbxAcquisitionParams(
    base = Param(0.01),     #< Base acquisition rate (Updated)
    time = Param(1, 0),     #< Time effect (Fixed)
    mass = Param(1),        #< Mass Mixing (Updated)
    freq = Param(1),        #< Frequency/Density effect (Updated)
    col_abx = Param(1, 0),  #< Colonized on antibiotics (Fixed)
    suss_abx = Param(1, 0), #< Susceptible on antibiotics (Fixed)
    suss_ever = Param(1, 0) #< Ever on antibiotics (Fixed)  
)

## -----------------------------------------------------------------------------
progression <- ProgressionParams(
    rate = Param(0.01),     #< Base progression rate (Updated)
    abx  = Param(1, 0),     #< Currently on antibiotics (Fixed)
    ever = Param(1, 0)      #< Ever on antibiotics (Fixed)
)

## -----------------------------------------------------------------------------
clearance <- ClearanceParams(
    rate = Param(0.01),     #< Base clearance rate (Updated)
    abx  = Param(1, 0),     #< Currently on antibiotics (Fixed)
    ever = Param(1, 0)      #< Ever on antibiotics (Fixed)
)

## -----------------------------------------------------------------------------
inunit <- ABXInUnitParams(
  acquisition = acquisition,
  progression = progression,
  clearance   = clearance
)

## -----------------------------------------------------------------------------
outcol <- OutOfUnitInfectionParams(
  acquisition = 0.1,
  clearance = 0.5
)

## -----------------------------------------------------------------------------
insitu <- InsituParams(
  # Starting 90/10 split uncolonized to colonized  
  probs = c(uncolonized = 0.90, #< names are unnecessary.
              colonized = 0.10)
)

## -----------------------------------------------------------------------------
surv <- SurveillanceTestParams(
    # Probability of a positive test when uncolonized
    uncolonized = Param(0),
    # Probability of a positive test when colonized
    colonized = Param(0.9, 1, TRUE)
)


## -----------------------------------------------------------------------------
clin <- ClinicalTestParams(
    # Rate of testing when uncolonized
    uncolonized = ParamWRate(Param(0, 0), rate = Param(1)),
    # Rate of testing when colonized
    colonized = ParamWRate(Param(0, 0), rate = Param(1))
)

## -----------------------------------------------------------------------------
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

## -----------------------------------------------------------------------------
results <- runMCMC(
  data = simulated.data,
  MCMCParameters = list(
    nburn = 0,
    nsims = 100,
    outputparam = TRUE,
    outputfinal = TRUE
  ),
  modelParameters = params,
  verbose = TRUE
)

