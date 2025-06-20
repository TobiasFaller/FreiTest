#include "Io/VerilogInstantiator/Primitive.hpp"

using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{
namespace Primitives
{

PrimitiveContext::PrimitiveContext(const Instance::Gate& gate, Builder::GroupId group, size_t inputs, size_t outputs):
	gate(gate),
	group(group),
	mappedInputs(inputs, std::make_pair(Builder::UNCONNECTED_MAPPED_PORT, 0u)),
	mappedOutputs(outputs, std::make_pair(Builder::UNCONNECTED_MAPPED_PORT, 0u)),
	unmappedInputs(inputs, std::make_pair(Builder::UNCONNECTED_UNMAPPED_PORT, 0u)),
	unmappedOutputs(outputs, std::make_pair(Builder::UNCONNECTED_UNMAPPED_PORT, 0u))
{
}

PrimitiveContext::~PrimitiveContext(void) = default;

Primitive::Primitive(std::string name, std::vector<Verilog::Port> inputs, std::vector<Verilog::Port> outputs):
	Verilog::Module(name),
	inputPorts(inputs),
	outputPorts(outputs)
{
}

Primitive::~Primitive(void) = default;

};
};
};
};
