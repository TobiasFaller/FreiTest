#pragma once

#include <limits>
#include <vector>
#include <map>

#include "Basic/Logic.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/SourceInformation.hpp"

namespace FreiTest
{
namespace Circuit
{
namespace Builder
{

using UnmappedNodeId = size_t;
using MappedNodeId = size_t;
using ConnectionId = size_t;
using SourceInfoId = size_t;
using GroupId = size_t;
using WireId = size_t;
using PortId = size_t;

using PinId = size_t;
using UnmappedPinId = int64_t;
using MappedPinId = int64_t;

constexpr UnmappedNodeId UNCONNECTED_UNMAPPED_PORT = std::numeric_limits<UnmappedNodeId>::max();
constexpr MappedNodeId UNCONNECTED_MAPPED_PORT = std::numeric_limits<MappedNodeId>::max();
constexpr ConnectionId UNCONNECTED_CONNECTION = std::numeric_limits<ConnectionId>::max();
constexpr SourceInfoId INVALID_SOURCE_INFO = std::numeric_limits<SourceInfoId>::max();
constexpr GroupId INVALID_GROUP = std::numeric_limits<GroupId>::max();

enum class PortType
{
	Input, Output, Bidirectional
};

struct UnmappedNode
{
	std::string name;
	std::string type;
	std::vector<UnmappedNodeId> inputs;
	std::vector<std::vector<UnmappedNodeId>> outputs;
	GroupId group;

	std::vector<std::string> inputPortNames;
	std::vector<std::string> outputPortNames;
	std::vector<ConnectionId> inputConnectionIds;
	std::vector<ConnectionId> outputConnectionIds;
	std::vector<std::string> inputConnectionNames;
	std::vector<std::string> outputConnectionNames;

	void SetInputNode(size_t input, UnmappedNodeId node);
	bool HasInputNode(size_t input) const;
	UnmappedNodeId& GetInputNode(size_t input);
	UnmappedNodeId const& GetInputNode(size_t input) const;

	void AddOutputNode(size_t output, UnmappedNodeId successor);
	bool RemoveOutputNode(size_t output, UnmappedNodeId successor);
	bool HasOutputNode(size_t output, UnmappedNodeId successor) const;
	std::vector<UnmappedNodeId>& GetOutputNodes(size_t output);
	std::vector<UnmappedNodeId> const& GetOutputNodes(size_t output) const;

	void SetGroup(GroupId group);
	GroupId& GetGroup(void);
	GroupId const& GetGroup(void) const;

	void SetInputPortName(size_t input, std::string name);
	void SetOutputPortName(size_t output, std::string name);
	void SetInputConnectionName(size_t input, std::string name);
	void SetOutputConnectionName(size_t output, std::string name);
	void SetInputConnectionId(size_t input, ConnectionId connection);
	void SetOutputConnectionId(size_t output, ConnectionId connection);
	std::string& GetInputPortName(size_t input);
	std::string& GetOutputPortName(size_t output);
	std::string& GetInputConnectionName(size_t input);
	std::string& GetOutputConnectionName(size_t output);
	ConnectionId& GetInputConnectionId(size_t input);
	ConnectionId& GetOutputConnectionId(size_t output);
	std::string const& GetInputPortName(size_t input) const;
	std::string const& GetOutputPortName(size_t output) const;
	std::string const& GetInputConnectionName(size_t input) const;
	std::string const& GetOutputConnectionName(size_t output) const;
	ConnectionId const& GetInputConnectionId(size_t input) const;
	ConnectionId const& GetOutputConnectionId(size_t output) const;

};

struct MappedNode
{
	std::string name;
	CellCategory cellCategory;
	CellType cellType;
	std::vector<MappedNodeId> inputs;
	std::vector<MappedNodeId> successors;
	GroupId group;

	std::vector<std::string> inputPortNames;
	std::string outputPortName;
	std::vector<size_t> inputConnectionIds;
	std::vector<std::string> inputConnectionNames;
	size_t outputConnectionId;
	std::string outputConnectionName;

	void SetInputNode(size_t input, MappedNodeId node);
	bool HasInputNode(size_t input) const;
	MappedNodeId& GetInputNode(size_t input);
	MappedNodeId const& GetInputNode(size_t input) const;

	void AddSuccessorNode(MappedNodeId successor);
	bool RemoveSuccessorNode(MappedNodeId successor);
	bool HasSuccessorNode(MappedNodeId successor) const;
	std::vector<MappedNodeId>& GetSuccessorNodes(void);
	std::vector<MappedNodeId> const& GetSuccessorNodes(void) const;

	void SetGroup(GroupId group);
	GroupId& GetGroup(void);
	GroupId const& GetGroup(void) const;

	void SetInputPortName(size_t input, std::string name);
	void SetOutputPortName(std::string name);
	void SetInputConnectionName(size_t input, std::string name);
	void SetOutputConnectionName(std::string name);
	void SetInputConnectionId(size_t input, ConnectionId connection);
	void SetOutputConnectionId(ConnectionId connection);
	std::string& GetInputPortName(size_t input);
	std::string& GetOutputPortName(void);
	std::string& GetInputConnectionName(size_t input);
	std::string& GetOutputConnectionName(void);
	ConnectionId& GetInputConnectionId(size_t input);
	ConnectionId& GetOutputConnectionId(void);
	std::string const& GetInputPortName(size_t input) const;
	std::string const& GetOutputPortName(void) const;
	std::string const& GetInputConnectionName(size_t input) const;
	std::string const& GetOutputConnectionName(void) const;
	ConnectionId const& GetInputConnectionId(size_t input) const;
	ConnectionId const& GetOutputConnectionId(void) const;
};

struct Wire
{
	std::string name;
	std::pair<size_t, size_t> size;
	SourceInfoId sourceInfo;
	std::vector<ConnectionId> connections;

	void SetName(std::string name);
	void SetSize(std::pair<size_t, size_t> size);
	void SetSourceInfo(SourceInfoId sourceInfo);
	void SetConnections(std::vector<ConnectionId> connections);
	std::string& GetName(void);
	std::pair<size_t, size_t>& GetSize(void);
	SourceInfoId& GetSourceInfo(void);
	std::vector<ConnectionId>& GetConnections(void);
	std::string const& GetName(void) const;
	std::pair<size_t, size_t> const& GetSize(void) const;
	SourceInfoId const& GetSourceInfo(void) const;
	const std::vector<ConnectionId>& GetConnections(void) const;

};

struct Port
{
	std::string name;
	std::pair<size_t, size_t> size;
	PortType type;
	SourceInfoId sourceInfo;
	std::vector<ConnectionId> connections;

	void SetName(std::string name);
	void SetSize(std::pair<size_t, size_t> size);
	void SetType(PortType type);
	void SetSourceInfo(SourceInfoId sourceInfo);
	void SetConnections(std::vector<ConnectionId> connections);
	std::string& GetName(void);
	std::pair<size_t, size_t>& GetSize(void);
	PortType& GetType(void);
	SourceInfoId& GetSourceInfo(void);
	std::vector<ConnectionId>& GetConnections(void);
	std::string const& GetName(void) const;
	std::pair<size_t, size_t> const& GetSize(void) const;
	PortType const& GetType(void) const;
	SourceInfoId const& GetSourceInfo(void) const;
	std::vector<ConnectionId> const& GetConnections(void) const;
};

struct Group
{
	std::string name;
	SourceInfoId sourceInfo;
	GroupId parent;
	std::vector<GroupId> groupIds;
	std::vector<Port> ports;
	std::vector<Wire> wires;

	std::vector<UnmappedNodeId> unmappedNodeIds;
	std::vector<MappedNodeId> mappedNodeIds;

	void SetName(std::string name);
	void SetSourceInfo(SourceInfoId sourceInfo);
	void SetParent(GroupId parent);
	std::string& GetName(void);
	SourceInfoId& GetSourceInfo(void);
	GroupId& GetParent(void);
	std::string const& GetName(void) const;
	SourceInfoId const& GetSourceInfo(void) const;
	GroupId const& GetParent(void) const;

	void AddGroup(GroupId group);
	bool RemoveGroup(GroupId group);
	bool HasGroup(GroupId group) const;
	std::vector<GroupId> const& GetGroups(void) const;
	std::vector<GroupId>& GetGroups(void);

	PortId EmplacePort(std::string name, PortType type);
	bool RemovePort(PortId port);
	bool HasPort(PortId port) const;
	Port& GetPort(PortId port);
	Port const& GetPort(PortId port) const;
	std::vector<Port>& GetPorts(void);
	std::vector<Port> const& GetPorts(void) const;

	WireId EmplaceWire(std::string name);
	bool RemoveWire(WireId wire);
	bool HasWire(WireId wire) const;
	Wire& GetWire(WireId wire);
	Wire const& GetWire(WireId wire) const;
	std::vector<Wire>& GetWires(void);
	std::vector<Wire> const& GetWires(void) const;

	void AddUnmappedNode(UnmappedNodeId unmappedNode);
	bool RemoveUnmappedNode(UnmappedNodeId unmappedNode);
	bool HasUnmappedNode(UnmappedNodeId unmappedNode) const;
	std::vector<UnmappedNodeId> const& GetUnmappedNodes(void) const;
	std::vector<UnmappedNodeId>& GetUnmappedNodes(void);

	void AddMappedNode(MappedNodeId mappedNode);
	bool RemoveMappedNode(MappedNodeId mappedNode);
	bool HasMappedNode(MappedNodeId mappedNode) const;
	std::vector<MappedNodeId> const& GetMappedNodes(void) const;
	std::vector<MappedNodeId>& GetMappedNodes(void);
};

struct Connection
{
};

struct BuildConfiguration
{
	BuildConfiguration(void):
		unmappedDisallowCycles(false),
		unmappedListCycles(false),
		unmappedRemoveUnconnectedGates(false),
		unmappedDisallowUnconnectedInputs(false),
		unmappedConnectUnconnectedInputsToConstant(false),
		unmappedConnectUnconnectedInputsToConstantValue(Basic::Logic::LOGIC_DONT_CARE),
		mappedDisallowCycles(false),
		mappedListCycles(true),
		mappedRemoveUnconnectedGates(true),
		mappedBreakCombinatorialCycles(true),
		mappedDisallowUnconnectedInputs(false),
		mappedConnectUnconnectedInputsToConstant(true),
		mappedConnectUnconnectedInputsToConstantValue(Basic::Logic::LOGIC_DONT_CARE)
	{
	}

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
	 * @brief CircuitBuilder: Lists all combinatorial and sequential cycles that were found in the unmapped circuit.
	 *
	 * Default: false, do not report
	 */
	bool unmappedListCycles;

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
	Basic::Logic unmappedConnectUnconnectedInputsToConstantValue;

	// ------------------------------------------------------------------------
	// Mapped Circuit
	// ------------------------------------------------------------------------

	/**
	 * @brief CircuitBuilder: Throws an error if a combinatorial cycle was found in the mapped circuit.
	 */
	bool mappedDisallowCycles;


	/**
	 * @brief CircuitBuilder: Lists all combinatorial cycles that were found in the mapped circuit.
	 *
	 * Default: false, do not report
	 */
	bool mappedListCycles;

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
	Basic::Logic mappedConnectUnconnectedInputsToConstantValue;

};

class CircuitBuilder
{
public:
	CircuitBuilder(void);
	virtual ~CircuitBuilder(void);

	void SetName(std::string name);
	std::string GetName(void) const;

	UnmappedNodeId EmplaceUnmappedNode(std::string name, std::string type, size_t inputs, size_t outputs);
	void RemoveUnmappedNode(UnmappedNodeId unmappedNode);
	UnmappedNode& GetUnmappedNode(UnmappedNodeId mappedNode);
	bool IsUnmappedNodeDeleted(UnmappedNodeId unmappedNode) const;

	MappedNodeId EmplaceMappedNode(std::string name, CellCategory cellCategory, CellType cellType, size_t inputs);
	void RemoveMappedNode(MappedNodeId mappedNode);
	MappedNode& GetMappedNode(MappedNodeId mappedNode);
	bool IsMappedNodeDeleted(MappedNodeId mappedNode) const;

	void LinkMappedToUnmappedNode(MappedNodeId mappedNode, UnmappedNodeId unmappedNode);
	void LinkMappedToUnmappedInputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId mappedPin, PinId unmappedPin);
	void LinkMappedToUnmappedOutputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId unmappedPin);

	void AddUnmappedPrimaryInput(UnmappedNodeId input);
	void AddUnmappedPrimaryOutput(UnmappedNodeId output);

	void AddMappedPrimaryInput(MappedNodeId input);
	void AddMappedPrimaryOutput(MappedNodeId output);

	void AddSecondaryOutput(MappedNodeId output);
	void AddSecondaryInput(MappedNodeId input);
	void LinkSecondaryPorts(MappedNodeId input, MappedNodeId output);

	SourceInfoId AddSourceInfo(SourceInformation source);
	SourceInformation& GetSourceInfo(SourceInfoId index);

	GroupId EmplaceGroup(std::string name);
	Group& GetGroup(GroupId index);
	std::string GetFullGroupName(GroupId index);

	ConnectionId EmplaceConnection(void);
	Connection& GetConnection(ConnectionId index);

	std::unique_ptr<CircuitEnvironment> BuildCircuitEnvironment(const BuildConfiguration& configuration);

private:
	void LinkMappedToUnmappedPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, MappedPinId mappedPin, UnmappedPinId unmappedPin);

	std::string _name;

	std::vector<UnmappedNode> _unmappedNodes;
	std::vector<bool> _deletedUnmappedNodes;

	std::vector<MappedNode> _mappedNodes;
	std::vector<bool> _deletedMappedNodes;

	std::map<MappedNodeId, UnmappedNodeId> _mappedToUnmappedNodes;
	std::map<std::pair<MappedNodeId, MappedPinId>, std::pair<UnmappedNodeId, UnmappedPinId>> _mappedToUnmappedPins;

	std::vector<UnmappedNodeId> _unmappedPrimaryInputs;
	std::vector<UnmappedNodeId> _unmappedPrimaryOutputs;

	std::vector<MappedNodeId> _mappedPrimaryInputs;
	std::vector<MappedNodeId> _mappedPrimaryOutputs;
	std::vector<MappedNodeId> _mappedSecondaryInputs;
	std::vector<MappedNodeId> _mappedSecondaryOutputs;
	std::map<MappedNodeId, MappedNodeId> _secondaryInputToOutput;

	std::vector<SourceInformation> _sourceInformation;
	std::vector<Group> _groups;
	std::vector<Connection> _connections;
};

};
};
};
