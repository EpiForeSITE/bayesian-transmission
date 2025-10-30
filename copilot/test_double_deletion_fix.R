#!/usr/bin/env Rscript
# Comprehensive test for double-deletion bug fix

cat("Testing double-deletion bug fix...\n\n")

devtools::load_all()

# Test 1: Basic scenario from original bug report
cat("Test 1: Basic getFacilities() scenario\n")
{
  facilities <- c(1, 1, 1, 1,   2, 2, 2, 2)
  units      <- c(1, 1, 1, 1,   1, 1, 1, 1)
  times      <- c(0, 1, 2, 3,   5, 6, 7, 8)
  patients   <- c(1, 1, 1, 1,   2, 2, 2, 2)
  types      <- c(1, 11, 12, 2,  1, 11, 12, 2)
  
  sys <- new(CppSystem, facilities, units, times, patients, types)
  fac_map <- sys$getFacilities()
  
  cat("  Facilities size:", fac_map$size, "\n")
  
  rm(fac_map, sys)
  gc()
  cat("  ✓ Test 1 passed - no segfault\n\n")
}

# Test 2: Multiple references
cat("Test 2: Multiple references to same object\n")
{
  sys <- new(CppSystem, c(1,1,1,1), c(1,1,1,1), c(0,1,2,3), c(1,1,1,1), c(1,11,12,2))
  fac1 <- sys$getFacilities()
  fac2 <- sys$getFacilities()
  pat1 <- sys$getPatients()
  pat2 <- sys$getPatients()
  
  cat("  fac1 size:", fac1$size, "\n")
  cat("  fac2 size:", fac2$size, "\n")
  cat("  pat1 size:", pat1$size, "\n")
  cat("  pat2 size:", pat2$size, "\n")
  
  rm(fac1, fac2, pat1, pat2, sys)
  gc()
  cat("  ✓ Test 2 passed - multiple references OK\n\n")
}

# Test 3: getPatients()
cat("Test 3: getPatients() scenario\n")
{
  sys <- new(CppSystem, c(1,1,1,1), c(1,1,1,1), c(0,1,2,3), c(1,1,1,1), c(1,11,12,2))
  pat_map <- sys$getPatients()
  
  cat("  Patients size:", pat_map$size, "\n")
  
  rm(pat_map, sys)
  gc()
  cat("  ✓ Test 3 passed - no segfault\n\n")
}

# Test 4: getEpisodes()
cat("Test 4: getEpisodes() scenario\n")
{
  sys <- new(CppSystem, c(1,1,1,1), c(1,1,1,1), c(0,1,2,3), c(1,1,1,1), c(1,11,12,2))
  pat_map <- sys$getPatients()
  pat_map$init()
  patient <- pat_map$nextValue()
  
  episodes <- sys$getEpisodes(patient)
  cat("  Episodes size:", episodes$size, "\n")
  
  rm(episodes, patient, pat_map, sys)
  gc()
  cat("  ✓ Test 4 passed - no segfault\n\n")
}

# Test 5: System deleted first
cat("Test 5: System deleted before maps\n")
{
  sys <- new(CppSystem, c(1,1,1,1), c(1,1,1,1), c(0,1,2,3), c(1,1,1,1), c(1,11,12,2))
  fac_map <- sys$getFacilities()
  pat_map <- sys$getPatients()
  
  rm(sys)  # Delete system first!
  gc()
  
  # Maps should still be valid (non-owning pointers)
  cat("  fac_map size:", fac_map$size, "\n")
  cat("  pat_map size:", pat_map$size, "\n")
  
  rm(fac_map, pat_map)
  gc()
  cat("  ✓ Test 5 passed - maps survive system deletion\n\n")
}

cat("═══════════════════════════════════════\n")
cat("All tests passed! Double-deletion bug is FIXED!\n")
cat("═══════════════════════════════════════\n")
