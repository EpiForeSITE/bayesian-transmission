#' Create a new C++ model object with parameters
#'
#' Creates and initializes a C++ model object based on the provided parameters.
#' This function wraps the underlying C++ model classes (LogNormalModel, LinearAbxModel,
#' LinearAbxModel2, MixedModel) in appropriate R reference classes that expose
#' the model's methods and properties.
#'
#' The function uses the existing `newModel` C++ function to instantiate the model
#' and configure all parameters, then wraps it in the appropriate R reference class
#' based on the model type specified in `modelParameters$modname`.
#'
#' @param modelParameters List of model parameters created using functions from
#'   constructors.R, such as:
#'   * `LogNormalModelParams()` - Basic log-normal model
#'   * `LinearAbxModel()` - Linear antibiotic model
#'   * Or custom parameter lists containing:
#'     - `modname`: Model name ("LogNormalModel", "LinearAbxModel", "LinearAbxModel2", "MixedModel")
#'     - `nstates`: Number of states (2 or 3)
#'     - `nmetro`: Number of Metropolis-Hastings steps
#'     - `forward`: Forward simulation flag
#'     - `cheat`: Cheat flag for debugging
#'     - `Insitu`: In situ parameters from `InsituParams()`
#'     - `SurveillanceTest`: Surveillance test parameters from `SurveillanceTestParams()`
#'     - `ClinicalTest`: Clinical test parameters from `ClinicalTestParams()`
#'     - `OutCol`: Out-of-unit infection parameters from `OutOfUnitInfectionParams()`
#'     - `InCol`: In-unit parameters from `InUnitParams()` or `ABXInUnitParams()`
#'     - `Abx`: Antibiotic parameters from `AbxParams()`
#'     - `AbxRate`: Antibiotic rate parameters from `AbxRateParams()`
#'
#' @param verbose Logical flag to print progress messages during model creation
#'   and parameter setup (default: FALSE)
#'
#' @return A reference class object wrapping the C++ model. The specific class depends
#'   on `modelParameters$modname`:
#'   * `CppLogNormalModel` - For "LogNormalModel"
#'   * `CppLinearAbxModel` - For "LinearAbxModel"
#'   * `CppLinearAbxModel2` - For "LinearAbxModel2"
#'   * `CppMixedModel` - For "MixedModel" (if exposed in C++)
#'
#'   All returned objects inherit from `CppBasicModel` and provide access to:
#'   * **Properties:**
#'     - `InColParams` - In-unit colonization parameters
#'     - `OutColParams` - Out-of-unit colonization parameters
#'     - `InsituParams` - In situ parameters
#'     - `SurveillanceTestParams` - Surveillance test parameters
#'     - `ClinicalTestParams` - Clinical test parameters
#'     - `AbxParams` - Antibiotic parameters
#'   * **Methods:**
#'     - `logLikelihood(hist)` - Calculate log likelihood for a SystemHistory
#'     - `getHistoryLinkLogLikelihoods(hist)` - Get individual link log likelihoods
#'     - `forwardSimulate(...)` - Perform forward simulation
#'     - `initEpisodeHistory(...)` - Initialize episode history
#'     - `sampleEpisodes(...)` - Sample episodes
#'     - `setAbx(...)` - Set antibiotic parameters
#'
#' @export
#'
#' @examples
#' \dontrun{
#' # Create a basic log-normal model
#' params <- LogNormalModelParams("LogNormalModel")
#' model <- newCppModel(params)
#'
#' # Access model properties
#' inColParams <- model$InColParams
#' insituParams <- model$InsituParams
#'
#' # Create a linear antibiotic model
#' params <- LinearAbxModel()
#' model <- newCppModel(params, verbose = TRUE)
#'
#' # Get parameter values
#' inColParams <- model$InColParams
#' paramValues <- inColParams$values
#'
#' # Use with a system history for likelihood calculation
#' # (requires data and SystemHistory object)
#' # ll <- model$logLikelihood(systemHistory)
#' }
#'
#' @seealso
#' * [LogNormalModelParams()] for creating model parameters
#' * [LinearAbxModel()] for linear antibiotic model parameters
#' * [InsituParams()], [SurveillanceTestParams()], etc. for parameter components
#' * [newModelExport()] for extracting parameter values from a model
newCppModel <- function(modelParameters, verbose = FALSE) {
  # Validate input
  if (!is.list(modelParameters)) {
    stop("modelParameters must be a list")
  }
  
  # Check required fields
  required_fields <- c("modname", "nstates", "nmetro", "forward", "cheat",
                       "Insitu", "SurveillanceTest", "ClinicalTest", 
                       "OutCol", "InCol", "Abx", "AbxRate")
  missing_fields <- setdiff(required_fields, names(modelParameters))
  if (length(missing_fields) > 0) {
    stop("modelParameters is missing required fields: ", 
         paste(missing_fields, collapse = ", "))
  }
  
  # Extract model name
  modname <- modelParameters$modname
  
  # Validate model name
  valid_models <- c("LogNormalModel", "LinearAbxModel", "LinearAbxModel2", "MixedModel")
  if (!modname %in% valid_models) {
    stop("Invalid model name '", modname, "'. Must be one of: ",
         paste(valid_models, collapse = ", "))
  }
  
  # Create the model using the internal C++ function
  # This function instantiates the appropriate C++ class and sets up all parameters
  if (verbose) {
    message("Creating C++ model of type: ", modname)
  }
  
  # Call the C++ newModel function through the exported wrapper
  model_ptr <- newCppModelInternal(modelParameters, verbose)
  
  # Return the wrapped model
  # The C++ side has already wrapped it in the correct reference class
  return(model_ptr)
}


#' Extract Model Parameters from C++ Model Object
#'
#' Convenience function to extract all parameter values from a C++ model object
#' created with `newCppModel()`. This is essentially a wrapper around accessing
#' the model's parameter properties.
#'
#' @param model A C++ model object created with `newCppModel()`
#'
#' @return A named list containing all model parameter values:
#'   * `Insitu` - In situ parameter values
#'   * `SurveillanceTest` - Surveillance test parameter values
#'   * `ClinicalTest` - Clinical test parameter values
#'   * `OutCol` - Out-of-unit colonization parameter values
#'   * `InCol` - In-unit colonization parameter values
#'   * `Abx` - Antibiotic parameter values (if applicable)
#'
#' @export
#'
#' @examples
#' \dontrun{
#' params <- LinearAbxModel()
#' model <- newCppModel(params)
#' 
#' # Extract all parameters
#' all_params <- getCppModelParams(model)
#' print(all_params$InCol)
#' }
getCppModelParams <- function(model) {
  if (!inherits(model, "C++Object")) {
    stop("model must be a C++ model object created with newCppModel()")
  }
  
  result <- list()
  
  # Try to access each parameter component
  # Use tryCatch in case some are NULL
  tryCatch({
    result$Insitu <- model$InsituParams$values
  }, error = function(e) {
    result$Insitu <<- NULL
  })
  
  tryCatch({
    result$SurveillanceTest <- model$SurveillanceTestParams$values
  }, error = function(e) {
    result$SurveillanceTest <<- NULL
  })
  
  tryCatch({
    result$ClinicalTest <- model$ClinicalTestParams$values
  }, error = function(e) {
    result$ClinicalTest <<- NULL
  })
  
  tryCatch({
    result$OutCol <- model$OutColParams$values
  }, error = function(e) {
    result$OutCol <<- NULL
  })
  
  tryCatch({
    result$InCol <- model$InColParams$values
  }, error = function(e) {
    result$InCol <<- NULL
  })
  
  tryCatch({
    abx_params <- model$AbxParams
    if (!is.null(abx_params)) {
      result$Abx <- abx_params$values
    }
  }, error = function(e) {
    result$Abx <<- NULL
  })
  
  return(result)
}
