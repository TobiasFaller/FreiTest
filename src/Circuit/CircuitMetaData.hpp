#pragma once

#include <tsl/htrie_map.h>

#include <string>
#include <tuple>
#include <vector>

#include "Circuit/SourceInformation.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };
class GroupMetaData;

class ConnectionMetaData
{
public:
	ConnectionMetaData(size_t connectionId);
	virtual ~ConnectionMetaData(void);

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

	friend Builder::CircuitBuilder;

private:
	size_t connectionId;
	std::vector<MappedCircuit::NodeAndPort> mappedSources;
	std::vector<MappedCircuit::NodeAndPort> mappedSinks;
	std::vector<UnmappedCircuit::NodeAndPort> unmappedSources;
	std::vector<UnmappedCircuit::NodeAndPort> unmappedSinks;

};

class SizeMetaData
{
public:
	SizeMetaData(size_t top, size_t bottom);
	virtual ~SizeMetaData(void);

	size_t GetTop(void) const;
	size_t GetBottom(void) const;
	size_t GetMin(void) const;
	size_t GetMax(void) const;
	size_t GetSize(void) const;

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

private:
	size_t top;
	size_t bottom;

};

class WireMetaData
{
public:

	WireMetaData(std::string name, SizeMetaData size, SourceInformation sourceInfo, std::vector<const ConnectionMetaData*> connections, const GroupMetaData* group);
	virtual ~WireMetaData(void);

	const std::string& GetName(void) const;
	const SizeMetaData& GetSize(void) const;
	std::string GetHierarchyName(void) const;
	const SourceInformation& GetSourceInfo(void) const;
	const std::vector<const ConnectionMetaData*>& GetConnections(void) const;
	const ConnectionMetaData* GetConnectionForIndex(size_t index) const;
	const GroupMetaData* GetGroup(void) const;

private:
	std::string name;
	SizeMetaData size;
	SourceInformation sourceInfo;
	std::vector<const ConnectionMetaData*> connections;
	const GroupMetaData* group;

};

class PortMetaData
{
public:
	PortMetaData(std::string name, SizeMetaData size, SourceInformation sourceInfo, std::vector<const ConnectionMetaData*> connections, const GroupMetaData* group);
	virtual ~PortMetaData(void);

	const std::string& GetName(void) const;
	const SizeMetaData& GetSize(void) const;
	std::string GetHierarchyName(void) const;
	const SourceInformation& GetSourceInfo(void) const;
	const std::vector<const ConnectionMetaData*>& GetConnections(void) const;
	const ConnectionMetaData* GetConnectionForIndex(size_t index) const;
	const GroupMetaData* GetGroup(void) const;

private:
	std::string name;
	SizeMetaData size;
	SourceInformation sourceInfo;
	std::vector<const ConnectionMetaData*> connections;
	const GroupMetaData* group;

};

class GroupMetaData
{
public:
	GroupMetaData(std::string name);
	virtual ~GroupMetaData(void);

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
	const GroupMetaData* FindParentWithProperty(std::string name, Function func) const
	{
		if (parent != nullptr)
		{
			auto const property = parent->sourceInfo.GetProperty<Property>(name);
			if (property.has_value() && func(property.value()))
			{
				return parent;
			}

			return parent->FindParentWithProperty<Property>(name, func);
		}

		return nullptr;
	}

	// Walks the children to find a group with the propertie's name
	// and then evaluates the filter function on the property,
	// and returns the group if the function returns true.
	template<typename Property, typename Function>
	const GroupMetaData* FindSubGroupWithProperty(std::string name, Function func) const
	{
		for (const auto& subGroup : subGroups)
		{
			ASSERT(subGroup != nullptr) << "Encountered subGroup which is null";
			auto const property = subGroup->sourceInfo.GetProperty<Property>(name);
			if (property.has_value() && func(property.value()))
			{
				return subGroup;
			}

			if (const auto& found = subGroup->FindSubGroupWithProperty<Property>(name, func);
				found != nullptr)
			{
				return found;
			}
		}

		return nullptr;
	}

	// Walks first the parents and then the children to find a group with the propertie's name
	// and then evaluates the filter function on the property,
	// and returns the group if the function returns true.
	template<typename Property, typename Function>
	const GroupMetaData* FindGroupWithProperty(std::string name, Function func) const
	{
		auto const property = sourceInfo.GetProperty<Property>(name);
		if (property.has_value() && func(property.value()))
		{
			return this;
		}

		if (const auto& found = FindParentWithProperty<Property>(name, func);
			found != nullptr)
		{
			return found;
		}

		if (const auto& found = FindSubGroupWithProperty<Property>(name, func);
			found != nullptr)
		{
			return found;
		}

		return nullptr;
	}

	friend Builder::CircuitBuilder;

private:
	std::string name;
	SourceInformation sourceInfo;
	const GroupMetaData* parent;
	std::vector<const GroupMetaData*> subGroups;

	std::vector<PortMetaData> ports;
	std::vector<WireMetaData> wires;

	std::vector<const PortMetaData*> inputPorts;
	std::vector<const PortMetaData*> outputPorts;

	tsl::htrie_map<char, size_t> nameToWires;
	tsl::htrie_map<char, size_t> nameToPorts;

	std::vector<const MappedNode*> mappedNodes;
	std::vector<const UnmappedNode*> unmappedNodes;

};

class CircuitMetaData
{
public:
	CircuitMetaData(void);
	virtual ~CircuitMetaData(void);

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

	friend Builder::CircuitBuilder;

private:
	std::vector<GroupMetaData> groups;
	std::vector<ConnectionMetaData> connections;

	std::vector<const GroupMetaData*> unmappedNodeGroups;
	std::vector<const GroupMetaData*> mappedNodeGroups;

	std::vector<const GroupMetaData*> allGroups;
	std::vector<const ConnectionMetaData*> allConnections;
	std::vector<const WireMetaData*> allWires;
	std::vector<const PortMetaData*> allPorts;

	tsl::htrie_map<char, size_t> nameToGroups;
	tsl::htrie_map<char, size_t> nameToConnections;
	tsl::htrie_map<char, size_t> nameToWires;
	tsl::htrie_map<char, size_t> nameToPorts;

};

};
};
