# Test System Construction and Validation
# 
# These tests verify that the System C++ class correctly parses event data
# and constructs the appropriate hierarchical structure of facilities, units,
# patients, episodes, and events.

test_that("System construction from simple synthetic data works", {
  # Create simple test data: 2 patients, 2 facilities, 2 episodes
  facilities <- c(1, 1, 1, 1,   2, 2, 2, 2)
  units      <- c(1, 1, 1, 1,   1, 1, 1, 1)
  times      <- c(0, 1, 2, 3,   5, 6, 7, 8)
  patients   <- c(1, 1, 1, 1,   2, 2, 2, 2)
  types      <- c(1, 11, 12, 2,  1, 11, 12, 2)  # admission, test, test, discharge
  
  # Create System using the C++ constructor
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Test facility count
  fac_map <- sys$getFacilities()
  expect_equal(fac_map$size, 2, label = "number of facilities")
  
  # Test unit count
  units_list <- sys$getUnits()
  expect_equal(units_list$size, 2, label = "number of units")
  
  # Test patient count
  pat_map <- sys$getPatients()
  expect_equal(pat_map$size, 2, label = "number of patients")
  
  # Test time bounds
  expect_equal(sys$startTime(), 0, label = "start time")
  expect_gte(sys$endTime(), 8, label = "end time should be >= max input time")
})
gc()
test_that("System correctly counts episodes and events", {
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 1, 2, 3)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)  # admission, test, test, discharge
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Count episodes manually
  total_episodes <- 0
  total_events <- 0
  
  patients_map <- sys$getPatients()
  patients_map$init()
  while (patients_map$hasNext) {
    patient <- patients_map$nextValue()
    episodes <- sys$getEpisodes(patient)
    
    episodes$init()
    while (episodes$hasNext) {
      episode <- episodes$nextValue()
      total_episodes <- total_episodes + 1
      
      events <- episode$getEvents()
      total_events <- total_events + events$size()
    }
  }
  
  expect_equal(total_episodes, 1, label = "should have 1 episode")
  expect_equal(total_events, 4, label = "should have 4 events")
})

test_that("System helper methods countEpisodes and countEvents work", {
  facilities <- c(1, 1, 1, 1,   1, 1, 1, 1)
  units      <- c(1, 1, 1, 1,   1, 1, 1, 1)
  times      <- c(0, 1, 2, 3,   5, 6, 7, 8)
  patients   <- c(1, 1, 1, 1,   2, 2, 2, 2)
  types      <- c(1, 11, 12, 2,  1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Use the new helper methods
  expect_equal(sys$countEpisodes(), 2, label = "countEpisodes()")
  expect_equal(sys$countEvents(), 8, label = "countEvents()")
  
  # Use getSystemCounts
  counts <- sys$getSystemCounts()
  counts$init()
  
  n_facilities <- counts$step()$value()
  n_units <- counts$nextValue()
  n_patients <- counts$nextValue()
  n_episodes <- counts$step()$value()
  n_events <- counts$step()$value()
  
  expect_equal(n_facilities, 1)
  expect_equal(n_units, 1)
  expect_equal(n_patients, 2)
  expect_equal(n_episodes, 2)
  expect_equal(n_events, 8)
})

test_that("System construction works with real simulated data", {
  data(simulated.data_sorted, package = "bayestransmission")
  
  # Create System
  sys <- new(CppSystem, 
             simulated.data_sorted$facility,
             simulated.data_sorted$unit,
             simulated.data_sorted$time,
             simulated.data_sorted$patient,
             simulated.data_sorted$type)
  
  # Test patient count matches input
  n_unique_patients <- length(unique(simulated.data_sorted$patient))
  expect_equal(sys$getPatients()$size(), n_unique_patients,
               label = "patient count should match unique patients in input")
  
  # Test that we have facilities and units
  expect_gt(sys$getFacilities()$size(), 0, label = "should have at least one facility")
  expect_gt(sys$getUnits()$size(), 0, label = "should have at least one unit")
  
  # Test that we have episodes
  expect_gt(sys$countEpisodes(), 0, label = "should have at least one episode")
  
  # Test time bounds are sensible
  input_start <- min(simulated.data_sorted$time)
  input_end <- max(simulated.data_sorted$time)
  
  expect_lte(abs(sys$startTime() - input_start), 1.0,
             label = "start time should be close to input minimum")
  expect_gte(sys$endTime(), input_end,
             label = "end time should be >= input maximum")
})

test_that("Each episode has admission and discharge events", {
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 1, 2, 3)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Check all episodes
  patients_map <- sys$getPatients()
  all_complete <- TRUE
  
  patients_map$init()
  while (patients_map$hasNext) {
    patient <- patients_map$nextValue()
    episodes <- sys$getEpisodes(patient)
    
    episodes$init()
  while (episodes$hasNext) {
      episode <- episodes$nextValue()
      
      has_admission <- !is.null(episode$Admission)
      has_discharge <- !is.null(episode$Discharge)
      
      expect_true(has_admission, label = "episode should have admission")
      expect_true(has_discharge, label = "episode should have discharge")
      
      if (!has_admission || !has_discharge) {
        all_complete <- FALSE
      }
    }
  }
  
  expect_true(all_complete, label = "all episodes should be complete")
})

test_that("Events are temporally ordered within episodes", {
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 1, 2, 3)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  patients_map <- sys$getPatients()
  
  patients_map$init()
  while (patients_map$hasNext) {
    patient <- patients_map$nextValue()
    episodes <- sys$getEpisodes(patient)
    
    episodes$init()
  while (episodes$hasNext) {
      episode <- episodes$nextValue()
      events <- episode$Events
      
      prev_time <- -Inf
      events$init()
  while (events$hasNext) {
        event <- events$nextValue()
        curr_time <- event$Time
        
        expect_gte(curr_time, prev_time,
                   label = "events should be in temporal order")
        prev_time <- curr_time
      }
    }
  }
})

test_that("Discharge time is after admission time for all episodes", {
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 1, 2, 3)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  patients_map <- sys$getPatients()
  
  patients_map$init()
  while (patients_map$hasNext) {
    patient <- patients_map$nextValue()
    episodes <- sys$getEpisodes(patient)
    
    episodes$init()
  while (episodes$hasNext) {
      episode <- episodes$nextValue()
      
      adm_time <- episode$Admission$Time
      dis_time <- episode$Discharge$Time
      
      expect_gt(dis_time, adm_time,
                label = "discharge should be after admission")
    }
  }
})

test_that("System handles implied admissions correctly", {
  # Test data without explicit admission
  facilities <- c(1, 1, 1)
  units      <- c(1, 1, 1)
  times      <- c(1, 2, 3)
  patients   <- c(1, 1, 1)
  types      <- c(11, 12, 2)  # test, test, discharge (NO admission!)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # System should add implied admission
  expect_gt(sys$countEpisodes(), 0, label = "should have at least one episode")
  
  # Check that episode has admission
  patients_map <- sys$getPatients()
  patients_map$init()
  patient <- patients_map$nextValue()
  episodes <- sys$getEpisodes(patient)
  episodes$init()
  episode <- episodes$nextValue()
  
  expect_false(is.null(episode$Admission),
               label = "system should add implied admission")
  
  # Check the log mentions adding admission
  log_output <- sys$get_log()
  expect_match(log_output, "Adding",
               label = "log should mention adding implied events")
})

test_that("System handles implied discharges correctly", {
  # Test data without explicit discharge
  facilities <- c(1, 1, 1)
  units      <- c(1, 1, 1)
  times      <- c(0, 1, 2)
  patients   <- c(1, 1, 1)
  types      <- c(1, 11, 12)  # admission, test, test (NO discharge!)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Check that episode has discharge
  patients_map <- sys$getPatients()
  patients_map$init()
  patient <- patients_map$nextValue()
  episodes <- sys$getEpisodes(patient)
  episodes$init()
  episode <- episodes$nextValue()
  
  expect_false(is.null(episode$Discharge),
               label = "system should add implied discharge")
  
  # Check the log mentions adding discharge
  log_output <- sys$get_log()
  expect_match(log_output, "Adding.*discharge",
               label = "log should mention adding discharge")
})

test_that("System correctly identifies in situ patients", {
  # Patient starting at time 0 should be marked as in situ
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 0.5, 1, 2)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)  # admission at t=0
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  # Get the first episode
  patients_map <- sys$getPatients()
  patients_map$init()
  patient <- patients_map$nextValue()
  episodes <- sys$getEpisodes(patient)
  episodes$init()
  episode <- episodes$nextValue()
  
  # Check if admission event is marked as insitu
  admission <- episode$Admission
  admission_type <- admission$Type
  
  # Event type should be "insitu" or one of the insitu variants
  expect_match(as.character(admission_type), "insitu",
               label = "patient at time 0 should be marked as in situ")
})

test_that("System event counts balance correctly", {
  data(simulated.data_sorted, package = "bayestransmission")
  
  sys <- new(CppSystem, 
             simulated.data_sorted$facility,
             simulated.data_sorted$unit,
             simulated.data_sorted$time,
             simulated.data_sorted$patient,
             simulated.data_sorted$type)
  
  n_episodes <- sys$countEpisodes()
  n_events <- sys$countEvents()
  
  # Each episode must have at least admission + discharge
  expected_min_events <- n_episodes * 2
  
  expect_gte(n_events, expected_min_events,
             label = "total events should be >= 2 * episodes (admission + discharge)")
})

test_that("System handles multiple episodes per patient", {
  # Patient with 2 episodes (2 admissions)
  facilities <- c(1, 1, 1,   1, 1, 1)
  units      <- c(1, 1, 1,   1, 1, 1)
  times      <- c(0, 1, 2,   5, 6, 7)
  patients   <- c(1, 1, 1,   1, 1, 1)
  types      <- c(1, 11, 2,  1, 11, 2)  # 2 complete episodes
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  expect_equal(sys$countEpisodes(), 2,
               label = "patient should have 2 episodes")
  
  # Check both episodes are properly formed
  patients_map <- sys$getPatients()
  patients_map$init()
  patient <- patients_map$nextValue()
  episodes <- sys$getEpisodes(patient)
  
  episode_count <- 0
  episodes$init()
  while (episodes$hasNext) {
    episode <- episodes$nextValue()
    episode_count <- episode_count + 1
    
    expect_false(is.null(episode$Admission))
    expect_false(is.null(episode$Discharge))
  }
  
  expect_equal(episode_count, 2)
})

test_that("System handles multiple facilities correctly", {
  # 1 patient, 2 facilities
  facilities <- c(1, 1, 1,   2, 2, 2)
  units      <- c(1, 1, 1,   1, 1, 1)
  times      <- c(0, 1, 2,   5, 6, 7)
  patients   <- c(1, 1, 1,   1, 1, 1)
  types      <- c(1, 11, 2,  1, 11, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  expect_equal(sys$getFacilities()$size(), 2,
               label = "should have 2 facilities")
  
  # Should create 2 episodes (one per facility visit)
  expect_equal(sys$countEpisodes(), 2,
               label = "should have 2 episodes")
})

test_that("System handles multiple units within a facility", {
  # 1 patient, 1 facility, 2 units
  facilities <- c(1, 1, 1,   1, 1, 1)
  units      <- c(1, 1, 1,   2, 2, 2)
  times      <- c(0, 1, 2,   5, 6, 7)
  patients   <- c(1, 1, 1,   1, 1, 1)
  types      <- c(1, 11, 2,  1, 11, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  expect_equal(sys$getFacilities()$size(), 1,
               label = "should have 1 facility")
  expect_equal(sys$getUnits()$size(), 2,
               label = "should have 2 units")
  
  # Should create 2 episodes (one per unit)
  expect_equal(sys$countEpisodes(), 2,
               label = "should have 2 episodes for 2 units")
})

test_that("System get_log returns string", {
  facilities <- c(1, 1, 1, 1)
  units      <- c(1, 1, 1, 1)
  times      <- c(0, 1, 2, 3)
  patients   <- c(1, 1, 1, 1)
  types      <- c(1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  
  log <- sys$get_log()
  expect_type(log, "character",
              label = "get_log should return character string")
})
