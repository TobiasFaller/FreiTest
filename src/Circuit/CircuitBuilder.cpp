#include "Circuit/CircuitBuilder.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <limits>
#include <set>
#include <type_traits>

#include "Basic/Logic.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Circuit
{
namespace Builder
{

// ----------------------------------------------------------------------------
// UnmappedNode
// ----------------------------------------------------------------------------

void UnmappedNode::SetInputNode(size_t input, UnmappedNodeId node)
{
	inputs[input] = node;
}

bool UnmappedNode::HasInputNode(size_t input) const
{
	return inputs[input] != UNCONNECTED_UNMAPPED_PORT;
}

UnmappedNodeId& UnmappedNode::GetInputNode(size_t input)
{
	return inputs[input];
}

UnmappedNodeId const& UnmappedNode::GetInputNode(size_t input) const
{
	return inputs[input];
}

void UnmappedNode::AddOutputNode(size_t output, UnmappedNodeId successor)
{
	outputs[output].push_back(successor);
}

bool UnmappedNode::RemoveOutputNode(size_t output, UnmappedNodeId successor)
{
	if (auto it = std::find(outputs[output].begin(), outputs[output].end(), successor);
		it != outputs[output].end())
	{
		outputs[output].erase(it);
		return true;
	}

	return false;
}

bool UnmappedNode::HasOutputNode(size_t output, UnmappedNodeId successor) const
{
	if (auto it = std::find(outputs[output].begin(), outputs[output].end(), successor);
		it != outputs[output].end())
	{
		return true;
	}

	return false;
}

std::vector<UnmappedNodeId>& UnmappedNode::GetOutputNodes(size_t output)
{
	return outputs[output];
}

std::vector<UnmappedNodeId> const& UnmappedNode::GetOutputNodes(size_t output) const
{
	return outputs[output];
}

void UnmappedNode::SetInputPortName(size_t input, std::string name)
{
	inputPortNames[input] = name;
}

void UnmappedNode::SetOutputPortName(size_t output, std::string name)
{
	outputPortNames[output] = name;
}

void UnmappedNode::SetInputConnectionId(size_t input, size_t id)
{
	inputConnectionIds[input] = id;
}

void UnmappedNode::SetOutputConnectionId(size_t output, size_t id)
{
	outputConnectionIds[output] = id;
}

void UnmappedNode::SetInputConnectionName(size_t input, std::string name)
{
	inputConnectionNames[input] = name;
}

void UnmappedNode::SetOutputConnectionName(size_t output, std::string name)
{
	outputConnectionNames[output] = name;
}

void UnmappedNode::SetGroup(GroupId group)
{
	this->group = group;
}

GroupId& UnmappedNode::GetGroup(void)
{
	return group;
}

GroupId const& UnmappedNode::GetGroup(void) const
{
	return group;
}

std::string& UnmappedNode::GetInputPortName(size_t input)
{
	return inputPortNames[input];
}

std::string& UnmappedNode::GetOutputPortName(size_t output)
{
	return outputPortNames[output];
}

size_t& UnmappedNode::GetInputConnectionId(size_t input)
{
	return inputConnectionIds[input];
}

size_t& UnmappedNode::GetOutputConnectionId(size_t output)
{
	return outputConnectionIds[output];
}

std::string& UnmappedNode::GetInputConnectionName(size_t input)
{
	return inputConnectionNames[input];
}

std::string& UnmappedNode::GetOutputConnectionName(size_t output)
{
	return outputConnectionNames[output];
}

std::string const& UnmappedNode::GetInputPortName(size_t input) const
{
	return inputPortNames[input];
}

std::string const& UnmappedNode::GetOutputPortName(size_t output) const
{
	return outputPortNames[output];
}

size_t const& UnmappedNode::GetInputConnectionId(size_t input) const
{
	return inputConnectionIds[input];
}

size_t const& UnmappedNode::GetOutputConnectionId(size_t output) const
{
	return outputConnectionIds[output];
}

std::string const& UnmappedNode::GetInputConnectionName(size_t input) const
{
	return inputConnectionNames[input];
}

std::string const& UnmappedNode::GetOutputConnectionName(size_t output) const
{
	return outputConnectionNames[output];
}

// ----------------------------------------------------------------------------
// MappedNode
// ----------------------------------------------------------------------------

void MappedNode::SetInputNode(size_t input, MappedNodeId node)
{
	inputs[input] = node;
}

bool MappedNode::HasInputNode(size_t input) const
{
	return inputs[input] != UNCONNECTED_MAPPED_PORT;
}

MappedNodeId& MappedNode::GetInputNode(size_t input)
{
	return inputs[input];
}

MappedNodeId const& MappedNode::GetInputNode(size_t input) const
{
	return inputs[input];
}

void MappedNode::AddSuccessorNode(MappedNodeId successor)
{
	successors.push_back(successor);
}

bool MappedNode::RemoveSuccessorNode(MappedNodeId successor)
{
	if (auto it = std::find(successors.begin(), successors.end(), successor); it != successors.end())
	{
		successors.erase(it);
		return true;
	}

	return false;
}

bool MappedNode::HasSuccessorNode(MappedNodeId successor) const
{
	if (auto it = std::find(successors.begin(), successors.end(), successor); it != successors.end())
	{
		return true;
	}

	return false;
}

std::vector<MappedNodeId>& MappedNode::GetSuccessorNodes(void)
{
	return successors;
}

std::vector<MappedNodeId> const& MappedNode::GetSuccessorNodes(void) const
{
	return successors;
}

void MappedNode::SetGroup(GroupId group)
{
	this->group = group;
}

GroupId& MappedNode::GetGroup(void)
{
	return group;
}

GroupId const& MappedNode::GetGroup(void) const
{
	return group;
}

void MappedNode::SetInputPortName(size_t input, std::string name)
{
	inputPortNames[input] = name;
}

void MappedNode::SetOutputPortName(std::string name)
{
	outputPortName = name;
}

void MappedNode::SetInputConnectionName(size_t input, std::string name)
{
	inputConnectionNames[input] = name;
}

void MappedNode::SetOutputConnectionName(std::string name)
{
	outputConnectionName = name;
}

void MappedNode::SetInputConnectionId(size_t input, size_t wire)
{
	inputConnectionIds[input] = wire;
}

void MappedNode::SetOutputConnectionId(size_t wire)
{
	outputConnectionId = wire;
}

std::string& MappedNode::GetInputPortName(size_t input)
{
	return inputPortNames[input];
}

std::string& MappedNode::GetOutputPortName(void)
{
	return outputPortName;
}

std::string& MappedNode::GetInputConnectionName(size_t input)
{
	return inputConnectionNames[input];
}

std::string& MappedNode::GetOutputConnectionName(void)
{
	return outputConnectionName;
}

size_t& MappedNode::GetInputConnectionId(size_t input)
{
	return inputConnectionIds[input];
}

size_t& MappedNode::GetOutputConnectionId(void)
{
	return outputConnectionId;
}

std::string const& MappedNode::GetInputPortName(size_t input) const
{
	return inputPortNames[input];
}

std::string const& MappedNode::GetOutputPortName(void) const
{
	return outputPortName;
}

std::string const& MappedNode::GetInputConnectionName(size_t input) const
{
	return inputConnectionNames[input];
}

std::string const& MappedNode::GetOutputConnectionName(void) const
{
	return outputConnectionName;
}

size_t const& MappedNode::GetInputConnectionId(size_t input) const
{
	return inputConnectionIds[input];
}

size_t const& MappedNode::GetOutputConnectionId(void) const
{
	return outputConnectionId;
}

// ----------------------------------------------------------------------------
// Group
// ----------------------------------------------------------------------------

void Group::SetName(std::string name)
{
	this->name = name;
}

void Group::SetSourceInfo(SourceInfoId sourceInfo)
{
	this->sourceInfo = sourceInfo;
}

std::string& Group::GetName(void)
{
	return name;
}

SourceInfoId& Group::GetSourceInfo(void)
{
	return sourceInfo;
}

std::string const& Group::GetName(void) const
{
	return name;
}

SourceInfoId const& Group::GetSourceInfo(void) const
{
	return sourceInfo;
}

void Group::AddUnmappedNode(UnmappedNodeId unmappedNode)
{
	unmappedNodeIds.push_back(unmappedNode);
}

bool Group::RemoveUnmappedNode(UnmappedNodeId unmappedNode)
{
	if (auto it = std::find(unmappedNodeIds.begin(), unmappedNodeIds.end(), unmappedNode); it != unmappedNodeIds.end())
	{
		unmappedNodeIds.erase(it);
		return true;
	}

	return false;
}

bool Group::HasUnmappedNode(UnmappedNodeId unmappedNode) const
{
	if (auto it = std::find(unmappedNodeIds.begin(), unmappedNodeIds.end(), unmappedNode); it != unmappedNodeIds.end())
	{
		return true;
	}

	return false;
}

std::vector<UnmappedNodeId> const& Group::GetUnmappedNodes(void) const
{
	return unmappedNodeIds;
}

std::vector<UnmappedNodeId>& Group::GetUnmappedNodes(void)
{
	return groupIds;
}

void Group::AddMappedNode(MappedNodeId mappedNode)
{
	mappedNodeIds.push_back(mappedNode);
}

bool Group::RemoveMappedNode(MappedNodeId mappedNode)
{
	if (auto it = std::find(mappedNodeIds.begin(), mappedNodeIds.end(), mappedNode); it != mappedNodeIds.end())
	{
		mappedNodeIds.erase(it);
		return true;
	}

	return false;
}

bool Group::HasMappedNode(MappedNodeId mappedNode) const
{
	if (auto it = std::find(mappedNodeIds.begin(), mappedNodeIds.end(), mappedNode); it != mappedNodeIds.end())
	{
		return true;
	}

	return false;
}

std::vector<MappedNodeId> const& Group::GetMappedNodes(void) const
{
	return mappedNodeIds;
}

std::vector<MappedNodeId>& Group::GetMappedNodes(void)
{
	return mappedNodeIds;
}

void Group::SetParent(GroupId parent)
{
	this->parent = parent;
}

GroupId& Group::GetParent(void)
{
	return parent;
}

GroupId const& Group::GetParent(void) const
{
	return parent;
}

void Group::AddGroup(GroupId group)
{
	return groupIds.push_back(group);
}

bool Group::RemoveGroup(GroupId group)
{
	if (auto it = std::find(groupIds.begin(), groupIds.end(), group); it != groupIds.end())
	{
		groupIds.erase(it);
		return true;
	}

	return false;
}

bool Group::HasGroup(GroupId group) const
{
	if (auto it = std::find(groupIds.begin(), groupIds.end(), group); it != groupIds.end())
	{
		return true;
	}

	return false;
}

std::vector<GroupId> const& Group::GetGroups(void) const
{
	return groupIds;
}

std::vector<GroupId>& Group::GetGroups(void)
{
	return groupIds;
}

PortId Group::EmplacePort(std::string name, PortType type)
{
	Port port;
	port.name = name;
	port.size = { 0u, 0u };
	port.type = type;
	port.sourceInfo = INVALID_SOURCE_INFO;
	port.connections = { UNCONNECTED_CONNECTION };

	size_t portId = ports.size();
	ports.emplace_back(port);
	return portId;
}

bool Group::RemovePort(PortId port)
{
	if (port < ports.size())
	{
		ports.erase(ports.begin() + port);
		return true;
	}

	return false;
}

bool Group::HasPort(PortId port) const
{
	return (port < ports.size());
}

Port& Group::GetPort(PortId port)
{
	return ports[port];
}

const Port& Group::GetPort(PortId port) const
{
	return ports[port];
}

std::vector<Port>& Group::GetPorts(void)
{
	return ports;
}

std::vector<Port> const& Group::GetPorts(void) const
{
	return ports;
}

WireId Group::EmplaceWire(std::string name)
{
	Wire wire;
	wire.name = name;
	wire.size = { 0u, 0u };
	wire.sourceInfo = INVALID_SOURCE_INFO;
	wire.connections = { UNCONNECTED_CONNECTION };

	size_t wireId = wires.size();
	wires.emplace_back(wire);
	return wireId;
}

bool Group::RemoveWire(WireId wire)
{
	if (wire < wires.size())
	{
		wires.erase(wires.begin() + wire);
		return true;
	}

	return false;
}

bool Group::HasWire(WireId wire) const
{
	return (wire < wires.size());
}

Wire& Group::GetWire(WireId wire)
{
	return wires[wire];
}

const Wire& Group::GetWire(WireId wire) const
{
	return wires[wire];
}

std::vector<Wire>& Group::GetWires(void)
{
	return wires;
}

std::vector<Wire> const& Group::GetWires(void) const
{
	return wires;
}

// ----------------------------------------------------------------------------
// Port
// ----------------------------------------------------------------------------

void Port::SetName(std::string name)
{
	this->name = name;
}

void Port::SetSize(std::pair<size_t, size_t> size)
{
	this->size = size;
}

void Port::SetType(PortType type)
{
	this->type = type;
}

void Port::SetSourceInfo(SourceInfoId sourceInfo)
{
	this->sourceInfo = sourceInfo;
}

void Port::SetConnections(std::vector<ConnectionId> connections)
{
	this->connections = connections;
}

std::string& Port::GetName(void)
{
	return name;
}

std::pair<size_t, size_t>& Port::GetSize(void)
{
	return size;
}

PortType& Port::GetType(void)
{
	return type;
}

SourceInfoId& Port::GetSourceInfo(void)
{
	return sourceInfo;
}

std::vector<ConnectionId>& Port::GetConnections(void)
{
	return connections;
}

std::string const& Port::GetName(void) const
{
	return name;
}

std::pair<size_t, size_t> const& Port::GetSize(void) const
{
	return size;
}

PortType const& Port::GetType(void) const
{
	return type;
}

SourceInfoId const& Port::GetSourceInfo(void) const
{
	return sourceInfo;
}

std::vector<ConnectionId> const& Port::GetConnections(void) const
{
	return connections;
}

// ----------------------------------------------------------------------------
// Wire
// ----------------------------------------------------------------------------

void Wire::SetName(std::string name)
{
	this->name = name;
}

void Wire::SetSize(std::pair<size_t, size_t> size)
{
	this->size = size;
}

void Wire::SetSourceInfo(SourceInfoId sourceInfo)
{
	this->sourceInfo = sourceInfo;
}

void Wire::SetConnections(std::vector<ConnectionId> connections)
{
	this->connections = connections;
}

std::string& Wire::GetName(void)
{
	return name;
}

std::pair<size_t, size_t>& Wire::GetSize(void)
{
	return size;
}

SourceInfoId& Wire::GetSourceInfo(void)
{
	return sourceInfo;
}

std::vector<ConnectionId>& Wire::GetConnections(void)
{
	return connections;
}

std::string const& Wire::GetName(void) const
{
	return name;
}

std::pair<size_t, size_t> const& Wire::GetSize(void) const
{
	return size;
}

SourceInfoId const& Wire::GetSourceInfo(void) const
{
	return sourceInfo;
}

std::vector<ConnectionId> const& Wire::GetConnections(void) const
{
	return connections;
}

// ----------------------------------------------------------------------------
// Connection
// ----------------------------------------------------------------------------

// Nothing to implement

// ----------------------------------------------------------------------------
// CircuitBuilder
// ----------------------------------------------------------------------------

CircuitBuilder::CircuitBuilder(void):
	_name("Unnamed"),
	_unmappedNodes(),
	_deletedUnmappedNodes(),
	_mappedNodes(),
	_deletedMappedNodes(),
	_mappedToUnmappedNodes(),
	_mappedToUnmappedPins(),
	_unmappedPrimaryInputs(),
	_unmappedPrimaryOutputs(),
	_mappedPrimaryInputs(),
	_mappedPrimaryOutputs(),
	_mappedSecondaryInputs(),
	_mappedSecondaryOutputs(),
	_secondaryInputToOutput(),
	_sourceInformation(),
	_groups(),
	_connections()
{
}

CircuitBuilder::~CircuitBuilder(void) = default;

void CircuitBuilder::SetName(std::string name)
{
	_name = name;
}

std::string CircuitBuilder::GetName(void) const
{
	return _name;
}

UnmappedNodeId CircuitBuilder::EmplaceUnmappedNode(std::string name, std::string type, size_t inputs, size_t outputs)
{
	UnmappedNode node;
	node.name = name;
	node.type = type;
	node.inputs.resize(inputs, UNCONNECTED_UNMAPPED_PORT);
	node.inputPortNames.resize(inputs);
	node.inputConnectionIds.resize(inputs, UNCONNECTED_CONNECTION);
	node.inputConnectionNames.resize(inputs);
	node.outputs.resize(outputs);
	node.outputPortNames.resize(outputs);
	node.outputConnectionIds.resize(outputs, UNCONNECTED_CONNECTION);
	node.outputConnectionNames.resize(outputs);

	for (size_t input = 0u; input < inputs; ++input)
	{
		node.inputPortNames[input] = boost::str(boost::format("UNNAMED_INPUT_PORT_%u") % input);
		node.inputConnectionNames[input] = boost::str(boost::format("UNCONNECTED_INPUT_WIRE_%u") % input);
	}
	for (size_t output = 0u; output < outputs; ++output)
	{
		node.outputPortNames[output] = boost::str(boost::format("UNNAMED_OUTPUT_PORT_%u") % output);
		node.outputConnectionNames[output] = boost::str(boost::format("UNCONNECTED_OUTPUT_WIRE_%u") % output);
	}

	UnmappedNodeId id = _unmappedNodes.size();
	_unmappedNodes.push_back(node);
	_deletedUnmappedNodes.push_back(false);
	return id;
}

void CircuitBuilder::RemoveUnmappedNode(UnmappedNodeId unmappedNode)
{
	_deletedUnmappedNodes[unmappedNode] = true;
}

UnmappedNode& CircuitBuilder::GetUnmappedNode(UnmappedNodeId unmappedNode)
{
	return _unmappedNodes[unmappedNode];
}

bool CircuitBuilder::IsUnmappedNodeDeleted(UnmappedNodeId unmappedNode) const
{
	return _deletedUnmappedNodes[unmappedNode];
}

MappedNodeId CircuitBuilder::EmplaceMappedNode(std::string name, CellCategory cellCategory, CellType cellType, size_t inputs)
{
	MappedNode node;
	node.name = name;
	node.cellCategory = cellCategory;
	node.cellType = cellType;
	node.inputs.resize(inputs, UNCONNECTED_MAPPED_PORT);
	node.inputPortNames.resize(inputs);
	node.inputConnectionNames.resize(inputs);
	node.inputConnectionIds.resize(inputs, UNCONNECTED_CONNECTION);

	for (size_t input = 0u; input < inputs; ++input)
	{
		node.inputPortNames[input] = boost::str(boost::format("UNNAMED_INPUT_PORT_%u") % input);
		node.inputConnectionNames[input] = boost::str(boost::format("UNCONNECTED_INPUT_WIRE_%u") % input);
	}

	node.outputPortName = "UNNAMED_OUTPUT_PORT";
	node.outputConnectionName = "UNCONNECTED_OUTPUT_WIRE";
	node.outputConnectionId = UNCONNECTED_CONNECTION;

	MappedNodeId id = _mappedNodes.size();
	_mappedNodes.push_back(node);
	_deletedMappedNodes.push_back(false);
	return id;
}

void CircuitBuilder::RemoveMappedNode(MappedNodeId mappedNode)
{
	_deletedMappedNodes[mappedNode] = true;
}

MappedNode& CircuitBuilder::GetMappedNode(MappedNodeId mappedNode)
{
	return _mappedNodes[mappedNode];
}

bool CircuitBuilder::IsMappedNodeDeleted(MappedNodeId mappedNode) const
{
	return _deletedMappedNodes[mappedNode];
}

void CircuitBuilder::LinkMappedToUnmappedNode(MappedNodeId mappedNode, UnmappedNodeId unmappedNode)
{
	_mappedToUnmappedNodes.insert_or_assign(mappedNode, unmappedNode);
}

void CircuitBuilder::LinkMappedToUnmappedInputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId mappedPin, PinId unmappedPin)
{
	LinkMappedToUnmappedPin(mappedNode, unmappedNode, static_cast<MappedPinId>(mappedPin + 1u), static_cast<UnmappedPinId>(unmappedPin));
}

void CircuitBuilder::LinkMappedToUnmappedOutputPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, PinId unmappedPin)
{
	LinkMappedToUnmappedPin(mappedNode, unmappedNode, static_cast<MappedPinId>(0u), -static_cast<UnmappedPinId>(unmappedPin + 1u));
}

/**
 * @brief Inserts a new mapping entry for one pin.
 *
 * MappedPinId: 0..n+1
 *   p = 0      is equal to output pin
 *   p = 1..n+1 is equal to input pin p-1u
 *
 * UnmappedPinId: -n..+m
 *   p = -n..-1 is equal to output pin abs(p)-1u
 *   p = 0..m   is equal to input pin p
 */
void CircuitBuilder::LinkMappedToUnmappedPin(MappedNodeId mappedNode, UnmappedNodeId unmappedNode, MappedPinId mappedPin, UnmappedPinId unmappedPin)
{
	_mappedToUnmappedPins.insert_or_assign(std::make_pair(mappedNode, mappedPin), std::make_pair(unmappedNode, unmappedPin));
}

void CircuitBuilder::AddSecondaryInput(MappedNodeId input)
{
	_mappedSecondaryInputs.push_back(input);
}

void CircuitBuilder::AddSecondaryOutput(MappedNodeId output)
{
	_mappedSecondaryOutputs.push_back(output);
}

void CircuitBuilder::AddUnmappedPrimaryInput(UnmappedNodeId input)
{
	_unmappedPrimaryInputs.push_back(input);
}

void CircuitBuilder::AddUnmappedPrimaryOutput(UnmappedNodeId output)
{
	_unmappedPrimaryOutputs.push_back(output);
}

void CircuitBuilder::AddMappedPrimaryInput(MappedNodeId input)
{
	_mappedPrimaryInputs.push_back(input);
}

void CircuitBuilder::AddMappedPrimaryOutput(MappedNodeId output)
{
	_mappedPrimaryOutputs.push_back(output);
}

void CircuitBuilder::LinkSecondaryPorts(MappedNodeId input, MappedNodeId output)
{
	_secondaryInputToOutput.insert_or_assign(input, output);
}

SourceInfoId CircuitBuilder::AddSourceInfo(SourceInformation source)
{
	size_t index = _sourceInformation.size();
	_sourceInformation.emplace_back(source);
	return index;
}

SourceInformation& CircuitBuilder::GetSourceInfo(SourceInfoId index)
{
	return _sourceInformation[index];
}

GroupId CircuitBuilder::EmplaceGroup(std::string name)
{
	Group group;
	group.name = name;
	group.sourceInfo = INVALID_SOURCE_INFO;
	group.unmappedNodeIds = {};
	group.mappedNodeIds = {};
	group.parent = INVALID_GROUP;
	group.groupIds = {};
	group.ports = {};
	group.wires = {};

	size_t index = _groups.size();
	_groups.emplace_back(group);
	return index;
}

Group& CircuitBuilder::GetGroup(GroupId index)
{
	return _groups[index];
}

std::string CircuitBuilder::GetFullGroupName(GroupId index)
{
	GroupId currentGroup = index;
	std::string currentName;

	do
	{
		auto& group = _groups[currentGroup];
		currentName = group.name + (currentName.empty() ? "" : "/") + currentName;
		currentGroup = group.parent;
	}
	while (currentGroup != INVALID_GROUP);

	return currentName;
}

ConnectionId CircuitBuilder::EmplaceConnection(void)
{
	size_t index = _connections.size();
	_connections.emplace_back();
	return index;
}

Connection& CircuitBuilder::GetConnection(ConnectionId index)
{
	return _connections[index];
}

// ----------------------------------------------------------------------------
// Circuit Builder
// ----------------------------------------------------------------------------

template<typename NodeType, typename NodeId>
bool IsDeletedNodeImpl(NodeId nodeId, const std::vector<bool>& deletedNodes)
{
	if constexpr (std::is_same_v<NodeType, UnmappedNode>)
	{
		return nodeId == UNCONNECTED_UNMAPPED_PORT || deletedNodes[nodeId];
	}
	else if constexpr (std::is_same_v<NodeType, MappedNode>)
	{
		return nodeId == UNCONNECTED_MAPPED_PORT || deletedNodes[nodeId];
	}
	else
	{
		static_assert(std::is_same_v<NodeType, UnmappedNode>, "Unsupported type");
	}
}

template<typename NodeType, typename NodeId>
std::vector<NodeId> GetNodeSuccessorsImpl(NodeId nodeId, const std::vector<NodeType>& nodes, const std::vector<bool>& deletedNodes)
{
	const auto IsNotDeletedNode = [&](const NodeId& nodeId) -> bool {
		return !IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};

	std::vector<NodeId> outputs;

	if constexpr (std::is_same_v<NodeType, UnmappedNode>)
	{
		for (auto output : nodes[nodeId].outputs)
		{
			std::copy_if(output.cbegin(), output.cend(), std::back_inserter(outputs), IsNotDeletedNode);
		}
	}
	else if constexpr (std::is_same_v<NodeType, MappedNode>)
	{
		std::copy_if(nodes[nodeId].successors.cbegin(), nodes[nodeId].successors.cend(), std::back_inserter(outputs), IsNotDeletedNode);
	}
	else
	{
		static_assert(std::is_same_v<NodeType, UnmappedNode>, "Unsupported type");
	}

	return outputs;
};

template<typename NodeType, typename NodeId>
std::vector<NodeId> GetNodePredecessorsImpl(NodeId nodeId, const std::vector<NodeType>& nodes, const std::vector<bool>& deletedNodes)
{
	const auto IsNotDeletedNode = [&](const NodeId& nodeId) -> bool {
		return !IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};

	std::vector<NodeId> inputs;
	std::copy_if(nodes[nodeId].inputs.cbegin(), nodes[nodeId].inputs.cend(), std::back_inserter(inputs), IsNotDeletedNode);
	return inputs;
};

template<typename NodeId>
struct NodeCycle
{
	NodeCycle(NodeId nodeId, NodeId successorId, std::vector<NodeId> path):
		nodeId(nodeId),
		successorId(successorId),
		path(path)
	{
	}

	NodeId nodeId;
	NodeId successorId;
	std::vector<NodeId> path;
};

template<typename NodeType, typename NodeId>
std::vector<NodeCycle<NodeId>> FindNodeCycles(std::vector<NodeType>& nodes, std::vector<bool>& deletedNodes)
{
	const auto IsDeletedNode = [&](const NodeId& nodeId) -> bool {
		return IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};
	const auto IsNotDeletedNode = [&](const NodeId& nodeId) -> bool {
		return !IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};
	const auto GetNodeSuccessors = [&](NodeId nodeId) -> std::vector<NodeId> {
		return GetNodeSuccessorsImpl<NodeType, NodeId>(nodeId, nodes, deletedNodes);
	};
	const auto GetNodePredecessors = [&](NodeId nodeId) -> const std::vector<NodeId> {
		return GetNodePredecessorsImpl<NodeType, NodeId>(nodeId, nodes, deletedNodes);
	};

	auto HasSuccessors = [&](NodeId nodeId) -> bool
	{
		for (NodeId successor : GetNodeSuccessors(nodeId))
		{
			if (IsNotDeletedNode(successor))
			{
				return true;
			}
		}
		return false;
	};
	auto HasPredecessors = [&](NodeId nodeId) -> bool
	{
		for (NodeId predecessor : GetNodePredecessors(nodeId))
		{
			if (IsNotDeletedNode(predecessor))
			{
				return true;
			}
		}
		return false;
	};

	enum class NodeVisitColor
	{
		Unvisited, //!< equal to white color in CRLS (Introduction to Algorithms)
		Visiting, //!< equal to gray color in CRLS (Introduction to Algorithms)
		Visited //!< equal to black color in CRLS (Introduction to Algorithms)
	};

	struct NodeVisitState
	{
		NodeVisitState(NodeId node, std::vector<NodeId> target):
			node(node),
			target(target)
		{
		}

		NodeId node;
		std::vector<NodeId> target;
	};

	enum class CycleDirection
	{
		Forward,
		Backward
	};

	auto GetCycle = [&](const std::vector<NodeVisitState>& visitStack, NodeId nodeId, CycleDirection direction) -> std::vector<NodeId> {
		std::vector<NodeId> cycleNodes;
		for (auto it = visitStack.crbegin(); it != visitStack.crend(); ++it)
		{
			auto [visitNodeId, visitTarget] = *it;
			(void) visitTarget; // Unused variable visitTarget

			cycleNodes.push_back(visitNodeId);

			// Found start of cycle
			if (visitNodeId == nodeId)
			{
				break;
			}
		}

		if (direction == CycleDirection::Forward)
		{
			std::reverse(cycleNodes.begin(), cycleNodes.end());
		}

		return cycleNodes;
	};

	// Find the start and end nodes of the circuit
	// - A start node has no connected inputs
	// - An end node has no connected outputs
	std::vector<NodeId> startNodes;
	std::vector<NodeId> endNodes;
	for (NodeId nodeId = 0u; nodeId < nodes.size(); ++nodeId)
	{
		if (IsDeletedNode(nodeId))
		{
			continue;
		}
		if (!HasPredecessors(nodeId))
		{
			startNodes.push_back(nodeId);
		}
		if (!HasSuccessors(nodeId))
		{
			endNodes.push_back(nodeId);
		}
	}

	/*
	 * Algorithm from CRLS (Introduction to Algorithms):
	 *
	 * The circuit is iterated from start to end nodes and then from end nodes to start.
	 * When a cycle is found, the cycle is either broken, repored or stored for later usage.
	 *
	 * The cycle search works on a depth-first search basis.
	 * Each node is initially colored "white", which is equal to the Unvisited state.
	 * When a new node is encountered and visited it is colored "gray", which is equal to
	 * the Visiting state.
	 * Then all the children of this node are visited.
	 * When all children have been visited the node color is changed to "black",
	 * which is equal to the Visited state.
	 *
	 * When a child node is visited which has the "gray" color we know that the current path
	 * connects to itself with a loop and we can handle this cycle.
	 * When a child node is visited which has the "black" color we know that we fully visited
	 * the node and its children and that the path starting at this node has no more
	 * visitable child nodes.
	 */
	std::vector<NodeVisitColor> nodeColors(nodes.size(), NodeVisitColor::Unvisited);
	std::vector<NodeCycle<NodeId>> nodeCycles;

	auto VisitForward = [&](NodeId startNode) -> void {
		std::vector<NodeVisitState> visitStack;
		visitStack.emplace_back(startNode, GetNodeSuccessors(startNode));

		while (!visitStack.empty())
		{
			auto& [nodeId, successors] = visitStack.back();
			if (successors.empty())
			{
				nodeColors[nodeId] = NodeVisitColor::Visited;
				visitStack.pop_back();
				continue;
			}

			const NodeId successorId = successors.back();
			successors.pop_back();

			switch (nodeColors[successorId])
			{
				case NodeVisitColor::Unvisited:
					// A new unvisited node
					visitStack.emplace_back(successorId, GetNodeSuccessors(successorId));
					nodeColors[successorId] = NodeVisitColor::Visiting;
					continue;

				case NodeVisitColor::Visiting:
					// Already visited node, which is in the current path -> cycle detected
					nodeCycles.emplace_back(nodeId, successorId, GetCycle(visitStack, successorId, CycleDirection::Forward));
					break;

				default:
				case NodeVisitColor::Visited:
					// Already fully visited node -> no cycle from here as this branch has no cycle
					continue;
			}
		}
	};
	auto VisitBackward = [&](NodeId endNode) -> void {
		std::vector<NodeVisitState> visitStack;
		visitStack.emplace_back(endNode, GetNodePredecessors(endNode));

		while (!visitStack.empty())
		{
			auto& [nodeId, predecessors] = visitStack.back();
			if (predecessors.empty())
			{
				nodeColors[nodeId] = NodeVisitColor::Visited;
				visitStack.pop_back();
				continue;
			}

			const NodeId predecessorId = predecessors.back();
			predecessors.pop_back();

			switch (nodeColors[predecessorId])
			{
				case NodeVisitColor::Unvisited:
					// A new unvisited node
					visitStack.emplace_back(predecessorId, GetNodePredecessors(predecessorId));
					nodeColors[predecessorId] = NodeVisitColor::Visiting;
					continue;

				case NodeVisitColor::Visiting:
					// Already visited node, which is in the current path -> cycle detected
					nodeCycles.emplace_back(predecessorId, nodeId, GetCycle(visitStack, predecessorId, CycleDirection::Backward));
					break;

				default:
				case NodeVisitColor::Visited:
					// Already fully visited node -> no loop as this branch has no loops
					continue;
			}
		}
	};

	for (NodeId startNode : startNodes)
	{
		if (IsNotDeletedNode(startNode) && nodeColors[startNode] == NodeVisitColor::Unvisited)
		{
			VisitForward(startNode);
		}
	}
	for (NodeId endNode : endNodes)
	{
		if (IsNotDeletedNode(endNode) && nodeColors[endNode] == NodeVisitColor::Unvisited)
		{
			VisitBackward(endNode);
		}
	}

	// All cycles between the start and end nodes are now broken up
	// or at least detected. Now only self-cycles not located between a start
	// and end node are remaining - if any exist.
	// We can detect them by searching for remaining "white", aka Unvisited nodes.
	for (NodeId nodeId = 0u; nodeId < nodes.size(); ++nodeId)
	{
		if (IsNotDeletedNode(nodeId) && nodeColors[nodeId] == NodeVisitColor::Unvisited)
		{
			VisitForward(nodeId);
		}
	}

	return nodeCycles;
}

template<typename NodeType, typename NodeId>
void BreakCycles(const std::vector<NodeCycle<NodeId>>& cycles, std::vector<NodeType>& nodes, std::vector<bool>& deletedNodes)
{
	size_t brokenCycles = 0u;

	for (auto& [nodeId, successorId, path] : cycles)
	{
		NodeType node = nodes[nodeId];
		NodeType successor = nodes[successorId];

		// Create new constant X input node
		NodeId inputNodeId = nodes.size();
		NodeType inputNode;
		inputNode.name = boost::str(boost::format("break_cycle_%u_%s_%s") % brokenCycles % node.name % successor.name);
		inputNode.cellType = CellType::PRESET_X;
		inputNode.cellCategory = CellCategory::MAIN_CONSTANT;
		inputNode.inputs = { };
		inputNode.inputPortNames = { };
		inputNode.inputConnectionNames = { };
		inputNode.successors = { nodeId };
		inputNode.outputPortName = "Z";
		inputNode.outputConnectionName = node.outputConnectionName;
		inputNode.outputConnectionId = node.outputConnectionId;
		nodes.push_back(inputNode);
		deletedNodes.push_back(false);

		// Remove connection to successor node
		std::replace_if(node.successors.begin(), node.successors.end(), [nodeId = successorId](auto node) { return node == nodeId; }, UNCONNECTED_MAPPED_PORT);

		// Connect output of new node to input of existing successor node
		std::replace_if(successor.inputs.begin(), successor.inputs.end(), [nodeId = nodeId](auto node) { return node == nodeId; }, inputNodeId);

		brokenCycles++;
	}
}

template<typename NodeType, typename NodeId>
std::string CycleToString(const NodeCycle<NodeId>& cycle, const std::vector<NodeType>& nodes)
{
	auto& [fromNodeId, toNodeId, pathNodeIds ] = cycle;

	std::vector<std::string> nodeNames;
	nodeNames.reserve(pathNodeIds.size());

	for (NodeId nodeId : pathNodeIds)
	{
		nodeNames.push_back(nodes[nodeId].name);
	}
	nodeNames.push_back(nodes[toNodeId].name);

	// 4u is the length of the " -> " string, keep synchronized with the code below
	size_t size = (std::max(static_cast<size_t>(1u), nodeNames.size()) - 1u) * 4u;
	for (auto nodeName : nodeNames)
	{
		size += nodeName.length();
	}

	// Now convert all node names to a string
	std::string result;
	result.reserve(size);

	for (auto it = nodeNames.cbegin(); it != nodeNames.cend(); ++it)
	{
		if (it != nodeNames.cbegin())
		{
			// Keep the length calculation in the code above synchronized with this
			// string for performance reasons
			result += " -> ";
		}
		result += *it;
	}

	return result;
}

template<typename NodeType, typename NodeId>
std::vector<size_t> TopologicalSort(const std::vector<NodeCycle<NodeId>>& cycles, const std::vector<NodeType>& nodes, const std::vector<bool>& deletedNodes)
{
	std::set<std::pair<NodeId, NodeId>> cycleSet;
	for (auto& [nodeId, successorId, pathNodeIds] : cycles)
	{
		cycleSet.emplace(nodeId, successorId);
	}

	const auto IsNotDeletedNode = [&](const NodeId& nodeId) -> bool {
		return !IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};

	/**
	 * @brief Creates and returns a list with all successor nodes of the node.
	 */
	const auto GetNodeSuccessors = [&](NodeId nodeId) -> std::vector<NodeId>
	{
		const auto IsValidSuccessor = [&, nodeId](NodeId successor) {
			return IsNotDeletedNode(successor) && cycleSet.find(std::make_pair(nodeId, successor)) == cycleSet.end();
		};

		std::vector<NodeId> outputs;

		if constexpr (std::is_same_v<NodeType, UnmappedNode>)
		{
			for (auto output : nodes[nodeId].outputs)
			{
				std::copy_if(output.cbegin(), output.cend(), std::back_inserter(outputs), IsValidSuccessor);
			}
		}
		else if constexpr (std::is_same_v<NodeType, MappedNode>)
		{
			std::copy_if(nodes[nodeId].successors.cbegin(), nodes[nodeId].successors.cend(), std::back_inserter(outputs), IsValidSuccessor);
		}
		else
		{
			static_assert(std::is_same_v<NodeType, UnmappedNode>, "Unsupported type");
		}

		return outputs;
	};

	/**
	 * @brief Creates and returns a list with all predecessor nodes of the node.
	 */
	const auto GetNodePredecessors = [&](NodeId nodeId) -> const std::vector<NodeId>
	{
		const auto IsValidPredecessor = [&, nodeId](NodeId predecessor) {
			return IsNotDeletedNode(predecessor) && cycleSet.find(std::make_pair(predecessor, nodeId)) == cycleSet.end();
		};

		std::vector<NodeId> inputs;
		std::copy_if(nodes[nodeId].inputs.cbegin(), nodes[nodeId].inputs.cend(), std::back_inserter(inputs), IsValidPredecessor);
		return inputs;
	};

	auto HasPredecessors = [&](NodeId nodeId) -> bool
	{
		for (NodeId predecessor : GetNodePredecessors(nodeId))
		{
			if (IsNotDeletedNode(predecessor))
			{
				return true;
			}
		}
		return false;
	};


	using NodeLevel = size_t;
	constexpr NodeLevel INF_LEVEL = std::numeric_limits<NodeLevel>::max();

	std::vector<std::vector<NodeId>> levels;
	std::vector<NodeLevel> nodeLevels(nodes.size(), INF_LEVEL);

	/*
	 * All nodes without inputs are treated as start nodes
	 * which are located on level 0.
	 */
	std::vector<NodeId> startNodes;
	for (NodeId nodeId = 0u; nodeId < nodes.size(); ++nodeId)
	{
		if (IsNotDeletedNode(nodeId) && !HasPredecessors(nodeId))
		{
			DVLOG(9) << "Node " << nodes[nodeId].name << " is solved to level 0";
			startNodes.push_back(nodeId);
			nodeLevels[nodeId] = 0u;
		}
	}
	levels.emplace_back(startNodes);

	std::vector<NodeId> activeNodes;
	for (const NodeId nodeId : startNodes)
	{
		auto successors = GetNodeSuccessors(nodeId);
		std::copy(successors.cbegin(), successors.cend(), std::back_inserter(activeNodes));
	}

	const auto NodeListToString = [nodes](const std::vector<NodeId>& list) -> std::string {
		std::string result;

		for (NodeId nodeId : list)
		{
			if (!result.empty())
			{
				result += ", ";
			}
			result += nodes[nodeId].name;
		}

		return result;
	};

	/*
	 * Do a breadth-first search and find all reachable nodes
	 * in levels. Each level can later be assigned separately to
	 * build a topological sorting.
	 */
	for(size_t level = 1u;; ++level)
	{
		std::vector<NodeId> newLevel;
		std::set<NodeId> newActiveNodes;

		for (const NodeId nodeId : activeNodes)
		{
			if (nodeLevels[nodeId] != INF_LEVEL)
			{
				continue;
			}

			// Check if level can now be settled now
			// Only if all inputs are defined - aka. not INF_LEVEL - the node is settled
			auto predecessors = GetNodePredecessors(nodeId);
			ASSERT(predecessors.size() > 0);

			bool solved = true;
			for (NodeId predecessor : predecessors)
			{
				if (nodeLevels[predecessor] == INF_LEVEL)
				{
					DVLOG(9) << "Node " << nodes[nodeId].name << " is not solved because of " << nodes[predecessor].name;
					solved = false;
					break;
				}
			}

			if (solved)
			{
				DVLOG(9) << "Node " << nodes[nodeId].name << " is solved to level " << level;
				newLevel.emplace_back(nodeId);
				nodeLevels[nodeId] = level;

				auto successors = GetNodeSuccessors(nodeId);
				std::copy(successors.cbegin(), successors.cend(), std::inserter(newActiveNodes, newActiveNodes.end()));
			}
		}

		if (newLevel.size() == 0u)
		{
			ASSERT(newActiveNodes.size() == 0u);
			break;
		}

		levels.emplace_back(std::move(newLevel));
		activeNodes = std::vector(newActiveNodes.cbegin(), newActiveNodes.cend());
	}

	for (size_t nodeId = 0u; nodeId < nodes.size(); ++nodeId)
	{
		DVLOG(9) << "Node " << nodes[nodeId].name << " is solved to level "
			<< ((nodeLevels[nodeId] == INF_LEVEL) ? "INF_LEVEL" : std::to_string(nodeLevels[nodeId]));
		if (nodeLevels[nodeId] == INF_LEVEL)
		{
			LOG(ERROR) << "Found node which was not assigned a level during breadth-first search";
			LOG(FATAL) << "This should never happen if the cycle detection works correctly";
			return std::vector<size_t>();
		}
	}

	/*
	 * Now assign the node tsorts on a per-level basis.
	 * Processing each level after the other ensures that the tsort
	 * is generated in ascending order.
	 */
	std::vector<size_t> tsortResult;
	tsortResult.resize(nodes.size(), 0u);

	size_t nodeTsort = 0u;
	for (auto level : levels)
	{
		for (NodeId nodeId : level)
		{
			tsortResult[nodeId] = nodeTsort;
			nodeTsort++;
		}
	}

	std::set<size_t> tsortCheck(tsortResult.cbegin(), tsortResult.cend());
	if (tsortCheck.size() != nodes.size())
	{
		LOG(ERROR) << "Not all nodes were assigned to a level or tsort is not unique";
		LOG(ERROR) << "There are " << nodes.size() << " nodes, but " << tsortCheck.size() << " tsort ids";
		return std::vector<size_t>();
	}

	return tsortResult;
}

template<typename NodeType, typename NodeId>
std::vector<std::pair<NodeId, size_t>> FindUnconnectedInputs(const std::vector<NodeType>& nodes, const std::vector<bool>& deletedNodes)
{
	const auto IsDeletedNode = [&](const NodeId& nodeId) -> bool {
		return IsDeletedNodeImpl<NodeType, NodeId>(nodeId, deletedNodes);
	};

	std::vector<std::pair<NodeId, PinId>> result;

	for (NodeId nodeId = 0u; nodeId < nodes.size(); ++nodeId)
	{
		if (IsDeletedNode(nodeId))
		{
			continue;
		}

		const NodeType& node = nodes[nodeId];
		for (size_t input = 0u; input < node.inputs.size(); ++input)
		{
			if (IsDeletedNode(node.inputs[input]))
			{
				result.emplace_back(nodeId, input);
			}
		}
	}

	return result;
}

std::unique_ptr<CircuitEnvironment> CircuitBuilder::BuildCircuitEnvironment(const BuildConfiguration& configuration)
{
	// Basic sanity checks to make sure that the data passed to the
	// CircuitBuilder actually might make sense.
	// Used to catch bugs in modules that use this CircuitBuilder.
	for (auto& group : _groups)
	{
		ASSERT(!group.name.empty()) << "Found group with empty name";
		ASSERT(group.sourceInfo == INVALID_SOURCE_INFO || group.sourceInfo < _sourceInformation.size())
				<< "Invalid source info ID detected in group " << group.name;
		ASSERT(group.parent == INVALID_GROUP || group.parent < _groups.size())
				<< "Invalid parent group ID detected in group " << group.name;

		for (auto& subGroup : group.groupIds)
		{
			ASSERT(subGroup == INVALID_GROUP || subGroup < _groups.size())
				<< "Invalid group ID detected in group " << group.name;
		}

		for (auto& port : group.ports)
		{
			ASSERT(!port.name.empty()) << "Found port with empty name in group " << group.name;
			ASSERT(port.sourceInfo == INVALID_SOURCE_INFO || port.sourceInfo < _sourceInformation.size())
				<< "Invalid source info ID detected in port " << port.name << " of group " << group.name;

			auto [top, bottom] = port.size;
			auto [min, max] = std::minmax(top, bottom);
			ASSERT(port.connections.size() == (max + 1u - min))
				<< "Connection count and port size does not match for port " << port.name << " of group " << group.name;
		}

		for (auto& wire : group.wires)
		{
			ASSERT(!wire.name.empty()) << "Found wire with empty name in group " << group.name;
			ASSERT(wire.sourceInfo == INVALID_SOURCE_INFO || wire.sourceInfo < _sourceInformation.size())
				<< "Invalid source info ID detected in wire " << wire.name << " of group " << group.name;

			auto [top, bottom] = wire.size;
			auto [min, max] = std::minmax(top, bottom);
			ASSERT(wire.connections.size() == (max + 1u - min))
				<< "Connection count and wire size does not match for wire " << wire.name << " of group " << group.name;
		}
	}

	auto environment = std::make_unique<CircuitEnvironment>();
	environment->_circuitName = _name;

	auto& unmappedCircuit = environment->_unmappedCircuit;
	auto& mappedCircuit = environment->_circuit;
	auto& metaData = environment->_metaData;

	const auto find_topmost_parent_with_connection = [&](auto connectionId, auto& groupId) -> GroupId {
		// The following assumptions are made:
		// 1) When the connection ID occurs in the parent it is via a wire or port.
		// 2) When the connection ID occurs in the parent a port in the group with the connection ID exists.
		// 3) A single connection IDs forms a single coherent net that is connected by wires and ports.
		//
		// Example:
		//      +-----------------------+
		//      |           group       |
		//      |  wire   +-------+     |
		// port +---+-----+port   |     |
		//      |   |     +-------+     |
		//      |   |                   |
		//      |   |     +-------+     |
		//      |   +-----+---[]  |     |
		//      |         +-------+     |
		//      +-----------------------+
		//
		// => Therefore, we only have to trace the top-most parent once
		//    from any of the starting nodes.

		while (groupId != 0u)
		{
			auto foundPortWithConnection = false;
			for (auto const& port : _groups[groupId].GetPorts())
			{
				if (port.GetType() != PortType::Input)
				{
					continue;
				}

				auto const& connections = port.GetConnections();
				if (std::find(connections.begin(), connections.end(), connectionId) != connections.end())
				{
					foundPortWithConnection = true;
					groupId = _groups[groupId].GetParent();
					break;
				}
			}
			if (!foundPortWithConnection)
			{
				return groupId;
			}
		}

		return groupId;
	};

	// ------------------------------------------------------------------------
	// Unmapped Circuit
	// ------------------------------------------------------------------------

	// Identify unconnected inputs in the unmapped circuit
	const auto unmappedUnconnectedInputs = FindUnconnectedInputs<UnmappedNode, UnmappedNodeId>(_unmappedNodes, _deletedUnmappedNodes);
	if (unmappedUnconnectedInputs.size() > 0u)
	{
		if (configuration.unmappedConnectUnconnectedInputsToConstant)
		{
			LOG(WARNING) << "Found unconnected input(s) in the unmapped circuit:";
			for (auto [nodeId, inputPin] : unmappedUnconnectedInputs)
			{
				LOG(INFO) << "Unconnected input pin " << inputPin << " at gate " << _unmappedNodes[nodeId].name;
			}

			const std::string cellType = [](Logic value) -> std::string {
				switch (value)
				{
					case Logic::LOGIC_ZERO:
						return "tie0";
					case Logic::LOGIC_ONE:
						return "tie1";
					case Logic::LOGIC_DONT_CARE:
						return "tieX";
					case Logic::LOGIC_UNKNOWN:
						return "tieU";
					default:
						LOG(FATAL) << "Unknown logic type " << value;
						return std::string("tie") + static_cast<char>(value);
				}
			}(configuration.unmappedConnectUnconnectedInputsToConstantValue);

			LOG(INFO) << "Creating constant inputs UNCONNECTED_INPUT_CONSTANT of type "
				<< configuration.unmappedConnectUnconnectedInputsToConstantValue << " for unconnected inputs";

			std::map<size_t, std::vector<std::pair<size_t, size_t>>> connectionIds;
			for (auto& [nodeId, inputPin]: unmappedUnconnectedInputs)
			{
				connectionIds[_unmappedNodes[nodeId].inputConnectionIds[inputPin]]
					.push_back(std::make_pair(nodeId, inputPin));
			}

			size_t index { 0u };
			for (auto& [connectionId, nodes] : connectionIds)
			{
				// NOTE: Do store the parent group only as id and not reference as a reallocation might ocurr
				ASSERT(nodes.size() != 0u) << "No nodes for the connection ID have been found";
				auto const& [nodeId, inputPin] = nodes[0];
				auto topMostParentId = find_topmost_parent_with_connection(connectionId, _unmappedNodes[nodeId].GetGroup());

				const auto groupName = boost::str(boost::format("unconnected-unmapped-%d") % index);
				const auto nodeName = boost::str(boost::format("UNCONNECTED_UNMAPPED_INPUT_CONSTANT_%d") % index++);

				SourceInformation groupSourceInfo { NO_SOURCE_FILE, _name, groupName, "constant" };
				groupSourceInfo.AddProperty<std::string>("constant-value", std::string() + static_cast<char>(configuration.unmappedConnectUnconnectedInputsToConstantValue));
				groupSourceInfo.AddProperty<std::string>("constant-type", "unconnected-input");
				SourceInfoId groupSourceInfoId = AddSourceInfo(groupSourceInfo);

				const GroupId groupId = EmplaceGroup(groupName);
				Group& group = GetGroup(groupId);
				group.SetSourceInfo(groupSourceInfoId);
				GetGroup(topMostParentId).AddGroup(groupId);

				SourceInformation portSourceInfo { NO_SOURCE_FILE, _name, "out", "port" };
				portSourceInfo.AddProperty<std::string>("port-name", groupName + "/out");
				portSourceInfo.AddProperty<size_t>("port-size", 1u);
				portSourceInfo.AddProperty<size_t>("port-top", 0u);
				portSourceInfo.AddProperty<size_t>("port-bottom", 0u);
				portSourceInfo.AddProperty<std::string>("port-type", "output");
				SourceInfoId portSourceInfoId = AddSourceInfo(portSourceInfo);

				PortId portId = group.EmplacePort("out", PortType::Output);
				Port& port = group.GetPort(portId);
				port.SetSourceInfo(portSourceInfoId);
				port.SetSize({ 0u, 0u });
				port.SetName("out");
				port.SetConnections({ connectionId });

				SourceInformation wireSourceInfo { NO_SOURCE_FILE, _name, groupName, "wire" };
				wireSourceInfo.AddProperty<std::string>("wire-name", groupName);
				wireSourceInfo.AddProperty<size_t>("wire-size", 1u);
				wireSourceInfo.AddProperty<size_t>("wire-top", 0u);
				wireSourceInfo.AddProperty<size_t>("wire-bottom", 0u);
				SourceInfoId wireSourceInfoId = AddSourceInfo(wireSourceInfo);

				const WireId wireId = GetGroup(topMostParentId).EmplaceWire(groupName);
				Wire& wire = GetGroup(topMostParentId).GetWire(wireId);
				wire.SetSourceInfo(wireSourceInfoId);
				wire.SetSize({ 0u, 0u });
				wire.SetName(groupName);
				wire.SetConnections({ connectionId });

				UnmappedNodeId unconnectedNodeId = EmplaceUnmappedNode(nodeName, cellType, 0, 1);
				UnmappedNode& unconnectedNode = GetUnmappedNode(unconnectedNodeId);
				unconnectedNode.outputPortNames = { "Z" };
				unconnectedNode.outputConnectionNames = { "UNCONNECTED_INPUT" };
				unconnectedNode.outputConnectionIds = { connectionId };
				unconnectedNode.outputs[0u].reserve(nodes.size());
				unconnectedNode.group = groupId;
				group.unmappedNodeIds.push_back(unconnectedNodeId);

				for (auto& [nodeId, inputPin]: nodes)
				{
					_unmappedNodes[nodeId].inputs[inputPin] = unconnectedNodeId;
					_unmappedNodes[nodeId].inputConnectionNames[inputPin] = unconnectedNode.outputConnectionNames[0u];
					_unmappedNodes[nodeId].inputConnectionIds[inputPin] = unconnectedNode.outputConnectionIds[0u];
					unconnectedNode.outputs[0u].push_back(nodeId);
				}
			}
		}
		else if (configuration.unmappedDisallowUnconnectedInputs)
		{
			LOG(ERROR) << "Found unconnected input(s) in the unmapped circuit:";
			for (auto [nodeId, inputPin] : unmappedUnconnectedInputs)
			{
				LOG(ERROR) << "Unconnected input pin " << inputPin << " at gate " << _unmappedNodes[nodeId].name;
			}
			return std::unique_ptr<CircuitEnvironment>();
		}
		else
		{
			LOG(WARNING) << "Found unconnected input(s) in the unmapped circuit:";
			for (auto [nodeId, inputPin] : unmappedUnconnectedInputs)
			{
				LOG(INFO) << "Unconnected input pin " << inputPin << " at gate " << _unmappedNodes[nodeId].name;
			}
		}
	}

	// Identify cycles in the unmapped circuit
	const auto unmappedCycles = FindNodeCycles<UnmappedNode, UnmappedNodeId>(_unmappedNodes, _deletedUnmappedNodes);
	if (unmappedCycles.size() > 0u)
	{
		if (configuration.unmappedDisallowCycles)
		{
			LOG(ERROR) << "Found cycle(s) in the unmapped circuit:";
			for (auto& cycle : unmappedCycles)
			{
				LOG(ERROR) << "Cycle from " << _unmappedNodes[cycle.successorId].name << " to itself: " << CycleToString(cycle, _unmappedNodes);
			}
			return std::unique_ptr<CircuitEnvironment>();
		}
		else if (configuration.unmappedListCycles)
		{
			LOG(INFO) << "Found cycle(s) in the unmapped circuit:";
			for (auto cycle : unmappedCycles)
			{
				LOG(INFO) << "Cycle from " << _unmappedNodes[cycle.successorId].name << " to itself: " << CycleToString(cycle, _unmappedNodes);
			}
		}
	}

	// Create a topological sorting of the unmapped circuit
	std::vector<size_t> unmappedTopologicalSort = TopologicalSort(unmappedCycles, _unmappedNodes, _deletedUnmappedNodes);
	DVLOG(6) << "Unmapped Topological Sorting:";
	for (size_t unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
	{
		DVLOG(6) << "Unmapped Node " << _unmappedNodes[unmappedNodeId].name << " has tsort " << unmappedTopologicalSort[unmappedNodeId];
	}

	auto IsDeletedUnmappedNode = [this](UnmappedNodeId node) { return node == UNCONNECTED_UNMAPPED_PORT || _deletedUnmappedNodes[node]; };
	auto IsNotDeletedUnmappedNode = [this](UnmappedNodeId node) { return node != UNCONNECTED_UNMAPPED_PORT && !_deletedUnmappedNodes[node]; };

	// Count the number of nodes and pins first
	size_t unmappedNodeCount = 0u;
	size_t unmappedPinCount = 0u;
	for (UnmappedNodeId nodeId = 0u; nodeId < _unmappedNodes.size(); ++nodeId)
	{
		if (IsNotDeletedUnmappedNode(nodeId))
		{
			unmappedNodeCount++;
			unmappedPinCount += _unmappedNodes[nodeId].inputs.size();
			unmappedPinCount += _unmappedNodes[nodeId].outputs.size();
		}
	}

	size_t unmappedPrimaryInputCount = std::count_if(_unmappedPrimaryInputs.cbegin(), _unmappedPrimaryInputs.cend(), IsNotDeletedUnmappedNode);
	size_t unmappedPrimaryOutputCount = std::count_if(_unmappedPrimaryOutputs.cbegin(), _unmappedPrimaryOutputs.cend(), IsNotDeletedUnmappedNode);

	// Now create the unmapped circuit according to the calculated sizes
	unmappedCircuit._nodeContainer.resize(unmappedNodeCount, Circuit::UnmappedNode());
	unmappedCircuit._nodes.resize(unmappedNodeCount, nullptr);
	unmappedCircuit._primInputs.resize(unmappedPrimaryInputCount, nullptr);
	unmappedCircuit._primOutputs.resize(unmappedPrimaryOutputCount, nullptr);
	unmappedCircuit._pinIdToNodeIdAndPort.resize(unmappedPinCount, { std::numeric_limits<size_t>::max(), { Circuit::PortType::Input, 0u } });
	// Already initialized: unmappedCircuit._nodeNameToNodeId
	// Already initialized: unmappedCircuit._wireNameToNodeId
	// Already initialized: unmappedCircuit._primInputNrMap
	// Already initialized: unmappedCircuit._secInputNrMap
	// Already initialized: unmappedCircuit._nodeSourceInformation
	// Already initialized: unmappedCircuit._pinSourceInformation
	// Already initialized: unmappedCircuit._sourceInformation

	// Create all unmapped circuit nodes
	for (UnmappedNodeId unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
	{
		if (IsDeletedUnmappedNode(unmappedNodeId))
		{
			continue;
		}

		size_t tsort = unmappedTopologicalSort[unmappedNodeId];
		unmappedCircuit._nodes[tsort] = &(unmappedCircuit._nodeContainer[tsort]);

		UnmappedNode& unmappedNode = _unmappedNodes[unmappedNodeId];

		// General attributes
		auto& newNode = unmappedCircuit._nodeContainer[tsort];
		newNode.name = unmappedNode.name; // The name is updated at the end of this method
		newNode.typeName = unmappedNode.type;
		newNode.tsort = tsort;

		// Input pins
		newNode.numberOfIns = unmappedNode.inputs.size();
		newNode.in = new Circuit::UnmappedNode*[unmappedNode.inputs.size()];
		newNode.inData = new Circuit::UnmappedPinData[unmappedNode.inputs.size()];

		// Output pins
		newNode.numberOfOutputPins = unmappedNode.outputs.size();
		newNode.numberOfOuts = new size_t[unmappedNode.outputs.size()];
		newNode.out = new Circuit::UnmappedNode**[unmappedNode.outputs.size()];
		newNode.outData = new Circuit::UnmappedPinData[unmappedNode.outputs.size()];
	}

	// Check if the storage was assigned correctly
	for (size_t tsort = 0u; tsort < unmappedCircuit._nodeContainer.size(); ++tsort)
	{
		const auto& node = unmappedCircuit._nodeContainer[tsort];
		if (node.in == nullptr || node.inData == nullptr
			|| node.numberOfOuts == nullptr || node.out == nullptr || node.outData == nullptr)
		{
			Logging::Panic("List pointer in unmapped circuit node not assigned");
		}
	}

	// Connect all unmapped circuit nodes
	for (UnmappedNodeId unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
	{
		if (IsDeletedUnmappedNode(unmappedNodeId))
		{
			continue;
		}

		size_t tsort = unmappedTopologicalSort[unmappedNodeId];
		UnmappedNode& unmappedNode = _unmappedNodes[unmappedNodeId];

		auto& newNode = unmappedCircuit._nodeContainer[tsort];
		for (size_t input = 0; input < unmappedNode.inputs.size(); ++input)
		{
			UnmappedNodeId unmappedInputId = unmappedNode.inputs[input];
			if (IsNotDeletedUnmappedNode(unmappedInputId))
			{
				size_t inputTsort = unmappedTopologicalSort[unmappedInputId];
				newNode.in[input] = &(unmappedCircuit._nodeContainer[inputTsort]);
			}
			else if (configuration.unmappedDisallowUnconnectedInputs || configuration.unmappedConnectUnconnectedInputsToConstant)
			{
				Logging::Panic("There is an unconnected input even though the configuration does not allow unconnected inputs");
			}
			else
			{
				newNode.in[input] = nullptr;
			}
		}

		for (size_t output = 0u; output < unmappedNode.outputs.size(); ++output)
		{
			// Count the number of successor nodes first (Output pins can be unconnected or deleted)
			size_t successorCount = std::count_if(unmappedNode.outputs[output].cbegin(),
				unmappedNode.outputs[output].cend(), IsNotDeletedUnmappedNode);
			newNode.numberOfOuts[output] = successorCount;
			newNode.out[output] = new Circuit::UnmappedNode*[successorCount];

			// Now assign the successors for each output (Ignore unconnected or deleted successors)
			size_t successorIndex = 0u;
			for (size_t successor = 0u; successor < unmappedNode.outputs[output].size(); ++successor)
			{
				UnmappedPinId unmappedSuccessorId = unmappedNode.outputs[output][successor];
				if (IsNotDeletedUnmappedNode(unmappedSuccessorId))
				{
					size_t successorTsort = unmappedTopologicalSort[unmappedSuccessorId];
					newNode.out[output][successorIndex] = &(unmappedCircuit._nodeContainer[successorTsort]);
					successorIndex++;
				}
			}
			ASSERT(successorIndex == successorCount);
		}
	}

	// Assign the primary inputs
	size_t unmappedPrimaryInputIndex = 0u;
	for (UnmappedNodeId unmappedNodeId : _unmappedPrimaryInputs)
	{
		if (IsNotDeletedUnmappedNode(unmappedNodeId))
		{
			size_t inputTsort = unmappedTopologicalSort[unmappedNodeId];
			unmappedCircuit._primInputs[unmappedPrimaryInputIndex] = &(unmappedCircuit._nodeContainer[inputTsort]);
			unmappedCircuit._primInputNrMap.emplace(inputTsort, unmappedPrimaryInputIndex);
			unmappedPrimaryInputIndex++;
		}
	}

	// Assign the primary outputs
	size_t unmappedPrimaryOutputIndex = 0u;
	for (UnmappedNodeId unmappedNodeId : _unmappedPrimaryOutputs)
	{
		if (IsNotDeletedUnmappedNode(unmappedNodeId))
		{
			size_t outputTsort = unmappedTopologicalSort[unmappedNodeId];
			unmappedCircuit._primOutputs[unmappedPrimaryOutputIndex] = &(unmappedCircuit._nodeContainer[outputTsort]);
			unmappedCircuit._primOutputNrMap.emplace(outputTsort, unmappedPrimaryOutputIndex);
			unmappedPrimaryOutputIndex++;
		}
	}

	// Create the pinsort for inputs and outputs
	size_t pinId = 0u;
	for (UnmappedNodeId unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
	{
		if (IsDeletedUnmappedNode(unmappedNodeId))
		{
			continue;
		}

		size_t tsort = unmappedTopologicalSort[unmappedNodeId];
		UnmappedNode& unmappedNode = _unmappedNodes[unmappedNodeId];

		auto& node = unmappedCircuit._nodeContainer[tsort];
		for (size_t output = 0u; output < node.numberOfOutputPins; output++)
		{
			node.outData[output].PinSort = pinId;
			node.outData[output].PinName = unmappedNode.outputPortNames[output];
			node.outData[output].PinWireId = UnmappedCircuit::NO_CONNECTION; // Replaced later
			node.outData[output].PinWireName = unmappedNode.outputConnectionNames[output];
			unmappedCircuit._pinIdToNodeIdAndPort[pinId] = { tsort, { Circuit::PortType::Output, output } };

			// Associate the wire name with the driving gate.
			unmappedCircuit._wireNameToNodeId.insert(unmappedNode.outputConnectionNames[output], { tsort, output });
			pinId++;
		}

		for (size_t input = 0u; input < node.numberOfIns; input++)
		{
			node.inData[input].PinSort = pinId;
			node.inData[input].PinName = unmappedNode.inputPortNames[input];
			node.inData[input].PinWireId = UnmappedCircuit::NO_CONNECTION; // Replaced later
			node.inData[input].PinWireName = unmappedNode.inputConnectionNames[input];
			unmappedCircuit._pinIdToNodeIdAndPort[pinId] = { tsort, { Circuit::PortType::Input, input } };
			pinId++;
		}
	}

	// ------------------------------------------------------------------------
	// Mapped Circuit
	// ------------------------------------------------------------------------

	// Identify unconnected inputs in the mapped circuit
	const auto mappedUnconnectedInputs = FindUnconnectedInputs<MappedNode, MappedNodeId>(_mappedNodes, _deletedMappedNodes);
	if (mappedUnconnectedInputs.size() > 0u)
	{
		if (configuration.mappedConnectUnconnectedInputsToConstant)
		{
			LOG(WARNING) << "Found unconnected input(s) in the mapped circuit:";
			for (auto [nodeId, inputPin] : mappedUnconnectedInputs)
			{
				LOG(INFO) << "Unconnected input pin " << inputPin << " at gate " << _mappedNodes[nodeId].name;
			}

			const CellType cellType = [](Logic value) -> CellType {
				switch (value)
				{
					case Logic::LOGIC_ZERO:
						return CellType::PRESET_0;
					case Logic::LOGIC_ONE:
						return CellType::PRESET_1;
					case Logic::LOGIC_DONT_CARE:
						return CellType::PRESET_X;
					case Logic::LOGIC_UNKNOWN:
						return CellType::PRESET_U;
					default:
						LOG(FATAL) << "Unknown logic type " << value;
						return CellType::UNDEFTYPE;
				}
			}(configuration.mappedConnectUnconnectedInputsToConstantValue);

			LOG(INFO) << "Creating constant inputs UNCONNECTED_INPUT_CONSTANT of type "
				<< configuration.mappedConnectUnconnectedInputsToConstantValue << " for unconnected inputs";

			std::map<size_t, std::vector<std::pair<size_t, size_t>>> connectionIds;
			for (auto& [nodeId, inputPin]: mappedUnconnectedInputs)
			{
				connectionIds[_mappedNodes[nodeId].inputConnectionIds[inputPin]]
					.push_back(std::make_pair(nodeId, inputPin));
			}

			size_t index { 0u };
			for (auto& [connectionId, nodes] : connectionIds)
			{
				// NOTE: Do store the parent group only as id and not reference as a reallocation might ocurr
				ASSERT(nodes.size() != 0u) << "No nodes for the connection ID have been found";
				auto const& [nodeId, inputPin] = nodes[0];
				auto const topMostParentId = find_topmost_parent_with_connection(connectionId, _mappedNodes[nodeId].GetGroup());

				const auto groupName = boost::str(boost::format("unconnected-mapped-%d") % index);
				const auto nodeName = boost::str(boost::format("UNCONNECTED_MAPPED_INPUT_CONSTANT_%d") % index++);

				SourceInformation groupSourceInfo { NO_SOURCE_FILE, _name, groupName, "constant" };
				groupSourceInfo.AddProperty<std::string>("constant-value", std::string() + static_cast<char>(configuration.mappedConnectUnconnectedInputsToConstantValue));
				groupSourceInfo.AddProperty<std::string>("constant-type", "unconnected-input");
				SourceInfoId groupSourceInfoId = AddSourceInfo(groupSourceInfo);

				GroupId groupId = EmplaceGroup(boost::str(boost::format("unconnected-mapped-%d") % index));
				Group& group = GetGroup(groupId);
				group.SetSourceInfo(groupSourceInfoId);
				GetGroup(topMostParentId).AddGroup(groupId);

				SourceInformation portSourceInfo { NO_SOURCE_FILE, _name, "out", "port" };
				portSourceInfo.AddProperty<std::string>("port-name", groupName + "/out");
				portSourceInfo.AddProperty<size_t>("port-size", 1u);
				portSourceInfo.AddProperty<size_t>("port-top", 0u);
				portSourceInfo.AddProperty<size_t>("port-bottom", 0u);
				portSourceInfo.AddProperty<std::string>("port-type", "output");
				SourceInfoId portSourceInfoId = AddSourceInfo(portSourceInfo);

				PortId portId = group.EmplacePort("out", PortType::Output);
				Port& port = group.GetPort(portId);
				port.SetSourceInfo(portSourceInfoId);
				port.SetSize({ 0u, 0u });
				port.SetName("out");
				port.SetConnections({ connectionId });

				SourceInformation wireSourceInfo { NO_SOURCE_FILE, _name, groupName, "wire" };
				wireSourceInfo.AddProperty<std::string>("wire-name", groupName);
				wireSourceInfo.AddProperty<size_t>("wire-size", 1u);
				wireSourceInfo.AddProperty<size_t>("wire-top", 0u);
				wireSourceInfo.AddProperty<size_t>("wire-bottom", 0u);
				SourceInfoId wireSourceInfoId = AddSourceInfo(wireSourceInfo);

				const WireId wireId = GetGroup(topMostParentId).EmplaceWire(groupName);
				Wire& wire = GetGroup(topMostParentId).GetWire(wireId);
				wire.SetSourceInfo(wireSourceInfoId);
				wire.SetSize({ 0u, 0u });
				wire.SetName(groupName);
				wire.SetConnections({ connectionId });

				MappedNodeId unconnectedNodeId = EmplaceMappedNode(nodeName, CellCategory::MAIN_CONSTANT, cellType, 0);
				MappedNode& unconnectedNode = GetMappedNode(unconnectedNodeId);
				unconnectedNode.outputPortName = "Z";
				unconnectedNode.outputConnectionName = "UNCONNECTED_INPUT";
				unconnectedNode.outputConnectionId = connectionId;
				unconnectedNode.successors.reserve(nodes.size());
				unconnectedNode.group = groupId;
				group.mappedNodeIds.push_back(unconnectedNodeId);

				for (auto& [nodeId, inputPin]: nodes)
				{
					_mappedNodes[nodeId].inputs[inputPin] = unconnectedNodeId;
					_mappedNodes[nodeId].inputConnectionNames[inputPin] = unconnectedNode.outputConnectionName;
					_mappedNodes[nodeId].inputConnectionIds[inputPin] = unconnectedNode.outputConnectionId;
					unconnectedNode.successors.push_back(nodeId);
				}
			}
		}
		else if (configuration.mappedDisallowUnconnectedInputs)
		{
			LOG(ERROR) << "Found unconnected input(s) in the mapped circuit:";
			for (auto [nodeId, inputPin] : mappedUnconnectedInputs)
			{
				LOG(ERROR) << "Unconnected input pin " << inputPin << " at gate " << _mappedNodes[nodeId].name;
			}
			return std::unique_ptr<CircuitEnvironment>();
		}
		else
		{
			LOG(WARNING) << "Found unconnected input(s) in the mapped circuit:";
			for (auto [nodeId, inputPin] : mappedUnconnectedInputs)
			{
				LOG(INFO) << "Unconnected input pin " << inputPin << " at gate " << _mappedNodes[nodeId].name;
			}
		}
	}

	// Identify cycles in the mapped circuit
	const auto mappedCycles = FindNodeCycles<MappedNode, MappedNodeId>(_mappedNodes, _deletedMappedNodes);
	if (mappedCycles.size() > 0u)
	{
		if (configuration.mappedBreakCombinatorialCycles)
		{
			LOG(INFO) << "Found cycle(s) in the mapped circuit:";
			for (auto& cycle : mappedCycles)
			{
				LOG(INFO) << "Breaking cycle from " << _mappedNodes[cycle.successorId].name << " to itself: " << CycleToString(cycle, _mappedNodes);
			}

			BreakCycles(mappedCycles, _mappedNodes, _deletedMappedNodes);
		}
		else if (configuration.mappedDisallowCycles)
		{
			LOG(ERROR) << "Found cycle(s) in the mapped circuit:";
			for (auto& cycle : mappedCycles)
			{
				LOG(ERROR) << "Cycle from " << _mappedNodes[cycle.successorId].name << " to itself: " << CycleToString(cycle, _mappedNodes);
			}
			return std::unique_ptr<CircuitEnvironment>();
		}
		else if (configuration.mappedListCycles)
		{
			LOG(INFO) << "Found cycle(s) in the mapped circuit:";
			for (auto cycle : mappedCycles)
			{
				LOG(INFO) << "Cycle from " << _mappedNodes[cycle.successorId].name << " to itself: " << CycleToString(cycle, _mappedNodes);
			}
		}
	}

	// Create a topological sorting of the mapped circuit
	std::vector<size_t> mappedTopologicalSort = TopologicalSort(mappedCycles, _mappedNodes, _deletedMappedNodes);
	DVLOG(6) << "Mmapped Topological Sorting:";
	for (size_t mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
	{
		DVLOG(6) << "Mapped Node " << _mappedNodes[mappedNodeId].name << " has tsort " << mappedTopologicalSort[mappedNodeId];
	}

	auto IsDeletedMappedNode = [this](MappedNodeId node) { return node == UNCONNECTED_MAPPED_PORT || _deletedMappedNodes[node]; };
	auto IsNotDeletedMappedNode = [this](MappedNodeId node) { return node != UNCONNECTED_MAPPED_PORT && !_deletedMappedNodes[node]; };

	// Count the number of nodes and pins first
	size_t mappedNodeCount = 0u;
	size_t mappedPinCount = 0u;
	size_t mappedInputCount = 0u;
	size_t mappedOutputCount = 0u;
	std::vector<size_t> successorCount;
	for (MappedNodeId nodeId = 0u; nodeId < _mappedNodes.size(); ++nodeId)
	{
		if (IsNotDeletedMappedNode(nodeId))
		{
			mappedNodeCount++;
			mappedPinCount += _mappedNodes[nodeId].inputs.size();
			mappedPinCount++; // There is always only one output

			// Required size for list of inputs / successors
			// The +1 might be removed in the future
			mappedInputCount += _mappedNodes[nodeId].inputs.size();
			mappedOutputCount += std::count_if(_mappedNodes[nodeId].successors.cbegin(),
				_mappedNodes[nodeId].successors.cend(), IsNotDeletedMappedNode);
		}
	}

	size_t mappedPrimaryInputCount = std::count_if(_mappedPrimaryInputs.cbegin(), _mappedPrimaryInputs.cend(), IsNotDeletedMappedNode);
	size_t mappedPrimaryOutputCount = std::count_if(_mappedPrimaryOutputs.cbegin(), _mappedPrimaryOutputs.cend(), IsNotDeletedMappedNode);
	size_t mappedSecondaryInputCount = std::count_if(_mappedSecondaryInputs.cbegin(), _mappedSecondaryInputs.cend(), IsNotDeletedMappedNode);
	size_t mappedSecondaryOutputCount = std::count_if(_mappedSecondaryOutputs.cbegin(), _mappedSecondaryOutputs.cend(), IsNotDeletedMappedNode);

	mappedCircuit._nodeContainer.resize(mappedNodeCount, Circuit::MappedNode());
	mappedCircuit._nodes.resize(mappedNodeCount, nullptr);
	mappedCircuit._primInputs.resize(mappedPrimaryInputCount, nullptr);
	mappedCircuit._primOutputs.resize(mappedPrimaryOutputCount, nullptr);
	mappedCircuit._secInputs.resize(mappedSecondaryInputCount, nullptr);
	mappedCircuit._secOutputs.resize(mappedSecondaryOutputCount, nullptr);
	mappedCircuit._inputs.resize(mappedPrimaryInputCount + mappedSecondaryInputCount, nullptr);
	mappedCircuit._outputs.resize(mappedPrimaryOutputCount + mappedSecondaryOutputCount, nullptr);
	mappedCircuit._pinIdToNodeIdAndPort.resize(mappedPinCount, { std::numeric_limits<size_t>::max(), { Circuit::PortType::Input, 0u } });
	// Already initialized: mappedCircuit._nodeNameToNodeId
	// Already initialized: mappedCircuit._wireNameToNodeId
	// Already initialized: mappedCircuit._sinToSoutMap
	// Already initialized: mappedCircuit._soutToSinMap
	// Already initialized: mappedCircuit._primInputNrMap
	// Already initialized: mappedCircuit._secInputNrMap
	// Already initialized: mappedCircuit._inputNrMap
	// Already initialized: mappedCircuit._primOutputNrMap
	// Already initialized: mappedCircuit._secOutputNrMap
	// Already initialized: mappedCircuit._outputNrMap
	mappedCircuit._nodePinData.resize(mappedInputCount);
	mappedCircuit._nodeConnectionData.resize(mappedInputCount);
	mappedCircuit._nodeIn.resize(mappedInputCount);
	mappedCircuit._nodeOut.resize(mappedOutputCount);
	mappedCircuit._nodeSuccessorInData.resize(mappedOutputCount, -1);

	// Create all mapped circuit nodes
	size_t inputIndex = 0u;
	size_t outputIndex = 0u;
	for (MappedNodeId mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
	{
		if (IsDeletedMappedNode(mappedNodeId))
		{
			continue;
		}

		size_t tsort = mappedTopologicalSort[mappedNodeId];
		mappedCircuit._nodes[tsort] = &(mappedCircuit._nodeContainer[tsort]);

		MappedNode& mappedNode = _mappedNodes[mappedNodeId];

		// General attributes
		auto& newNode = mappedCircuit._nodeContainer[tsort];
		newNode.tsort = tsort;
		newNode.name = mappedNode.name;  // The name is updated at the end of this method
		newNode.signalName = mappedNode.outputConnectionName;
		newNode.mainType = mappedNode.cellCategory;
		newNode.type = mappedNode.cellType;

		// Input / output pins
		newNode.numberOfIns = mappedNode.inputs.size();
		newNode.numberOfOuts = std::count_if(_mappedNodes[mappedNodeId].successors.cbegin(),
				_mappedNodes[mappedNodeId].successors.cend(), IsNotDeletedMappedNode);

		// Use one array for all input / output pointer
		newNode.in = &mappedCircuit._nodeIn[inputIndex];
		newNode.out = &mappedCircuit._nodeOut[outputIndex];
		newNode.successorInDataID = &mappedCircuit._nodeSuccessorInData[outputIndex];
		inputIndex += newNode.numberOfIns;
		outputIndex += newNode.numberOfOuts;
	}

	// Check if the storage was assigned correctly
	for (size_t tsort = 0u; tsort < mappedCircuit._nodeContainer.size(); ++tsort)
	{
		auto& node = mappedCircuit._nodeContainer[tsort];
		if (node.in == nullptr || node.out == nullptr || node.successorInDataID == nullptr)
		{
			Logging::Panic("List pointer in mapped circuit node not assigned");
		}
	}

	// Connect all mapped circuit nodes
	for (MappedNodeId mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
	{
		if (IsDeletedMappedNode(mappedNodeId))
		{
			continue;
		}

		size_t tsort = mappedTopologicalSort[mappedNodeId];
		MappedNode& mappedNode = _mappedNodes[mappedNodeId];

		auto& newNode = mappedCircuit._nodeContainer[tsort];
		for (size_t input = 0; input < mappedNode.inputs.size(); ++input)
		{
			MappedNodeId mappedInputId = mappedNode.inputs[input];
			if (IsNotDeletedMappedNode(mappedInputId))
			{
				size_t inputTsort = mappedTopologicalSort[mappedInputId];
				newNode.in[input] = &(mappedCircuit._nodeContainer[inputTsort]);
			}
			else if (configuration.mappedDisallowUnconnectedInputs || configuration.mappedConnectUnconnectedInputsToConstant)
			{
				Logging::Panic("There is an unconnected input even though the configuration does not allow unconnected inputs");
			}
			else
			{
				newNode.in[input] = nullptr;
			}
		}

		size_t successorIndex = 0u;
		for (size_t successor = 0u; successor < mappedNode.successors.size(); ++successor)
		{
			MappedNodeId mappedSuccessorId = mappedNode.successors[successor];
			if (IsNotDeletedMappedNode(mappedSuccessorId))
			{
				size_t successorTsort = mappedTopologicalSort[mappedSuccessorId];
				newNode.out[successorIndex] = &(mappedCircuit._nodeContainer[successorTsort]);
				successorIndex++;
			}
		}
		ASSERT(successorIndex == newNode.numberOfOuts);
	}

	// Create the pinsort for inputs and outputs
	size_t mappedPinId = 0u;
	size_t mappedInputId = 0u;
	for (MappedNodeId mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
	{
		if (IsDeletedMappedNode(mappedNodeId))
		{
			continue;
		}

		size_t tsort = mappedTopologicalSort[mappedNodeId];
		auto& node = mappedCircuit._nodeContainer[tsort];

		node.outDataID = mappedPinId;
		node.outConnectionID = MappedCircuit::NO_CONNECTION; // Replaced later
		mappedCircuit._pinIdToNodeIdAndPort[mappedPinId] = { node.tsort, { Circuit::PortType::Output, 0u } };
		mappedPinId++;

		node.inDataID = &mappedCircuit._nodePinData[mappedInputId];
		node.inConnectionID = &mappedCircuit._nodeConnectionData[mappedInputId];
		for (size_t input = 0u; input < node.numberOfIns; ++input)
		{
			node.inDataID[input] = mappedPinId;
			node.inConnectionID[input] = MappedCircuit::NO_CONNECTION; // Replaced later
			mappedCircuit._pinIdToNodeIdAndPort[mappedPinId] = { node.tsort, { Circuit::PortType::Input, input } };
			mappedPinId++;
			mappedInputId++;
		}
	}

	// Add successor input-pin index data for current node.
	for (size_t tsort = 0u; tsort < mappedCircuit._nodeContainer.size(); ++tsort)
	{
		auto* node = &(mappedCircuit._nodeContainer[tsort]);

		// Each node has only one output pin so we can infer the connections of the successor nodes.
		// However, if we have multiple connections between this node and a successor
		// we have to count the number of occurrences of connections
		// to assign all inputs of the successor node (and not only the first one).
		//
		// The resulting successorInDataID should look like:
		// successorInDataID = [0, 1]
		//
		//  +-----+             +-----+
		//  |     |      +------+     |
		//  |  A  +------+      |  B  |
		//  |     |      +------+     |
		//  +-----+             +-----+
		//
		// If we naively do a break after the first match the result would look like:
		// successorInDataID = [0, 0] (one pin connected twice)
		//
		//  +-----+             +-----+
		//  |     |  +------+---+     |
		//  |  A  +--+      |   |  B  |
		//  |     |  +------+   +     |
		//  +-----+             +-----+

		// map<successor tsort, connection count>
		std::map<size_t, size_t> connectionOccurrences;
		for (size_t successor = 0u; successor < node->numberOfOuts; ++successor)
		{
			connectionOccurrences.emplace(node->out[successor]->tsort, 0u);
		}

		for (size_t successor = 0u; successor < node->numberOfOuts; ++successor)
		{
			const auto* successorNode = node->out[successor];

			size_t connectionOccurrence = 0u;
			for (size_t input = 0u; input < successorNode->GetNumberOfInputs(); ++input)
			{
				if (successorNode->in[input] == node)
				{
					// Ignore the previous occurrences to not re-assign an already
					// assigned index that is connected to the same parent node.
					if (connectionOccurrences[successorNode->tsort] == connectionOccurrence)
					{
						node->successorInDataID[successor] = input;
						break;
					}
					connectionOccurrence++;
				}
			}
		}
	}

	// Assign the primary inputs
	size_t mappedPrimaryInputIndex = 0u;
	size_t mappedInputIndex = 0u;
	for (MappedNodeId mappedNodeId : _mappedPrimaryInputs)
	{
		if (IsNotDeletedMappedNode(mappedNodeId))
		{
			size_t inputTsort = mappedTopologicalSort[mappedNodeId];
			auto* inputNode = &(mappedCircuit._nodeContainer[inputTsort]);

			mappedCircuit._primInputs[mappedPrimaryInputIndex] = inputNode;
			mappedCircuit._primInputNrMap.emplace(inputNode->tsort, mappedPrimaryInputIndex);
			mappedPrimaryInputIndex++;

			mappedCircuit._inputs[mappedInputIndex] = inputNode;
			mappedCircuit._inputNrMap.emplace(inputNode->tsort, mappedInputIndex);
			mappedInputIndex++;
		}
	}

	// Assign the primary outputs
	size_t mappedPrimaryOutputIndex = 0u;
	size_t mappedOutputIndex = 0u;
	for (MappedNodeId mappedNodeId : _mappedPrimaryOutputs)
	{
		if (IsNotDeletedMappedNode(mappedNodeId))
		{
			size_t outputTsort = mappedTopologicalSort[mappedNodeId];
			auto* outputNode = &(mappedCircuit._nodeContainer[outputTsort]);

			mappedCircuit._primOutputs[mappedPrimaryOutputIndex] = outputNode;
			mappedCircuit._primOutputNrMap.emplace(outputNode->tsort, mappedPrimaryOutputIndex);
			mappedPrimaryOutputIndex++;

			mappedCircuit._outputs[mappedOutputIndex] = outputNode;
			mappedCircuit._outputNrMap.emplace(outputNode->tsort, mappedOutputIndex);
			mappedOutputIndex++;
		}
	}

	// Assign the secondary inputs
	size_t mappedSecondaryInputIndex = 0u;
	for (MappedNodeId mappedNodeId : _mappedSecondaryInputs)
	{
		if (IsNotDeletedMappedNode(mappedNodeId))
		{
			size_t inputTsort = mappedTopologicalSort[mappedNodeId];
			auto* inputNode = &(mappedCircuit._nodeContainer[inputTsort]);

			mappedCircuit._secInputs[mappedSecondaryInputIndex] = inputNode;
			mappedCircuit._secInputNrMap.emplace(inputNode->tsort, mappedSecondaryInputIndex);
			mappedSecondaryInputIndex++;

			mappedCircuit._inputs[mappedInputIndex] = inputNode;
			mappedCircuit._inputNrMap.emplace(inputNode->tsort, mappedInputIndex);
			mappedInputIndex++;
		}
	}

	// Assign the secondary outputs
	size_t mappedSecondaryOutputIndex = 0u;
	for (MappedNodeId mappedNodeId : _mappedSecondaryOutputs)
	{
		if (IsNotDeletedMappedNode(mappedNodeId))
		{
			size_t outputTsort = mappedTopologicalSort[mappedNodeId];
			auto* outputNode = &(mappedCircuit._nodeContainer[outputTsort]);

			mappedCircuit._secOutputs[mappedSecondaryOutputIndex] = outputNode;
			mappedCircuit._secOutputNrMap.emplace(outputNode->tsort, mappedSecondaryOutputIndex);
			mappedSecondaryOutputIndex++;

			mappedCircuit._outputs[mappedOutputIndex] = outputNode;
			mappedCircuit._outputNrMap.emplace(outputNode->tsort, mappedOutputIndex);
			mappedOutputIndex++;
		}
	}

	// Assign secondary input / output mappings
	for (auto&[inputNodeId, outputNodeId] : _secondaryInputToOutput)
	{
		if (IsNotDeletedMappedNode(inputNodeId) && IsNotDeletedMappedNode(outputNodeId))
		{
			size_t inputTsort = mappedTopologicalSort[inputNodeId];
			size_t outputTsort = mappedTopologicalSort[outputNodeId];
			auto* inputNode = &(mappedCircuit._nodeContainer[inputTsort]);
			auto* outputNode = &(mappedCircuit._nodeContainer[outputTsort]);

			mappedCircuit._sinToSoutMap.emplace(inputNode->tsort, outputNode);
			mappedCircuit._soutToSinMap.emplace(outputNode->tsort, inputNode);
		}
	}

	// ------------------------------------------------------------------------
	// Mappings between Unmapped and Mapped Circuit
	// ------------------------------------------------------------------------

	// Resize the mapping in the circuit environment accordingly
	environment->_mappedToUnmappedTSorts.resize(mappedNodeCount, NO_UNMAPPED_NODE);
	environment->_mappedToUnmappedPSorts.resize(mappedPinCount, NO_UNMAPPED_PIN);

	environment->_unmappedToMappedTSorts.resize(unmappedNodeCount);
	environment->_unmappedToMappedPSorts.resize(unmappedPinCount);

	// Apply the tsort mappings
	for (auto& [mappedNodeId, unmappedNodeId] : _mappedToUnmappedNodes)
	{
		if (IsNotDeletedMappedNode(mappedNodeId) && IsNotDeletedUnmappedNode(unmappedNodeId))
		{
			size_t mappedTsort = mappedTopologicalSort[mappedNodeId];
			size_t unmappedTsort = unmappedTopologicalSort[unmappedNodeId];

			environment->_mappedToUnmappedTSorts[mappedTsort] = unmappedTsort;
			environment->_unmappedToMappedTSorts[unmappedTsort].push_back(mappedTsort);
		}
	}

	// Apply the psort mappings
	for (const auto& [mappedPinData, unmappedPinData] : _mappedToUnmappedPins)
	{
		const auto& [mappedNodeId, mappedPinId] = mappedPinData;
		const auto& [unmappedNodeId, unmappedPinId] = unmappedPinData;

		if (IsNotDeletedMappedNode(mappedNodeId) && IsNotDeletedUnmappedNode(unmappedNodeId))
		{
			size_t mappedTsort = mappedTopologicalSort[mappedNodeId];
			size_t unmappedTsort = unmappedTopologicalSort[unmappedNodeId];

			auto& unmappedNode = unmappedCircuit._nodeContainer[unmappedTsort];
			auto& mappedNode = mappedCircuit._nodeContainer[mappedTsort];

			ASSERT(mappedPinId < static_cast<int64_t>(mappedNode.numberOfIns + 1u));
			ASSERT(unmappedPinId >= -static_cast<int64_t>(unmappedNode.numberOfOutputPins));
			ASSERT(unmappedPinId < static_cast<int64_t>(unmappedNode.numberOfIns));

			size_t unmappedPSort = (unmappedPinId < 0)
				? unmappedNode.outData[static_cast<size_t>(-(unmappedPinId + 1))].PinSort
				: unmappedNode.inData[static_cast<size_t>(unmappedPinId)].PinSort;
			size_t mappedPSort = (mappedPinId == 0)
				? mappedNode.outDataID
				: mappedNode.inDataID[static_cast<size_t>(mappedPinId - 1)];

			environment->_mappedToUnmappedPSorts[mappedPSort] = unmappedPSort;
			environment->_unmappedToMappedPSorts[unmappedPSort].push_back(mappedPSort);
		}
	}

	// ------------------------------------------------------------------------
	// Create the Circuit Hierarchy
	// ------------------------------------------------------------------------

	// Find deleted groups that have no nodes in them
	std::vector<bool> deletedGroups(_groups.size(), false);
	auto RemoveEmptyGroups = [&](void) -> void {
		std::vector<GroupId> parentGroups(_groups.size(), std::numeric_limits<GroupId>::max());
		std::vector<GroupId> leaveGroups;
		for (GroupId groupId = 0u; groupId < _groups.size(); ++groupId)
		{
			if (_groups[groupId].groupIds.empty())
			{
				leaveGroups.push_back(groupId);
				continue;
			}

			for (auto& subGroup : _groups[groupId].groupIds)
			{
				parentGroups[subGroup] = groupId;
			}
		}

		// Traverse graph from childs to root and eliminate groups that have no children.
		while (!leaveGroups.empty())
		{
			std::vector<GroupId> newLeaves;
			while (!leaveGroups.empty())
			{
				GroupId leaveId = leaveGroups.back();
				Group& leave = _groups[leaveId];
				leaveGroups.pop_back();

				// A leave group is empty if it has no nodes in it.
				bool hasNodes = false;
				for (auto unmappedNode : leave.unmappedNodeIds)
				{
					if (!IsDeletedUnmappedNode(unmappedNode))
					{
						hasNodes = true;
						break;
					}
				}
				for (auto mappedNode : leave.mappedNodeIds)
				{
					if (!IsDeletedMappedNode(mappedNode))
					{
						hasNodes = true;
						break;
					}
				}
				if (hasNodes)
				{
					continue;
				}

				if (parentGroups[leaveId] == std::numeric_limits<GroupId>::max())
				{
					// This node has no parent and as the root.
					// => Don't remove this node.
					continue;
				}

				// Remove this group from the group list and from its parent
				deletedGroups[leaveId] = true;
				auto& parentsChildren = _groups[parentGroups[leaveId]].groupIds;
				parentsChildren.erase(std::find(parentsChildren.begin(), parentsChildren.end(), leaveId));

				if (parentsChildren.empty())
				{
					newLeaves.push_back(parentGroups[leaveId]);
				}
			}

			// Now check if the new leaves are empty.
			leaveGroups = newLeaves;
		}
	};
	RemoveEmptyGroups();

	auto IsDeletedGroup = [&deletedGroups](GroupId group) { return deletedGroups[group]; };
	auto IsNotDeletedGroup = [&deletedGroups](GroupId group) { return !deletedGroups[group]; };

	std::vector<bool> deletedConnections(_connections.size(), true);
	auto DeleteUnconnectedConnections = [this, &deletedConnections, &IsDeletedGroup, &IsDeletedUnmappedNode, &IsDeletedMappedNode](void) {
		// Connections to ports are never considered deleted
		for (size_t groupId = 0u; groupId < _groups.size(); ++groupId)
		{
			if (IsDeletedGroup(groupId))
			{
				continue;
			}

			for (auto& port : _groups[groupId].ports)
			{
				for (auto& connectionId : port.connections)
				{
					if (connectionId != UNCONNECTED_CONNECTION)
					{
						deletedConnections[connectionId] = false;
					}
				}
			}

			for (auto& wire : _groups[groupId].wires)
			{
				for (auto& connectionId : wire.connections)
				{
					if (connectionId != UNCONNECTED_CONNECTION)
					{
						deletedConnections[connectionId] = false;
					}
				}
			}
		}

		// Connections to unmapped nodes are not deleted
		for (size_t unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
		{
			if (IsDeletedUnmappedNode(unmappedNodeId))
			{
				continue;
			}

			for (auto& connectionId : _unmappedNodes[unmappedNodeId].inputConnectionIds)
			{
				if (connectionId != UNCONNECTED_CONNECTION)
				{
					deletedConnections[connectionId] = false;
				}
			}
			for (auto& connectionId : _unmappedNodes[unmappedNodeId].outputConnectionIds)
			{
				if (connectionId != UNCONNECTED_CONNECTION)
				{
					deletedConnections[connectionId] = false;
				}
			}
		}

		// Connections to mapped nodes are not deleted
		for (size_t mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
		{
			if (IsDeletedMappedNode(mappedNodeId))
			{
				continue;
			}

			for (auto& connectionId : _mappedNodes[mappedNodeId].inputConnectionIds)
			{
				if (connectionId != UNCONNECTED_CONNECTION)
				{
					deletedConnections[connectionId] = false;
				}
			}
			if (auto& connectionId = _mappedNodes[mappedNodeId].outputConnectionId;
				connectionId != UNCONNECTED_CONNECTION)
			{
				deletedConnections[connectionId] = false;
			}
		}
	};
	DeleteUnconnectedConnections();

	auto IsDeletedConnection = [&deletedConnections](ConnectionId connection) { return connection == UNCONNECTED_CONNECTION || deletedConnections[connection]; };
	auto IsNotDeletedConnection = [&deletedConnections](ConnectionId connection) { return connection != UNCONNECTED_CONNECTION && !deletedConnections[connection]; };

	std::vector<size_t> groupMapping(_groups.size(), std::numeric_limits<size_t>::max());
	std::vector<size_t> connectionMapping(_connections.size(), std::numeric_limits<size_t>::max());

	size_t groupCount = 0u;
	size_t connectionCount = 0u;
	for (size_t groupId = 0u; groupId < _groups.size(); ++groupId)
	{
		if (IsNotDeletedGroup(groupId))
		{
			groupCount++;
		}
	}
	for (size_t connectionId = 0u; connectionId < _connections.size(); ++connectionId)
	{
		if (IsNotDeletedConnection(connectionId))
		{
			connectionCount++;
		}
	}
	metaData.groups.reserve(groupCount);
	metaData.connections.reserve(connectionCount);
	metaData.unmappedNodeGroups.resize(unmappedCircuit._nodeContainer.size(), nullptr);
	metaData.mappedNodeGroups.resize(mappedCircuit._nodeContainer.size(), nullptr);

	// Create non-deleted groups
	for (GroupId groupId = 0u; groupId < _groups.size(); ++groupId)
	{
		if (IsDeletedGroup(groupId))
		{
			continue;
		}

		groupMapping[groupId] = metaData.groups.size();
		metaData.groups.emplace_back(_groups[groupId].name);

		// metaData.groups has been pre-allocated, so taking the address is possible.
		metaData.allGroups.emplace_back(&metaData.groups.back());
	}

	// Create non-deleted wires
	for (ConnectionId connectionId = 0u; connectionId < _connections.size(); ++connectionId)
	{
		if (IsDeletedConnection(connectionId))
		{
			continue;
		}

		connectionMapping[connectionId] = metaData.connections.size();
		metaData.connections.emplace_back(connectionMapping[connectionId]);

		// metaData.connections has been pre-allocated, so taking the address is possible.
		metaData.allConnections.emplace_back(&metaData.connections.back());
	}

	for (GroupId groupId = 0u; groupId < _groups.size(); ++groupId)
	{
		if (IsDeletedGroup(groupId))
		{
			continue;
		}

		auto& newGroup = metaData.groups[groupMapping[groupId]];
		if (INVALID_SOURCE_INFO != _groups[groupId].sourceInfo)
		{
			newGroup.sourceInfo = _sourceInformation[_groups[groupId].sourceInfo];
		}

		// Propagate the subgroup information
		for (const auto& subGroupId : _groups[groupId].groupIds)
		{
			if (IsDeletedGroup(subGroupId))
			{
				continue;
			}

			newGroup.subGroups.push_back(&metaData.groups[groupMapping[subGroupId]]);
			metaData.groups[groupMapping[subGroupId]].parent = &metaData.groups[groupMapping[groupId]];
		}

		// Propagate the unmapped node information
		for (const auto& unmappedNodeId : _groups[groupId].unmappedNodeIds)
		{
			if (IsDeletedUnmappedNode(unmappedNodeId))
			{
				continue;
			}

			const auto tsort = unmappedTopologicalSort[unmappedNodeId];
			auto* node = &(unmappedCircuit._nodeContainer[tsort]);
			newGroup.unmappedNodes.push_back(node);
			metaData.unmappedNodeGroups[tsort] = &(metaData.groups[groupMapping[groupId]]);

			if (auto parent = newGroup.GetParent(); parent)
			{
				// Update the name if the parent group exists
				node->name = parent->GetHierarchyName() + "/" + node->name;
			}

			for (size_t input = 0u; input < _unmappedNodes[unmappedNodeId].inputConnectionIds.size(); ++input)
			{
				auto connectionId =  _unmappedNodes[unmappedNodeId].inputConnectionIds[input];
				if (IsDeletedConnection(connectionId))
				{
					continue;
				}

				metaData.connections[connectionMapping[connectionId]].unmappedSinks.push_back({ node, { Circuit::PortType::Input, input} });
			}
			for (size_t output = 0u; output < _unmappedNodes[unmappedNodeId].outputConnectionIds.size(); ++output)
			{
				auto connectionId =  _unmappedNodes[unmappedNodeId].outputConnectionIds[output];
				if (IsDeletedConnection(connectionId))
				{
					continue;
				}

				metaData.connections[connectionMapping[connectionId]].unmappedSources.push_back({ node, { Circuit::PortType::Output, output} });
			}
		}

		// Propagate the mapped node information
		for (const auto& mappedNodeId : _groups[groupId].mappedNodeIds)
		{
			if (IsDeletedMappedNode(mappedNodeId))
			{
				continue;
			}

			const auto tsort = mappedTopologicalSort[mappedNodeId];
			auto* node = &(mappedCircuit._nodeContainer[tsort]);
			newGroup.mappedNodes.push_back(node);
			metaData.mappedNodeGroups[tsort] = &(metaData.groups[groupMapping[groupId]]);

			// Fix the node's name to include the parent group's name
			if (auto parent = newGroup.GetParent(); parent)
			{
				node->name = parent->GetHierarchyName() + "/" + node->name;
			}

			// Fix the unconnected wire's names to include the group's name
			if (node->signalName == "UNCONNECTED_OUTPUT_WIRE" || node->signalName.rfind("UNCONNECTED_INPUT_WIRE_", 0u) == 0u)
			{
				node->signalName = newGroup.GetHierarchyName() + "/" + node->signalName;
			}

			for (size_t input = 0u; input < _mappedNodes[mappedNodeId].inputConnectionIds.size(); ++input)
			{
				auto connectionId =  _mappedNodes[mappedNodeId].inputConnectionIds[input];
				if (IsDeletedConnection(connectionId))
				{
					continue;
				}

				metaData.connections[connectionMapping[connectionId]].mappedSinks.push_back({ node, { Circuit::PortType::Input, input} });
			}

			if (auto connectionId = _mappedNodes[mappedNodeId].outputConnectionId; !IsDeletedConnection(connectionId))
			{
				metaData.connections[connectionMapping[connectionId]].mappedSources.push_back({ node, { Circuit::PortType::Output, 0u} });
			}
		}
	}

	// Propagate the port / wire information after all connections have been initialized.
	// Otherwise, this will result in a race condition where not all wires have been populated and nodes will be missing.
	for (GroupId groupId = 0u; groupId < _groups.size(); ++groupId)
	{
		if (IsDeletedGroup(groupId))
		{
			continue;
		}

		auto& newGroup = metaData.groups[groupMapping[groupId]];
		metaData.nameToGroups.emplace(newGroup.GetHierarchyName(), groupMapping[groupId]);

		// Propagate the wire information
		newGroup.wires.reserve(_groups[groupId].wires.size());
		for (const Builder::Wire& wire : _groups[groupId].wires)
		{
			const auto [top, bottom] = wire.size;
			ASSERT(wire.connections.size() == (std::max(top, bottom) + 1u - std::min(top, bottom)))
				<< "Connection count and wire size does not match";

			std::vector<const Circuit::ConnectionMetaData*> connections;
			for (auto& connectionId : wire.connections)
			{
				if (IsDeletedConnection(connectionId))
				{
					connections.push_back(nullptr);
				}
				else
				{
					connections.push_back(&(metaData.connections[connectionMapping[connectionId]]));
				}
			}

			// Add the wire name as an alias into the wire name to node mapping
			const size_t min = std::min(top, bottom);
			for (size_t index = 0u; index < wire.connections.size(); ++index)
			{
				const size_t connectionId = wire.connections[index];
				if (IsDeletedConnection(connectionId))
				{
					continue;
				}

				metaData.nameToConnections.emplace(boost::str(boost::format("%s/%s [%d]")
					% newGroup.GetHierarchyName() % wire.name % (min + index)), connectionMapping[connectionId]);

				for (auto [node, port] : metaData.connections[connectionMapping[connectionId]].unmappedSources)
				{
					unmappedCircuit._wireNameToNodeId.emplace(boost::str(boost::format("%s/%s [%d]")
							% newGroup.GetHierarchyName() % wire.name % (min + index)),
						UnmappedCircuit::NodeIdAndPinIndex { node->tsort, port.portNumber });
				}
				for (auto [node, port] : metaData.connections[connectionMapping[connectionId]].mappedSources)
				{
					mappedCircuit._wireNameToNodeId.emplace(boost::str(boost::format("%s/%s [%d]")
							% newGroup.GetHierarchyName() % wire.name % (min + index)), node->tsort);
				}
			}

			SourceInformation sourceInfo { };
			if (wire.sourceInfo != INVALID_SOURCE_INFO)
			{
				sourceInfo = _sourceInformation[wire.sourceInfo];
			}

			newGroup.wires.emplace_back(wire.name, SizeMetaData { top, bottom }, sourceInfo, connections, &newGroup);
			newGroup.nameToWires.emplace(wire.name, newGroup.wires.size() - 1u);
			metaData.allWires.push_back(&newGroup.wires.back());
			metaData.nameToWires.emplace(boost::str(boost::format("%s/%s")
				% newGroup.GetHierarchyName() % wire.name), metaData.allWires.size() - 1u);
		}

		// Propagate the port information
		newGroup.ports.reserve(_groups[groupId].ports.size());
		for (const Builder::Port& port : _groups[groupId].ports)
		{
			const auto [top, bottom] = port.size;
			ASSERT(port.connections.size() == (std::max(top, bottom) + 1u - std::min(top, bottom)))
				<< "Connection count and port size does not match";

			std::vector<const Circuit::ConnectionMetaData*> connections;
			for (auto& connectionId : port.connections)
			{
				if (IsDeletedConnection(connectionId))
				{
					connections.push_back(nullptr);
				}
				else
				{
					connections.push_back(&(metaData.connections[connectionMapping[connectionId]]));
				}
			}

			// Add the port name as an alias into the wire name to node mapping
			const size_t min = std::min(top, bottom);
			for (size_t index = 0u; index < port.connections.size(); ++index)
			{
				auto connectionId = port.connections[index];
				if (IsDeletedConnection(connectionId))
				{
					continue;
				}

				metaData.nameToConnections.emplace(boost::str(boost::format("%s/%s [%d]")
					% newGroup.GetHierarchyName() % port.name % (min + index)), connectionMapping[connectionId]);

				auto& unmappedSources = metaData.connections[connectionMapping[connectionId]].GetUnmappedSources();
				for (auto& [sourceNode, sourcePort] : unmappedSources)
				{
					unmappedCircuit._wireNameToNodeId.emplace(
						boost::str(boost::format("%s/%s [%d]") % newGroup.GetHierarchyName() % port.name % (min + index)),
						UnmappedCircuit::NodeIdAndPinIndex { sourceNode->tsort, sourcePort.portNumber });
				}

				auto& mappedSources = metaData.connections[connectionMapping[connectionId]].GetMappedSources();
				for (auto& [sourceNode, sourcePort] : mappedSources)
				{
					mappedCircuit._wireNameToNodeId.emplace(
						boost::str(boost::format("%s/%s [%d]") % newGroup.GetHierarchyName() % port.name % (min + index)),
						sourceNode->tsort);
				}
			}

			SourceInformation sourceInfo { };
			if (port.sourceInfo != INVALID_SOURCE_INFO)
			{
				sourceInfo = _sourceInformation[port.sourceInfo];
			}
			newGroup.ports.emplace_back(port.name, SizeMetaData { top, bottom }, sourceInfo, connections, &newGroup);
			newGroup.nameToPorts.emplace(port.name, newGroup.ports.size() - 1u);
			metaData.allPorts.push_back(&newGroup.ports.back());
			metaData.nameToPorts.emplace(boost::str(boost::format("%s/%s")
				% newGroup.GetHierarchyName() % port.name), metaData.allPorts.size() - 1u);

			switch (port.type)
			{
				case Builder::PortType::Input:
					newGroup.inputPorts.push_back(&(newGroup.ports.back()));
					break;
				case Builder::PortType::Output:
					newGroup.outputPorts.push_back(&(newGroup.ports.back()));
					break;
				case Builder::PortType::Bidirectional:
					newGroup.inputPorts.push_back(&(newGroup.ports.back()));
					newGroup.outputPorts.push_back(&(newGroup.ports.back()));
					break;
			}
		}
	}

	for (size_t unmappedNodeId = 0; unmappedNodeId < unmappedCircuit._nodeContainer.size(); unmappedNodeId++)
	{
		// Associate the node name with the node id
		unmappedCircuit._nodeNameToNodeId.insert(unmappedCircuit._nodeContainer[unmappedNodeId].name, unmappedNodeId);
	}

	for (size_t mappedNodeId = 0; mappedNodeId < mappedCircuit._nodeContainer.size(); mappedNodeId++)
	{
		// Associate the node name with the node id
		mappedCircuit._nodeNameToNodeId.insert(mappedCircuit._nodeContainer[mappedNodeId].name, mappedNodeId);
		// Associate the wire name with the node id
		mappedCircuit._wireNameToNodeId.insert(mappedCircuit._nodeContainer[mappedNodeId].signalName, mappedNodeId);
	}

	// Apply the connection IDs to the unmapped node information
	for (UnmappedNodeId unmappedNodeId = 0u; unmappedNodeId < _unmappedNodes.size(); ++unmappedNodeId)
	{
		if (IsDeletedUnmappedNode(unmappedNodeId))
		{
			continue;
		}

		UnmappedNode& unmappedNode = _unmappedNodes[unmappedNodeId];
		size_t tsort = unmappedTopologicalSort[unmappedNodeId];
		auto& node = unmappedCircuit._nodeContainer[tsort];

		for (size_t output = 0u; output < node.numberOfOutputPins; output++)
		{
			node.outData[output].PinWireId = IsNotDeletedConnection(unmappedNode.outputConnectionIds[output])
				? connectionMapping[unmappedNode.outputConnectionIds[output]]
				: UnmappedCircuit::NO_CONNECTION;
		}

		for (size_t input = 0u; input < node.numberOfIns; input++)
		{
			node.inData[input].PinWireId = IsNotDeletedConnection(unmappedNode.inputConnectionIds[input])
				? connectionMapping[unmappedNode.inputConnectionIds[input]]
				: UnmappedCircuit::NO_CONNECTION;
		}
	}

	// Apply the connection IDs to the mapped node information
	for (MappedNodeId mappedNodeId = 0u; mappedNodeId < _mappedNodes.size(); ++mappedNodeId)
	{
		if (IsDeletedMappedNode(mappedNodeId))
		{
			continue;
		}

		MappedNode& mappedNode = _mappedNodes[mappedNodeId];
		size_t tsort = mappedTopologicalSort[mappedNodeId];
		auto& node = mappedCircuit._nodeContainer[tsort];

		node.outConnectionID = IsNotDeletedConnection(mappedNode.outputConnectionId)
			? connectionMapping[mappedNode.outputConnectionId]
			: MappedCircuit::NO_CONNECTION;

		for (size_t input = 0u; input < node.numberOfIns; ++input)
		{
			node.inConnectionID[input] = IsNotDeletedConnection(mappedNode.inputConnectionIds[input])
				? connectionMapping[mappedNode.inputConnectionIds[input]]
				: MappedCircuit::NO_CONNECTION;
		}
	}

	// ------------------------------------------------------------------------
	// Additional final checks
	// ------------------------------------------------------------------------

	// Check if the unmapped tsort is (still) consistent
	size_t unmappedTsort = 0u;
	for (const auto& unmappedNode : unmappedCircuit._nodeContainer)
	{
		ASSERT(unmappedNode.tsort == unmappedTsort);
		unmappedTsort++;
	}

	// Check if the mapped tsort is (still) consistent
	size_t mappedTsort = 0u;
	for (const auto& mappedNode : mappedCircuit._nodeContainer)
	{
		ASSERT(mappedNode.tsort == mappedTsort);
		mappedTsort++;
	}

	// Check if we have
	for (const auto* node : unmappedCircuit._nodes)
	{
		ASSERT(node != nullptr) << "There is a nullptr in the node list of the unmapped circuit.";
	}
	for (const auto* node : mappedCircuit._nodes)
	{
		ASSERT(node != nullptr) << "There is a nullptr in the node list of the mapped circuit.";
	}

	for (const auto* primaryInput : unmappedCircuit._primInputs)
	{
		ASSERT(unmappedCircuit._primInputNrMap.contains(primaryInput->tsort))
			<< "There is no mapping from primary input " << primaryInput->name
			<< " to an input number in the unmapped circuit.";
		ASSERT(primaryInput->typeName == "input" || primaryInput->typeName == "inout")
			<< "The primary input " << primaryInput->name
			<< " has an invalid type of \"" << primaryInput->typeName << "\" in the unmapped circuit";
	}
	for (const auto* primaryOutput : unmappedCircuit._primOutputs)
	{
		ASSERT(unmappedCircuit._primOutputNrMap.contains(primaryOutput->tsort))
			<< "There is no mapping from primary output " << primaryOutput->name
			<< " to an output number in the unmapped circuit.";
		ASSERT(primaryOutput->typeName == "output" || primaryOutput->typeName == "inout")
			<< "The primary output " << primaryOutput->name
			<< " has an invalid type of \"" << primaryOutput->typeName << "\" in the unmapped circuit";
	}

	ASSERT(mappedCircuit._secInputs.size() == mappedCircuit._secOutputs.size()) << "Number of secondary inputs and outputs does not match";
	for (const auto* primaryInput : mappedCircuit._primInputs)
	{
		ASSERT(mappedCircuit._primInputNrMap.contains(primaryInput->tsort))
			<< "There is no mapping from primary input " << primaryInput->name
			<< " to an input number in the mapped circuit.";
		// FIXME: P_INOUT
		ASSERT(primaryInput->type == CellType::P_IN || primaryInput->type == CellType::P_OUT)
			<< "The primary input " << primaryInput->name
			<< " has an invalid type of \"" << to_string(primaryInput->type) << "\" in the mapped circuit";
	}
	for (const auto* primaryOutput : mappedCircuit._primOutputs)
	{
		ASSERT(mappedCircuit._primOutputNrMap.contains(primaryOutput->tsort))
			<< "There is no mapping from primary output " << primaryOutput->name
			<< " to an output number in the mapped circuit.";
		ASSERT(primaryOutput->type == CellType::P_OUT) << "The primary output " << primaryOutput->name
			<< " has an invalid type of \"" << to_string(primaryOutput->type) << "\" in the mapped circuit";
	}
	for (const auto* secondaryInput : mappedCircuit._secInputs)
	{
		ASSERT(mappedCircuit._sinToSoutMap.contains(secondaryInput->tsort))
			<< "There is no mapping from secondary input " << secondaryInput->name
			<< " to a secondary output in the mapped circuit.";
		ASSERT(mappedCircuit._secInputNrMap.contains(secondaryInput->tsort))
			<< "There is no mapping from secondary input " << secondaryInput->name
			<< " to a flip-flop number in the mapped circuit.";
		ASSERT(secondaryInput->type == CellType::S_IN) << "The secondary input " << secondaryInput->name
			<< " has an invalid type of \"" << to_string(secondaryInput->type) << "\" in the mapped circuit";
	}
	for (const auto* secondaryOutput : mappedCircuit._secOutputs)
	{
		ASSERT(mappedCircuit._soutToSinMap.contains(secondaryOutput->tsort))
			<< "There is no mapping from secondary output " << secondaryOutput->name
			<< " to a secondary input in the mapped circuit.";
		ASSERT(mappedCircuit._secOutputNrMap.contains(secondaryOutput->tsort))
			<< "There is no mapping from secondary output " << secondaryOutput->name
			<< " to a flip-flop number in the mapped circuit.";
		ASSERT(secondaryOutput->type == CellType::S_OUT || secondaryOutput->type == CellType::S_OUT_CLK || secondaryOutput->type == CellType::S_OUT_EN)
			<< "The secondary output " << secondaryOutput->name << " has an invalid type of \""
			<< to_string(secondaryOutput->type) << "\" in the mapped circuit";
	}

	for (const auto& node : unmappedCircuit._nodeContainer)
	{
		if (node.typeName == "input")
		{
			ASSERT(unmappedCircuit._primInputNrMap.contains(node.tsort))
				<< "There is no mapping from primary input " << node.name
				<< " to an input number in the unmapped circuit.";
		}
		else if (node.typeName == "output")
		{
			ASSERT(unmappedCircuit._primOutputNrMap.contains(node.tsort))
				<< "There is no mapping from primary output " << node.name
				<< " to an output number in the unmapped circuit.";
		}
	}

	for (const auto& node : mappedCircuit._nodeContainer)
	{
		if (node.mainType == CellCategory::MAIN_IN)
		{
			ASSERT(mappedCircuit._inputNrMap.contains(node.tsort))
				<< "There is no mapping from input " << node.name
				<< " to an input number in the mapped circuit.";
		}
		else if (node.mainType == CellCategory::MAIN_OUT)
		{
			ASSERT(mappedCircuit._outputNrMap.contains(node.tsort))
				<< "There is no mapping from output " << node.name
				<< " to an output number in the mapped circuit.";
		}
		else
		{
			ASSERT(!mappedCircuit._inputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-input " << node.name
				<< " to an input number in the mapped circuit.";
			ASSERT(!mappedCircuit._outputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-output " << node.name
				<< " to an output number in the mapped circuit.";
		}

		if (node.type == CellType::P_IN)
		{
			ASSERT(mappedCircuit._primInputNrMap.contains(node.tsort))
				<< "There is no mapping from primary input " << node.name
				<< " to an input number in the mapped circuit.";
		}
		else if (node.type == CellType::P_OUT)
		{
			ASSERT(mappedCircuit._primOutputNrMap.contains(node.tsort))
				<< "There is no mapping from primary output " << node.name
				<< " to an output number in the mapped circuit.";
		}
		else if (node.type == CellType::S_IN)
		{
			ASSERT(mappedCircuit._secInputNrMap.contains(node.tsort))
				<< "There is no mapping from secondary input " << node.name
				<< " to an input number in the mapped circuit.";
			ASSERT(mappedCircuit._sinToSoutMap.contains(node.tsort))
				<< "There is no mapping from secondary input " << node.name
				<< " to a secondary output in the mapped circuit.";
		}
		else if (node.type == CellType::S_OUT || node.type == CellType::S_OUT_CLK || node.type == CellType::S_OUT_EN)
		{
			ASSERT(mappedCircuit._secOutputNrMap.contains(node.tsort))
				<< "There is no mapping from secondary output " << node.name
				<< " to an output number in the mapped circuit.";
			ASSERT(mappedCircuit._soutToSinMap.contains(node.tsort))
				<< "There is no mapping from secondary output " << node.name
				<< " to a secondary input in the mapped circuit.";
		}
		else
		{
			ASSERT(!mappedCircuit._primInputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-primary input " << node.name
				<< " to a primary input number in the mapped circuit.";
			ASSERT(!mappedCircuit._primOutputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-primary output " << node.name
				<< " to a primary output number in the mapped circuit.";
			ASSERT(!mappedCircuit._secInputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-secondary input " << node.name
				<< " to a secondary input number in the mapped circuit.";
			ASSERT(!mappedCircuit._secOutputNrMap.contains(node.tsort))
				<< "There is an invalid mapping from non-secondary output " << node.name
				<< " to a secondary output number in the mapped circuit.";
		}
	}

	for (const auto* wire : metaData.allWires)
	{
		auto& connections = wire->GetConnections();
		auto& size = wire->GetSize();

		ASSERT(connections.size() == size.GetSize())
			<< "Not all connections have been populated in wire " << wire->GetHierarchyName();
	}

	for (const auto* port : metaData.allPorts)
	{
		auto& connections = port->GetConnections();
		auto& size = port->GetSize();

		ASSERT(connections.size() == size.GetSize())
			<< "Not all connections have been populated in port " << port->GetHierarchyName();
	}

	// ------------------------------------------------------------------------
	// Debug output for the generated circuit
	// ------------------------------------------------------------------------

	for (size_t unmappedTsort = 0u; unmappedTsort < unmappedCircuit._nodeContainer.size(); unmappedTsort++)
	{
		const auto& unmappedNode = unmappedCircuit._nodeContainer[unmappedTsort];

		std::string inputs;
		for (size_t input = 0u; input < unmappedNode.numberOfIns; ++input)
		{
			if (input != 0)
			{
				inputs += ", ";
			}

			if (unmappedNode.in[input] == nullptr)
			{
				inputs += "unconnected";
			}
			else
			{
				inputs += unmappedNode.in[input]->name;
			}
		}

		std::string outputs;
		for (size_t output = 0; output < unmappedNode.numberOfOutputPins; ++output)
		{
			if (output != 0)
			{
				outputs += ", ";
			}

			outputs += "[";
			for (size_t successor = 0u; successor < unmappedNode.numberOfOuts[output]; ++successor)
			{
				if (successor != 0)
				{
					outputs += ", ";
				}

				if (unmappedNode.out[output][successor] == nullptr)
				{
					outputs += "unconnected";
				}
				else
				{
					outputs += unmappedNode.out[output][successor]->name;
				}
			}
			outputs += "]";
		}

		VLOG(5) << "Unmapped Node " << unmappedNode.name << ": " << unmappedNode.typeName << " (" << inputs << ") -> [" << outputs << "]";
	}

	for (size_t mappedTsort = 0u; mappedTsort < mappedCircuit._nodeContainer.size(); mappedTsort++)
	{
		const auto& mappedNode = mappedCircuit._nodeContainer[mappedTsort];

		std::string inputs;
		for (size_t input = 0; input < mappedNode.numberOfIns; ++input)
		{
			if (input != 0)
			{
				inputs += ", ";
			}

			if (mappedNode.in[input] == nullptr)
			{
				inputs += "unconnected";
			}
			else
			{
				inputs += mappedNode.in[input]->GetName();
			}
		}

		std::string outputs;
		for (size_t output = 0; output < mappedNode.numberOfOuts; ++output)
		{
			if (output != 0)
			{
				outputs += ", ";
			}

			if (mappedNode.out[output] == nullptr)
			{
				outputs += "unconnected";
			}
			else
			{
				outputs += mappedNode.out[output]->GetName();
			}
		}

		VLOG(5) << "Mapped Node " << mappedNode.GetName() << ": " << to_string(mappedNode.mainType) << " (" << inputs << ") -> [" << outputs << "]";
	}

	return environment;
}

};
};
};
