#include "Circuit/CircuitMetaData.hpp"

#include <boost/format.hpp>

#include <numeric>

#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Circuit
{

ConnectionMetaData::ConnectionMetaData(size_t connectionId):
	connectionId(connectionId),
	mappedSources(),
	mappedSinks(),
	unmappedSources(),
	unmappedSinks()
{
}

ConnectionMetaData::~ConnectionMetaData(void) = default;

size_t ConnectionMetaData::GetConnectionId(void) const
{
	return connectionId;
}

const std::vector<MappedCircuit::NodeAndPort>& ConnectionMetaData::GetMappedSources(void) const
{
	return mappedSources;
}

const std::vector<MappedCircuit::NodeAndPort>& ConnectionMetaData::GetMappedSinks(void) const
{
	return mappedSinks;
}

const std::vector<UnmappedCircuit::NodeAndPort>& ConnectionMetaData::GetUnmappedSources(void) const
{
	return unmappedSources;
}

const std::vector<UnmappedCircuit::NodeAndPort>& ConnectionMetaData::GetUnmappedSinks(void) const
{
	return unmappedSinks;
}

SizeMetaData::SizeMetaData(size_t top, size_t bottom):
	top(top),
	bottom(bottom)
{
}

SizeMetaData::~SizeMetaData(void) = default;

size_t SizeMetaData::GetTop(void) const
{
	return top;
}

size_t SizeMetaData::GetBottom(void) const
{
	return bottom;
}

size_t SizeMetaData::GetMin(void) const
{
	return std::min(top, bottom);
}

size_t SizeMetaData::GetMax(void) const
{
	return std::max(top, bottom);
}

size_t SizeMetaData::GetSize(void) const
{
	return std::max(top, bottom) - std::min(top, bottom) + 1u;
}

std::vector<size_t> SizeMetaData::GetIndicesBottomToTop(void) const
{
	std::vector<size_t> result(GetSize());
	std::iota(std::begin(result), std::end(result), std::min(bottom, top));
	if (bottom > top) {
		std::reverse(std::begin(result), std::end(result));
	}
	return result;
}

std::vector<size_t> SizeMetaData::GetIndicesTopToBottom(void) const
{
	std::vector<size_t> result(GetSize());
	std::iota(std::begin(result), std::end(result), std::min(bottom, top));
	if (bottom < top) {
		std::reverse(std::begin(result), std::end(result));
	}
	return result;
}

std::vector<size_t> SizeMetaData::GetIndicesMinToMax(void) const
{
	std::vector<size_t> result(GetSize());
	std::iota(std::begin(result), std::end(result), std::min(bottom, top));
	return result;
}

std::vector<size_t> SizeMetaData::GetIndicesMaxToMin(void) const
{
	std::vector<size_t> result(GetSize());
	std::iota(std::begin(result), std::end(result), std::min(bottom, top));
	std::reverse(std::begin(result), std::end(result));
	return result;
}

WireMetaData::WireMetaData(std::string name, SizeMetaData size, SourceInformation sourceInfo, std::vector<const ConnectionMetaData*> connections, const GroupMetaData* group):
	name(name),
	size(size),
	sourceInfo(sourceInfo),
	connections(connections),
	group(group)
{
}

WireMetaData::~WireMetaData(void) = default;

const std::string& WireMetaData::GetName(void) const
{
	return name;
}

const SizeMetaData& WireMetaData::GetSize(void) const
{
	return size;
}

std::string WireMetaData::GetHierarchyName() const
{
	return group->GetHierarchyName() + "/" + name;
}

const SourceInformation& WireMetaData::GetSourceInfo(void) const
{
	return sourceInfo;
}

const std::vector<const ConnectionMetaData*>& WireMetaData::GetConnections(void) const
{
	return connections;
}

const ConnectionMetaData* WireMetaData::GetConnectionForIndex(size_t index) const
{
	return connections[index - std::min(size.GetTop(), size.GetBottom())];
}

const GroupMetaData* WireMetaData::GetGroup(void) const
{
	return group;
}

PortMetaData::PortMetaData(std::string name, SizeMetaData size, SourceInformation sourceInfo, std::vector<const ConnectionMetaData*> connections, const GroupMetaData* group):
	name(name),
	size(size),
	sourceInfo(sourceInfo),
	connections(connections),
	group(group)
{
}

PortMetaData::~PortMetaData(void) = default;

const std::string& PortMetaData::GetName(void) const
{
	return name;
}

const SizeMetaData& PortMetaData::GetSize(void) const
{
	return size;
}

std::string PortMetaData::GetHierarchyName() const
{
	return group->GetHierarchyName() + "/" + name;
}

const SourceInformation& PortMetaData::GetSourceInfo(void) const
{
	return sourceInfo;
}

const std::vector<const ConnectionMetaData*>& PortMetaData::GetConnections(void) const
{
	return connections;
}

const ConnectionMetaData* PortMetaData::GetConnectionForIndex(size_t index) const
{
	return connections[index - std::min(size.GetTop(), size.GetBottom())];
}

const GroupMetaData* PortMetaData::GetGroup(void) const
{
	return group;
}

GroupMetaData::GroupMetaData(std::string name):
	name(name),
	sourceInfo(),
	parent(nullptr),
	subGroups(),
	ports(),
	wires(),
	mappedNodes(),
	unmappedNodes()
{
}

GroupMetaData::~GroupMetaData(void) = default;

const std::string& GroupMetaData::GetName(void) const
{
	return name;
}

std::string GroupMetaData::GetHierarchyName(void) const
{
	if (parent != nullptr)
	{
		return parent->GetHierarchyName() + "/" + name;
	}

	return name;
}

const SourceInformation& GroupMetaData::GetSourceInfo(void) const
{
	return sourceInfo;
}

const GroupMetaData* GroupMetaData::GetParent(void) const
{
	return parent;
}

const std::vector<const GroupMetaData*>& GroupMetaData::GetSubGroups(void) const
{
	return subGroups;
}

const std::vector<PortMetaData>& GroupMetaData::GetPorts(void) const
{
	return ports;
}

bool GroupMetaData::HasPort(const std::string& name) const
{
	return (nameToPorts.find(name) != nameToPorts.end());
}

const PortMetaData* GroupMetaData::GetPort(const std::string& name) const
{
	if (auto it = nameToPorts.find(name); it != nameToPorts.end())
	{
		return &(ports[*it]);
	}

	return nullptr;
}

const PortMetaData* GroupMetaData::GetPort(const Circuit::Port& port) const
{
	switch (port.portType)
	{
		case Circuit::PortType::Input:
			return (port.portNumber < inputPorts.size()) ? inputPorts[port.portNumber] : nullptr;
		case Circuit::PortType::Output:
			return (port.portNumber < outputPorts.size()) ? outputPorts[port.portNumber] : nullptr;
	}

	return nullptr;
}

std::vector<const PortMetaData*> GroupMetaData::GetPortsWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToPorts.equal_prefix_range(prefix);

	std::vector<const PortMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(&(ports[*element]));
	}
	return result;
}

std::tuple<const PortMetaData*, size_t> GroupMetaData::GetPortWithConnection(size_t connectionId) const
{
	for (const auto& port : ports)
	{
		for (auto index : port.GetSize().GetIndicesTopToBottom())
		{
			if (const auto& connection = port.GetConnectionForIndex(index);
				connection != nullptr &&
				connection->GetConnectionId() == connectionId)
			{
				return { &port, index };
			}
		}
	}

	return { nullptr, std::numeric_limits<size_t>::max() };
}

const std::vector<WireMetaData>& GroupMetaData::GetWires(void) const
{
	return wires;
}

bool GroupMetaData::HasWire(const std::string& name) const
{
	return (nameToWires.find(name) != nameToWires.end());
}

const WireMetaData* GroupMetaData::GetWire(const std::string& name) const
{
	if (auto it = nameToWires.find(name); it != nameToWires.end())
	{
		return &(wires[*it]);
	}

	return nullptr;
}

std::vector<const WireMetaData*> GroupMetaData::GetWiresWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToWires.equal_prefix_range(prefix);

	std::vector<const WireMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(&(wires[*element]));
	}
	return result;
}

std::tuple<const WireMetaData*, size_t> GroupMetaData::GetWireWithConnection(size_t connectionId) const
{
	for (const auto& wire : wires)
	{
		for (auto index : wire.GetSize().GetIndicesTopToBottom())
		{
			if (const auto& connection = wire.GetConnectionForIndex(index);
				connection != nullptr &&
				connection->GetConnectionId() == connectionId)
			{
				return { &wire, index };
			}
		}
	}

	return { nullptr, std::numeric_limits<size_t>::max() };
}

const std::vector<const MappedNode*>& GroupMetaData::GetMappedNodes(void) const
{
	return mappedNodes;
}

const std::vector<const UnmappedNode*>& GroupMetaData::GetUnmappedNodes(void) const
{
	return unmappedNodes;
}

CircuitMetaData::CircuitMetaData(void):
	groups(),
	connections(),
	unmappedNodeGroups(),
	mappedNodeGroups(),
	allGroups(),
	allConnections(),
	allWires(),
	allPorts(),
	nameToGroups(),
	nameToConnections(),
	nameToWires(),
	nameToPorts()
{
}

CircuitMetaData::~CircuitMetaData(void) = default;

const GroupMetaData* CircuitMetaData::GetRoot(void) const
{
	return &(groups[0u]);
}

const GroupMetaData* CircuitMetaData::GetGroup(const UnmappedNode* node) const
{
	return unmappedNodeGroups[node->GetNodeId()];
}

const GroupMetaData* CircuitMetaData::GetGroup(const MappedNode* node) const
{
	ASSERT(mappedNodeGroups.size() > node->GetNodeId()) << "Failed to find nodeId " << std::to_string(node->GetNodeId());
	return mappedNodeGroups[node->GetNodeId()];
}

const std::vector<const GroupMetaData*>& CircuitMetaData::GetGroups(void) const
{
	return allGroups;
}

const std::vector<const ConnectionMetaData*>& CircuitMetaData::GetConnections(void) const
{
	return allConnections;
}

const std::vector<const WireMetaData*>& CircuitMetaData::GetWires(void) const
{
	return allWires;
}

const std::vector<const PortMetaData*>& CircuitMetaData::GetPorts(void) const
{
	return allPorts;
}

bool CircuitMetaData::HasGroup(const std::string& name) const
{
	return (nameToGroups.find(name) != nameToGroups.end());
}

const GroupMetaData* CircuitMetaData::GetGroup(const std::string& name) const
{
	if (auto it = nameToGroups.find(name); it != nameToGroups.end())
	{
		return allGroups[*it];
	}

	return nullptr;
}

std::vector<const GroupMetaData*> CircuitMetaData::GetGroupsWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToGroups.equal_prefix_range(prefix);

	std::vector<const GroupMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(allGroups[*element]);
	}
	return result;
}

bool CircuitMetaData::HasConnection(const std::string& name) const
{
	return (nameToConnections.find(name) != nameToConnections.end());
}

const ConnectionMetaData* CircuitMetaData::GetConnection(const std::string& name) const
{
	if (auto it = nameToConnections.find(name); it != nameToConnections.end())
	{
		return allConnections[*it];
	}

	return nullptr;
}

std::vector<const ConnectionMetaData*> CircuitMetaData::GetConnectionsWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToConnections.equal_prefix_range(prefix);

	std::vector<const ConnectionMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(allConnections[*element]);
	}
	return result;
}

bool CircuitMetaData::HasWire(const std::string& name) const
{
	return (nameToWires.find(name) != nameToWires.end());
}

const WireMetaData* CircuitMetaData::GetWire(const std::string& name) const
{
	if (auto it = nameToWires.find(name); it != nameToWires.end())
	{
		return allWires[*it];
	}

	return nullptr;
}

std::vector<const WireMetaData*> CircuitMetaData::GetWiresWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToWires.equal_prefix_range(prefix);

	std::vector<const WireMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(allWires[*element]);
	}
	return result;
}

bool CircuitMetaData::HasPort(const std::string& name) const
{
	return (nameToPorts.find(name) != nameToPorts.end());
}

const PortMetaData* CircuitMetaData::GetPort(const std::string& name) const
{
	if (auto it = nameToPorts.find(name); it != nameToPorts.end())
	{
		return allPorts[*it];
	}

	return nullptr;
}

std::vector<const PortMetaData*> CircuitMetaData::GetPortsWithPrefix(const std::string& prefix) const
{
	auto [start, end] = nameToPorts.equal_prefix_range(prefix);

	std::vector<const PortMetaData*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(allPorts[*element]);
	}
	return result;
}


template<typename NodeAndPort>
static std::string GetFriendlyNameImpl(const CircuitMetaData& metaData, const NodeAndPort& nodeAndPort)
{
	auto [node, port] = nodeAndPort;
	auto connectionId = node->GetPortConnectionId(port);
	ASSERT(connectionId != Circuit::NO_CONNECTION);

	auto const* gateGroup = metaData.GetGroup(node);
	auto const* parentGroup = gateGroup->GetParent();
	auto const* topGroup = metaData.GetRoot();

	std::string name;
	if (gateGroup->GetSourceInfo().template GetProperty<std::string>("primitive-category").value_or("gate") == "port")
	{
		// This is a top-level circuit port, so we want to get the proper top-level
		// primary circuit port name for the fault location.
		auto [port, portIndex] = topGroup->GetPortWithConnection(connectionId);
		ASSERT(port != nullptr) << "Found port gate that is not connected to any primary port of the circuit";

		return (port->GetSize().GetSize() > 1)
			? boost::str(boost::format("%s [%d]") % port->GetHierarchyName() % portIndex)
			: port->GetHierarchyName();
	}

	// This is a fault at an internal gate (gate primitive) which we map to a cell port name or a wire name.
	if (auto [port, portIndex] = parentGroup->GetPortWithConnection(connectionId); port != nullptr) {
		return (port->GetSize().GetSize() > 1)
			? boost::str(boost::format("%s [%d]") % port->GetHierarchyName() % portIndex)
			: port->GetHierarchyName();
	}
	if (auto [wire, wireIndex] = parentGroup->GetWireWithConnection(connectionId); wire != nullptr) {
		return (wire->GetSize().GetSize() > 1)
			? boost::str(boost::format("%s [%d]") % wire->GetHierarchyName() % wireIndex)
			: wire->GetHierarchyName();
	}

	// This is a fault internal gate (gate primitive) which can not be mapped to a cell.
	if (auto [port, portIndex] = gateGroup->GetPortWithConnection(connectionId); port != nullptr) {
		return (port->GetSize().GetSize() > 1)
			? boost::str(boost::format("%s [%d]") % port->GetHierarchyName() % portIndex)
			: port->GetHierarchyName();
	}
	if (auto [wire, wireIndex] = gateGroup->GetWireWithConnection(connectionId); wire != nullptr) {
		return (wire->GetSize().GetSize() > 1)
			? boost::str(boost::format("%s [%d]") % wire->GetHierarchyName() % wireIndex)
			: wire->GetHierarchyName();
	}

	Logging::Panic("Found port that is not connected to anything");
}

std::string CircuitMetaData::GetFriendlyName(const Circuit::MappedCircuit::NodeAndPort& nodeAndPort) const
{
	return GetFriendlyNameImpl(*this, nodeAndPort);
}

std::string CircuitMetaData::GetFriendlyName(const Circuit::UnmappedCircuit::NodeAndPort& nodeAndPort) const
{
	return GetFriendlyNameImpl(*this, nodeAndPort);
}

};
};
