# Tests for classes exposed in src/Module-Infect.cpp
# Classes are tested in the order they appear in the C++ file

# Test data setup (used by multiple tests)
data(simulated.data, package = "bayestransmission")

# 1. AbxLocationState ----
test_that("CppAbxLocationState properties", {
  skip("Need to create standalone test - currently tested via integration")
})

# 2. AbxPatientState ----
test_that("CppAbxPatientState properties", {
  skip("Need to create standalone test - currently tested via integration")
})

# 3. CountLocationState ----
test_that("CppCountLocationState properties", {
  skip("Need to create standalone test - currently tested via integration")
})

# 4. Event ----
test_that("CppEvent properties and methods", {
  skip("Need to create standalone test - currently tested via integration")
})

# 5. Facility ----
test_that("CppFacility constructor and properties", {
  fac <- CppFacility$new(1L)
  expect_s4_class(fac, "Rcpp_CppFacility")
  expect_equal(fac$id, 1L)
})

# 6. HistoryLink ----
test_that("CppHistoryLink properties", {
  skip("Need to create standalone test - currently tested via integration")
})

# 7. LocationState ----
test_that("CppLocationState properties", {
  skip("Need to create standalone test - currently tested via integration")
})

# 8. Model ----
test_that("CppModel base class properties", {
  skip("Abstract class - tested via derived classes in other modules")
})

# 9. Patient ----
test_that("CppPatient constructor and properties", {
  pat <- CppPatient$new(42L)
  expect_s4_class(pat, "Rcpp_CppPatient")
  expect_equal(pat$id, 42L)
  
  # Test group property
  pat$group <- 5L
  expect_equal(pat$group, 5L)
})

# 10. PatientState ----
test_that("CppPatientState constructor and properties", {
  pat <- CppPatient$new(1L)
  
  # Constructor with Patient only
  ps1 <- CppPatientState$new(pat)
  expect_s4_class(ps1, "Rcpp_CppPatientState")
  
  # Constructor with Patient and nStates
  ps2 <- CppPatientState$new(pat, 3L)
  expect_s4_class(ps2, "Rcpp_CppPatientState")
})

# 11. RawEventList ----
test_that("CppRawEventList constructor and methods", {
  expect_s4_class(CppRawEventList, "C++Class")

  rel <- CppRawEventList$new(
    rep(0, 10),         # facilities
    rep(0:1, each = 5), # units
    1:10,               # times
    rep(1, 10),         # patients
    rep(0:1, 5)         # types
  )

  expect_s4_class(rel, "Rcpp_CppRawEventList")
  expect_equal(rel$FirstTime(), 1)
  expect_equal(rel$LastTime(), 10)
})

test_that("CppRawEventList with simulated data", {
  rel <- CppRawEventList$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  
  expect_s4_class(rel, "Rcpp_CppRawEventList")
  expect_true(rel$FirstTime() <= rel$LastTime())
})

# 12. Sampler ----
test_that("CppSampler constructor", {
  # Create system and model for sampler
  sys <- CppSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  
  model <- CppLinearAbxModel$new(2, 10, 1, 0)
  hist <- CppSystemHistory$new(sys, model, FALSE)
  rr <- RRandom$new()
  
  sampler <- CppSampler$new(hist, model, rr)
  expect_s4_class(sampler, "Rcpp_CppSampler")
})

# 13. System ----
test_that("CppSystem constructor and properties", {
  sys <- CppSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  
  expect_s4_class(sys, "Rcpp_CppSystem")
  expect_equal(sys$startTime(), 0)
  expect_equal(sys$endTime(), 1734)
  # Note: log property may contain debug output depending on build
})

test_that("CppSystem with minimal data", {
  sys <- CppSystem$new(
    rep(0, 5),    # facilities
    rep(0, 5),    # units
    1:5,          # times
    rep(1, 5),    # patients
    rep(0, 5)     # types
  )
  
  expect_s4_class(sys, "Rcpp_CppSystem")
  expect_equal(sys$startTime(), 1)
  expect_equal(sys$endTime(), 5)
  sys$countEvents()
})

# 14. SystemHistory ----
test_that("CppSystemHistory constructor and properties", {
  sys <- CppSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  
  model <- CppLinearAbxModel$new(2, 10, 1, 0)
  hist <- CppSystemHistory$new(sys, model, FALSE)
  
  expect_s4_class(hist, "Rcpp_CppSystemHistory")
  
  # Test properties
  expect_s4_class(hist$UnitHeads, "Rcpp_CppMap")
  expect_s4_class(hist$PatientHeads, "Rcpp_CppMap")
  expect_s4_class(hist$FacilityHeads, "Rcpp_CppMap")
  
  # Check that we have the expected number of units
  expect_equal(hist$UnitHeads$size, 3)
})

# 15. TestParamsAbx ----
test_that("CppTestParamsAbx class exists", {
  expect_s4_class(CppTestParamsAbx, "C++Class")
  skip("Constructor parameters not documented - needs investigation")
})

# 16. Unit ----
test_that("CppUnit constructor and properties", {
  fac <- CppFacility$new(1L)
  unit <- CppUnit$new(fac, 5L)
  
  expect_s4_class(unit, "Rcpp_CppUnit")
  expect_equal(unit$id, 5L)
  
  # Test getName method
  name <- unit$getName()
  expect_type(name, "character")
})
