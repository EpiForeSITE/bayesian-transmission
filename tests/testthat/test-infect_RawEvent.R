test_that("multiplication works", {
    admit <- CppRawEvent$new(
        facility = 1,
        unit = 2,
        time = 0,
        patient = 3,
        type = EventToCode("admission")
    )
    expect_s4_class(admit, "Rcpp_CppRawEvent")
    expect_equal(admit$type, EventToCode("admission"))
    expect_equal(admit$facility, 1)
    expect_equal(admit$unit, 2)
    expect_equal(admit$time, 0)
    expect_equal(admit$patient, 3)

    for(type in CodeToEvent(-1:19)) {
        event <- CppRawEvent$new(
            facility = 1,
            unit = 2,
            time = 0,
            patient = 3,
            type = EventToCode(type)
        )
        expect_equal(event$type, EventToCode(type))
    }

})
