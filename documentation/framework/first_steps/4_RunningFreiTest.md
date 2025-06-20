[Previous Topic](3_BuildSetup.md) - [Index](../../README.md) - [Next Topic](5_DevelopingApplications.md)

# Running FreiTest

Usage: `freitest [log-options] [options]`

Arguments for [log-options]:

See the [logging chapter](#logging-configuration) below.

- `--log-verbose` Enables verbose logging, see chapter below
- `--log-verbose-level=` Sets the verbose logging level, see chapter below
- `--log-verbose-modules=` Sets the verbose logging level per module, see chapter below
- `--log-level=` Set the log level, see chapter below
- `--log-color=` Sets the color scheme used for logging

Arguments for [options]:

See the [configuration options](#configuration-files) for more information.

- `--OPTION VALUE` Assign the value `VALUE` to the configuration option `OPTION`.
  See "Configuration options" below for possible options.
  Options not listed there are passed directly to the application.
  See "Application selection option" below for additional options for each application.
- Use the option `--Settings` to load configuration options from a settings file

## Logging Configuration

The logging of the application can be configured via the `--log-level` option.
All levels are sorted hierarchically in the order shown below.
See the examples under verbose mode for more information.

Log levels (in increasing verbosity):

- **fatal**: Show only fatal errors which abort the application
- **error**: Show also errors which are severe
- **warn**: Show also warnings
- **info** (default): Show all normal messages, warnings and above
- **verbose** with level 1 (lowest) up to 9 (highest): \
  Additionally show verbose output with up to the specified verbosity
- **debug**: Show information which is relevant to debugging \
  This does not enable verbose mode by default.
  All debug outputs can be disabled statically during compilation by using the NDEBUG option.
- **trace**: Show stack traces and detailed error information for debugging \
  This does not enable verbose mode by default.
  The trace output can not be disabled via a macro definition.

The color scheme can be modified with the `--log-color` option.
The following schemes are currently supported.

Log color schemes:

- **dark**: Colors optimized for a dark terminal
- **light**: Colors optimized for a light terminal
- **none**: No colors are used (useful when redirecting into a file)

Verbose mode:

- The `-v` and `--verbose` switches enable the verbose output up to level 9 (highest, most detailed).
  If the log level is lower than verbose mode it will be set to **verbose**.
- The `--log-verbose-level=<level>` switch enables verbose mode up to the specified level.
  As above, the verbose mode will be turned on if the current log level is less verbose.
- The `--log-verbose-modules=<modules>` switch enables verbose mode for the specified modules with the specified levels.
  The verbose mode will be enabled if it is not active and all modules will have a verbosity of 0 (off) by default.
  When this switch is specified the value of the `--log-verbose-level` switch is ignored.
  Below is an example of how to use this switch to selectively enable verbose module for different modules.

```bash
# Enable verbose logging
freitest --verbose

# Enable only warnings and more severe messages
freitest --log-level=warn

# Disable the color output
freitest --log-color=none

# Enable verbose messages with up to level 9 (highest)
freitest --log-level=verbose --log-verbose-level=9

# Enable verbose logging level 9 for the VerilogParser module
# and verbose level 5 for the CircuitGenerator module
freitest --log-verbose-modules=VerilogParser=9,CircuitGenerator=5

# Enable verbose logging level 9 for the VerilogParser module
# and verbose level 5 for the CircuitGenerator module
freitest --log-level=verbose --log-verbose-modules=VerilogParser=9,CircuitGenerator=5

# Enable verbose logging level 9 for the VerilogParser module
# and enable debug messages and stack traces
freitest --log-level=trace --log-verbose-modules=VerilogParser=9

# Be careful! The ORDER MATTERS:
# This example enables debug logging and then changes
# to the warn level, which disables debug logging
freitest --log-level=debug --log-level=warn

# Be careful! The ORDER MATTERS:
# This example enables verbose mode and disables it with
# the next --log-level switch
freitest --log-verbose-modules=VerilogParser=9 --log-level=info
```

See easylogging++ on [github.com/amrayn/easyloggingpp](https://github.com/amrayn/easyloggingpp) for more information.

## Application Settings

Options can either specified on the command line or in setting files.
The order of declaration is important and parameters are typically overwritten by later declarations (some allow multiple declarations to append elements).
The `--Setting` option can be used to load a settings file from the command line and acts as if the options in the file are specified at this location on the command line.

The settings file is in the JSON with Comments format and contains key-value pairs and comments.
Comments start either with `//` (single-line) or are between `/*` and `*/` (multi-line).
File and directory names may contain references in square brackets that are resolved when the file / directory is accessed in an application.
This enables to define options that use placeholders that are specified by the user.
Below is an example settings file that runs the full-scan stuck-at ATPG workflow for the ITC 99 benchmarks:

```jsonc
[
  // The application that is run by the framework
  {"application": "SCALE4EDGE_SAT_FULLSCAN_STUCK_AT_ATPG"},

  // Directory settings
  {"define": "OutputDir", "value": "./output"},
  {"define": "CircuitBaseDir", "value": "./data/benchmarks/public/itc99"},
  {"define": "LibraryDir", "value": "./data/cell-libraries/generic"},

  {"setting": "DataExportDirectory", "value": "[OutputDir]/[CircuitName]"},
  {"setting": "StatisticsExportFilename", "value": "[DataExportDirectory]/statistics.json"},

  // Source file settings
  {"setting": "CircuitName", "value": "b01"},
  {"setting": "CircuitSourceType", "value": "Verilog"},
  {"setting": "CircuitBaseDirectory", "value": "[CircuitBaseDir]"},
  {"setting": "VerilogLibraryFilename", "value": "[LibraryDir]/generic_freitest.v"},
  {"setting": "VerilogImportFilename", "value": "[CircuitName]-generic.v"},
]
```

Special statements that affect the parsing process:

- `{"application": "APPLICATION"}` Sets the workflow that is executed by the framework
- `{"define": "NAME", "value": "VALUE"}` Creates a new placeholder that can be used for file paths
- `{"undefine": "NAME"}` Deletes the definition of the placeholder and makes it unavailable
- `{"include": "PATH"}` Includes another configuration file

Usable reference / placeholder values for paths:

- `[CircuitName]`: The name of the targeted circuit
- `[DataImportDirectory]`: The directory from which to load existing data
- `[DataExportDirectory]`: The directory to which to store generated data
- All values previously defined with `define` statements (example below)

```jsonc
{
  {"define": "TestPath", "value": "my_tests"},
  {"setting": "CircuitBaseDirectory", "value": "data/benchmarks/[TestPath]"},
}
```

## FreiTest settings

Legend: Configuration value types for the following sections

- `none`: This setting's value is ignored.
- `string`: This setting accepts a string (text) as value.
- `int`: This setting accepts a number as value.
- `float`: This setting accepts a number with decimal places as value.
- `bool`: This setting accepts the value 0 or 1 as value.
- `file`: This setting accepts a file or directory path as value. Placeholders might be supported.
- `options`: The accepted values for the setting are given below the option name.

Framework options:

- <span style="color: #C21; font-weight: bold">(essential)</span> `Application <options>` Selects the goal / application to run
  - `None`: Does not run any application at all \
    This mode can be used to test the configuration files.
  - Other options: See below
  - Default: "None"
- `DataImportDirectory <path: file>` Import directory for pre-existing data that is required for the workflows execution
  - Default: "./output/"
- `DataExportDirectory <path: file>` Export directory for data that is generated by the workflow
  - Default: "./output/"
- `StatisticsExportFilename <file: file>` The file to export the statistics to that were collected by the workflow
  - Default: "./output/[Circuit]_stat.xml"
- `Parameter <param: string>` Passes the parameter to the target application by appending it to the parameter list
  - Default: "" (empty)

Solver options:

- `SatSolver <options>` The SAT-solver to use for SAT-based problems. \
  This option is only a suggestion and the workflow is free to ignore this configuration option.
  - `PROD_SAT_SINGLE_GLUCOSE_421`: Single-threaded Glucose 4.2.1 solver for medium to large circuits. 
  - `PROD_SAT_PARALLEL_GLUCOSE_421`: Multi-threaded Glucose 4.2.1 solver for medium to large circuits that might be slower than the single-threaded version in some cases. Use only when solving large SAT-problems with few solver invocations where no parallelism is otherwise possible.
  - `PROD_SAT_CADICAL`: Cadical solver suitable for medium to large circuits.
  - `PROD_SAT_MINISAT`: MiniSat solver suitable for medium to large circuits.
  - `PROD_SAT_CRYPTO_MINISAT`: CryptoMiniSat solver that might be better suited for circuits with a lot of XOR-gates. Use with caution as the supplied version is buggy.
  - `PROD_SAT_IPASIR`: Generic IPASIR API solver that is statically linked into the framework. The used solver depends on the library configuration.
  - `PROD_MAX_SAT_PACOSE`: The Max-SAT Pacose solver that supports multiple solver backends.
  - `PROD_MAX_SAT_INC_BMO_COMPLETE`: The incremental Max-SAT Open WBO solver running with Glucose 4.1.
  - `EXPORT_SAT_DIMACS`: Debug export "solver" that writes the SAT-problem into a DIMACS (CNF) file and does not solve anything.
  - `EXPORT_MAX_SAT_DIMACS`: Debug export "solver" that writes the SAT-problem into a weighted DIMACS (WCNF) file and does not solve anything.
- `BmcSolver <options>` The BMC-solver to use for BMC-based problems. \
  This option is only a suggestion and the workflow is free to ignore this configuration option.
  - `PROD_NCIP`: BMC-solver that supports Craig interpolation developed by the University of Freiburg.
  - `EXPORT_CIP`: Debug export "solver" that writes the BMC-problem into a CIP (Craig interpolant prover) file and does not solve anything.
- `IpasirSatSolverLibrary <options>` Set the path for the Ipasir sat-solver library [optional].

Circuit options:

- `CircuitSourceType <options>` Selects the parser for the circuit input
  - `None`: Do not load a circuit
  - `Verilog`: Use the Verilog parser to load the circuit
  - Default: None
- `CircuitName <name: string>` The name that is used internally for the circuit
  - Default: "UnnamedCircuit"
- `CircuitBaseDirectory <directory: file>` This directory will be used to search for files. \
  When searching for a file to read the framework will search in each directory in the order they where specified.
  This option can be specified multiple times to define multiple directories.
  If you do not specify a file name then the option will be reset to an empty list.
  - Default:
    - "." (Current working directory)
- `VerilogImportFilename <file: file>` The Verilog source for the top level module. \
  This option can be specified multiple times to define multiple imported source files.
  If you do not specify a file name then the option will be reset to an empty list.
  - Default: "" (empty)
- `VerilogLibraryFilename <file: file>` Specifies a Verilog library which holds standard cell implementations. \
  This option can be specified multiple times to define multiple imported library files.
  If you do not specify a file name then the option will be reset to an empty list.
  - Default: "" (empty)
- `TopLevelModuleName <name: string>` The name of the top level Verilog module \
  Use `FIRST_DEFINED` to auto-detect the top-level module name by using the first declared module.
  Use `LAST_DEFINED` to auto-detect the top-level module name by using the last declared module.
  Use an explicit name if you want to make sure that the correct module is created as circuit.
  - Default: "LAST_DEFINED"
- <span style="color: #0A5; font-weight: bold">(debug)</span> `VerilogExportPreprocessedFilename <file: file>` Exports the preprocessed Verilog content \
  If the filename is not empty a Verilog file will be created where specified.
  The content of the file contains the (macro) preprocessed Verilog source code.
  - Default: "" (empty)
- <span style="color: #0A5; font-weight: bold">(debug)</span> `VerilogExportProcessedFilename <file: file>` Exports the processed Verilog content \
  If the filename is not empty a Verilog file will be created where specified.
  The content of the file contains the parsed Verilog modules exported as Verilog source code.
  - Default: "" (empty)

[Previous Topic](3_BuildSetup.md) - [Index](../../README.md) - [Next Topic](5_DevelopingApplications.md)
