pkgname <- "bayestransmission"
source(file.path(R.home("share"), "R", "examples-header.R"))
options(warn = 1)
library('bayestransmission')

base::assign(".oldSearch", base::search(), pos = 'CheckExEnv')
base::assign(".old_wd", base::getwd(), pos = 'CheckExEnv')
cleanEx()
nameEx("AbxParams")
### * AbxParams

flush(stderr()); flush(stdout())

### Name: AbxParams
### Title: Antibiotic Parameters
### Aliases: AbxParams

### ** Examples

AbxParams()



cleanEx()
nameEx("AbxRateParams")
### * AbxRateParams

flush(stderr()); flush(stdout())

### Name: AbxRateParams
### Title: Antibiotic Administration Rate Parameters
### Aliases: AbxRateParams

### ** Examples

AbxRateParams()



cleanEx()
nameEx("ClearanceParams")
### * ClearanceParams

flush(stderr()); flush(stdout())

### Name: ClearanceParams
### Title: Clearance Parameters
### Aliases: ClearanceParams

### ** Examples

ClearanceParams()



cleanEx()
nameEx("CodeToEvent")
### * CodeToEvent

flush(stderr()); flush(stdout())

### Name: CodeToEvent
### Title: Convert coded events to string events
### Aliases: CodeToEvent

### ** Examples

CodeToEvent(c(-1:19, 21:23, 31:33, -999))



cleanEx()
nameEx("InUnitParams")
### * InUnitParams

flush(stderr()); flush(stdout())

### Name: InUnitParams
### Title: In Unit Parameters
### Aliases: InUnitParams ABXInUnitParams

### ** Examples

InUnitParams(
  acquisition = LinearAbxAcquisitionParams(),
  progression = ProgressionParams(),
  clearance = ClearanceParams()
)
ABXInUnitParams(
  acquisition = LinearAbxAcquisitionParams(),
  progression = ProgressionParams(),
  clearance = ClearanceParams()
)



cleanEx()
nameEx("InsituParams")
### * InsituParams

flush(stderr()); flush(stdout())

### Name: InsituParams
### Title: InSitu Parameters
### Aliases: InsituParams

### ** Examples

InsituParams()
InsituParams(nstates = 2)  # [0.9, 0.0, 0.1]
InsituParams(nstates = 3)  # [0.98, 0.01, 0.01]



cleanEx()
nameEx("LinearAbxAcquisitionParams")
### * LinearAbxAcquisitionParams

flush(stderr()); flush(stdout())

### Name: LinearAbxAcquisitionParams
### Title: Linear Antibiotic Acquisition Parameters
### Aliases: LinearAbxAcquisitionParams

### ** Examples

LinearAbxAcquisitionParams()



cleanEx()
nameEx("LogNormalModelParams")
### * LogNormalModelParams

flush(stderr()); flush(stdout())

### Name: LogNormalModelParams
### Title: Model Parameters for a Log Normal Model
### Aliases: LogNormalModelParams LinearAbxModel

### ** Examples

LogNormalModelParams("LogNormalModel")



cleanEx()
nameEx("OutOfUnitInfectionParams")
### * OutOfUnitInfectionParams

flush(stderr()); flush(stdout())

### Name: OutOfUnitInfectionParams
### Title: Out of Unit Infection Parameters
### Aliases: OutOfUnitInfectionParams

### ** Examples

OutOfUnitInfectionParams()



cleanEx()
nameEx("Param")
### * Param

flush(stderr()); flush(stdout())

### Name: Param
### Title: Construct a parameter with a prior, weight and an update flag.
### Aliases: Param

### ** Examples

# Fully specified parameter.
Param(init = 0, weight = 1, update = TRUE, prior = 0.5)
# Fixed parameter
# Weight = 0 implies update=FALSE and prior is ignored.
Param(0, 0)
# Update parameter that starts at zero.
Param(0, weight =1, update=TRUE)
# Parameters specified at zero implies fixed.
Param(0)



cleanEx()
nameEx("ParamWRate")
### * ParamWRate

flush(stderr()); flush(stdout())

### Name: ParamWRate
### Title: Specify a random testing parameter with a rate.
### Aliases: ParamWRate

### ** Examples

ParamWRate(Param(0.5, 0), rate = Param(1, 0))



cleanEx()
nameEx("ProgressionParams")
### * ProgressionParams

flush(stderr()); flush(stdout())

### Name: ProgressionParams
### Title: Progression Parameters
### Aliases: ProgressionParams

### ** Examples

ProgressionParams()



cleanEx()
nameEx("RandomTestParams")
### * RandomTestParams

flush(stderr()); flush(stdout())

### Name: RandomTestParams
### Title: Random Testing Parameter Set
### Aliases: RandomTestParams ClinicalTestParams

### ** Examples

RandomTestParams()



cleanEx()
nameEx("SurveillanceTestParams")
### * SurveillanceTestParams

flush(stderr()); flush(stdout())

### Name: SurveillanceTestParams
### Title: Surveillance Test Parameters
### Aliases: SurveillanceTestParams

### ** Examples

SurveillanceTestParams()



### * <FOOTER>
###
cleanEx()
options(digits = 7L)
base::cat("Time elapsed: ", proc.time() - base::get("ptime", pos = 'CheckExEnv'),"\n")
grDevices::dev.off()
###
### Local variables: ***
### mode: outline-minor ***
### outline-regexp: "\\(> \\)?### [*]+" ***
### End: ***
quit('no')
