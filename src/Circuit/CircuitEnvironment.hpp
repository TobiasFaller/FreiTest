#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <limits>

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/CircuitMetaData.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };

constexpr size_t NO_UNMAPPED_NODE = std::numeric_limits<size_t>::max();
constexpr size_t NO_UNMAPPED_PIN = std::numeric_limits<size_t>::max();
constexpr size_t NO_CONNECTION = std::numeric_limits<size_t>::max();

class CircuitEnvironment
{
public:
	CircuitEnvironment(void);
	virtual ~CircuitEnvironment(void);

	// Disallow move and copy
	CircuitEnvironment(CircuitEnvironment&& other) = delete;
	CircuitEnvironment(const CircuitEnvironment& other) = delete;
	CircuitEnvironment& operator=(CircuitEnvironment&& other) = delete;
	CircuitEnvironment& operator=(const CircuitEnvironment& other) = delete;

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

	friend MappedCircuit;
	friend UnmappedCircuit;
	friend Builder::CircuitBuilder;

private:
	std::vector<size_t> _mappedToUnmappedPSorts;
	std::vector<std::vector<size_t>> _unmappedToMappedPSorts;

	std::vector<std::vector<size_t>> _unmappedToMappedTSorts;
	std::vector<size_t> _mappedToUnmappedTSorts;

	UnmappedCircuit _unmappedCircuit;
	MappedCircuit _circuit;
	CircuitMetaData _metaData;

	std::string _circuitName;
};

};
};
