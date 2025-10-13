test_that("multiplication works", {
    model <- CppDummyModel$new(2)
    # model <- CppLinearAbxModel$new(
    #     nstates = 2,
    #     nmetro = 10,
    #     abxmode = 1,
    #     abxtest = TRUE,
    #     forward = TRUE,
    #     cheating = FALSE)

    rel <- CppRawEventList$new(
        facility = c(1L, 1L, 1L),
        unit     = c(1L, 1L, 1L),
        time     = c(1L, 5L, 9L),
        patient  = c(1L, 1L, 1L),
        type     = EventToCode(c('admission', 'possurvtest', 'discharge'))
    )
    events <- rel$getEvents()
    sapply(events, \(e)e$type)


    sys <- CppSystem$new(
        facility = rep(1, 6),
        unit = rep(1, 6),
        time = c(1,5,9, 2, 6, 9),
        patient = c(1,1,1, 2, 2, 2),
        type = EventToCode(c('admission', 'possurvtest', 'discharge', 'admission', 'negsurvtest', 'discharge'))
    )
    expect_equal(length(sys$Units), 1)
    expect_equal(length(sys$Facilities), 1)
    expect_equal(length(sys$Patients), 2)
    expect_equal(sys$log, "")

    hist <- CppSystemHistory$new(sys, model, FALSE)
    expect_s4_class(hist, "Rcpp_CppSystemHistory")
    link <- hist$UnitHeads[[1]]
    links <- list(link)
    expect_true(link$hasUnitNext)
    expect_false(link$hasUnitPrev)

    link$hasFacilityNext
    link$hasHistoryNext
    link$hasHistoryPrev

    while(link$hasUnitNext){
        link$hasUnitNext
        link <- link$UnitNext
        links <- c(links, link)
    }

    sapply(links, \(l)l$Event$type)

    UnitLinkedModel::countUnitStats(link)

    model$SurveillanceTestParams

})
