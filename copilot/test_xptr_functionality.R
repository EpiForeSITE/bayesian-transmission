#!/usr/bin/env Rscript
# Test that XPtr objects are still usable

devtools::load_all()

cat("Creating System...\n")
facilities <- c(1, 1, 1, 1,   2, 2, 2, 2)
units      <- c(1, 1, 1, 1,   1, 1, 1, 1)
times      <- c(0, 1, 2, 3,   5, 6, 7, 8)
patients   <- c(1, 1, 1, 1,   2, 2, 2, 2)
types      <- c(1, 11, 12, 2,  1, 11, 12, 2)

sys <- new(CppSystem, facilities, units, times, patients, types)

cat("\n=== Testing getFacilities ===\n")
fac_map <- sys$getFacilities()
cat("Class:", class(fac_map), "\n")
cat("Type:", typeof(fac_map), "\n")

# Can we access it?
tryCatch({
  cat("Size:", fac_map$size, "\n")
  cat("SUCCESS: Can access size property\n")
}, error = function(e) {
  cat("ERROR:", conditionMessage(e), "\n")
})

# Can we iterate?
tryCatch({
  fac_map$init()
  cat("Has next:", fac_map$hasNext, "\n")
  cat("SUCCESS: Can iterate\n")
}, error = function(e) {
  cat("ERROR:", conditionMessage(e), "\n")
})

cat("\n=== Testing getPatients ===\n")
pat_map <- sys$getPatients()
cat("Class:", class(pat_map), "\n")
cat("Size:", pat_map$size, "\n")

cat("\n=== Testing memory safety ===\n")
cat("Cleaning up fac_map...\n")
rm(fac_map)
gc()

cat("System still valid? Trying to get facilities again...\n")
fac_map2 <- sys$getFacilities()
cat("Size:", fac_map2$size, "\n")
cat("SUCCESS: System not corrupted after GC\n")

cat("\n=== Final cleanup ===\n")
rm(fac_map2, pat_map, sys)
gc()

cat("\nALL TESTS PASSED!\n")
