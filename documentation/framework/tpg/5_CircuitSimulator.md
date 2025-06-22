[Previous Topic](4_LogicGenerator.md) - [Index](../../../README.md)

# Circuit Simulator

The circuit simulator allows circuit simulation with and without fault model.
The fault model is specified to the simulation methods via a template parameter.
If no fault model is desired then `FaultFreeModel` can be used.
The circuit simulator provides multiple methods to simulate the circuit:

- SimulateNaive: Simulates every gate unconditionally
- SimulateEventDriven: Simulates only gates with defined inputs, assumes simulation result to be initialized with all DONT_CARE values
- SimulateEventDrivenIncremental: Simulates based on a previous fault-free result, assumes simulation result to be initialized with fault-free result

The simulation result stores the output value of each gate indexed by the gate's node id.
The index operator `[]` provides access to timeframes and simulation values per gate.

```cpp
MappedCircuit& circuit = ...
TestPattern& pattern = ...
Fault::SingleStuckAtFault fault = ...;

Simulation::SimulationResult result(pattern.GetNumberOfTimeframes(), circuit.NumberOfNodes());
Simulation::SimulationResult resultFaulty(pattern.GetNumberOfTimeframes(), circuit.NumberOfNodes());

Simulation::SimulateEventDriven<Fault::FaultFree>(circuit, pattern, { }, result);
resultFaulty.ReplaceWith(result);
Simulation::SimulateEventDrivenIncremental<Fault::SingleStuckAtFault>(circuit, pattern, fault, result, resultFaulty);

for (size_t timeframeId { 0u }; timeframeId < result.GetNumberOfTimeframes(); timeframeId++)
{
    auto& timeframe { result[timeframeId] };
    auto& timeframeFaulty { resultFaulty[timeframeId] };

    LOG(INFO) << "TF " << std::to_string(timeframeId)
        << " " << to_string(timeframe.GetValues())
        << " vs " << to_string(timeframeFaulty.GetValues());
}
```

[Previous Topic](4_LogicGenerator.md) - [Index](../../../README.md)
