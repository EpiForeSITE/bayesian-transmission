test_that("Can create RawEventList with data.frame", {
  expect_s4_class(RawEventList, "C++Class")

  test <- RawEventList$new(
    rep(0, 10), # std::vector<int>,    // facilities
    rep(0:1, each = 5), # std::vector<int>,    // units
    1:10, # std::vector<double>, // times
    rep(1, 10), # std::vector<int>,    // patients
    rep(0:1, 5) # std::vector<int>     // types
  )

  expect_s4_class(test, "Rcpp_RawEventList")
  expect_equal(test$FirstTime(), 1)
  expect_equal(test$LastTime(), 10)
})
test_that("Can Create System class", {
  data(simulated.data, package = "bayestransmission")

  test <- TransmissionSystem$new(
    simulated.data$facility,
    simulated.data$unit,
    simulated.data$time,
    simulated.data$patient,
    simulated.data$type
  )
  expect_s4_class(test, "Rcpp_TransmissionSystem")
  expect_equal(test$start, 0)
  expect_equal(test$end, 1734)

  expect_equal(test$log, "")
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

test_that("InsituParams", {
  IP <- InsituParams$new(3)

  expect_equal(IP$nParam, 3L)
  expect_length(IP$paramNames, 3L)
  expect_silent(IP$set(1, 1, 1))


  IP2 <- InsituParams$new(2)

  expect_equal(IP2$nParam, 2L)
  expect_length(IP2$paramNames, 2L)
})
test_that("CppTestParams", {
  TP <- CppTestParams$new(3)
  TP$values
  TP$paramNames
  expect_equal(TP$nParam, 3L)
  expect_length(TP$values, 3L)
  expect_length(TP$paramNames, 3L)
  expect_silent(IP$set(1, 1, 1))


  TP2 <- CppTestParams$new(2)

  expect_length(TP2$values, 2L)
  expect_length(TP2$paramNames, 2L)
})
