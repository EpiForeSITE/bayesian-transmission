#!/usr/bin/env Rscript
# Check if insitu events are being created properly

library(bayestransmission)
data("simulated.data", package = "bayestransmission")

cat("\n=== Checking Event Type Conversion ===\n\n")

# Check the raw data
cat("Raw data event types at time 0:\n")
time_zero <- simulated.data[simulated.data$time < 0.001, ]
print(table(time_zero$type))
cat("Event type 0 at time 0:", sum(time_zero$type == 0), "events\n")

# Look at the first few time-zero events
cat("\nFirst 10 time-zero events:\n")
print(head(time_zero, 10))

# Now check what the C++ sees after System construction
# We need to actually run some initialization to see this
modelParams <- LinearAbxModel(nstates = 2L)

cat("\n\nNow checking what happens in C++ initialization...\n")
cat("(This requires running the model to see internal state)\n\n")

# Try running just initialization
cat("Note: The C++ System constructor should convert time=0 'admission' events to 'insitu' events\n")
cat("This is done in System::setInsitus() called from System::init()\n\n")

cat("Patient 698 should be converted from admission (0) to insitu\n")
cat("Patient 698 events in raw data:\n")
p698 <- simulated.data[simulated.data$patient == 698, ]
print(p698)
