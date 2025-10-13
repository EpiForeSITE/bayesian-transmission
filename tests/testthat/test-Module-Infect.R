test_that("Can create RawEventList with data.frame", {
  expect_s4_class(CppRawEventList, "C++Class")

  test <- CppRawEventList$new(
    rep(0, 10), # std::vector<int>,    // facilities
    rep(0:1, each = 5), # std::vector<int>,    // units
    1:10, # std::vector<double>, // times
    rep(1, 10), # std::vector<int>,    // patients
    rep(0:1, 5) # std::vector<int>     // types
  )

  expect_s4_class(test, "Rcpp_CppRawEventList")
  expect_equal(test$FirstTime(), 1)
  expect_equal(test$LastTime(), 10)
})
test_that("Can Create System class", {
  data(simulated.data, package = "bayestransmission")

  expect_s4_class(CppSystem, "C++Class")

  test <- CppSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  expect_s4_class(test, "Rcpp_CppSystem")
  expect_equal(test$start, min(simulated.data$time))
  expect_equal(test$end, max(simulated.data$time))
})
test_that("RRandom", {
  RR <- RRandom$new()

  runif(1)
  seed <- .GlobalEnv$.Random.seed

  a <- RR$runif()
  b <- RR$runif()
  .GlobalEnv$.Random.seed <- seed
  c <- RR$runif()
  .GlobalEnv$.Random.seed <- seed
  d <- runif(1)

  expect_true(0 <= a && a <= 1)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)

  seed <- .GlobalEnv$.Random.seed
  a <- RR$runif2(50, 100)
  b <- RR$runif2(50, 100)
  .GlobalEnv$.Random.seed <- seed
  c <- RR$runif2(50, 100)
  .GlobalEnv$.Random.seed <- seed
  d <- runif(1, 50, 100)

  expect_true(50 <= a && a <= 100)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)


  seed <- .GlobalEnv$.Random.seed
  a <- RR$rexp()
  b <- RR$rexp()
  .GlobalEnv$.Random.seed <- seed
  c <- RR$rexp()
  .GlobalEnv$.Random.seed <- seed
  d <- rexp(1)
  expect_true(0 <= a)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)

  seed <- .GlobalEnv$.Random.seed
  a <- RR$rexp1(100)
  b <- RR$rexp1(100)
  .GlobalEnv$.Random.seed <- seed
  c <- RR$rexp1(100)
  .GlobalEnv$.Random.seed <- seed
  d <- rexp(1, 100)
  expect_true(0 <= a)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)

  seed <- .GlobalEnv$.Random.seed
  a <- RR$rgamma(1, 1)
  b <- RR$rgamma(1, 1)
  .GlobalEnv$.Random.seed <- seed
  c <- RR$rgamma(1, 1)
  .GlobalEnv$.Random.seed <- seed
  d <- rgamma(1, 1)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)

  seed <- .GlobalEnv$.Random.seed
  a <- RR$rnorm()
  b <- RR$rnorm()
  .GlobalEnv$.Random.seed <- seed
  c <- RR$rnorm()
  .GlobalEnv$.Random.seed <- seed
  d <- rnorm(1)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)

  seed <- .GlobalEnv$.Random.seed
  a <- RR$rpoisson(5)
  b <- RR$rpoisson(5)
  .GlobalEnv$.Random.seed <- seed
  c <- RR$rpoisson(5)
  .GlobalEnv$.Random.seed <- seed
  d <- rpois(1, 5)
  expect_false(a == b)
  expect_true(a == c)
  expect_true(a == d)
})

test_that("infect::SystemHistory", {
  dm <- CppDummyModel$new(2)
  sys <- CppSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  hist <- CppSystemHistory$new(sys, dm, FALSE)
  expect_s4_class(hist, "Rcpp_CppSystemHistory")

  patients <- hist$PatientHeads
  expect_true(is.list(patients))
  expect_equal(length(patients), dplyr::n_distinct(simulated.data$patient))
  expect_s4_class(patients[[1]], "Rcpp_CppHistoryLink")
  expect_equal(patients[[1]]$Event$type, 'admission')

  units <- hist$UnitHeads
  expect_true(is.list(units))
  expect_equal(length(units), dplyr::n_distinct(simulated.data$unit))
  expect_s4_class(units[[1]], "Rcpp_CppHistoryLink")
  expect_equal(units[[1]]$Event$type, 'start')

  facilities <- hist$FacilityHeads
  expect_true(is.list(facilities))
  expect_equal(length(facilities), dplyr::n_distinct(simulated.data$facility))
  expect_s4_class(facilities[[1]], "Rcpp_CppHistoryLink")
  expect_equal(facilities[[1]]$Event$type, 'start')

  systemhead <- hist$SystemHead
  expect_s4_class(systemhead, "Rcpp_CppHistoryLink")
  expect_equal(systemhead$Event$type, 'start')

  # episodes <- hist$Episodes
  # admissions <- hist$Admissions
  # discharges <- hist$Discharges
})

test_that("infect::RawEvent", {
  event <- CppRawEvent$new(1, 2, 1.5, 3, EventToCode('abxon'))
  expect_equal(event$facility, 1L)
  expect_equal(event$unit, 2L)
  expect_equal(event$time, 1.5)
  expect_equal(event$patient, 3L)
  expect_equal(event$type, 10L)
})

test_that("CppRawEventList", {
  sub <- dplyr::filter(simulated.data, patient <= 10) %>%
    dplyr::mutate(event = CodeToEvent(type)) %>%
    dplyr::arrange(patient, time)

  rel <- CppRawEventList$new(sub$facility, sub$unit, sub$time, sub$patient, sub$type)

  events <- rel$getEvents()

  purrr::map(events, \(x)x$type)


  sys <- CppSystem$new(
    sub$facility,
    sub$unit,
    sub$time,
    sub$patient,
    sub$type
  )
  sys$log == ""
  sys$log %>% cat()

  sub %>% dplyr::arrange(patient, time)

  hist <- CppSystemHistory$new(sys, CppDummyModel$new(2), FALSE)


})


test_that("CppLinearAbxModel", {

  model <- CppLinearAbxModel$new(
    nstates = 2,
    nmetro = 10,
    abxmode = 1,
    abxtest = TRUE,
    forward = TRUE,
    cheating = FALSE)

  expect_s4_class(model, "Rcpp_CppLinearAbxModel")
  expect_equal(model$AbxMode, "on/off")

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
  hist <- CppSystemHistory$new(sys, model, TRUE)
  expect_equal(hist$errlog, "")

  model$count(hist)

  isp <- model$InsituParams
  expect_s4_class(isp, "Rcpp_CppInsituParams")
  expect_equal(isp$paramNames, c("Insit.P(unc)", "Insit.P(col)"))
  expect_equal(sum(isp$values), 1)
  expect_equal(isp$nStates, 2L)

  isp$counts

  icp <- model$InColParams
  expect_s4_class(icp, "Rcpp_CppLogNormalAbxICP")
  expect_equal(icp$nStates, 2)


  icp$values
  icp$names


  icp$timeOrigin <- (sys$end - sys$start)/2


#Crash Here
  units <- hist$Units
  expect_true(is.list(units))
  expect_s4_class(units[[1]], "Rcpp_CppUnit")
  expect_length(units, 3)

  unit <- units[[1]]
  tmp <- unit$getHistory()
  expect_true(is.list(tmp))
  expect_s4_class(tmp[[1]], "Rcpp_CppHistoryLink")
  length(tmp)

  event.types <- purrr::map_chr(tmp, function(x) x$Event$type)
  event.ints  <- purrr::map_int(tmp, function(x) x$Event$type_int)
  compare_event_counts <- function(event){
    expect_equal(
      sum(event.types == event),
      simulated.data %>%
        dplyr::filter(
          unit == 1,
          type == EventToCode(event)
        ) %>%
        nrow()
    )
  }

  compare_event_counts('admission')
  compare_event_counts('discharge')
  compare_event_counts('abxon')
  compare_event_counts('abxoff')
  compare_event_counts('negsurvtest')
  compare_event_counts('possurvtest')

  patients <- purrr::map_int(tmp, \(x){x$Event$patient$id %||% NA_integer_})
  times <- purrr::map_dbl(tmp, \(x)x$Event$time)

  LLs <- purrr::map_dbl(tmp, function(x) model$logLikelihood_HL(x))
  i <- min(which(is.infinite(LLs)))

  hl <- tmp[i][[1]]
  hl$Event$time
  hl$Event$patient$id
  hl$Event$type
  hl$PatientState$infectionStatus


  model$SurveillanceTestParams$values
  model$SurveillanceTestParams$names
  model$SurveillanceTestParams$values <- c(0.95, 0.05, 0.95, 0.05)
  model$logLikelihood_HL(hl)
  survtsp <- model$SurveillanceTestParams
  survtsp$logProb(hl)
  survtsp$values

  PE <- tibble::tibble(patient = patients, event = event.types, int= event.ints, time=times)

  PE %>% dplyr::filter(patient == 1)
  simulated.data %>%
    dplyr::filter(patient <= 10) %>%
    dplyr::mutate(
      EVENT = CodeToEvent(type)
    )



  first <- asHistoryLink(h$FirstValue())
  last <- asHistoryLink(h$LastValue())
  if(F){
  first$linked
  first$hidden
  first$PatientPrev
  first$PatientNext
  first$HistoryNext
  first$HistoryPrev
  first$SystemPrev
  first$SystemNext
  first$FacilityPrev
  first$FacilityNext
  first$UnitPrev
  first$UnitNext
  first$Event
  first$PatientState
  first$UnitState
  first$FacilityState
  first$SystemState

  model$logLikelihood(last)

  last$linked
  last$hidden
  last$PatientPrev
  last$PatientNext
  last$HistoryNext
  last$HistoryPrev
  last$SystemPrev
  last$SystemNext
  last$FacilityPrev
  last$FacilityNext
  last$UnitPrev
  last$UnitNext
  last$Event
  last$Event$type
  last$PatientState
  last$UnitState
  last$FacilityState
  last$SystemState
}

  l <- last
  links <- list(first)

  expect_equal(first$Event$type, "start")

  expect_equal(model$logLikelihood_HL(l), 0)


  {
    (l <- l$UnitNext)
    format(l$.pointer) %>% gsub("<pointer: 0x(\\w+)>", "\\1", .) %>%
      grepl("2136af11970", .)
    #0x26e22f74ca0
    l$Event$type
    l$Event$time
    l$Event$isAdmission

    prev <- l$PatientPrev
    prev$Event$type
    prev$Event$time

    l$Event$patient$id
    prev$Event$patient$id

    l$Event$hash
    prev$Event$hash

    icp <- model$InColParams
    survtsp <- model$SurveillanceTestParams
    clintsp <- model$ClinicalTestParams
    isp <- model$InsituParams
    ocp <- model$OutColParams
    # abxp <- model$AbxParams

    prev <- l$UnitPrev
    prev$Event$type

    l$Event$time



    # icp$logProbGap(prev, l)
    LocationState_s <- asAbxLocationState(prev$UnitState)
    class(LocationState_s)

    t0 <- prev$Event$time
    t1 <- l$Event$time
    # LogNormalAbxICP::logProgressionGap icp$logProgressionGap(t0, t1, LocationState_s)
    icp$progressionRate
    LocationState_s$Never


    ll <- model$logLikelihood_HL(l)

    links <- c(links, l)
    if(identical(

    )) break
  }





  rr <- RRandom$new()
  mc <- CppSampler$new(hist, model, rr)

  hist$ll

  history_links <-

  model$logLikelihood(hist)

})



