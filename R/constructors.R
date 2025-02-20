#' Construct a parameter with a prior, weight and an update flag.
#'
#' @param init the initial value of the parameter.
#' @param weight the weight of the prior.
#' @param update a flag indicating if the parameter shouldbe updated in the MCMC.
#' @param prior mean value of the prior distribution, may be used with weight to fully determine prior parameters.
#'
#' @returns
#' @export
#'
#' @examples
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
  x$param <- check_param(x$param, paste0(name, "$parameter"))
  x$rate <- check_param(x$rate, paste0(name, "$rate"))
  return(x)
}

#' Title
#'
#' @param probs
#' @param priors
#' @param doit
#'
#' @returns
#' @export
#'
#' @examples
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
#' @param recovered Testing when the individual is recovered.
#'
#' @returns list of parameters for random testing.
#' @export
#'
#' @examples
#' RandomTestParams()
RandomTestParams <- function(
    uncolonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
    colonized = ParamWRate(Param(0.5, 0), Param(1, 0)),
    recovered = ParamWRate(Param(0), Param(0))) {
  uncolonized <- check_paramwrate(uncolonized)
  colonized <- check_paramwrate(colonized)
  recovered <- check_paramwrate(recovered)

  list(
    uncolonized = uncolonized,
    colonized = colonized,
    recovered = recovered
  )
}
#' @describeIn RandomTestParams Clinical Test Parameters Alias
ClinicalTestParams <- RandomTestParams


#' Out of Unit Infection Parameters
#'
#' @param acquisition
#' @param clearance
#' @param thirdoption What should this be?
#'
#' @returns A list of parameters for out of unit infection.
#' @export
#'
#' @examples
OutOfUnitInfectionParams <- function(
    acquisition = Param(0),
    clearance = Param(0),
    thirdoption = Param(0)) {
  list(
    acquisition = acquisition,
    clearance = clearance,
    thirdoption = thirdoption
  )
}

#' Surveillance Test Parameters
#'
#' Specify the rates of positive tests for each state of the model.
#'
#' @param colonized Also known as the true positive rate for a two state model.
#' @param uncolonized Also known as the false positive rate for a two state model.
#' @param recovered The rate of positive tests when the individual is in the (optional) recovered state.
SurveillanceTestParams <- function(
    colonized = Param(0, 0),
    uncolonized = Param(0.8, 1),
    recovered = Param(0, 0)) {
  list(
    colonized = colonized,
    uncolonized = uncolonized,
    recovered = recovered
  )
}

#' Antibiotic Parameters
#'
#' @param onoff If Anti-biotic are used or not.
#' @param delay The delay in using antibiotics.
#' @param life The life of antibiotics.
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
#' @param recovered Rate of antibiotic administration when the individual is recovered.
#'
#' @returns list of parameters for antibiotic administration.
#' @export
#'
#' @examples
#' AbxRateParams()
AbxRateParams <- function(
    uncolonized = Param(1, 0),
    colonized = Param(1, 0),
    recovered = Param(0)) {
  uncolonized <- check_param(uncolonized)
  colonized <- check_param(colonized)
  recovered <- check_param(recovered)
  list(
    uncolonized = uncolonized,
    colonized = colonized,
    recovered = recovered
  )
}

#' Title
#'
#' @param base
#' @param time
#' @param mass
#' @param freq
#' @param col_abx
#' @param suss_abx
#' @param suss_ever
#'
#' @returns
#' @export
#'
#' @examples
LinearAbxAcquisitionParams <- function(
    base = Param(0.01),
    time = Param(1, 0),
    mass = Param(1),
    freq = Param(1),
    col_abx = Param(1, 0),
    suss_abx = Param(1, 0),
    suss_ever = Param(1, 0)) {
  list(
    base = base,
    time = time,
    mass = mass,
    freq = freq,
    col_abx = col_abx,
    suss_abx = suss_abx,
    suss_ever = suss_ever
  )
}

#' Progression Parameters
#'
#' @param rate
#' @param abx
#' @param ever_abx
#'
#' @returns
#' @export
#'
#' @examples
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
#' @returns
#' @export
#'
#' @examples
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


LogNormalInUnitAcquisition <- function(
    acquisition = AcquisitionParams(),
    progression = ProgressionParams(),
    clearance = ClearanceParams()) {
  list(
    acquisition = acquisition,
    progression = progression,
    clearance = clearance
  )
}
LogNormalABXInUnitParameters <- function(
    acquisition = LinearAbxAcquisitionParams(),
    progression = ProgressionParams(),
    clearance = ClearanceParams()) {
  list(
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
#' @param SurveilenceTest Surveillance Testing Parameters
#' @param ClinicalTest Clinical Testing Parameters
#' @param OutOfUnitInfection Out of Unit Infection Parameters
#' @param InUnit In Unit Parameters, should be a list of lists with parameters
#'               for the acquisition, progression and clearance of the disease.
#' @param Abx Antibiotic Parameters
#' @param AbxRate Antibiotic Rate Parameters
#'
#' @returns
#' @export
#'
#' @examples
LogNormalModelParams <-
  function(modname,
           nstates = 2,
           nmetro = 1,
           forward = TRUE,
           cheat = FALSE,
           Insitu = InsituParams(),
           SurveilenceTest = SurveillanceTestParams(),
           ClinicalTest = ClinicalTestParams(),
           OutOfUnitInfection = OutOfUnitInfectionParams(),
           InUnit = LogNormalInUnitAcquisition(),
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
      SurveilenceTest = SurveilenceTest,
      ClinicalTest = ClinicalTest,
      OutCol = OutOfUnitInfection,
      InCol = InUnit,
      Abx = Abx,
      AbxRate = AbxRate
    )
  }


#' @describeIn LogNormalModelParams Linear Antibiotic Model Alias
LinearAbxModel <- function(
    ...,
    InUnit = LogNormalABXInUnitParameters()) {
  LogNormalModelParams("LinearAbxModel", ..., InUnit = InUnit)
}
