test_that("models::InsituParameters", {
    isp0 <- CppInsituParams$new()
    expect_equal(isp0$nStates, 0)


    isp1 <- CppInsituParams$new(2)
    expect_equal(isp1$nStates, 2)
    expect_equal(isp1$paramNames, c("Insit.P(unc)", "Insit.P(col)"))

    isp3 <- CppInsituParams$new(
        probs = c(80, 5, 15),
        priors = c(0.1, 0.2, 0.3),
        doit = c(TRUE, TRUE, TRUE)
    )

    expect_equal(isp3$nStates, 3)
    expect_equal(isp3$paramNames, c("Insit.P(unc)", "Insit.P(lat)", "Insit.P(col)"))
    expect_equal(isp3$values, c(0.8, 0.05, 0.15))
    expect_equal(isp3$counts, c(0.1, 0.2, 0.3))
    isp3$counts <- c(90, 3, 7)
    expect_equal(isp3$counts, c(90, 3, 7))


    rand <- RRandom$new()
    isp3$update(rand, TRUE)

    expect_equal(isp3$values, c(89/97, 2/97, 6/97))


})

test_that("models::UnitLinkedModel", {
    model <- CppDummyModel$new(nstates = 2)
    isp <- model$InsituParams


})
