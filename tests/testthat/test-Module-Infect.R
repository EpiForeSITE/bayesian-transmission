
test_that("Can create RawEventList with data.frame", {
    expect_s4_class(RawEventList, "C++Class")

    test <- RawEventList$new(
                rep(0, 10),       # std::vector<int>,    // facilities
                rep(0:1, each=5), # std::vector<int>,    // units
                1:10, # std::vector<double>, // times
                rep(1, 10), # std::vector<int>,    // patients
                rep(0:1, 5)# std::vector<int>     // types
    )

    expect_s4_class(test, "Rcpp_RawEventList")
    expect_equal(test$FirstTime(), 1)
    expect_equal(test$LastTime(), 10)
})
test_that("Can Create system class", {
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
