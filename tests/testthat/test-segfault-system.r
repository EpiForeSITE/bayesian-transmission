# Load the package
devtools::load_all()

# Create a System object
facilities <- c(1, 1, 1, 1,   2, 2, 2, 2)
units      <- c(1, 1, 1, 1,   1, 1, 1, 1)
times      <- c(0, 1, 2, 3,   5, 6, 7, 8)
patients   <- c(1, 1, 1, 1,   2, 2, 2, 2)
types      <- c(1, 11, 12, 2,  1, 11, 12, 2)  # admission, test, test, discharge

# Create System using the C++ constructor
sys <- new(CppSystem, facilities, units, times, patients, types)

# create a sub part
fac_map <- sys$getFacilities()

# Run garbage collection to clean up any previous objects

rm(fac_map, sys)
gc()
