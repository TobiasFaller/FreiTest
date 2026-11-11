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

# Switching Activity Maximization

- `POLITO_CPU_REPEATABLE_CONSTANT_SWITCHING_MAXIMIZATION`: Constructs a sequence of instructions that maximizes the switching activity of a sub-unit for burn-in when executed repeatedly
- `POLITO_MULTI_POINT_STIMULI_GENERATION`: Constructs a sequence of instructions that maximizes the switching activity in a set of nets for bridge defect burn-in when executed

Repeatable Constant Switching Maximization Options:

- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetHierarchyName <name: str>`: The path to the sub-unit to stress in the circuit
  - Default: `UNINITIALIZED_STRESS_TARGET`
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetCells <enable: options>`: Include / exclude nets inside cells in stressing
  - `IncludeCellNets`: Add nets that are present inside cells in the stressing
  - `ExcludeCellNets`: Don't add the nets to the list of stressed nets
  - Default: IncludeCellNets
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetSubmodules <enable: options>`: Inlcude / exclude nets inside sub-modules in stressing
  - `IncludeSubmodules`: Add nets that are present inside sub-modules of the stress target in the stressing
  - `ExcludeSubmodules`: Don't add the nets that are inside sub-modules to the stress target
  - Default: IncludeSubmodules
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/RiscvCore <disassemble: bool>`: Disassemble instructions according to RISC-V specification
  - `Yes`: Disassemble RISC-V instructions from `module/decode_instr` watcher
  - `No`: Don't attempt disassembly (for non RISC-V processors)
  - Default: No
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/Timeframes <timeframes: uint>`: The number of timeframes to unroll for
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateA <timeframe: uint>`: Timeframe for the first state (first instruction)
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateB <timeframe: uint>`: Timeframe for the second state (second instruction)
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateC <timeframe: uint>`: Timeframe for the third state that is forced to be equivalent to the first state to make the sequence repeatable (ideally is the last timeframe)
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/ModuleWatcher <path: str>`: Path of the processor's watchers file in JSON format to extract logic values
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/ModulePrinter <path: str>`: Path of the processor's printers file in JSON format to format and print the watched values
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/VcmWatcher <path: str>`: Path of the VCM's watchers file in JSON format to extract logic values
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/VcmPrinter <path: str>`: Path of the VCM's printers file in JSON format to format and print the watched values
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/MaxSATSolverSetting <maxsolver: options>`: The Max-SAT solver setting to choose
  - `WARNERS`: Configure Pacose with the `WARNERS` option
  - `BAILLEUX`: Configure Pacose with the `BAILLEUX` option
  - `ASIN`: Configure Pacose with the `ASIN` option
  - `OGAWA`: Configure Pacose with the `OGAWA` option
  - `BAILLEUXW2`: Configure Pacose with the `BAILLEUXW2` option
  - `WEIGHTED_TOTALIZER`: Configure Pacose with the `WEIGHTED_TOTALIZER` option
  - `MIXED_RADIX_WEIGHTED_TOTALIZER`: Configure Pacose with the `MIXED_RADIX_WEIGHTED_TOTALIZER` option
  - `MIXED_RADIX_WEIGHTED_TOTALIZER2`: Configure Pacose with the `MIXED_RADIX_WEIGHTED_TOTALIZER2` option
  - `MIXED_RADIX_WEIGHTED_TOTALZIER19`: Configure Pacose with the `MIXED_RADIX_WEIGHTED_TOTALZIER19` option
  - `MIXED_RADIX_WEIGHTED_TOTALIZER19_2`: Configure Pacose with the `MIXED_RADIX_WEIGHTED_TOTALIZER19_2` option
  - `DYNAMIC_GLOBAL_POLYNOMIAL_WATCHDOG`: Configure Pacose with the `DYNAMIC_GLOBAL_POLYNOMIAL_WATCHDOG` option
  - `HEURISTIC_Q_MAXSAT_17`: Configure Pacose with the `HEURISTIC_Q_MAXSAT_17` option
  - `HEURISTIC20`: Configure Pacose with the `HEURISTIC20` option
  - `HEURISTIC1819`: Configure Pacose with the `HEURISTIC1819` option
  - `Q_MAXSAT_19`: Configure Pacose with the `Q_MAXSAT_19` option
  - `WBO`: Configure INC-BMO with the `WBO` option
  - `LINEAR_SU`: Configure INC-BMO with the `LINEAR_SU` option
  - `PART_MSU3`: Configure INC-BMO with the `PART_MSU3` option
  - `LSU_INCBMO`: Configure INC-BMO with the `LSU_INCBMO` option
  - `LSU_MRSBEAVER`: Configure INC-BMO with the `LSU_MRSBEAVER` option
  - `LSU_MCS`: Configure INC-BMO with the `LSU_MCS` option
  - `OLL`: Configure INC-BMO with the `OLL` option
  - `BEST`: Configure INC-BMO with the `BEST` option
- `PoliTO/RepeatableConstantSwitchingStimuliGeneration/LiteralDChainOptimization <enable: options>`: Enable or disable D-Chain-like structural switching encoding for nets to speed up solving
  - `Yes`: Enable switching encoding and chaining of the switching literals
  - `No`: Don't encode a switching chain
  - Default: No

Multipoint Switching Maximization Options:

- `PoliTO/MultiPointStressStimuliGeneration/NetSampleSource <source: options>`:
  - `FreiTest`: Automatically generate the list of net pairs from the net names list given by `NetNamesPath`
  - `File`: Read the list of net pairs to switch from the net pair list given by `NetSamplesPath`
  - Default: None
- `PoliTO/MultiPointStressStimuliGeneration/NetNamesPath <path: str>`: Path to a list of nets separated by newlines
- `PoliTO/MultiPointStressStimuliGeneration/NetSampleCount <samples: uint>`: Number of switching pairs to generate (`NetSampleSource` set to `FreiTest` only)
- `PoliTO/MultiPointStressStimuliGeneration/NetSampleSize <set: uint>`: The size of each net pair / set
  - Default: 2
- `PoliTO/MultiPointStressStimuliGeneration/NetSamplesPath <path: str>`: Path to a list of net pairs separated by newlines. The pairs on each line are separated by semicolons
- `PoliTO/MultiPointStressStimuliGeneration/ModuleWatcherModuleWatcher <path: str>`: Path of the processor's watchers file in JSON format to extract logic values
- `PoliTO/MultiPointStressStimuliGeneration/ModulePrinterModulePrinter <path: str>`: Path of the processor's printers file in JSON format to format and print the watched values
- `PoliTO/MultiPointStressStimuliGeneration/VcmWatcherVcmWatcher <path: str>`: Path of the VCM's watchers file in JSON format to extract logic values
- `PoliTO/MultiPointStressStimuliGeneration/VcmPrinterVcmPrinter <path: str>`: Path of the VCM's printers file in JSON format to format and print the watched values
- `PoliTO/MultiPointStressStimuliGeneration/ExportRiscvStressPrograms <enable: options>`:
  - `Enabled`: Enable export of RISC-V instructions from stress result. Requires `PrologueEpilogue` to be specified
  - `Disabled`: Do not export the stress program
  - Default: Disabled
- `PoliTO/MultiPointStressStimuliGeneration/PrologueEpilogue <path: str>`: Path to file containing assembly prologue and epilogue for program generation \
  The prologue is to be specified in the file between lines containing only PROLOGUE_BEGIN and PROLOGUE_END.
  The epilogue is to be specified in the file between lines containing only EPILOGUE_BEGIN and EPILOGUE_END
- `PoliTO/MultiPointStressStimuliGeneration/DisassemblerSource <path: str>`: Path to lua script handling the RISC-V disassembly from watcher data `module/fetch_instruction_data` \
  See `scripts/polito/rv_disassembler.lua` for reference
- `PoliTO/MultiPointStressStimuliGeneration/DisassemblerJsonSource <path: str>`: Path to JSON source file containing the RISC-V instruction set specification \
  See `scripts/generators/generate_vcm/json-dbt-rise/*.json` for example specifications

[Previous Topic](4_Scale4Edge.md) - [Index](../../README.md)
