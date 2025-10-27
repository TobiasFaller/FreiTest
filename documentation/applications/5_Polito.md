[Previous Topic](4_Scale4Edge.md) - [Index](../../README.md)

# MACISH (Approximate MAC Unit)

- `POLITO_MACISH_MULTIPLIER_FAULT_STATISTICS`: Analyzes multipliers M0 to M4 for error limits and lists faults where there is a non-linear correlation between fault propagation bit and error size
- `POLITO_MACISH_SAFE_FAULT_ANALYSIS_STUCK_AT`: Analyzes for safe faults in multipliers (8, 16, 32 bit) by doing statistical fault simulations and finding the maximum error per fault
- `POLITO_MACISH_SAFE_FAULT_ANALYSIS_TRANSITION`: Analyzes for safe faults in multipliers (8, 16, 32 bit) by doing statistical fault simulations and finding the maximum error per fault
- `POLITO_MACISH_SAFE_FAULT_ANALYSIS_CELL_AWARE`: Analyzes for safe faults in multipliers (8, 16, 32 bit) by doing statistical fault simulations and finding the maximum error per fault

Multiplier Statistic Options:

- `Polito/MacishMultiplierFaultStatistics/Verbosity <verbosity: options>`: If a fault shows non-linear behavior print all input combinations or just the relevant (non-linear) part
  - `ListAll`: Prints all input combinations for the fault
  - `ListRelevant`: Prints only the relevant port of input combinations for the fault
  - Default: ListRelevant

Safe Fault Analysis Options:

- `Polito/MacishSafeFaultAnalysis/Target <target: options>`: Set the analysis mode of the workflow
  - `Macish`: Run analysis on the whole MAC unit
  - `Multiplier`: Run analysis on only the multiplier
  - Default: Multiplier
- `Polito/MacishSafeFaultAnalysis/Bits <bits: options>`: The number of bits of the unit (multiplier / macish)
  - `8`: 8 Bit
  - `16`: 16 Bit
  - `32`: 32 Bit
  - Default: 8
- `Polito/MacishSafeFaultAnalysis/Complete <complete: options>`: Find exact maximum error of faults or approximate search with only `MinDigits` bits
  - `Complete`: Estimate the maximum error per fault with exactly 8 / 16 / 32 bit
  - `Incomplete`: Estimate the maximum error per fault with `MinDigit` bits and then assume all remaining bits to be 1 (maximum possible error)
  - Default: Complete
- `Polito/MacishSafeFaultAnalysis/MinDigits <value: uint>`: The number of most significant bits to estimate the maximum error with
  - Default: 0
- `Polito/MacishSafeFaultAnalysis/Trace <trace: options>`: For each fault export a trace of finding the maximum error
  - `Off`: No traces will be exported
  - `On`: Traces will be exported for each fault which shows the process of finding the maximum error
  - Default: Off
- `Polito/MacishSafeFaultAnalysis/Samples <samples: uint>`: The number normally distributed inputs to apply per fault to estimate the average error
  - Default: 100000

[Previous Topic](4_Scale4Edge.md) - [Index](../../README.md)
