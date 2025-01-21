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
    assert_that::has_names(x, c("init", "weight", "update", "prior")),
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
    stop(glue::glue("Parameter '{name}' is not valid. Use `Param()`"))
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
    acquisition = 0,
    clearance = 0,
    thirdoption) {
  list(
    acquisition = acquisition,
    clearance = clearance
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



#' In-Unit Acquisition Parameters
#'
#' @param base The base rate of acquisition
#' @param time The time effect on acquisition
#' @param mass The mass mixing probability
#' @param dens The density mixing probability
#' @param col The colonization effect
#' @param suss The susceptibility effect
#' @param suss_ever The susceptibility ever effect
#' @param clearance The clearance rate
#' @param clearance_abx The clearance rate effect of antibiotics
#' @param clearance_ever_abx The clearance rate effect of ever having taken antibiotics
#'
#' @returns A list of parameters for in-unit acquisition.
#' @export
#' @examples
#' LogNormalICP()
LogNormalICP <- function(
    base = 0,
    time = 0,
    mass = 1,
    dens = 1,
    col = 1,
    suss = 1,
    suss_ever = 1,
    clearance = 0,
    clearance_abx = 1,
    clearance_ever_abx = 1) {
  list(
    base = base,
    time = time,
    mass = mass,
    dens = dens,
    col = col,
    suss = suss,
    suss_ever = suss_ever,
    clearance = clearance,
    clearance_abx = clearance_abx,
    clearance_ever_abx = clearance_ever_abx
  )
}

#' Antibiotic Parameters
#'
#' @param onoff If Anti-biotic are used or not.
#' @param delay The delay in using antibiotics.
#' @param life The life of antibiotics.
AbxParams <- function(
    onoff = 0,
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
    recovered = param(0)) {
  uncolonized <- check_param(uncolonized)
  colonized <- check_param(colonized)
  recovered <- check_param(recovered)
  list(
    uncolonized = uncolonized,
    colonized = colonized,
    recovered = recovered
  )
}

#' In unit model parameters
#'
#' @param acquisition_base TODO
#' @param acquisition_time_effect TODO
#' @param acquisition_mass_mixing_prob TODO
#' @param acquisition_dens_mixing_prob  TODO
#' @param acquisition_col_patient_abx_effect TODO
#' @param acquisition_suss_abx_effect  TODO
#' @param acquisition_suss_ever_abx_effect  TODO
#' @param clearance_rate  TODO
#' @param clearance_abx_effect TODO
#' @param clearance_ever_abx_effect TODO
#'
#' @returns
#' @export
#'
#' @examples
InUnitParameters <- function(
    acquisition_base = Param(0, TRUE),
    acquisition_time_effect = Param(1, FALSE),
    acquisition_mass_mixing_prob = Param(1, FALSE),
    acquisition_dens_mixing_prob = Param(1, FALSE),
    acquisition_col_patient_abx_effect = Param(1, FALSE),
    acquisition_suss_abx_effect = Param(1, FALSE),
    acquisition_suss_ever_abx_effect = Param(1, FALSE),
    clearance_rate = Param(0, TRUE),
    clearance_abx_effect = Param(1, FALSE),
    clearance_ever_abx_effect = Param(1, FALSE)) {
  list(
    acquisition_base = acquisition_base,
    acquisition_time_effect = acquisition_time_effect,
    acquisition_mass_mixing_prob = acquisition_mass_mixing_prob,
    acquisition_dens_mixing_prob = acquisition_dens_mixing_prob,
    acquisition_col_patient_abx_effect = acquisition_col_patient_abx_effect,
    acquisition_suss_abx_effect = acquisition_suss_abx_effect,
    acquisition_suss_ever_abx_effect = acquisition_suss_ever_abx_effect,
    clearance_rate = clearance_rate,
    clearance_abx_effect = clearance_abx_effect,
    clearance_ever_abx_effect = clearance_ever_abx_effect
  )
}


#' Model Parameters for a Log Normal Model
#'
#' @param modname The name of the model used. Usually specified by specification functions.
#' @param nstates The number of states in the model.
#' @param nmetro TODO
#' @param forward TODO
#' @param cheat TODO
#' @param Insitu In Situ Parameters
#' @param SurveilenceTest Surveillance Testing Parameters
#' @param ClinicalTest Clinical Testing Parameters
#' @param OutOfUnitInfection Out of Unit Infection Parameters
#' @param InUnitAcquisition In Unit Acquisition Parameters
#' @param Abx Antibiotic Parameters
#' @param AbxRate Antibiotic Rate Parameters
#'
#' @returns
#' @export
#'
#' @examples
LogNormalModelParameters <-
  function(modname,
           nstates = 2,
           nmetro = 1000,
           forward = TRUE,
           cheat = FALSE,
           Insitu = InsituParams(),
           SurveilenceTest = TestParams(),
           ClinicalTest = ClinicalTestParams(),
           OutOfUnitInfection = OutOfUnitInfectionParams(),
           InUnitAcquisition = InUnitAcquisitionParams(),
           Abx = AbxParams(),
           AbxRate = AbxParams()) {
    list(
      modname = modname,
      nstates = nstates,
      nmetro = nmetro,
      forward = forward,
      cheat = cheat,
      Insitu = Insitu,
      SurveilenceTest = SurveilenceTest,
      ClinicalTest = ClinicalTest,
      OutCol = OutOfUnitInfection,
      InCol = InUnitAcquisition,
      Abx = Abx,
      AbxRate = AbxRate
    )
  }


LinearAbxModel <- function(...) {
  LogNormalModelParameters("LinearAbxModel", ...)
}
