test_that("runMCMC Works", {

  modelParameters <- LinearAbxModel(nstates = 2)

  expect_named(modelParameters, c("modname", "nstates", "nmetro", "forward",
    "cheat", "Insitu", "SurveilenceTest",
    "ClinicalTest", "OutCol", "InCol", "Abx",
    "AbxRate"))
  expect_true(rlang::is_string(modelParameters$modname))
  expect_true(rlang::is_integer(modelParameters$nstates))
  expect_true(rlang::is_integer(modelParameters$nmetro))
  expect_true(rlang::is_logical(modelParameters$forward))
  expect_true(rlang::is_logical(modelParameters$cheat))

  expect_true(rlang::is_list(modelParameters$Insitu))
  expect_named(modelParameters$Insitu, c("probs", "priors", "doit"))
  expect_double(modelParameters$Insitu$probs, 0, 1, len = 3)
  expect_double(modelParameters$Insitu$priors, 0, 1, len = 3)
  expect_logical(modelParameters$Insitu$doit, len = 3)

  expect_true(rlang::is_list(modelParameters$SurveilenceTest))
  expect_named(modelParameters$SurveilenceTest, c("colonized", "uncolonized", "recovered"))

  expect_true(rlang::is_list(modelParameters$ClinicalTest))
  expect_named(modelParameters$SurveilenceTest, c("colonized", "uncolonized", "recovered"))

  expect_true(rlang::is_list(modelParameters$OutCol))
  expect_named(modelParameters$OutCol, c("acquisition", "clearance"))

  expect_true(rlang::is_list(modelParameters$InCol))
  expect_named(modelParameters$InCol, c("acquisition", "progression", "clearance"))

  expect_true(rlang::is_list(modelParameters$Abx))
  expect_named(modelParameters$Abx, c("onoff", "delay", "life"))

  expect_true(rlang::is_list(modelParameters$AbxRate))
  expect_named(modelParameters$AbxRate, c("onoff", "delay", "life"))


  results <- runMCMC(
    modname = "LinearAbxModel",
    data = simulated.data,
    MCMCParameters = list(
      nburn = 2,
      nsims = 3,
      outputparam = TRUE,
      outputfinal = TRUE
    ),
    modelParameters = modelParameters,
    nstates = 2,
    verbose = TRUE
  )
})
