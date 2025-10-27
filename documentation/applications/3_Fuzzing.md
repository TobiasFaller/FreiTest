[Previous Topic](2_Circuit.md) - [Index](../../README.md) - [Next](4_Scale4Edge.md)

# Fuzzing Applications

## Stimuli Simulation without Fault Model

- `FUZZING_STIMULI_SIMULATION`: Simulates the stimuli input on the circuit
- `FUZZING_STIMULI_SAT`: Simulates the stimuli input on the circuit by encoding it as SAT instance
- `FUZZING_STIMULI_BMC`: Simulates the stimuli input on the circuit by encoding it as BMC instance

Options:

- `Fuzzing/Stimuli <path: string>`: Path to the JSON stimuli input
  - Default: `UNINITIALIZED`

Below is an example for the JSON-based stimuli format:

```json
{
  "circuit": {
    "name": "benchmark",
    "flipflops": ["ff1", "ff2", "ff3", "m1/ff1", "m1/ff2", "m1/ff3"],
      "inputs": ["IN1", "IN2", "IN3", "IN4 [2]", "IN4 [3]"],
      "outputs": ["OUT1 [3]", "OUT1 [2]", "OUT2"],
      "clocks": ["CLOCK"]
  },
  "stimuli": {
      "flipflops": "110111",
      "inputs": [
          "01110",
          "11100",
          "01011",
          "01000",
          "00100"
      ]
  }
}
```

## Random Simulation with Fault Model

- `FUZZING_SIMULATION_STUCK_AT`: Simulates random inputs on the circuit with the stuck-at fault model and compares the different simulator implementations
- `FUZZING_SIMULATION_TRANSITION_FAULT`: Simulates random inputs on the circuit with the transition-delay fault model and compares the different simulator implementations
- `FUZZING_SIMULATION_CELL_AWARE`: Simulates random inputs on the circuit with the cell-aware fault model and compares the different simulator implementations

Options:

- `Fuzzing/Seed <value: uint>`: The seed for the random number generator
  - Default: 0
- `Fuzzing/Timeframes <value: uint>`: The length in clock cycles for each input pattern
  - Default: 100
- `Fuzzing/Iterations <value: uint>`: The number of input patterns to apply
  - Default: 5
- `Fuzzing/UdfmImportPath: <path: string>`: Path to the user-defined fault model for cell-aware testing

[Previous Topic](2_Circuit.md) - [Index](../../README.md) - [Next](4_Scale4Edge.md)
