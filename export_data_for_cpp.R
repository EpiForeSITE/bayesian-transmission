#!/usr/bin/env Rscript
# Export simulated.data to C++ format
# Format: facility unit time patient type [comment]

library(bayestransmission)

# Load the data
data("simulated.data")

# The data should be in a format with columns: facility, unit, time, patient, type
# Let's check what we have
cat("Structure of simulated.data:\n")
str(simulated.data)
cat("\nFirst few rows:\n")
print(head(simulated.data))

# Write to C++ format (space-separated, no header)
outfile <- "/home/bt/inst/original_cpp/simulated_data_cpp.txt"
write.table(simulated.data, file=outfile, 
            row.names=FALSE, col.names=FALSE, quote=FALSE)

cat("\nExported to:", outfile, "\n")
cat("Number of events:", nrow(simulated.data), "\n")
