#' Construct a parameter with a prior, weight and an update flag.
#'
#' @param init the initial value of the parameter.
#' @param weight the weight of the prior.
#' @param update a flag indicating if the parameter shouldbe updated in the MCMC.
#' @param prior mean value of the prior distribution, may be used with weight to fully determine prior parameters.
#'
#' @returns A list with the following elements:
#'  * `init` the initial value of the parameter.
#'  * `weight` the weight of the prior.
#'  * `update` a flag indicating if the parameter shouldbe updated in the MCMC.
#'  * `prior` mean value of the prior distribution, may be used with weight to fully determine prior parameters.
#'
#' @export
#'
#' @examples
#' # Fully specified parameter.
#' Param(init = 0, weight = 1, update = TRUE, prior = 0.5)
#' # Fixed parameter
#' # Weight = 0 implies update=FALSE and prior is ignored.
#' Param(0, 0)
#' # Update parameter that starts at zero.
#' Param(0, weight =1, update=TRUE)
#' # Parameters specified at zero implies fixed.
#' Param(0)
Param <- function(init, weight = if_else(init == 0, 0, 1), update = weight > 0, prior = init) {
  structure(
    list(
      init = init,
      update = update,
      prior = prior,
      weight = weight
    ),
    class = "Param"
  )
}
is_valid_param <- function(x) {
  assertthat::see_if(
    is.list(x),
    length(x) == 4,
    all(c("init", "weight", "update", "prior") %in% names(x)),
    is.numeric(x$init),
    is.numeric(x$weight),
    is.logical(x$update),
    is.numeric(x$prior)
  )
}

check_param <- function(param, name = deparse(substitute(param))) {
  if (rlang::is_scalar_double(param)) {
    param <- Param(param)
  } else {
    assertthat::assert_that(is_valid_param(param))
  }
  return(param)
}


#' Specify a random testing parameter with a rate.
#'
#' @param param Values for the positive rate of the test.
#' @param rate Values for the rate of the test.
#'
#' @returns A list of with param and rate.
#' @export
#'
#' @examples
#' ParamWRate(Param(0.5, 0), rate = Param(1, 0)
ParamWRate <- function(param = Param(), rate = Param()) {
  if (!inherits(param, "Param") || !inherits(rate, "Param")) {
    if (rlang::is_scalar_double(param)) {
      param <- Param(param)
    } else {
      stop("param and rate should be of class Param")
    }
    if (rlang::is_scalar_double(rate)) {
      rate <- Param(rate)
    } else {
      stop("param and rate should be of class Param")
    }
  }
  list(
    param = param,
    rate = rate
  )
}
check_paramwrate <- function(x, name = deparse(substitute(x))) {
  assertthat::assert_that(is.list(x),
              x %has_name% 'param',
              x %has_name% 'rate')
  x$param <- check_param(x$param, paste0(name, "$parameter"))
  x$rate <- check_param(x$rate, paste0(name, "$rate"))
  return(x)
}

#' InSitu Parameters
#'
#' @param probs The probability of the individual being in each state.
#' @param priors The prior probability of the individual being in each state.
#' @param doit A flag indicating if the rate(s) should be updated in the MCMC.
#'
#' @returns A list of parameters for in situ testing.
#' @export
#'
#' @examples
#' InsituParams()
InsituParams <- function(probs = c(0.5, 0.5, 0), priors = probs * doit, doit = probs != 0) {
  stopifnot(2 <= length(probs) && length(probs) <= 3)
  if (length(probs) == 2) probs <- c(probs, 0)
  list(
    probs = probs,
    priors = priors,
    doit = doit
  )
}


TestParams <- function(n, probs = c(0.5, 0.5, 0), priors = probs, doit = probs != 0) {
  stopifnot(2 <= length(probs) && length(probs) <= 3)
  if (length(probs) == 2) probs <- c(probs, 0)
  stopifnot(
    length(n) == length(probs),
    length(n) == length(priors),
    length(n) == length(doit)
  )

  list(
    probs = probs,
    priors = priors,
    doit = doit,
    n = n
  )
}

#' Random Testing Parameter Set
#'
#' @param uncolonized Testing when the individual is uncolonized.
#' @param colonized Testing when the individual is colonized.
#' @param latent Testing when the individual is latent.
#'
#' @returns list of parameters for random testing.
#' @export
#'
#' @examples
#' RandomTestParams()
RandomTestParams <- function(
    uncolonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
    colonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
    latent = ParamWRate(Param(0), Param(0))) {
  uncolonized <- check_paramwrate(uncolonized)
  colonized <- check_paramwrate(colonized)
  latent <- check_paramwrate(latent)

  list(
    uncolonized = uncolonized,
    colonized = colonized,
    latent = latent
  )
}
#' @describeIn RandomTestParams Clinical Test Parameters Alias
ClinicalTestParams <- RandomTestParams


#' Out of Unit Infection Parameters
#'
#' Effects on patients between hospital visits.
#'
#' @param acquisition Rate of acquisition of the disease moving into latent state.
#' @param clearance Rate of clearance of the disease moving into uncolonized state.
#' @param progression Rate of progression of the disease moving into colonized state.
#'
#' @returns A list of parameters for out of unit infection.
#' @export
#'
#' @examples
#' OutOfUnitInfectionParams()
OutOfUnitInfectionParams <- function(
    acquisition = Param(0),
    clearance = Param(0),
    progression = Param(0)) {
  acquisition <- check_param(acquisition)
  clearance <- check_param(clearance)
  progression <- check_param(progression)
  list(
    acquisition = acquisition,
    clearance = clearance,
    progression = progression
  )
}

#' Surveillance Test Parameters
#'
#' Specify the rates of positive tests for each state of the model.
#'
#' @param colonized Also known as the true positive rate for a two state model.
#' @param uncolonized Also known as the false positive rate for a two state model.
#' @param latent The rate of positive tests when the individual is in the (optional) latent state.
#' @export
#' @examples
#' SurveillanceTestParams()
SurveillanceTestParams <- function(
    colonized = Param(0, 0),
    uncolonized = Param(0.8, 1),
    latent = Param(0, 0)) {
  colonized <- check_param(colonized, "colonized")
  uncolonized <- check_param(uncolonized, "uncolonized")
  latent <- check_param(latent, "latent")
  list(
    uncolonized = uncolonized,
    latent = latent,
    colonized = colonized
  )
}

#' Surveillance Test Parameters with Antibiotics
#'
#' Specify the rates of positive tests for each state of the model.
#'
#' @param uncolonized_off,uncolonized_on The probability of receiving a positive test when uncolonized, without or with antibiotic use, respectively.
#' @param colonized_off,colonized_on The probability of receiving a positive test when colonized, without or with antibiotic use, respectively.
#' @param latent_off,latent_on The probability of receiving a positive test when in latent state, without or with antibiotic use, respectively.
#' @export
#' @examples
#' SurveillanceTestParamsAbx()
SurveillanceTestParamsAbx <- function(
    colonized_off = Param(0, 0),
    colonized_on = Param(0, 0),
    uncolonized_off = Param(0.8, 1),
    uncolonized_on = Param(0.8, 1),
    latent_off = Param(0, 0),
    latent_on = Param(0, 0)) {
  colonized_off <- check_param(colonized_off, "colonized")
  colonized_on <- check_param(colonized_on, "colonized")
  uncolonized_off <- check_param(uncolonized_off, "uncolonized")
  uncolonized_on <- check_param(uncolonized_on, "uncolonized")
  latent_off <- check_param(latent_off, "latent")
  latent_on <- check_param(latent_on, "latent")
  list(
    uncolonized = list(off=uncolonized_off,on=uncolonized_on),
    latent = list(off=latent_off,on=latent_on),
    colonized = list(off=colonized_off,on=colonized_on)
  )
}

#' Antibiotic Parameters
#'
#' @param onoff The mode of the antibiotics, dose with delay and life vs on/off events.
#' @param delay The delay in using antibiotics.
#' @param life The life of antibiotics.
#' @export
#'
#' @examples
#' AbxParams()
AbxParams <- function(
    onoff = FALSE,
    delay = 0.0,
    life = 1.0) {
  list(
    onoff = onoff,
    delay = delay,
    life = life
  )
}


#' Antibiotic Administration Rate Parameters
#'
#' @param uncolonized Rate of antibiotic administration when the individual is uncolonized.
#' @param colonized Rate of antibiotic administration when the individual is colonized.
#' @param latent Rate of antibiotic administration when the individual is latent.
#'
#' @returns list of parameters for antibiotic administration.
#' @export
#'
#' @examples
#' AbxRateParams()
AbxRateParams <- function(
    uncolonized = Param(1, 0),
    colonized = Param(1, 0),
    latent = Param(0)) {
  uncolonized <- check_param(uncolonized)
  colonized <- check_param(colonized)
  latent <- check_param(latent)
  list(
    uncolonized = uncolonized,
    colonized = colonized,
    latent = latent
  )
}

#' Linear Antibiotic Acquisition Parameters
#'
#' The model for this acquisition model is given by
#'
#' \deqn{
#' P(\mathrm{Acq(t)}) =
#'     \left[e^{\beta_\mathrm{time}(t-t_0)}\right]\\
#' \left\{e^{\beta_0}
#'     \left[
#'         \left(\frac{\beta_\mathrm{freq}}{P(t)}+(1 - e^{\beta_\mathrm{freq}})\right)
#'         e^{\beta_\mathrm{mass}}\left(
#'             (N_c(t) - N_{ca}(t)) + e^{\beta_\mathrm{col\_abx}}N_{ca}(t)
#'             \right)
#'         + 1 - e^{\beta_\mathrm{mass}}
#'         \right]
#'     \right\}\\
#' \left[
#'     N_S(t) - N_E(t) + e^{\beta_\mathrm{suss\_ever}}\left(\left(E_i(t)-A_i(t)\right) +A_i(t)e^{\beta_\mathrm{suss\_abx}}\right)
#'     \right]
#' }{TODO: Add equation to the documentation}
#'
#' @param base The base rate of acquisition.
#' @param time The time effect on acquisition.
#' @param mass The mass action effect on acquisition.
#' @param freq The frequency effect on acquisition.
#' @param col_abx The effect for colonized on antibiotics.
#' @param suss_abx The effect on susceptible being currently on antibiotics.
#' @param suss_ever The effect on susceptible ever being on antibiotics.
#'
#' @returns A list of parameters for acquisition.
#' @export
#'
#' @examples
#' LinearAbxAcquisitionParams()
LinearAbxAcquisitionParams <- function(
    base = Param(0.01),
    time = Param(1, 0),
    mass = Param(1),
    freq = Param(1),
    col_abx = Param(1, 0),
    suss_abx = Param(1, 0),
    suss_ever = Param(1, 0)) {
  list(
    base = check_param(base),
    time = check_param(time),
    mass = check_param(mass),
    freq = check_param(freq),
    col_abx = check_param(col_abx),
    suss_abx = check_param(suss_abx),
    suss_ever = check_param(suss_ever)
  )
}

#' Progression Parameters
#'
#' @param rate Base progression rate
#' @param abx  Effect of current antibiotics on progression
#' @param ever_abx Effect of ever having taken antibiotics on progression
#'
#' @returns A list of parameters for progression.
#' @export
#'
#' @examples
#' ProgressionParams()
ProgressionParams <- function(
    rate = Param(0.01),
    abx = Param(1, 0),
    ever_abx = Param(1, 0)) {
  list(
    rate = rate,
    abx = abx,
    ever_abx = ever_abx
  )
}

#' Clearance Parameters
#'
#' @param rate base rate of clearance
#' @param abx effect of antibiotics on clearance
#' @param ever_abx effect of ever having taken antibiotics on clearance
#'
#' @returns A list of parameters for clearance.
#' @export
#'
#' @examples
#' ClearanceParams()
ClearanceParams <- function(
    rate = Param(0.01),
    abx = Param(1, 0),
    ever_abx = Param(1, 0)) {
  list(
    rate = rate,
    abx = abx,
    ever_abx = ever_abx
  )
}


#' In Unit Parameters
#'
#' @param acquisition Acquisition, for rate of acquisition of the disease moving into latent state.
#' @param progression Progression from latent state to colonized state.
#' @param clearance Clearance from colonized state to uncolonized state.
#'
#' @export
#' @examples
#' InUnitParameters()
InUnitParams <- function(
    acquisition = AcquisitionParams(),
    progression = ProgressionParams(),
    clearance = ClearanceParams()) {
  assertthat::assert_that(
    is.list(acquisition),
    is.list(progression),
    is.list(clearance)
  )
  list(
    acquisition = acquisition,
    progression = progression,
    clearance = clearance
  )
}

#' @describeIn InUnitParams In Unit Parameters with Antibiotics.
#' @export
#' @examples
#' ABXInUnitParameters()
ABXInUnitParams <- function(
    acquisition = LinearAbxAcquisitionParams(),
    progression = ProgressionParams(),
    clearance = ClearanceParams()) {
  InUnitParams(
    acquisition = acquisition,
    progression = progression,
    clearance = clearance
  )
}


#' Model Parameters for a Log Normal Model
#'
#' @param modname The name of the model used. Usually specified by specification functions.
#' @param nstates The number of states in the model.
#' @param nmetro The number of Metropolis-Hastings steps to take between outputs.
#' @param forward TODO
#' @param cheat TODO
#' @param Insitu In Situ Parameters
#' @param SurveillanceTest Surveillance Testing Parameters
#' @param ClinicalTest Clinical Testing Parameters
#' @param OutOfUnitInfection Out of Unit Infection Parameters
#' @param InUnit In Unit Parameters, should be a list of lists with parameters
#'               for the acquisition, progression and clearance of the disease.
#' @param Abx Antibiotic Parameters
#' @param AbxRate Antibiotic Rate Parameters
#'
#' @returns A list of parameters for the model.
#' @export
#'
#' @examples
#' LogNormalModelParams()
LogNormalModelParams <-
  function(modname,
           nstates = 2L,
           nmetro = 1L,
           forward = TRUE,
           cheat = FALSE,
           Insitu = InsituParams(),
           SurveillanceTest = SurveillanceTestParams(),
           ClinicalTest = ClinicalTestParams(),
           OutOfUnitInfection = OutOfUnitInfectionParams(),
           InUnit = InUnitParameters(),
           Abx = AbxParams(),
           AbxRate = AbxRateParams()) {
    assertthat::assert_that(
      assertthat::is.string(modname),
      assertthat::is.count(nstates),
      assertthat::is.count(nmetro),
      assertthat::is.flag(forward),
      assertthat::is.flag(cheat)
    )

    list(
      modname = modname,
      nstates = as.integer(nstates),
      nmetro = as.integer(nmetro),
      forward = as.logical(forward),
      cheat = cheat,
      Insitu = Insitu,
      SurveillanceTest = SurveillanceTest,
      ClinicalTest = ClinicalTest,
      OutCol = OutOfUnitInfection,
      InCol = InUnit,
      Abx = Abx,
      AbxRate = AbxRate
    )
  }


#' @describeIn LogNormalModelParams Linear Antibiotic Model Alias
#' @export
LinearAbxModel <- function(
    ...,
    InUnit = ABXInUnitParams()) {
    LogNormalModelParams("LinearAbxModel", ..., InUnit = InUnit)
}
