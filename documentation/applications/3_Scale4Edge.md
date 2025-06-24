[Previous Topic](2_Circuit.md) - [Index](../../README.md)

# Scale4Edge Applications

## Test Pattern Generation and Export

Stuck-At Fault Model:

- `SCALE4EDGE_SAT_FULLSCAN_STUCK_AT_ATPG`: SAT-based full-scan stuck-at ATPG for a circuit
- `SCALE4EDGE_SAT_SEQUENTIAL_STUCK_AT_ATPG`: SAT-based sequential stuck-at ATPG for a circuit
- `SCALE4EDGE_BMC_SEQUENTIAL_STUCK_AT_ATPG`: BMC-based sequential stuck-at ATPG for a circuit
- `SCALE4EDGE_RANDOM_STUCK_AT_ATPG`: Stuck-at ATPG by using random data
- `SCALE4EDGE_LFSR_STUCK_AT_ATPG`: Stuck-at ATPG by using randomly generated polynominal LSFRs inputs
- `SCALE4EDGE_PATTERNS_TO_VCD_STUCK_AT`: Load existing test patterns and simulate them for stuck-at faults, export traces as VCD
- `SCALE4EDGE_PATTERNS_TO_STATISTICS_STUCK_AT`: Load existing test patterns and simulate them for stuck-at faults, export fault statistics
- `SCALE4EDGE_SAT_FULLSCAN_STUCK_AT_FUZZ`: Fuzzing of full-scan stuck-at ATPG with randomly generated circuits
- `SCALE4EDGE_BMC_SEQUENTIAL_STUCK_AT_FUZZ`: Fuzzing of sequential stuck-at ATPG with randomly generated circuits

Gross Transition-Delay Fault Model:

- `SCALE4EDGE_SAT_FULLSCAN_TRANSITION_ATPG`: SAT-based full-scan transition-delay fault ATPG for a circuit
- `SCALE4EDGE_SAT_SEQUENTIAL_TRANSITION_ATPG`: SAT-based sequential transition-delay fault ATPG for a circuit
- `SCALE4EDGE_BMC_SEQUENTIAL_TRANSITION_ATPG`: BMC-based sequential transition-delay fault ATPG for a circuit
- `SCALE4EDGE_RANDOM_TRANSITION_ATPG`: Transition-delay fault ATPG by using random data
- `SCALE4EDGE_LFSR_TRANSITION_ATPG`: Transition-delay fault ATPG by using randomly generated polynominal LSFRs inputs
- `SCALE4EDGE_PATTERNS_TO_VCD_TRANSITION`: Load existing test patterns and simulate them for transition-delay faults, export traces as VCD
- `SCALE4EDGE_PATTERNS_TO_STATISTICS_TRANSITION`: Load existing test patterns and simulate them for transition-delay faults, export fault statistics
- `SCALE4EDGE_SAT_FULLSCAN_STUCK_TRANSITION`: Fuzzing of full-scan transition-delay fault ATPG with randomly generated circuits
- `SCALE4EDGE_BMC_SEQUENTIAL_STUCK_TRANSITION`: Fuzzing of sequential transition-delay fault ATPG with randomly generated circuits

Cell-Aware Fault Model:

- `SCALE4EDGE_SAT_FULLSCAN_CELL_AWARE_ATPG`: SAT-based full-scan cell-aware ATPG for a circuit
- `SCALE4EDGE_SAT_SEQUENTIAL_CELL_AWARE_ATPG`: SAT-based sequential cell-aware ATPG for a circuit
- `SCALE4EDGE_BMC_SEQUENTIAL_CELL_AWARE_ATPG`: BMC-based sequential cell-aware ATPG for a circuit
- `SCALE4EDGE_RANDOM_CELL_AWARE_ATPG`: Cell-aware ATPG by using random data
- `SCALE4EDGE_LFSR_CELL_AWARE_ATPG`: Cell-aware ATPG by using randomly generated polynominal LSFRs inputs
- `SCALE4EDGE_PATTERNS_TO_VCD_CELL_AWARE`: Load existing test patterns and simulate them for cell-aware faults, export traces as VCD
- `SCALE4EDGE_PATTERNS_TO_STATISTICS_CELL_AWARE`: Load existing test patterns and simulate them for cell-aware faults, export fault statistics
- `SCALE4EDGE_SAT_FULLSCAN_CELL_AWARE_FUZZ`: Fuzzing of full-scan cell-aware ATPG with randomly generated circuits (warning: broken right now)
- `SCALE4EDGE_BMC_SEQUENTIAL_CELL_AWARE_FUZZ`: Fuzzing of sequential cell-aware ATPG with randomly generated circuits (warning: broken right now)


## Test Pattern Generation Options (Basic)

- `Scale4Edge/TestPatternGeneration/StatisticsDataExport <enabled: options>`: Enables the detailed export of fault coverage and generated test pattern statistics
  - `Disabled`: No additional information will be exported
  - `Enabled`: Additional iteration-based data will be exported
  - Default: Disabled
- `Scale4Edge/TestPatternGeneration/StatisticsDataPlot <enabled: options>`: Enables plotting the detailed fault coverage and generated test pattern statistics via GNU Plot. Depends on `StatisticsDataExport` to be enabled
  - `Disabled`: The statistics will not be plotted
  - `Enabled`: Additional iteration-based data will be plotted
  - Default: Disabled
- `Scale4Edge/TestPatternGeneration/PrintFaultStatisticReport <enabled: options>`: Enables giving a report about fault statistics at the end of the ATPG
  - `PrintDetail`: Prints a detailed report about the fault statistics, listing the status of each fault
  - `PrintSummary`: Prints a short summary about the fault statitics
  - `PrintNothing`: Prints no report and no summary
  - Default: PrintNothing
- `Scale4Edge/TestPatternGeneration/PrintTestPatternReport <enabled: options>`: Enables giving a report about test pattern statistics at the end of the ATPG
  - `PrintDetail`: Prints a detailed report about the test patterns, listing each test pattern
  - `PrintSummary`: Prints a short summary about the test pattern statitics
  - `PrintNothing`: Prints no report and no summary
  - Default: PrintSummary
- `Scale4Edge/TestPatternGeneration/PrintFaultListReport <enabled: options>`: Enables giving a report about fault statistics at the end of the ATPG
  - `PrintDetail`: Prints a detailed report about the faults, listing each fault
  - `PrintSummary`: Prints a short summary about the faults
  - `PrintNothing`: Prints no report and no summary
  - Default: PrintSummary
- `Scale4Edge/TestPatternGeneration/VcdExport <enabled: options>` Enables the export of the generated test patterns to VCD files.
  - `Disabled`: No patterns are exported
  - `Enabled`: The generated test patterns are exported to the data export directory in VCD format
  - Default: Disabled
- `Scale4Edge/TestPatternGeneration/VcdExportDirectory <directory: string>` The directory where to store the VCD files simulating test patterns
  - Default: ""
- `Scale4Edge/TestPatternGeneration/VcdExportSamples <samples: uint>` The number of samples to export, or 0 to export all VCD files
  - Default: 0 (Export all)
- `Scale4Edge/TestPatternGeneration/VcdExportCircuitName <filename: string>` The filename to export the parsed circuit to, matching the VCD files
  - Default ""
- `Scale4Edge/TestPatternGeneration/FaultListSource <source: options>` The source for the used fault list
  - `FreiTest`: The fault list is automatically generated using the circuit
  - `File`: The fault list is loaded from a file
  - Default: FreiTest
- `Scale4Edge/TestPatternGeneration/FaultListFile <filename: string>` The path to the fault list file
  - Default: ""
- `Scale4Edge/TestPatternGeneration/TestPatternExport <enabled: options>` Enables the export of the generated test patterns to a patterns.json file.
  - `Disabled`: No patterns are exported
  - `Enabled`: The generated test patterns are exported to the data export directory in "test pattern exchange format" (JSON)
  - Default: Disabled
- `Scale4Edge/TestPatternGeneration/PatternGenerationThreadLimit <threads: uint>`: The number of parallel threads to use to generate test patterns.
  A value of 0 is equivalent to the number of cores in the system.
  - Default: 0 (Unconstrained)
- `Scale4Edge/TestPatternGeneration/SolverThreadLimit <threads: uint>`: The maximum number of parallel solver threads to use for the generation of one test pattern.
  A value of 0 is equivalent to the number of cores in the system.
  - Default: 1
- `Scale4Edge/TestPatternGeneration/SolverTimeout <time: uint>`: The absolute solver timeout in seconds that is used for each generated test pattern
  - Default: 600 (10 minutes)
- `Scale4Edge/TestPatternGeneration/SolverUntestabilityTimeout <time: uint>`: The absolute solver timeout in seconds that is used for testing combinational fault untestability
  - Default: 180 (30 minutes)
- `Scale4Edge/TestPatternGeneration/SimulationThreadLimit <threads: uint>`: The maximum number of parallel threads to use for test pattern simulation \
  This option imposes only an additional limit if the test pattern compaction is enabled and multiple faults are simulated in parallel.
  A value of 0 is equivalent to the number of cores in the system.
  - Default: 0 (Unconstrained)
- `Scale4Edge/TestPatternGeneration/FaultStartIndex <index: uint>`: The start index of the fault in the fault list where the ATPG should start.
  - Default: 0 (From the start)
- `Scale4Edge/TestPatternGeneration/FaultEndIndex <index: uint>`: The end index of the fault in the fault list where the ATPG should start.
  - Default: (2^64)-1 (INT_MAX) (Till the end)
- `Scale4Edge/TestPatternGeneration/FaultListFilter <filter: string>`: A regex to apply as filter to the fault list. Faults that don't match the filter are removed (applied before FaultListExclude)
  - Default ".*"
- `Scale4Edge/TestPatternGeneration/FaultListExclude <filter: string>`: A regex to apply as exclusion filter to the fault list. Faults that match the filter are removed (applied after FaultListFilter)
- `Scale4Edge/TestPatternGeneration/FaultSimulation <options>` Sets the simulation of generated test patterns for all unclassified faults.
  - `Disabled`: Only simulate the test pattern for the single targeted fault
  - `Enabled`: Simulate the test pattern for all unclassified faults
  - Default: Enabled
- `Scale4Edge/TestPatternGeneration/UdfmImportPath <file: string>`: The file where the UDFM (User-Defined Fault Model) is located (cell-aware fault model only).\
  If no UDFM file is provided for a cell-aware workflow, a fatal error will occur.
  - Default: "" (empty)
- <span style="color: #0A5; font-weight: bold">(debug)</span>  `Scale4Edge/TestPatternGeneration/IncrementalSimulation <enabled: options>`: Disables the more efficient, incremental simulation.
  - `Disabled`: Only a naiive, slower simulation is performed.
  - `Enabled`: An optimized incremental simulation is performed.
  - Default: Enabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/SimulateAllFaults <enabled: options>`: Enables the simulation of all faults. This checks for faults that have wrongly be classified as untestable.
  - `Disabled`: No additional fault simulations are conducted
  - `Enabled`: Each test pattern is simulated for all faults
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckSimulation <enabled: options>`: Enables comparison of the incremental simulation with a slower naiive implementation.
  - `Disabled`: No additional fault simulations are conducted
  - `Enabled`: Two fault simulations are conducted for each pattern / fault
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckSimulationInitialState <enabled: options>`: Compares the initial state of the simulation result with the ATPG result and good / bad simulation.
  - `Disabled`: No additional checks are performed.
  - `CheckEqual`: Additional checks are performed.
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckSimulationInputs <enabled: options>`: Compares the inputs of the simulation result with the ATPG result and good / bad simulation.
  - `Disabled`: No additional checks are performed.
  - `CheckEqual`: Additional checks are performed.
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckSimulationFlipFlops`: Compares all logic values of secondary inputs for the generated test pattern with the good simulation result (except the fault location).
  - `Disabled`: No additional checks are performed.
  - `CheckEqual`: Additional checks are performed.
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckAtpgResult <enabled: options>`: Compares all logic values of the ATPG with the logic simulation.
  - `Disabled`: No additional checks are performed.
  - `CheckEqual`: Additional checks are performed.
  - Default: Disabled
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/CheckMaxIterationCovered <enabled: options>`: Checks for all faults covered by the simulation if the fault was wrongly classified as "aborted-max-iterations" by the SAT-solver.
  - `Disabled`: No additional checks are performed.
  - `Enabled`: Additional checks are performed.

## Test Pattern Generation Options (Sequential)

- `Scale4Edge/TestPatternGeneration/SolverMaximumTimeframes <timeframes: uint>`: The maximum number of clock cycles that are targeted by the test pattern generation.
  - Default: 10
- `Scale4Edge/TestPatternGeneration/CombinationalTestabilityCheck <enabled: options>`: Enables the combinational testability check of all faults \
  This checks for faults that can easily be proven to be untestable.
  - `Disabled`: No combinational testability check is conducted
  - `Enabled`: Each combinational testability check for all faults
  - Default: Enabled

## Validity Checker Module Options

- `Scale4Edge/TestPatternGeneration/Vcm <enabled: options>` Enables or disables usage of the VCM (Validity Checker Module) during ATPG
  - `Enabled`: Use the specified VCM and apply it during ATPG and fault simulation
  - `Disabled`: Don't use the VCM
  - Default: Disabled
- `Scale4Edge/TestPatternGeneration/VcmBaseDirectory <directory: string>`: The directory where to load the VCM from
  - Default: "" (empty)
- `Scale4Edge/TestPatternGeneration/VcmImportFilename <filename: string>`: The filename of the VCM.
  This filename will be searched in the VCM import directories.
  - Default: "" (empty)
- `Scale4Edge/TestPatternGeneration/VcmLibraryFilename <filename: string>`: The filename of the cell adapter library for the VCM.
  This option can be specified multiple times to define multiple files.
  If you do not specify a file name then the option will be reset to an empty list.
  - Default: "" (empty)
- `Scale4Edge/TestPatternGeneration/VcmTopLevelModuleName <module: string>`: The name of the top level VCM verilog module \
  This option can be set to "LAST_DEFINED" to use the last defined verilog module as top level module.
  - Default: "" (empty)
- `Scale4Edge/TestPatternGeneration/VcmTag <tag: string>`: Adds a tag (non-empty value) to the VCM or clears (empty value) all tags. Tags are used by the watchers and printers to filter entries.
  - Example: `--Scale4Edge/TestPatternGeneration/VcmTag "riscv_rv32i"` adds the `riscv_rv32i` tag to the tag list
  - Default: [] (empty list)
- `Scale4Edge/TestPatternGeneration/VcmParameter <parameter:  string>`: Adds a VCM parameter (non-empty value) to the VCM or clears (empty value) all parameters \
  Parameters are used by the VCM as configuration inputs.
  The parameter is giving with a value, separated by an equals sign.
  - Example: `--Scale4Edge/TestPatternGeneration/VcmParameter "allow_stall=01"` adds the parameter `allow_stall` parameter with two bits to the parameter list
- `Scale4Edge/TestPatternGeneration/VcmConfiguration <configuration: string>`: Specifies a configuration to read from the configurations file
  - Default: ""
- `Scale4Edge/TestPatternGeneration/VcmConfigurationsFilename <filename: string>`: Specifies the path to a JSON file containing parameters for the VCM
  - Default: ""
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/VcmExportPreprocessedFilename <filename: string>`: Exports the preprocessed Verilog content \
  If the filename is not empty a Verilog file will be created where specified.
  The content of the file contains the (macro) preprocessed Verilog source code.
  - Default: "" (empty)
- <span style="color: #0A5; font-weight: bold">(debug)</span> `Scale4Edge/TestPatternGeneration/VcmExportProcessedFilename <filename: string>` Exports the processed Verilog content \
  If the filename is not empty a Verilog file will be created where specified.
  The content of the file contains the parsed Verilog modules exported as Verilog source code.
  - Default: "" (empty)

The VCM parameters JSON file format is as follows:
It consists of a list with names configurations.
Each configuration has a list of parameters that are defiend.
Parameters can have one of three types (binary, uint32, uint64).
The values for the uint types can start with "0x" to use the hexadecimal notation.
See an example below:

```jsonc
[
	{
		"name": "config1",
		"parameters": [
			{ "name": "myparam1", "value": "00010", "type": "binary" },
			{ "name": "myparam2", "value": "0x00010000", "type": "uint32" },
			{ "name": "myparam3", "value": "5", "type": "uint32" },
		],
		"tags": [ "mytag1", "mytag2" ]
	},
	{
		"name": "config2",
		"parameters": [
			{ "name": "myparam1", "value": "00011", "type": "binary" },
			{ "name": "myparam2", "value": "0x00020000", "type": "uint32" },
			{ "name": "myparam3", "value": "6", "type": "uint32" },
		],
		"tags": [ "mytag3", "mytag4" ]
	}
]
```


## Fault Compaction

- `Scale4Edge/FaultCompaction/CompactionOrder <order: options>`: Defines the order in which test patterns are selected for inclusion
  - `Ascending`: Apply test patterns in order of the input format, drop unnecessary patterns
  - `Descending`: Apply test patterns in reverse order of the input format, drop unnecessary patterns
  - Default: Ascending
- `Scale4Edge/FaultCompaction/UdfmImportPath <file: string>`: The file where the UDFM (User-Defined Fault Model) is located (cell-aware fault model only).\
  If no UDFM file is provided for a cell-aware workflow, a fatal error will occur.
  - Default: "" (empty)

## Fault Coverage Export

- `Scale4Edge/FaultCoverageExport/FaultCoverageFileName <filename: string>`: The file name to export the coverage as JSON format to
  - Default: ""
- `Scale4Edge/FaultCoverageExport/SimulationThreadLimit <threads: uint>`: The number of threads to use for simulating the test patterns in parallel
  - Default: 0 (unlimited)
- `Scale4Edge/FaultCoverageExport/ExportThreadLimit <threads: uint>`: The number of threads to write the per-pattern coverage files
  - Default: 0 (unlimited)
- `Scale4Edge/FaultCoverageExport/UdfmImportPath <file: string>`: The file where the UDFM (User-Defined Fault Model) is located (cell-aware fault model only).\
  If no UDFM file is provided for a cell-aware workflow, a fatal error will occur.
  - Default: "" (empty)

[Previous Topic](2_Circuit.md) - [Index](../../README.md)
