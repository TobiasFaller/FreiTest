#include "Io/VerilogInstantiator/BuiltinPrimitives.hpp"

#include <boost/format.hpp>

#include <string>
#include <vector>
#include <memory>
#include <tuple>

#include "Basic/Settings.hpp"
#include "Basic/Logging.hpp"
#include "Circuit/CellLibrary.hpp"
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

Verilog::Port operator "" _In(const char* arr, size_t size)
{
    return Verilog::Port(std::string(arr, size), Verilog::PortType::PORT_IN, { 0, 0 });
}
Verilog::Port operator "" _Out(const char* arr, size_t size)
{
    return Verilog::Port(std::string(arr, size), Verilog::PortType::PORT_OUT, { 0, 0 });
}

struct BasicPrimitive: Primitive
{
    BasicPrimitive(std::string name, std::vector<Verilog::Port> inputs, std::vector<Verilog::Port> outputs, CellCategory cellCategory, CellType cellType):
        Primitive(name, inputs, outputs),
		cellCategory(cellCategory),
		cellType(cellType)
    {
		std::copy(outputs.begin(), outputs.end(), std::back_inserter(ports));
		std::copy(inputs.begin(), inputs.end(), std::back_inserter(ports));
    }

	bool ConvertToCircuit(Builder::CircuitBuilder& builder, const Builder::BuildConfiguration& configuration, PrimitiveContext& context) override
	{
		ASSERT(outputPorts.size()) << "Can not build primitive with no outputs. Override this method if you need to have a gate without outputs.";
		ASSERT(context.mappedInputs.size() == inputPorts.size());
		ASSERT(context.mappedOutputs.size() == outputPorts.size());
		ASSERT(context.unmappedInputs.size() == inputPorts.size());
		ASSERT(context.unmappedOutputs.size() == outputPorts.size());

		// Unmapped gate
		Builder::UnmappedNodeId unmappedNode = builder.EmplaceUnmappedNode(context.gate.name, name, inputPorts.size(), outputPorts.size());
		Builder::UnmappedNode& unmappedNodeRef = builder.GetUnmappedNode(unmappedNode);
		unmappedNodeRef.SetGroup(context.group);
		builder.GetGroup(context.group).AddUnmappedNode(unmappedNode);

        for (size_t input = 0u; input < inputPorts.size(); ++input)
        {
            context.unmappedInputs[input] = { unmappedNode, input };
			unmappedNodeRef.SetInputPortName(input, inputPorts[input].name);
        }
		context.unmappedOutputs[0u] = { unmappedNode, 0u };
		unmappedNodeRef.SetOutputPortName(0u, outputPorts[0u].name);

		Builder::MappedNodeId mappedNode = builder.EmplaceMappedNode(context.gate.name, cellCategory, cellType, inputPorts.size());
		Builder::MappedNode& mappedNodeRef = builder.GetMappedNode(mappedNode);
		mappedNodeRef.SetGroup(context.group);
		builder.LinkMappedToUnmappedNode(mappedNode, unmappedNode);
		builder.GetGroup(context.group).AddMappedNode(mappedNode);

		for (size_t input = 0u; input < inputPorts.size(); ++input)
		{
			context.mappedInputs[input] = { mappedNode, input };
			mappedNodeRef.SetInputPortName(input, inputPorts[input].name);
		}
		context.mappedOutputs[0u] = { mappedNode, 0u };
		mappedNodeRef.SetOutputPortName(outputPorts[0u].name);

		if (name == "input")
		{
			builder.AddUnmappedPrimaryInput(unmappedNode);
			builder.AddMappedPrimaryInput(mappedNode);
		}
		else if (name == "output")
		{
			builder.AddUnmappedPrimaryOutput(unmappedNode);
			builder.AddMappedPrimaryOutput(mappedNode);
		}
		else if (name == "inout")
		{
			builder.AddUnmappedPrimaryInput(unmappedNode);
			builder.AddMappedPrimaryInput(mappedNode);
			builder.AddUnmappedPrimaryOutput(unmappedNode);
			builder.AddMappedPrimaryOutput(mappedNode);
		}

		return true;
	}

protected:
	CellCategory cellCategory;
	CellType cellType;

};

struct SequentialPrimitive: BasicPrimitive
{
    template<typename... In, typename... Out>
    SequentialPrimitive(std::string name, std::vector<Verilog::Port> inputs, std::vector<Verilog::Port> outputs, CellCategory cellCategory, CellType cellType):
        BasicPrimitive(name, inputs, outputs, cellCategory, cellType)
    {
		assert(inputPorts.size() == 4);
		assert(outputPorts.size() == 2);
	}

	bool ConvertToCircuit(Builder::CircuitBuilder& builder, const Builder::BuildConfiguration& configuration, PrimitiveContext& context) override
	{
		assert(context.mappedInputs.size() == inputPorts.size());
		assert(context.mappedOutputs.size() == outputPorts.size());
		// Unmapped gate
		Builder::UnmappedNodeId unmappedNode = builder.EmplaceUnmappedNode(context.gate.name, name, inputPorts.size(), outputPorts.size());
		Builder::UnmappedNode& unmappedNodeRef = builder.GetUnmappedNode(unmappedNode);
		unmappedNodeRef.SetGroup(context.group);
		builder.GetGroup(context.group).AddUnmappedNode(unmappedNode);

		// Connection between S_IN, Q and QN
		Builder::ConnectionId qToBufferConnectionId = builder.EmplaceConnection();
		Builder::WireId qToBufferWire = builder.GetGroup(context.group).EmplaceWire(context.gate.name + "_q");
		builder.GetGroup(context.group).GetWire(qToBufferWire).SetConnections({ qToBufferConnectionId });
		const auto qToBufferName = builder.GetFullGroupName(context.group) + "/" + context.gate.name + "_q";

		// Output connection from S_OUT leading to nowhere
		Builder::ConnectionId soutConnectionId = builder.EmplaceConnection();
		Builder::WireId soutWire = builder.GetGroup(context.group).EmplaceWire(context.gate.name + "_sout");
		builder.GetGroup(context.group).GetWire(soutWire).SetConnections({ soutConnectionId });
		const auto soutName = builder.GetFullGroupName(context.group) + "/" + context.gate.name + "_sout";

        for (size_t input = 0u; input < inputPorts.size(); ++input)
        {
            context.unmappedInputs[input] = std::make_pair(unmappedNode, input);
			unmappedNodeRef.SetInputPortName(input, inputPorts[input].name);
        }
		for (size_t output = 0u; output < outputPorts.size(); ++output)
        {
            context.unmappedOutputs[output] = std::make_pair(unmappedNode, output);
			unmappedNodeRef.SetOutputPortName(output, outputPorts[output].name);
        }

		// Mapped gate
		Builder::MappedNodeId mappedOutputId = builder.EmplaceMappedNode(context.gate.name + "_sout", CellCategory::MAIN_OUT, cellType, 4);
		Builder::MappedNodeId mappedInputId = builder.EmplaceMappedNode(context.gate.name + "_sin", CellCategory::MAIN_IN, CellType::S_IN, 0);
		Builder::MappedNodeId mappedBufferId = builder.EmplaceMappedNode(context.gate.name + "_buf", CellCategory::MAIN_BUF, CellType::BUF, 1);
		Builder::MappedNodeId mappedInverterId = builder.EmplaceMappedNode(context.gate.name + "_inv", CellCategory::MAIN_INV, CellType::INV, 1);
		Builder::MappedNode& mappedOutput = builder.GetMappedNode(mappedOutputId);
		Builder::MappedNode& mappedInput = builder.GetMappedNode(mappedInputId);
		Builder::MappedNode& mappedBuffer = builder.GetMappedNode(mappedBufferId);
		Builder::MappedNode& mappedInverter = builder.GetMappedNode(mappedInverterId);
		mappedOutput.SetGroup(context.group);
		mappedInput.SetGroup(context.group);
		mappedBuffer.SetGroup(context.group);
		mappedInverter.SetGroup(context.group);
		builder.LinkMappedToUnmappedNode(mappedOutputId, unmappedNode);
		builder.LinkMappedToUnmappedNode(mappedInputId, unmappedNode);
		builder.LinkMappedToUnmappedNode(mappedBufferId, unmappedNode);
		builder.LinkMappedToUnmappedNode(mappedInverterId, unmappedNode);
		builder.GetGroup(context.group).AddMappedNode(mappedOutputId);
		builder.GetGroup(context.group).AddMappedNode(mappedInputId);
		builder.GetGroup(context.group).AddMappedNode(mappedBufferId);
		builder.GetGroup(context.group).AddMappedNode(mappedInverterId);
		builder.AddSecondaryOutput(mappedOutputId);
		builder.AddSecondaryInput(mappedInputId);
		builder.LinkSecondaryPorts(mappedInputId, mappedOutputId);

		mappedInput.AddSuccessorNode(mappedBufferId);
		mappedBuffer.SetInputNode(0u, mappedInputId);
		mappedInput.AddSuccessorNode(mappedInverterId);
		mappedInverter.SetInputNode(0u, mappedInputId);

		for (size_t input = 0u; input < 4u; ++input)
		{
			context.mappedInputs[input] = std::make_pair(mappedOutputId, input);
			mappedOutput.SetInputPortName(input, inputPorts[input].name);
		}
		mappedOutput.SetOutputPortName("s_out");
		mappedOutput.SetOutputConnectionName(soutName);
		mappedOutput.SetOutputConnectionId(soutConnectionId);

		// SIN has no input pin -> don't assign a name here
		mappedInput.SetOutputPortName("q_out");
		mappedInput.SetOutputConnectionName(qToBufferName);
		mappedInput.SetOutputConnectionId(qToBufferConnectionId);

		context.mappedOutputs[0u] = std::make_pair(mappedBufferId, 0u);
		mappedBuffer.SetInputPortName(0u, "q_in");
		mappedBuffer.SetInputConnectionName(0u, qToBufferName);
		mappedBuffer.SetInputConnectionId(0u, qToBufferConnectionId);
		mappedBuffer.SetOutputPortName(outputPorts[0u].name);

		context.mappedOutputs[1u] = std::make_pair(mappedInverterId, 0u);
		mappedInverter.SetInputPortName(0u, "q_in");
		mappedInverter.SetInputConnectionName(0u, qToBufferName);
		mappedInverter.SetInputConnectionId(0u, qToBufferConnectionId);
		mappedInverter.SetOutputPortName(outputPorts[1u].name);

		return true;
	}

};

template<typename T, typename...Args>
void EmplaceGate(PrimitiveCollection& primitives, std::string name, std::vector<Verilog::Port> inputs, std::vector<Verilog::Port> outputs, Args... args)
{
	primitives.emplace_back(
		std::make_pair(name, inputs.size() + outputs.size()),
		std::move(std::make_shared<T>(name, inputs, outputs, std::forward<Args>(args)...)));
}

void DefineConstantPrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<BasicPrimitive>(primitives, "tie0", { }, { "out"_Out }, CellCategory::MAIN_CONSTANT, CellType::PRESET_0);
	EmplaceGate<BasicPrimitive>(primitives, "tie1", { }, { "out"_Out }, CellCategory::MAIN_CONSTANT, CellType::PRESET_1);
	EmplaceGate<BasicPrimitive>(primitives, "tieX", { }, { "out"_Out }, CellCategory::MAIN_CONSTANT, CellType::PRESET_X);
	EmplaceGate<BasicPrimitive>(primitives, "tieU", { }, { "out"_Out }, CellCategory::MAIN_CONSTANT, CellType::PRESET_U);
}

void DefinePrimaryPortPrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<BasicPrimitive>(primitives, "input", { }, { "out"_Out }, CellCategory::MAIN_IN, CellType::P_IN);
	EmplaceGate<BasicPrimitive>(primitives, "output", { "in"_In }, { "out"_Out }, CellCategory::MAIN_OUT, CellType::P_OUT);
	EmplaceGate<BasicPrimitive>(primitives, "inout", { "in"_In }, { "out"_Out }, CellCategory::MAIN_INOUT, CellType::P_INOUT);
}

void DefineBufferPrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<BasicPrimitive>(primitives, "buf", { "in"_In }, { "out"_Out }, CellCategory::MAIN_BUF, CellType::BUF);
	EmplaceGate<BasicPrimitive>(primitives, "not", { "in"_In }, { "out"_Out }, CellCategory::MAIN_INV, CellType::INV);
}
void DefineCombinatorialPrimitives(PrimitiveCollection& primitives)
{
    EmplaceGate<BasicPrimitive>(primitives, "or", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_OR, CellType::OR);
    EmplaceGate<BasicPrimitive>(primitives, "nor", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_NOR, CellType::NOR);
    EmplaceGate<BasicPrimitive>(primitives, "xor", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_XOR, CellType::XOR);
    EmplaceGate<BasicPrimitive>(primitives, "xnor", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_XNOR, CellType::XNOR);
    EmplaceGate<BasicPrimitive>(primitives, "and", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_AND, CellType::AND);
    EmplaceGate<BasicPrimitive>(primitives, "nand", { "in1"_In, "in2"_In }, { "out"_Out }, CellCategory::MAIN_NAND, CellType::NAND);

    EmplaceGate<BasicPrimitive>(primitives, "or", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_OR, CellType::OR);
    EmplaceGate<BasicPrimitive>(primitives, "nor", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_NOR, CellType::NOR);
    EmplaceGate<BasicPrimitive>(primitives, "xor", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_XOR, CellType::XOR);
    EmplaceGate<BasicPrimitive>(primitives, "xnor", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_XNOR, CellType::XNOR);
    EmplaceGate<BasicPrimitive>(primitives, "and", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_AND, CellType::AND);
    EmplaceGate<BasicPrimitive>(primitives, "nand", { "in1"_In, "in2"_In, "in3"_In }, { "out"_Out }, CellCategory::MAIN_NAND, CellType::NAND);

    EmplaceGate<BasicPrimitive>(primitives, "or", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_OR, CellType::OR);
    EmplaceGate<BasicPrimitive>(primitives, "nor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_NOR, CellType::NOR);
    EmplaceGate<BasicPrimitive>(primitives, "xor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_XOR, CellType::XOR);
    EmplaceGate<BasicPrimitive>(primitives, "xnor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_XNOR, CellType::XNOR);
    EmplaceGate<BasicPrimitive>(primitives, "and", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_AND, CellType::AND);
    EmplaceGate<BasicPrimitive>(primitives, "nand", { "in1"_In, "in2"_In, "in3"_In, "in4"_In }, { "out"_Out }, CellCategory::MAIN_NAND, CellType::NAND);

    EmplaceGate<BasicPrimitive>(primitives, "or", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_OR, CellType::OR);
    EmplaceGate<BasicPrimitive>(primitives, "nor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_NOR, CellType::NOR);
    EmplaceGate<BasicPrimitive>(primitives, "xor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_XOR, CellType::XOR);
    EmplaceGate<BasicPrimitive>(primitives, "xnor", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_XNOR, CellType::XNOR);
    EmplaceGate<BasicPrimitive>(primitives, "and", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_AND, CellType::AND);
    EmplaceGate<BasicPrimitive>(primitives, "nand", { "in1"_In, "in2"_In, "in3"_In, "in4"_In, "in5"_In }, { "out"_Out }, CellCategory::MAIN_NAND, CellType::NAND);
}
void DefineMultiplexerPrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<BasicPrimitive>(primitives, "mux", { "in1"_In, "in2"_In, "select"_In }, { "out"_Out }, CellCategory::MAIN_MUX, CellType::MUX);
}
void DefineTristatePrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<BasicPrimitive>(primitives, "bufif1", { "in"_In, "enable"_In }, { "out"_Out }, CellCategory::MAIN_BUFIF, CellType::BUFIF1);
	EmplaceGate<BasicPrimitive>(primitives, "notif1", { "in"_In, "enable"_In }, { "out"_Out }, CellCategory::MAIN_NOTIF, CellType::NOTIF1);
	EmplaceGate<BasicPrimitive>(primitives, "bufif0", { "in"_In, "enable"_In }, { "out"_Out }, CellCategory::MAIN_BUFIF, CellType::BUFIF0);
	EmplaceGate<BasicPrimitive>(primitives, "notif0", { "in"_In, "enable"_In }, { "out"_Out }, CellCategory::MAIN_NOTIF, CellType::NOTIF0);
}
void DefineSequentialPrimitives(PrimitiveCollection& primitives)
{
	EmplaceGate<SequentialPrimitive>(primitives, "dsequential", { "d"_In, "clock"_In, "set"_In, "reset"_In }, { "q"_Out, "qn"_Out }, CellCategory::MAIN_UNKNOWN, CellType::S_OUT);
    EmplaceGate<SequentialPrimitive>(primitives, "dflipflop", { "d"_In,"clock"_In, "set"_In, "reset"_In }, { "q"_Out, "qn"_Out }, CellCategory::MAIN_UNKNOWN, CellType::S_OUT_CLK);
    EmplaceGate<SequentialPrimitive>(primitives, "dlatch", { "d"_In, "enable"_In, "set"_In, "reset"_In }, { "q"_Out, "qn"_Out }, CellCategory::MAIN_UNKNOWN, CellType::S_OUT_EN);
}

};
};
};
};
