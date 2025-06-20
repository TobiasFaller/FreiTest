[Previous Topic](2_VerilogInstantiator.md) - [Index](../../../README.md) -  [Next Topic](4_GateLibrary.md)

# Circuit Builder

The circuit builder is responsible for creating the final circuit representations.
It applies a topological sorting to the nodes, connects unconnected inputs to constant sources and builds compact data structures for efficient use in subsequent ATPG steps.
The final result of the circuit build process is a `CircuitEnvironment` which is returned by the `BuildCircuitEnvironment` method.
This method accepts its configuration via the `BuildConfiguration` structure.
See below for all configuration options.

The methods `AddUnmappedNode` and `AddMappedNode` create new nodes in the circuit.
Nodes can be deleted with the `RemoveUnmappedNode` and `RemoveMappedNode` methods.

When creating elements which are present in the mapped and unmapped circuit they are linked via the `LinkMappedToUnmappedNode` method.
The methods `LinkMappedToUnmappedInputPin` and `LinkMappedToUnmappedOutputPin` are used to store information about mapped pins in the circuit.

Below is an example on how to create a simple circuit which contains two inputs, one output and one two-input and gate.

```cpp
CircuitBuilder builder;

// ----------------------------------------------------------------------------
// Unmapped circuit
// ----------------------------------------------------------------------------

// Create the unmapped circuit nodes
auto uInputAId = builder.AddUnmappedNode("inputA", "input", 0, 1);
auto uInputBId = builder.AddUnmappedNode("inputB", "input", 0, 1);
auto uOutputId = builder.AddUnmappedNode("output", "output", 1, 0);
auto uAndGateId = builder.AddUnmappedNode("andGate", "and", 2, 1);

builder.AddUnmappedPrimaryInput(uInputAId);
builder.AddUnmappedPrimaryInput(uInputBId);
builder.AddUnmappedPrimaryOutput(uOutputId);

auto& uInputA = builder.GetUnmappedNode(uInputAId);
auto& uInputB = builder.GetUnmappedNode(uInputBId);
auto& uOutput = builder.GetUnmappedNode(uOutputId);
auto& uAndGateId = builder.GetUnmappedNode(uAndId);

// Name ports
uInputA.SetOutputPortName(0, "OUT");
uInputB.SetOutputPortName(0, "OUT");
uOutput.SetInputPortName(0, "IN");

uAndGate.SetInputPortName(0, "IN1");
uAndGate.SetInputPortName(1, "IN2");
uAndGate.SetOutputPortName(0, "OUT");

// Connect the unmapped circuit
uInputA.AddOutputNode(0, uAndGateId);
uAndGate.SetInputNode(0, uInputAId);

uInputB.AddOutputNode(0, uAndGateId);
uAndGate.SetInputNode(1, uInputBId);

uAndGateId.AddOutputNode(0, uOutputId);
uOutput.SetInputNode(0, uAndGateId);

// Name wires (connections)
uInputA.SetOutputWireName(0, "IN_A");
uInputB.SetOutputWireName(0, "IN_B");
uOutput.SetInputWireName(0, "OUTPUT");

uAndGate.SetInputWireName(0, "IN_A");
uAndGate.SetInputWireName(1, "IN_B");
uAndGate.SetOutputWireName(0, "OUTPUT");

// ----------------------------------------------------------------------------
// Mapped circuit
// ----------------------------------------------------------------------------

// Create the mapped circuit nodes
auto mInputAId = builder.AddMappedNode("inputA", CellCategory::MAIN_IN, CellType::P_IN, 0);
auto mInputBId = builder.AddMappedNode("inputB", CellCategory::MAIN_IN, CellType::P_IN, 0);
auto mOutputId = builder.AddMappedNode("output", CellCategory::MAIN_OUT, CellType::P_OUT, 1);
auto mAndGateId = builder.AddMappedNode("andGate", CellCategory::MAIN_AND, CellType::AND2, 2);

builder.AddMappedPrimaryInput(uInputAId);
builder.AddMappedPrimaryInput(uInputBId);
builder.AddMappedPrimaryOutput(uOutputId);

auto& mInputA = builder.GetUnmappedNode(mInputAId);
auto& mInputB = builder.GetUnmappedNode(mInputBId);
auto& mOutput = builder.GetUnmappedNode(mOutputId);
auto& mAndGateId = builder.GetUnmappedNode(mAndId);

// Name ports
mInputA.SetOutputPortName("OUT");
mInputB.SetOutputPortName("OUT");
mOutput.SetInputPortName(0, "IN");

mAndGate.SetInputPortName(0, "IN1");
mAndGate.SetInputPortName(1, "IN2");
mAndGate.SetOutputPortName("OUT");

// Connect the unmapped circuit
mInputA.AddSuccessorNode(mAndGateId);
mAndGate.SetInputNode(0, mInputAId);

mInputB.AddSuccessorNode(mAndGateId);
mAndGate.SetInputNode(1, mInputBId);

mAndGateId.AddSuccessorNode(mOutputId);
mOutput.SetInputNode(0, mAndGateId);

// Name wires (connections)
mInputA.SetOutputWireName("IN_A");
mInputB.SetOutputWireName("IN_B");
mOutput.SetInputWireName(0, "OUTPUT");

mAndGate.SetInputWireName(0, "IN_A");
mAndGate.SetInputWireName(1, "IN_B");
mAndGate.SetOutputWireName("OUTPUT");

// ----------------------------------------------------------------------------
// Link the mapped and unmapped circuit
// ----------------------------------------------------------------------------

// Link nodes
builder.LinkMappedToUnmappedNode(mInputAId, uInputAId);
builder.LinkMappedToUnmappedNode(mInputBId, uInputBId);
builder.LinkMappedToUnmappedNode(mOutputId, uOutputId);
builder.LinkMappedToUnmappedNode(mAndGate, uAndGate);

// Link pins
builder.LinkMappedToUnmappedOutputPin(mInputAId, 0, uInputAId, 0);
builder.LinkMappedToUnmappedOutputPin(mInputBId, 0, uInputBId, 0);
builder.LinkMappedToUnmappedInputPin(mOutputId, 0, uOutputId, 0);

builder.LinkMappedToUnmappedInputPin(mAndGate, 0, mAndGate, 0);
builder.LinkMappedToUnmappedOutputPin(mAndGate, 0, mAndGate, 0);
builder.LinkMappedToUnmappedOutputPin(mAndGate, 1, mAndGate, 1);

// ----------------------------------------------------------------------------
// Build the circuit
// ----------------------------------------------------------------------------

BuildConfiguration config;
config.mappedDisallowCycles = true;
...

auto circuit = builder.BuildCircuitEnvironment(config);
```

Below is the full interface of the CircuitBuilder class.

```cpp
// See Circuit/CircuitBuilder.h
class CircuitBuilder {

    // ----------------------------------------------
    // UnmappedNode specific interface
    // ----------------------------------------------

	UnmappedNodeId AddUnmappedNode(std::string name, std::string type, size_t inputs, size_t outputs);
	void RemoveUnmappedNode(UnmappedNodeId unmappedNode);
	UnmappedNode& GetUnmappedNode(UnmappedNodeId mappedNode);
	bool IsUnmappedNodeDeleted(UnmappedNodeId unmappedNode) const;

	void AddUnmappedPrimaryInput(UnmappedNodeId input);
	void AddUnmappedPrimaryOutput(UnmappedNodeId output);

    // ----------------------------------------------
    // MappedNode specific interface
    // ----------------------------------------------

	MappedNodeId AddMappedNode(std::string name, cell_type_id cellType, size_t inputs);
	void RemoveMappedNode(MappedNodeId mappedNode);
	MappedNode& GetMappedNode(MappedNodeId mappedNode);
	bool IsMappedNodeDeleted(MappedNodeId mappedNode) const;

	void AddMappedPrimaryInput(MappedNodeId input);
	void AddMappedPrimaryOutput(MappedNodeId output);

	void AddSecondaryOutput(MappedNodeId output);
	void AddSecondaryInput(MappedNodeId input);
	void LinkSecondaryPorts(MappedNodeId input, MappedNodeId output);

    // ----------------------------------------------
    // MappedNode to UnmappedNode interface
    // ----------------------------------------------

	void LinkMappedToUnmappedNode(MappedNodeId mappedNode, UnmappedNodeId unmappedNode);
	void LinkMappedToUnmappedInputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId mappedPin, PinId unmappedPin);
	void LinkMappedToUnmappedOutputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId unmappedPin);

    // ----------------------------------------------
    // Circuit building interface
    // ----------------------------------------------

	std::unique_ptr<CircuitEnvironment> BuildCircuitEnvironment(const BuildConfiguration& configuration);

};

// Used for unconnected ports
constexpr UnmappedNodeId UNCONNECTED_UNMAPPED_PORT;

struct UnmappedNode
{
	std::string name;
	std::string type;
	std::vector<UnmappedNodeId> inputs;
	std::vector<std::vector<UnmappedNodeId>> outputs;

	std::vector<std::string> inputPortNames;
	std::vector<std::string> outputPortNames;
	std::vector<std::string> inputWireNames;
	std::vector<std::string> outputWireNames;

	void SetInputNode(size_t input, UnmappedNodeId node);
	bool HasInputNode(size_t input) const;

	void AddOutputNode(size_t output, UnmappedNodeId successor);
	bool RemoveOutputNode(size_t output, UnmappedNodeId successor);
	bool HasOutputNode(size_t output, UnmappedNodeId successor) const;

	void SetInputPortName(size_t input, std::string name);
	void SetOutputPortName(size_t output, std::string name);
	void SetInputWireName(size_t input, std::string name);
	void SetOutputWireName(size_t output, std::string name);
};

// Used for unconnected ports
constexpr MappedNodeId UNCONNECTED_MAPPED_PORT;

struct MappedNode
{
	std::string name;
	cell_type_id cellType;
	std::vector<MappedNodeId> inputs;
	std::vector<MappedNodeId> successors;

	std::vector<std::string> inputPortNames;
	std::string outputPortName;
	std::vector<std::string> inputWireNames;
	std::string outputWireName;

	void SetInputNode(size_t input, MappedNodeId node);
	bool HasInputNode(size_t input) const;

	void AddSuccessorNode(MappedNodeId successor);
	bool RemoveSuccessorNode(MappedNodeId successor);
	bool HasSuccessorNode(MappedNodeId successor) const;

	void SetInputPortName(size_t input, std::string name);
	void SetOutputPortName(std::string name);
	void SetInputWireName(size_t input, std::string name);
	void SetOutputWireName(std::string name);
};
```

## Configuration options

Below is a list of currently implemented configuration options.

```cpp
// See Circuit/CircuitBuilder.h
struct BuildConfiguration
{
	// ------------------------------------------------------------------------
	// Unmapped Circuit
	// ------------------------------------------------------------------------

	/**
	 * @brief CircuitBuilder: Throws an error if a circuit cycle was found in the unmapped circuit.
	 *
	 * Default: false, ignore cycles
	 */
	bool unmappedDisallowCycles;

	/**
	 * @brief CircuitBuilder: Removes unmapped nodes which are not connected to an input and output port.
	 *
	 * Default: true, remove unconnected gates
	 */
	bool unmappedRemoveUnconnectedGates;

	/**
	 * @brief CircuitBuilder: Throws an error if a gate has an unconnected input in the unmapped circuit.
	 *
	 * Priority: first unmappedConnectUnconnectedInputsToConstant, then unmappedDisallowUnconnectedInputs
	 * Default: false, ignore unconnected inputs
	 */
	bool unmappedDisallowUnconnectedInputs;

	/**
	 * @brief CircuitBuilder: Connects a constant gate to all unconnected input pins in the unmapped circuit.
	 *
	 * Priority: first unmappedConnectUnconnectedInputsToConstant, then unmappedDisallowUnconnectedInputs
	 * Default: false, leave unconnected inputs as nullptr
	 */
	bool unmappedConnectUnconnectedInputsToConstant;

	/**
	 * @brief CircuitBuilder: The logic value to connect to all unconnected input pins in the unmapped circuit.
	 *
	 * Default: Logic::LOGIC_DONT_CARE, use "X" as constant
	 */
	Logic unmappedConnectUnconnectedInputsToConstantValue;

	// ------------------------------------------------------------------------
	// Mapped Circuit
	// ------------------------------------------------------------------------

	/**
	 * @brief CircuitBuilder: Throws an error if a combinatorial cycle was found in the mapped circuit.
	 */
	bool mappedDisallowCycles;

	/**
	 * @brief CircuitBuilder: Removes mapped nodes which are not connected to an input and output port.
	 */
	bool mappedRemoveUnconnectedGates;

	/**
	 * @brief CircuitBuilder: Removes combinatorial cycles during mapped circuit creation by breaking them
	 * and using Logic::LOGIC_DONT_CARE as the source signal for the unconnected input.
	 */
	bool mappedBreakCombinatorialCycles;

	/**
	 * @brief CircuitBuilder: Throws an error if a gate has an unconnected input in the mapped circuit.
	 *
	 * Priority: first mappedConnectUnconnectedInputsToConstant, then mappedDisallowUnconnectedInputs
	 * Default: false, ignore unconnected inputs
	 */
	bool mappedDisallowUnconnectedInputs;

	/**
	 * @brief CircuitBuilder: Connects a constant gate to all unconnected input pins in the mapped circuit.
	 *
	 * Priority: first mappedConnectUnconnectedInputsToConstant, then mappedDisallowUnconnectedInputs
	 * Default: false, leave unconnected inputs as nullptr
	 */
	bool mappedConnectUnconnectedInputsToConstant;

	/**
	 * @brief CircuitBuilder: The logic value to connect to all unconnected input pins in the mapped circuit.
	 *
	 * Default: Logic::LOGIC_DONT_CARE, use "X" as constant
	 */
	Logic mappedConnectUnconnectedInputsToConstantValue;

};
```

[Previous Topic](2_VerilogInstantiator.md) - [Index](../../../README.md) -  [Next Topic](4_GateLibrary.md)
