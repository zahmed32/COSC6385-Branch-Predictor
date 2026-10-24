Name- Zarif Ahmed
PSID- 2281643

# Branch Prediction Assignment

Build and run the hybrid branch predictor using the supplied CBP2 simulator and benchmark traces. See the report for design details and analysis.

# Requirements

- macOS or a compatible Unix environment
- A C++ compiler available as g++
- make, bzip2, and awk

The supplied traces remain compressed- the simulator decompresses them automatically.

# Build

Start in the branch-prediction assignment folder:

cd cbp2-infrastructure-v2/src
make predict


This creates predict, which runs pm_predictor from my_predictor.h

# Run one trace

From the same src directory:

./predict ../traces/164.gzip/gzip.trace.bz2


Expected output for the submitted configuration:


12.570 MPKI

MPKI means mispredictions per thousand program instructions. Lower is better; it is not a percentage. Replace the trace path to test another benchmark.