#!/usr/bin/env Rscript

# Compare likelihood outputs from R package vs original C++

# Read original C++ output (skip header lines)
cpp_lines <- readLines("original_cpp_output.csv")
cpp_start <- which(cpp_lines == "Unit,Link,EventType,Time,PatientID,LogLik") + 1
cpp_data_lines <- cpp_lines[cpp_start:length(cpp_lines)]

# Parse C++ output
cpp_data <- do.call(rbind, lapply(cpp_data_lines, function(line) {
  parts <- strsplit(line, ",")[[1]]
  data.frame(
    Unit = as.integer(parts[1]),
    Link = as.integer(parts[2]),
    EventType = as.integer(parts[3]),
    Time = as.numeric(parts[4]),
    PatientID = parts[5],
    LogLik_CPP = ifelse(parts[6] == "-inf", -Inf, as.numeric(parts[6])),
    stringsAsFactors = FALSE
  )
}))

# Run R version and capture output
library(bayestransmission)
data(simulated.data)

# Capture R output
r_output <- capture.output({
  testHistoryLinkLogLikelihoods(simulated.data)
})

# Find where the actual data starts
r_start <- which(grepl("^Unit=", r_output))
r_data_lines <- r_output[r_start]

# Parse R output
r_data <- do.call(rbind, lapply(r_data_lines, function(line) {
  # Extract values using regex
  unit <- as.integer(sub(".*Unit=([0-9]+).*", "\\1", line))
  link <- as.integer(sub(".*Link=([0-9]+).*", "\\1", line))
  event <- as.integer(sub(".*EventType=([0-9]+).*", "\\1", line))
  time <- as.numeric(sub(".*Time=([0-9.e+-]+).*", "\\1", line))
  patient <- sub(".*PatientID=([^,]+).*", "\\1", line)
  loglik <- as.numeric(sub(".*LogLik=([0-9.e+-]+).*", "\\1", line))
  
  data.frame(
    Unit = unit,
    Link = link,
    EventType = event,
    Time = time,
    PatientID = patient,
    LogLik_R = loglik,
    stringsAsFactors = FALSE
  )
}))

# Merge the two datasets
comparison <- merge(cpp_data, r_data, by = c("Unit", "Link", "EventType", "Time", "PatientID"))

# Add comparison columns
comparison$CPP_is_inf <- is.infinite(comparison$LogLik_CPP) & comparison$LogLik_CPP < 0
comparison$R_is_inf <- is.infinite(comparison$LogLik_R) & comparison$LogLik_R < 0
comparison$Both_finite <- !comparison$CPP_is_inf & !comparison$R_is_inf
comparison$Diff <- ifelse(comparison$Both_finite, 
                          abs(comparison$LogLik_R - comparison$LogLik_CPP),
                          NA)

# Summary statistics
cat("\n=== COMPARISON SUMMARY ===\n")
cat(sprintf("Total history links: %d\n", nrow(comparison)))
cat(sprintf("C++ has -Inf: %d\n", sum(comparison$CPP_is_inf)))
cat(sprintf("R has -Inf: %d\n", sum(comparison$R_is_inf)))
cat(sprintf("Both have -Inf: %d\n", sum(comparison$CPP_is_inf & comparison$R_is_inf)))
cat(sprintf("Only C++ has -Inf: %d\n", sum(comparison$CPP_is_inf & !comparison$R_is_inf)))
cat(sprintf("Only R has -Inf: %d\n", sum(!comparison$CPP_is_inf & comparison$R_is_inf)))
cat(sprintf("Both finite: %d\n", sum(comparison$Both_finite)))

cat("\n=== FINITE VALUE COMPARISON ===\n")
if (sum(comparison$Both_finite) > 0) {
  finite_diff <- comparison$Diff[comparison$Both_finite]
  cat(sprintf("Max absolute difference: %.10e\n", max(finite_diff, na.rm = TRUE)))
  cat(sprintf("Mean absolute difference: %.10e\n", mean(finite_diff, na.rm = TRUE)))
  cat(sprintf("Median absolute difference: %.10e\n", median(finite_diff, na.rm = TRUE)))
  
  # Show cases with large differences
  large_diff_idx <- which(comparison$Both_finite & comparison$Diff > 0.01)
  if (length(large_diff_idx) > 0) {
    cat(sprintf("\n%d cases with |difference| > 0.01:\n", length(large_diff_idx)))
    print(comparison[large_diff_idx[1:min(10, length(large_diff_idx))], 
                     c("Unit", "Link", "EventType", "Time", "LogLik_CPP", "LogLik_R", "Diff")])
  }
}

# Show examples where C++ has -Inf but R doesn't
cpp_inf_not_r <- which(comparison$CPP_is_inf & !comparison$R_is_inf)
if (length(cpp_inf_not_r) > 0) {
  cat(sprintf("\n=== EXAMPLES: C++ has -Inf but R doesn't (showing first 20) ===\n"))
  print(head(comparison[cpp_inf_not_r, 
                        c("Unit", "Link", "EventType", "Time", "PatientID", "LogLik_CPP", "LogLik_R")], 20))
}

# Show examples where R has -Inf but C++ doesn't
r_inf_not_cpp <- which(!comparison$CPP_is_inf & comparison$R_is_inf)
if (length(r_inf_not_cpp) > 0) {
  cat(sprintf("\n=== EXAMPLES: R has -Inf but C++ doesn't (showing first 20) ===\n"))
  print(head(comparison[r_inf_not_cpp, 
                        c("Unit", "Link", "EventType", "Time", "PatientID", "LogLik_CPP", "LogLik_R")], 20))
}

# Save full comparison
write.csv(comparison, "likelihood_comparison.csv", row.names = FALSE)
cat("\nFull comparison saved to likelihood_comparison.csv\n")
