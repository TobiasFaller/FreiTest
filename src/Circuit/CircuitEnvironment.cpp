#include "Circuit/CircuitEnvironment.hpp"

#include "Basic/Logging.hpp"

using namespace std;

namespace FreiTest
{
namespace Circuit
{

CircuitEnvironment::CircuitEnvironment(void):
	_mappedToUnmappedPSorts(),
	_unmappedToMappedPSorts(),
	_unmappedToMappedTSorts(),
	_mappedToUnmappedTSorts(),
	_unmappedCircuit(),
	_circuit(),
	_metaData(),
	_circuitName("Undefined")
{
}

CircuitEnvironment::~CircuitEnvironment(void) = default;

string CircuitEnvironment::GetName(void) const
{
	return _circuitName;
}

const UnmappedCircuit& CircuitEnvironment::GetUnmappedCircuit(void) const
{
	return _unmappedCircuit;
}

const MappedCircuit& CircuitEnvironment::GetMappedCircuit(void) const
{
	return _circuit;
}

const CircuitMetaData& CircuitEnvironment::GetMetaData(void) const
{
	return _metaData;
}

bool CircuitEnvironment::HasUnmappedNodeId(size_t mappedNodeId) const
{
	ASSERT(mappedNodeId < _mappedToUnmappedTSorts.size()) << "Requested invalid node id: " << mappedNodeId;
	return _mappedToUnmappedTSorts[mappedNodeId] != NO_UNMAPPED_PIN;
}

bool CircuitEnvironment::HasMappedNodeIds(size_t unmappedNodeId) const
{
	ASSERT(unmappedNodeId < _unmappedToMappedTSorts.size()) << "Requested invalid node id: " << unmappedNodeId;
	return !_unmappedToMappedTSorts[unmappedNodeId].empty();
}

size_t CircuitEnvironment::GetUnmappedNodeId(size_t mappedNodeId) const
{
	ASSERT(mappedNodeId < _mappedToUnmappedTSorts.size()) << "Requested invalid node id: " << mappedNodeId;
	return _mappedToUnmappedTSorts[mappedNodeId];
}

std::vector<size_t> CircuitEnvironment::GetMappedNodeIds(size_t unmappedNodeId) const
{
	ASSERT(unmappedNodeId < _unmappedToMappedTSorts.size()) << "Requested invalid node id: " << unmappedNodeId;
	return _unmappedToMappedTSorts[unmappedNodeId];
}

bool CircuitEnvironment::HasUnmappedPinId(size_t mappedPinId) const
{
	ASSERT(mappedPinId < _mappedToUnmappedPSorts.size()) << "Requested invalid pin id: " << mappedPinId;
	return _mappedToUnmappedPSorts[mappedPinId] != NO_UNMAPPED_PIN;
}

bool CircuitEnvironment::HasMappedPinIds(size_t unmappedPinId) const
{
	ASSERT(unmappedPinId < _unmappedToMappedPSorts.size()) << "Requested invalid pin id: " << unmappedPinId;
	return !_unmappedToMappedPSorts[unmappedPinId].empty();
}

size_t CircuitEnvironment::GetUnmappedPinId(size_t mappedPinId) const
{
	ASSERT(mappedPinId < _mappedToUnmappedPSorts.size()) << "Requested invalid pin id: " << mappedPinId;
	return _mappedToUnmappedPSorts[mappedPinId];
}

std::vector<size_t> CircuitEnvironment::GetMappedPinIds(size_t unmappedPinId) const
{
	ASSERT(unmappedPinId < _unmappedToMappedPSorts.size()) << "Requested invalid pin id: " << unmappedPinId;
	return _unmappedToMappedPSorts[unmappedPinId];
}

};
};
