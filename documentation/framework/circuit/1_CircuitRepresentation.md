[Index](../../../README.md) -  [Next Topic](2_VerilogInstantiator.md)

# Circuit Representation

The circuit is stored in a class called `CircuitEnvironment` that encapsulates the three representations `MappedCircuit`, `UnmappedCircuit` and `CircuitMeataData`.

```text
+-------------------------------------------------------+
| Circuit Environment                                   |
| +----------------+ +------------------+ +-----------+ |
| | Mapped Circuit | | Unmapped Circuit | | Meta Data | |
| +----------------+ +------------------+ +-----------+ |
+-------------------------------------------------------+
```

The three representations are providing different abstraction levels:

- `CircuitEnvironment`: \
  Contains all three representations of the circuit and allows mapping gates and gate pins from the Mapped/Unmapped Circuit to each other.
- `CircuitMetadata`: \
  Contains all information about source modules, ports, wires and names and is directly derived from the input sources (Verilog).
  If you need the relation from an element in the original sources to the MappedCircuit or UnmappedCircuit, then use this data structure.
- `UnmappedCircuit`: \
  Contains a flat graph of the gates as they have been extracted from the source files.
  Inputs and outputs of the top-level circuit are represented by input and output gates.
  This representation contains all the gates without transformations and is not directly used in Automatic Test Pattern Generation (ATPG).
  If you need to traverse the flat circuit graph with the original (multi-output) gates then use this representation.
- `MappedCircuit`: \
  Contains a transformed version of the circuit where sequential elements have been split up into secondary ports (pairs of secondary inputs and secondary outputs).
  It is used for Automatic Test Pattern Generation (ATPG) and traversed for encoding of the Conjunctive Normal Form (CNF) via an implicit Tseitin transformation.
  The nodes are represented as graph where each node's output points to a list of following gates.
  Each gate only has a single output but possibly multiple inputs.

## CircuitEnvironment

The circuit environment holds all the circuit graph data and accompanying metadata.
It allows relating nodes and pins from the mapped and unmapped circuit representations.

```cpp
// See Circuit/CircuitEnvironment.hpp
class CircuitEnvironment {

	std::string GetName(void) const;

	const UnmappedCircuit& GetUnmappedCircuit(void) const;
	const MappedCircuit& GetMappedCircuit(void) const;
	const CircuitMetaData& GetMetaData(void) const;

	bool HasUnmappedNodeId(size_t mappedNodeId) const;
	bool HasMappedNodeIds(size_t unmappedNodeId) const;
	size_t GetUnmappedNodeId(size_t mappedNodeId) const;
	std::vector<size_t> GetMappedNodeIds(size_t unmappedNodeId) const;

	bool HasUnmappedPinId(size_t mappedPinId) const;
	bool HasMappedPinIds(size_t unmappedPinId) const;
	size_t GetUnmappedPinId(size_t mappedPinId) const;
	std::vector<size_t> GetMappedPinIds(size_t unmappedPinId) const;

};
```

## CircuitMetadata

The circuit metadata is used to get information about the original circuit representation, for example finding module boundaries, ports and wires.
Modules, cells and gates are represented by groups and can be identified via the source information in the respective group.
The circuit metadata contains a single root group representing the whole circuit that contains sub-groups representing modules, cells and gates.

The HasX and GetX methods use full hierarchical names for finding elements.
The circuit metadata allows to quickly iterate over all elements with certain prefixes using the GetXWithPrefix methods.

```cpp
auto cell { metadata->GetGroup("mycircuit/inst_adder/half_adder1") };
auto port { metadata->GetPort("mycircuit/inst_adder/half_adder1/A") };
auto gate { metadata->GetGroup("mycircuit/inst_adder/half_adder1/xor1") };
auto allCellsAndGates { metadata->GetGroupsWithPrefix("mycircuit/inst_adder/") };
```

```cpp
// See Circuit/CircuitMetaData.hpp
class CircuitMetaData {

	const GroupMetaData* GetRoot(void) const;
	const GroupMetaData* GetGroup(const MappedNode* node) const;
	const GroupMetaData* GetGroup(const UnmappedNode* node) const;
	const std::vector<const GroupMetaData*>& GetGroups(void) const;
	const std::vector<const ConnectionMetaData*>& GetConnections(void) const;
	const std::vector<const WireMetaData*>& GetWires(void) const;
	const std::vector<const PortMetaData*>& GetPorts(void) const;

	bool HasGroup(const std::string& name) const;
	const GroupMetaData* GetGroup(const std::string& name) const;
	std::vector<const GroupMetaData*> GetGroupsWithPrefix(const std::string& prefix) const;

	bool HasConnection(const std::string& name) const;
	const ConnectionMetaData* GetConnection(const std::string& name) const;
	std::vector<const ConnectionMetaData*> GetConnectionsWithPrefix(const std::string& prefix) const;

	bool HasWire(const std::string& name) const;
	const WireMetaData* GetWire(const std::string& name) const;
	std::vector<const WireMetaData*> GetWiresWithPrefix(const std::string& prefix) const;

	bool HasPort(const std::string& name) const;
	const PortMetaData* GetPort(const std::string& name) const;
	std::vector<const PortMetaData*> GetPortsWithPrefix(const std::string& prefix) const;

	// Finds a wire or port name for the mapped / unmapped node and port
	std::string GetFriendlyName(const Circuit::MappedCircuit::NodeAndPort& nodeAndPort) const;
	std::string GetFriendlyName(const Circuit::UnmappedCircuit::NodeAndPort& nodeAndPort) const;

};

class GroupMetaData {

	const std::string& GetName(void) const;
	std::string GetHierarchyName(void) const;
	const SourceInformation& GetSourceInfo(void) const;
	const GroupMetaData* GetParent(void) const;
	const std::vector<const GroupMetaData*>& GetSubGroups(void) const;

	const std::vector<PortMetaData>& GetPorts(void) const;
	bool HasPort(const std::string& name) const;
	const PortMetaData* GetPort(const std::string& name) const;
	const PortMetaData* GetPort(const Circuit::Port& port) const;
	std::vector<const PortMetaData*> GetPortsWithPrefix(const std::string& prefix) const;
	std::tuple<const PortMetaData*, size_t> GetPortWithConnection(size_t connectionId) const;

	const std::vector<WireMetaData>& GetWires(void) const;
	bool HasWire(const std::string& name) const;
	const WireMetaData* GetWire(const std::string& name) const;
	std::vector<const WireMetaData*> GetWiresWithPrefix(const std::string& prefix) const;
	std::tuple<const WireMetaData*, size_t> GetWireWithConnection(size_t connectionId) const;

	const std::vector<const MappedNode*>& GetMappedNodes(void) const;
	const std::vector<const UnmappedNode*>& GetUnmappedNodes(void) const;

	// Walks the parent chain to find a group with the propertie's name
	// and then evaluates the filter function on the property,
	// and returns the group if the function returns true.
	template<typename Property, typename Function>
	const GroupMetaData* FindParentWithProperty(std::string name, Function func) const;

	// Walks the children to find a group with the propertie's name
	// and then evaluates the filter function on the property,
	// and returns the group if the function returns true.
	template<typename Property, typename Function>
	const GroupMetaData* FindSubGroupWithProperty(std::string name, Function func) const;

	// Walks first the parents and then the children to find a group with the propertie's name
	// and then evaluates the filter function on the property,
	// and returns the group if the function returns true.
	template<typename Property, typename Function>
	const GroupMetaData* FindGroupWithProperty(std::string name, Function func) const;

};

struct SourceInformation {

	template<typename T>
	void AddProperty(std::string name, T value);

	template<typename T>
	bool HasProperty(std::string name) const;

	template<typename T>
	std::optional<T> GetProperty(std::string name) const;

};

class ConnectionMetaData {

    // Each connection has a unique ID, making it identifiable on other gate's ports
	size_t GetConnectionId(void) const;

	const std::vector<MappedCircuit::NodeAndPort>& GetMappedSources(void) const;
	const std::vector<MappedCircuit::NodeAndPort>& GetMappedSinks(void) const;
	const std::vector<UnmappedCircuit::NodeAndPort>& GetUnmappedSources(void) const;
	const std::vector<UnmappedCircuit::NodeAndPort>& GetUnmappedSinks(void) const;

	void AddMappedSource(MappedCircuit::NodeAndPort node);
	void AddMappedSink(MappedCircuit::NodeAndPort node);
	void AddUnmappedSource(UnmappedCircuit::NodeAndPort node);
	void AddUnmappedSink(UnmappedCircuit::NodeAndPort node);

};

class SizeMetaData {

	size_t GetTop(void) const;
	size_t GetBottom(void) const;

    // Returns an iterable (for-loop) list of indices of the port / wire.
    // Usage:
    //
    //     for (auto index : port.GetSize().GetIndicesBottomToTop()) { ... }
    //     for (auto index : wire.GetSize().GetIndicesTopToBottom()) { ... }
    //     for (auto index : port.GetSize().GetIndicesMinToMax()) { ... }
    //     for (auto index : wire.GetSize().GetIndicesMaxToMin()) { ... }
    //
	std::vector<size_t> GetIndicesBottomToTop(void) const;
	std::vector<size_t> GetIndicesTopToBottom(void) const;
	std::vector<size_t> GetIndicesMinToMax(void) const;
	std::vector<size_t> GetIndicesMaxToMin(void) const;

};

class WireMetaData {

	const std::string& GetName(void) const;
	const SizeMetaData& GetSize(void) const;
	std::string GetHierarchyName(void) const;
	const SourceInformation& GetSourceInfo(void) const;
	const std::vector<const ConnectionMetaData*>& GetConnections(void) const;
	const ConnectionMetaData* GetConnectionForIndex(size_t index) const;
	const GroupMetaData* GetGroup(void) const;

};

class PortMetaData {

	const std::string& GetName(void) const;
	const SizeMetaData& GetSize(void) const;
	std::string GetHierarchyName(void) const;
	const SourceInformation& GetSourceInfo(void) const;
	const std::vector<const ConnectionMetaData*>& GetConnections(void) const;
	const ConnectionMetaData* GetConnectionForIndex(size_t index) const;
	const GroupMetaData* GetGroup(void) const;

};
```

### SourceInformation

Below is a list of metadata contained in the source information.
The properties can checked via `sourceInfo.GetProperty<TYPE>("NAME")`.

Ports:

- `port-name`: string
- `port-type`: string (`input` / `output` / `inout`)
- `port-size`: size_t
- `port-top`: size_t
- `port-bottom`: size_t
- `port-absolute-index`: size_t
- `port-relative-index`: size_t

Wires:

- `wire-name`: string
- `wire-size`: size_t
- `wire-top`: size_t
- `wire-bottom`: size_t
- `wire-absolute-index`: size_t
- `wire-relative-index`: size_t

Modules (All):

- `module-name`: string
- `module-type`: string
- `module-is-cell`: bool
- `module-is-primitive`: bool
- `module-is-virtual`: bool
- Verilog modules (`module NAME(...); ... endmodule`)
	- `module-name`: string
	- `module-type`: string
	- `module-category`: string (`generic`)
- Cells (modules between `'celldefine ... 'endcelldefine`)
	- `cell-name`: string
	- `cell-type`: string
	- `cell-category`: string (`generic`)
- Virtual elements (Verilog constants, Verilog assignments, primary inputs and outputs)
	- `virtual-name`: string
	- `virtual-type`: string (`input`, `output`, `inout` / `assignment` / `0`, `1`, `X`, `U`)
	- `virtual-category`: string (`port` / `assignment` / `constant`)
- Primitives (primitive gates in cells `and gate1(X1, X2, Y);`, assignment buffers, input / output ports)
	- `primitive-name`: string
	- `primitive-type`: string (`...` / `input`, `output`, `inout` / `buf` / `tie0`, `tie1`, `tieX`, `tieU`)
	- `primitive-category`: string (`gate` / `port` / `assignment` / `constant`)

---

Assume the following hierarchy:

```text
+-----------------------+
| mycircuit             |
| +-------------------+ |
| | inst_adder        | |
| |    +---------+    | |
| |    | ha1     |    | |
| |  A | +-----+ |    | |
| |  --+-+     | | S  | |
| |    | | XOR +-+--  | |
| |  --+-+     | |    | |
| |  B | +-----+ |    | |
| |    +---------+    | |
| +-------------------+ |
+-----------------------+
```

Here are some basic properties that show the usage of the source information:

```cpp
auto cellInfo { metadata->GetGroup("mycircuit/inst_adder/half_adder1").GetSourceInfo() };
assert(cellInfo.template GetProperty<bool>("module-is-cell").value_or(false));
assert(cellInfo.template GetProperty<bool>("cell-type").value_or("") == "FA");

auto gateInfo { metadata->GetGroup("mycircuit/inst_adder/half_adder1/xor1").GetSourceInfo() };
assert(portInfo.template GetProperty<bool>("module-is-primitive").value_or(false));
assert(portInfo.template GetProperty<std::string>("primitive-type").value_or("") == "xor");
assert(portInfo.template GetProperty<std::string>("primitive-category").value_or("") == "gate");

auto portInfo { metadata->GetPort("mycircuit/inst_adder/half_adder1/A").GetSourceInfo() };
assert(portInfo.template GetProperty<bool>("port-size").value_or(0) == 1);
assert(portInfo.template GetProperty<std::string>("port-type").value_or("") == "input");
assert(portInfo.template GetProperty<bool>("port-top").value_or(0) == 0);
assert(portInfo.template GetProperty<bool>("port-bottom").value_or(0) == 0);
```

## UnmappedCircuit

The unmapped circuit is a flat graph with nodes (gates) and edges (connections).
Each gate has a unique ID (unmapped node ID) and has multiple input and output pins.
Input pins are represented as single reference to the source gate, while output pins are represented as list of successor gates.
Pins have a unique ID (unmapped pin ID) which can be used to identify connections between gate's pins.

```cpp
// See Circuit/UnmappedCircuit.hpp
class UnmappedCircuit {

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

    // Returns an iterable (for-loop) list of nodes in the circuit.
    // Each element in this list is a "const UnmappedNode*".
    // Usage:
    //
    //     for (auto node : circuit.GetNodes()) { ... }
    //     for (auto node : circuit.GetPrimaryInputs()) { ... }
    //     for (auto node : circuit.GetPrimaryOutputs()) { ... }
    //
	UnmappedNodeList GetNodes(void) const;
	UnmappedNodeList GetPrimaryInputs(void) const;
	UnmappedNodeList GetPrimaryOutputs(void) const;

    // Returns an iterable (for-loop) list of nodes and index in the circuit.
    // Each element in this enumeration is a "const UnmappedNode*".
    // Usage:
    //
    //     for (auto [nodeId, node] : circuit.EnumerateNodes()) { ... }
    //     for (auto [inputId, node] : circuit.EnumeratePrimaryInputs()) { ... }
    //     for (auto [outputId, node] : circuit.EnumeratePrimaryOutputs()) { ... }
    //
	UnmappedNodeEnumeration EnumerateNodes(void) const;
	UnmappedNodeEnumeration EnumeratePrimaryInputs(void) const;
	UnmappedNodeEnumeration EnumeratePrimaryOutputs(void) const;

    // Returns a single node or primary port (a node too).
	const UnmappedNode* GetNode(size_t nodeId) const;
	const UnmappedNode* GetPrimaryInput(size_t inputNr) const;
	const UnmappedNode* GetPrimaryOutput(size_t outputNr) const;

	bool IsPrimaryInput(size_t nodeId) const;
	bool IsPrimaryOutput(size_t nodeId) const;

	size_t GetPrimaryInputNumber(const UnmappedNode* primaryInput) const;
	size_t GetSecondaryInputNumber(const UnmappedNode* secondaryInput) const;

};

// See Circuit/UnmappedNode.hpp
class UnmappedNode {

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	size_t GetNodeId(void) const;
	const std::string& GetName(void) const;
	const std::string& GetType(void) const;

    // ----------------------------------------------
    // Connection specific interface
    // ----------------------------------------------

    // Returns the input / output node-list connected to this node.
    // Usage:
    //
    //     for (auto node : node.GetInputs()) { ... }
    //     for (auto output : node.GetOutputs()) {
    //         for (auto successor : output) { ... }
    //     }
    //     for (auto successor : circuit.GetSuccessors(0u)) { ... }
    //
	const UnmappedInputList GetInputs(void) const;
	const UnmappedOutputList GetOutputs(void) const;
	const UnmappedSuccessorList GetSuccessors(size_t output) const;

    // Returns the input / output node-list with indices connected to this node.
    // Usage:
    //
    //     for (auto [inputId, node] : node.GetInputs()) { ... }
    //     for (auto [outputId, nodes] : node.GetOutputs()) {
    //         for (auto successor : nodes) { ... }
    //     }
    //     for (auto [successorId, node] : circuit.GetSuccessors(0u)) { ... }
    //
	const UnmappedInputEnumeration EnumerateInputs(void) const;
	const UnmappedOutputEnumeration EnumerateOutputs(void) const;
	const UnmappedSuccessorEnumeration EnumerateSuccessors(size_t output) const;
};
```

## MappedCircuit

The mapped circuit is a flat graph with nodes (gates) and edges (connections).
Each gate has a unique ID (mapped node ID) and has multiple input and a single output pin.
Input pins are represented as single reference to the source gate, while output pins are represented as list of successor gates.
Pins have a unique ID (mapped pin ID) which can be used to identify connections between gates.

```cpp
// See Circuit/MappedCircuit.hpp
class MappedCircuit {

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

    // Returns an iterable (for-loop) list of nodes in the circuit.
    // Each element in this list is a "const MappedNode*".
    // Usage:
    //
    //     for (auto node : circuit.GetNodes()) { ... }
    //     for (auto node : circuit.GetPrimaryInputs()) { ... }
    //     for (auto node : circuit.GetSecondaryOutputs()) { ... }
    //
	MappedNodeList GetNodes(void) const;
	MappedNodeList GetInputs(void) const;
	MappedNodeList GetOutputs(void) const;
	MappedNodeList GetPrimaryInputs(void) const;
	MappedNodeList GetPrimaryOutputs(void) const;
	MappedNodeList GetSecondaryInputs(void) const;
	MappedNodeList GetSecondaryOutputs(void) const;

    // Returns an iterable (for-loop) list of nodes and index in the circuit.
    // Each element in this enumeration is a "const MappedNode*".
    // Usage:
    //
    //     for (auto [nodeId, node] : circuit.EnumerateNodes()) { ... }
    //     for (auto [inputId, node] : circuit.EnumeratePrimaryInputs()) { ... }
    //     for (auto [outputId, node] : circuit.EnumerateSecondaryOutputs()) { ... }
    //
	MappedNodeEnumeration EnumerateNodes(void) const;
	MappedNodeEnumeration EnumerateInputs(void) const;
	MappedNodeEnumeration EnumerateOutputs(void) const;
	MappedNodeEnumeration EnumeratePrimaryInputs(void) const;
	MappedNodeEnumeration EnumeratePrimaryOutputs(void) const;
	MappedNodeEnumeration EnumerateSecondaryInputs(void) const;
	MappedNodeEnumeration EnumerateSecondaryOutputs(void) const;

	const MappedNode* GetNode(size_t nodeId) const;
	const MappedNode* GetInput(size_t inputNr) const;
	const MappedNode* GetPrimaryInput(size_t inputNr) const;
	const MappedNode* GetSecondaryInput(size_t inputNr) const;
	const MappedNode* GetOutput(size_t outputNr) const;
	const MappedNode* GetPrimaryOutput(size_t outputNr) const;
	const MappedNode* GetSecondaryOutput(size_t outputNr) const;

	bool IsInput(size_t nodeId) const;
	bool IsOutput(size_t nodeId) const;
	bool IsPrimaryInput(size_t nodeId) const;
	bool IsPrimaryOutput(size_t nodeId) const;
	bool IsSecondaryInput(size_t nodeId) const;
	bool IsSecondaryOutput(size_t nodeId) const;

	size_t GetInputNumber(const MappedNode* input) const;
	size_t GetPrimaryInputNumber(const MappedNode* primaryInput) const;
	size_t GetSecondaryInputNumber(const MappedNode* secondaryInput) const;
	size_t GetOutputNumber(const MappedNode* output) const;
	size_t GetPrimaryOutputNumber(const MappedNode* primaryOutput) const;
	size_t GetSecondaryOutputNumber(const MappedNode* secondaryOutput) const;

	const Node* GetSecondaryInputForSecondaryOutput(const Node* secondaryInput) const;
	const Node* GetSecondaryOutputForSecondaryInput(const Node* secondaryOutput) const;

};

// See Circuit/MappedNode.hpp
class MappedNode {
    
    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	size_t GetNodeId(void) const;
	const std::string& GetName(void) const;
	CellType GetCellType(void) const;
	CellCategory GetCellCategory(void) const;

    // ----------------------------------------------
    // Connection specific interface
    // ----------------------------------------------

    // Returns the input / output node-list connected to this node.
    // Usage:
    //
    //     for (auto node : node.GetInputs()) { ... }
    //     for (auto successor : circuit.GetSuccessors()) { ... }
    //
	const MappedInputList GetInputs(void) const;
	const MappedSuccessorList GetSuccessors(void) const;

    // Returns the input / output node-list with indices connected to this node.
    // Usage:
    //
    //     for (auto [inputId, node] : node.GetInputs()) { ... }
    //     for (auto [successorId, node] : circuit.GetSuccessors()) { ... }
    //
	const MappedInputEnumeration EnumerateInputs(void) const;
	const MappedSuccessorEnumeration EnumerateSuccessors(void) const;

};
```

[Index](../../../README.md) -  [Next Topic](2_VerilogInstantiator.md)
