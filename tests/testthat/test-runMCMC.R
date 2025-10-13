test_that("runMCMC Works", {
  zero <- 2e-4
  modelParameters <- {LinearAbxModel(
    nstates = 3,
    Insitu = InsituParams(probs = c(0.5, 0.5-zero, zero)),
    SurveillanceTest = SurveillanceTestParamsAbx(
      colonized_on = Param(0.8),
      colonized_off = Param(0.8),
      latent_on = 0.5,
      latent_off = 0.5,
      uncolonized_off = 0.01,
      uncolonized_on = 0.01),
    ClinicalTest = ClinicalTestParams(
      unc = ParamWRate(zero, 0.05),
      col = ParamWRate(0.99, 0.8)
    ),
    OutOfUnitInfection = OutOfUnitInfectionParams(
      acquisition = 0.1,
      clearance = 0.05,
      progression = 0.01
    ),
    InUnit = ABXInUnitParams(
      acquisition = LinearAbxAcquisitionParams(
        base =  0.01,
        time =  0.02,
        mass = exp(-0.5),
        freq =  0.5,
        col_abx = exp(-0.75),
        suss_abx =  exp(-0.5),
        suss_ever = 0.25
      ),
      # progression = ProgressionParams(),
      clearance = ClearanceParams()
    ),
    Abx = AbxParams(onoff = TRUE),
    AbxRate = AbxRateParams(
      colonized = 0.1,
      latent = 0.05,
      uncolonized = zero)
    )}
  {
    expect_named(modelParameters, c("modname", "nstates", "nmetro", "forward",
      "cheat", "Insitu", "SurveillanceTest",
      "ClinicalTest", "OutCol", "InCol", "Abx",
      "AbxRate"))
    expect_true(rlang::is_string(modelParameters$modname))
    expect_true(rlang::is_integer(modelParameters$nstates))
    expect_true(rlang::is_integer(modelParameters$nmetro))
    expect_true(rlang::is_logical(modelParameters$forward))
    expect_true(rlang::is_logical(modelParameters$cheat))

    expect_true(rlang::is_list(modelParameters$Insitu))
    expect_named(modelParameters$Insitu, c("probs", "priors", "doit"))
    checkmate::expect_double(modelParameters$Insitu$probs, 0, 1, len = 3)
    checkmate::expect_double(modelParameters$Insitu$priors, 0, 1, len = 3)
    checkmate::expect_logical(modelParameters$Insitu$doit, len = 3)

    expect_true(rlang::is_list(modelParameters$SurveillanceTest))
    expect_named(modelParameters$SurveillanceTest, c("colonized", "latent", "uncolonized"), ignore.order = TRUE)
    expect_true(rlang::is_list(modelParameters$SurveillanceTest$colonized))
    expect_true(rlang::is_list(modelParameters$SurveillanceTest$latent))
    expect_true(rlang::is_list(modelParameters$SurveillanceTest$uncolonized))
    expect_named(modelParameters$SurveillanceTest$colonized, c("off", "on"))
    expect_named(modelParameters$SurveillanceTest$colonized$on, c("init", "update", "prior", "weight"))


    expect_true(rlang::is_list(modelParameters$ClinicalTest))
    expect_named(modelParameters$ClinicalTest, c("colonized", "latent", "uncolonized"), ignore.order = TRUE)

    expect_true(rlang::is_list(modelParameters$OutCol))
    expect_named(modelParameters$OutCol, c("acquisition", "clearance", "progression"), ignore.order = TRUE)

    expect_true(rlang::is_list(modelParameters$InCol))
    expect_named(modelParameters$InCol, c("acquisition", "progression", "clearance"), ignore.order = TRUE)

    expect_true(rlang::is_list(modelParameters$Abx))
    expect_named(modelParameters$Abx, c("onoff", "delay", "life"), ignore.order = TRUE)

    expect_true(rlang::is_list(modelParameters$AbxRate))
    expect_named(modelParameters$AbxRate, c("colonized", "latent", "uncolonized"), ignore.order = TRUE)
  }
  {
  model <- MakeCPPModel(modelParameters, verbose = TRUE)
  expect_s4_class(model, "Rcpp_CppLogNormalModel")
  expect_equal(model$nstates, modelParameters$nstates)
  expect_equal(model$className, modelParameters$modname)


  insitu <-  model$InsituParams
  expect_equal(insitu$values, modelParameters$Insitu$probs)

  OutCol <- model$OutColParams
  expect_equal(OutCol$className, "OutColParams")
  expect_equal(OutCol$values, c(modelParameters$OutCol$acquisition$init,
                                modelParameters$OutCol$progression$init,
                                modelParameters$OutCol$clearance$init
                                ))

  SurveillanceTest <- model$SurveillanceTestParams
  expect_equal(SurveillanceTest$className, "TestParamsAbx")
  expect_s4_class(SurveillanceTest, "Rcpp_CppTestParams")
  expect_equal(SurveillanceTest$values,
               c(
                 modelParameters$SurveillanceTest$uncolonized$off$init,
                 modelParameters$SurveillanceTest$latent$off$init,
                 modelParameters$SurveillanceTest$colonized$off$init,
                 modelParameters$SurveillanceTest$uncolonized$on$init,
                 modelParameters$SurveillanceTest$latent$on$init,
                 modelParameters$SurveillanceTest$colonized$on$init
                 ))

  ClinicalTest <- model$ClinicalTestParams
  expect_equal(ClinicalTest$className, "RandomTestParams")
  expect_equal(model$ClinicalTestParams$values,
               c(
                 modelParameters$ClinicalTest$uncolonized$param$init,
                 modelParameters$ClinicalTest$latent$param$init,
                 modelParameters$ClinicalTest$colonized$param$init,
                 modelParameters$ClinicalTest$uncolonized$rate$init,
                 modelParameters$ClinicalTest$latent$rate$init,
                 modelParameters$ClinicalTest$colonized$rate$init
               ))
  expect_equal( length(model$ClinicalTestParams$names)
              , length(model$ClinicalTestParams$values)
              )
  val <- runif(1)
  model$ClinicalTestParams$set(1, val, TRUE, 0.9, 1)
  expect_equal(model$ClinicalTestParams$values[2], val)
  }

  {
  incol <- model$IncolParams
  expect_s4_class(incol, "Rcpp_CppLogNormalAbxICP")
  expect_equal(incol$className, 'LinearAbxICP')
  expect_equal(length(incol$names), length(incol$values))
  expect_equal(incol$values,
               # Acquisition
               c(modelParameters$InCol$acquisition$base$init,
                 modelParameters$InCol$acquisition$time$init,
                 modelParameters$InCol$acquisition$mass$init,
                 modelParameters$InCol$acquisition$freq$init,
                 modelParameters$InCol$acquisition$col_abx$init,
                 modelParameters$InCol$acquisition$suss_abx$init,
                 modelParameters$InCol$acquisition$suss_ever$init,
               # Progression
                 modelParameters$InCol$progression$rate$init,
                 modelParameters$InCol$progression$abx$init,
                 modelParameters$InCol$progression$ever_abx$init,
               # Clearance
                 modelParameters$InCol$clearance$rate$init,
                 modelParameters$InCol$clearance$abx$init,
                 modelParameters$InCol$clearance$ever_abx$init
                ))
  incol$values[1:7] # Acquisition

  expect_equal(incol$values[8:10],
               c(# Progression
                 modelParameters$InCol$progression$rate$init,
                 modelParameters$InCol$progression$abx$init,
                 modelParameters$InCol$progression$ever_abx$init))
  expect_equal(incol$values[11:13],
               c(# Clearance
                 modelParameters$InCol$clearance$rate$init,
                 modelParameters$InCol$clearance$abx$init,
                 modelParameters$InCol$clearance$ever_abx$init
               ))
  }

  {
    if(modelParameters$Abx$onoff){
      expect_equal(model$AbxMode, 'on/off')
    } else {
      expect_equal(model$AbxMode, 'dose')
      expect_equal(model$AbxLife, modelParameters$Abx$life)
      expect_equal(model$AbxDelay, modelParameters$Abx$delay)
    }
    expect_equal( length(model$AbxParams$names), length(model$AbxParams$values))
    expect_equal( model$AbxParams$values
                , c( modelParameters$AbxRate$uncolonized$init
                   , modelParameters$AbxRate$latent$init
                   , modelParameters$AbxRate$colonized$init
                ))
  }

  data.in <- simulated.data %>%
    dplyr::arrange(facility, unit, patient, time)
  sys <- CppSystem$new(
    data.in$facility,
    data.in$unit,
    data.in$time,
    data.in$patient,
    data.in$type
  )
  expect_equal(sys$log, "")
  expect_s4_class(sys, "Rcpp_CppSystem")
  hist <- CppSystemHistory$new(sys, model, TRUE)
  expect_equal(hist$errlog, "")

  rand <- RRandom$new()
  expect_s4_class(rand, "Rcpp_RRandom")
if(F){
  model$InsituParams$counts


  model$SurveillanceTestParams$getCount(0,0)
  model$SurveillanceTestParams$getCount(0,1)
  model$SurveillanceTestParams$getCount(1,0)
  model$SurveillanceTestParams$getCount(1,1)
  model$SurveillanceTestParams$getCount(2,0)
  model$SurveillanceTestParams$getCount(2,1)
  expect_error(model$SurveillanceTestParams$getCount(3,1), "Index out of range")
  expect_error(model$SurveillanceTestParams$getCount(0,2), "Index out of range")

  model$SurveillanceTestParams$counts

  model$ClinicalTestParams$getCount(0,0)

  hist$UnitHeads

  model$count(hist)

  model$SurveillanceTestParams$counts

  model$InsituParams$counts
  model$OutColParams$values
  model$InColParams$values
  model$SurveillanceTestParams$values
  model$SurveillanceTestParams$counts

  expect_equal(model$SurveillanceTestParams$values,
               c(0.8, 0.05))
  modelParameters$SurveillanceTest$uncolonized$init
  modelParameters$SurveillanceTest$colonized$init
  modelParameters$SurveillanceTest$latent$init
}
  pos <- hist$positives
  # episodes <- hist$Episodes

  sampler <- CppSampler$new(sys, model, rand)

  results <- runMCMC(
    data = dplyr::arrange(simulated.data, facility, unit, patient, time),
    MCMCParameters = list(
      nburn = 2,
      nsims = 3,
      outputparam = TRUE,
      outputfinal = TRUE
    ),
    modelParameters = modelParameters,
    verbose = TRUE
  )

})
