#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <string>
#include <memory>
#include <utility>
#include <vector>

#include "Circuit/CircuitBuilder.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"
#include "Io/VerilogInstantiator/ModuleInstance.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{
namespace Primitives
{

/**
 * @brief Context for primitive to circuit generation.
 *
 * The primitive is responsible for assigning the input and output ports with the pins of the generated logic gates.
 * The input / output pins are then later connected by the VerilogInstantiator class to the other instantiated primitives.
 * The circuit between the input / output pins can be generated according to the needs of each primitive.
 */
struct PrimitiveContext
{
	PrimitiveContext(const Instance::Gate& gate, Circuit::Builder::GroupId group, size_t inputs, size_t outputs);
	virtual ~PrimitiveContext(void);

	const Instance::Gate& gate;
	const Circuit::Builder::GroupId group;

	std::vector<std::pair<Circuit::Builder::MappedNodeId, Circuit::Builder::PinId>> mappedInputs;
	std::vector<std::pair<Circuit::Builder::MappedNodeId, Circuit::Builder::PinId>> mappedOutputs;

	std::vector<std::pair<Circuit::Builder::UnmappedNodeId, Circuit::Builder::PinId>> unmappedInputs;
	std::vector<std::pair<Circuit::Builder::UnmappedNodeId, Circuit::Builder::PinId>> unmappedOutputs;

};

struct Primitive: Verilog::Module
{
    Primitive(std::string name, std::vector<Verilog::Port> inputs, std::vector<Verilog::Port> outputs);
	virtual ~Primitive(void);

	const std::vector<Verilog::Port> inputPorts;
	const std::vector<Verilog::Port> outputPorts;

    virtual bool ConvertToCircuit(Circuit::Builder::CircuitBuilder& builder, const Circuit::Builder::BuildConfiguration& configuration, PrimitiveContext& context) = 0;

};

struct PrimitiveNameAndPortCompare {
	bool operator()(const std::pair<std::string, size_t>& primitive1, const std::pair<std::string, size_t>& primitive2) const
	{
		return (primitive1.first != primitive2.first)
			? (primitive1.first < primitive2.first)
			: (primitive1.second < primitive2.second);
	}
};

struct primitive_name_and_ports_tag {};
using NamedPrimitive = std::pair<std::pair<std::string, size_t>, std::shared_ptr<Primitive>>;
using PrimitiveCollection = boost::multi_index_container<
    NamedPrimitive,
    boost::multi_index::indexed_by<
        boost::multi_index::random_access<>,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<primitive_name_and_ports_tag>,
            boost::multi_index::member<NamedPrimitive, std::pair<std::string, size_t>, &NamedPrimitive::first>,
			PrimitiveNameAndPortCompare
        >
    >
>;

};
};
};
};
