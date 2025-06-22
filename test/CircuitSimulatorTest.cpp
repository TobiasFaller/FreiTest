#define BOOST_TEST_MODULE CircuitSimulator
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <cmath>
#include <string>
#include <iostream>
#include <tuple>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Simulation/CircuitSimulator.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace FreiTest::Pattern;
using namespace FreiTest::Simulation;

struct Gate
{
	std::string name;
	bool sequential;
	SetResetModel setResetModel;
	CellCategory category;
	CellType type;
	size_t inputs;
	size_t outputs;
	std::vector<std::string> values;
};

const std::vector<Gate> GATES = {
	{ .name = "CONST0", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_CONSTANT, .type = CellType::PRESET_0,  .inputs = 0u, .outputs = 1u, .values = { "0" } },
	{ .name = "CONST1", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_CONSTANT, .type = CellType::PRESET_1,  .inputs = 0u, .outputs = 1u, .values = { "1" } },
	{ .name = "CONSTX", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_CONSTANT, .type = CellType::PRESET_X,  .inputs = 0u, .outputs = 1u, .values = { "X" } },
	{ .name = "CONSTU", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_CONSTANT, .type = CellType::PRESET_U,  .inputs = 0u, .outputs = 1u, .values = { "U" } },
	{ .name = "BUF",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_BUF,      .type = CellType::BUF,       .inputs = 1u, .outputs = 1u, .values = { "U01X" } },
	{ .name = "INV",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_INV,      .type = CellType::INV,       .inputs = 1u, .outputs = 1u, .values = { "U10X" } },
	{ .name = "XOR",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_XOR,      .type = CellType::XOR,       .inputs = 2u, .outputs = 1u, .values = { "UUUUU01XU10XUXXX" } },
	{ .name = "AND",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_AND,      .type = CellType::AND,       .inputs = 2u, .outputs = 1u, .values = { "U0UU0000U01XU0XX" } },
	{ .name = "OR",     .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_OR,       .type = CellType::OR,        .inputs = 2u, .outputs = 1u, .values = { "UU1UU01X1111UX1X" } },
	{ .name = "XNOR",   .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_XNOR,     .type = CellType::XNOR,      .inputs = 2u, .outputs = 1u, .values = { "UUUUU10XU01XUXXX" } },
	{ .name = "NAND",   .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_NAND,     .type = CellType::NAND,      .inputs = 2u, .outputs = 1u, .values = { "U1UU1111U10XU1XX" } },
	{ .name = "NOR",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_NOR,      .type = CellType::NOR,       .inputs = 2u, .outputs = 1u, .values = { "UU0UU10X0000UX0X" } },
	{ .name = "BUFIF0", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_BUFIF,    .type = CellType::BUFIF0,    .inputs = 2u, .outputs = 1u, .values = { "UUUUU01XUUUUUUUU" } },
	{ .name = "NOTIF0", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_NOTIF,    .type = CellType::NOTIF0,    .inputs = 2u, .outputs = 1u, .values = { "UUUUU10XUUUUUUUU" } },
	{ .name = "BUFIF1", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_BUFIF,    .type = CellType::BUFIF1,    .inputs = 2u, .outputs = 1u, .values = { "UUUUUUUUU01XUUUU" } },
	{ .name = "NOTIF1", .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_NOTIF,    .type = CellType::NOTIF1,    .inputs = 2u, .outputs = 1u, .values = { "UUUUUUUUU10XUUUU" } },
	{ .name = "MUX",    .sequential = false, .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_MUX,      .type = CellType::MUX,       .inputs = 3u, .outputs = 1u, .values = { "UUUUUUUUUUUUUUUU" "U01XU01XU01XU01X" "UUUU00001111XXXX" "XXXXXXXXXXXXXXXX", "UUUUUUUUUUUUUUUU" "U10XU10XU10XU10X" "UUUU11110000XXXX" "XXXXXXXXXXXXXXXX" } },
	{ .name = "DFF",    .sequential = true,  .setResetModel = SetResetModel::None,             .category = CellCategory::MAIN_UNKNOWN,  .type = CellType::UNDEFTYPE, .inputs = 3u, .outputs = 2u, .values = { "U01XU01XU01XU01X" "U01XU01XU01XU01X" "U01XU01XU01XU01X" "U01XU01XU01XU01X", "U10XU10XU10XU10X" "U10XU10XU10XU10X" "U10XU10XU10XU10X" "U10XU10XU10XU10X" } },
	{ .name = "DFFS",   .sequential = true,  .setResetModel = SetResetModel::OnlySet,          .category = CellCategory::MAIN_UNKNOWN,  .type = CellType::UNDEFTYPE, .inputs = 3u, .outputs = 2u, .values = { "UUUUU01X1111XXXX" "UUUUU01X1111XXXX" "UUUUU01X1111XXXX" "UUUUU01X1111XXXX", "UUUUU10X0000XXXX" "UUUUU10X0000XXXX" "UUUUU10X0000XXXX" "UUUUU10X0000XXXX" } },
	{ .name = "DFFR",   .sequential = true,  .setResetModel = SetResetModel::OnlyReset,        .category = CellCategory::MAIN_UNKNOWN,  .type = CellType::UNDEFTYPE, .inputs = 3u, .outputs = 2u, .values = { "UUUUUUUUUUUUUUUU" "U01XU01XU01XU01X" "0000000000000000" "XXXXXXXXXXXXXXXX", "UUUUUUUUUUUUUUUU" "U10XU10XU10XU10X" "1111111111111111" "XXXXXXXXXXXXXXXX" } },
	{ .name = "DFFSR",  .sequential = true,  .setResetModel = SetResetModel::SetHasPriority,   .category = CellCategory::MAIN_UNKNOWN,  .type = CellType::UNDEFTYPE, .inputs = 3u, .outputs = 2u, .values = { "UUUUUUUU1111XXXX" "UUUUU01X1111XXXX" "UUUU00001111XXXX" "UUUUXXXX1111XXXX", "UUUUUUUU0000XXXX" "UUUUU10X0000XXXX" "UUUU11110000XXXX" "UUUUXXXX0000XXXX" } },
	{ .name = "DFFRS",  .sequential = true,  .setResetModel = SetResetModel::ResetHasPriority, .category = CellCategory::MAIN_UNKNOWN,  .type = CellType::UNDEFTYPE, .inputs = 3u, .outputs = 2u, .values = { "UUUUUUUUUUUUUUUU" "UUUUU01X1111XXXX" "0000000000000000" "XXXXXXXXXXXXXXXX", "UUUUUUUUUUUUUUUU" "UUUUU10X0000XXXX" "1111111111111111" "XXXXXXXXXXXXXXXX" } },
};

Logic ConvertIndexToLogic(size_t index)
{
	return GetLogicValuesForString("U01X")[index];
}

int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace", "--log-verbose", "--log-verbose-modules=CircuitSimulator=9" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( CircuitSimulatorTest )

BOOST_AUTO_TEST_CASE( TestCombinationalGates )
{
	for (const auto& gate : GATES)
	{
		if (gate.sequential)
		{
			continue;
		}

		LOG(INFO) << "Testing combinational gate " << gate.name;

		std::vector<Builder::MappedNodeId> inputs;
		std::vector<Builder::ConnectionId> inputConnections;
		std::vector<Builder::MappedNodeId> outputs;
		std::vector<Builder::ConnectionId> outputConnections;

		Builder::CircuitBuilder builder;
		builder.SetName(gate.name);

		auto gateId = builder.EmplaceMappedNode("gate", gate.category, gate.type, gate.inputs);

		for (size_t index { 0u }; index < gate.inputs; index++)
		{
			auto inputId = inputs.emplace_back(builder.EmplaceMappedNode("input" + index, CellCategory::MAIN_IN, CellType::P_IN, 0u));
			auto connectionId = inputConnections.emplace_back(builder.EmplaceConnection());
			auto& mappedInput = builder.GetMappedNode(inputId);
			mappedInput.SetOutputConnectionId(connectionId);
			mappedInput.SetOutputConnectionName("input" + index);
			mappedInput.SetOutputPortName("out");
			mappedInput.AddSuccessorNode(gateId);
			builder.AddMappedPrimaryInput(inputId);

			auto& mappedGate = builder.GetMappedNode(gateId);
			mappedGate.SetInputConnectionId(index, connectionId);
			mappedGate.SetInputConnectionName(index, "input" + index);
			mappedGate.SetInputPortName(index, "in" + index);
			mappedGate.SetInputNode(index, inputId);
		}
		for (size_t index { 0u }; index < gate.outputs; index++)
		{
			auto outputId  = outputs.emplace_back(builder.EmplaceMappedNode("output" + index, CellCategory::MAIN_OUT, CellType::P_OUT, 1u));
			auto connectionId = outputConnections.emplace_back(builder.EmplaceConnection());
			auto& mappedOutput = builder.GetMappedNode(outputId);
			mappedOutput.SetInputConnectionId(0u, connectionId);
			mappedOutput.SetInputConnectionName(0u, "output" + index);
			mappedOutput.SetInputPortName(0u, "in");
			mappedOutput.SetInputNode(0u, gateId);
			builder.AddMappedPrimaryOutput(outputId);

			auto& mappedGate = builder.GetMappedNode(gateId);
			mappedGate.SetOutputConnectionId(connectionId);
			mappedGate.SetOutputConnectionName("output" + index);
			mappedGate.SetOutputPortName("out" + index);
			mappedGate.AddSuccessorNode(outputId);
		}

		Builder::BuildConfiguration config;
		auto env = builder.BuildCircuitEnvironment(config);
		const auto& mappedCircuit { env->GetMappedCircuit() };

		for (size_t value { 0u }; value < std::pow(2u, 2u * gate.inputs); value++)
		{
			TestPattern pattern(1u, gate.inputs, 0u);
			for (size_t index { 0u }; index < gate.inputs; index++)
			{
				auto inputIndex { (value >> (2u * index)) & 0x03 };
				pattern.SetPrimaryInput(0u, index, ConvertIndexToLogic(inputIndex));
			}

			SimulationResult result(1u, mappedCircuit.GetNumberOfNodes());
			SimulationConfig config { MakeSimulationConfig(MakeUnclockedSimpleFlipFlopModel()) };
			SimulateTestPatternNaive<FaultFreeModel>(mappedCircuit, pattern, {}, result, config);

			for (size_t index { 0u }; index < gate.outputs; index++)
			{
				BOOST_CHECK_EQUAL(
					result[0u][mappedCircuit.GetPrimaryOutput(index)->GetNodeId()],
					GetLogicForCharacter(gate.values[index][value])
				);
			}
		}
	}
}

BOOST_AUTO_TEST_CASE( TestSequentialGates )
{
	for (const auto& gate : GATES)
	{
		if (!gate.sequential)
		{
			continue;
		}

		LOG(INFO) << "Testing sequential gate " << gate.name;

		std::vector<Builder::MappedNodeId> inputs;
		std::vector<Builder::ConnectionId> inputConnections;
		std::vector<Builder::MappedNodeId> outputs;
		std::vector<Builder::ConnectionId> outputConnections;

		Builder::CircuitBuilder builder;
		builder.SetName(gate.name);

		auto secondaryInputId = builder.EmplaceMappedNode("secondary-input", CellCategory::MAIN_IN, CellType::S_IN, 0u);
		auto secondaryOutputId = builder.EmplaceMappedNode("secondary-output", CellCategory::MAIN_OUT, CellType::S_OUT, 4u);
		auto bufferId = builder.EmplaceMappedNode("secondary-output-q", CellCategory::MAIN_BUF, CellType::BUF, 1u);
		auto inverterId = builder.EmplaceMappedNode("secondary-output-qn", CellCategory::MAIN_INV, CellType::INV, 1u);
		auto constantId = builder.EmplaceMappedNode("dont-care", CellCategory::MAIN_CONSTANT, CellType::PRESET_X, 0u);

		for (size_t index { 0u }; index < gate.inputs; index++)
		{
			auto inputId = inputs.emplace_back(builder.EmplaceMappedNode("input" + index, CellCategory::MAIN_IN, CellType::P_IN, 0u));
			auto connectionId = inputConnections.emplace_back(builder.EmplaceConnection());
			auto& mappedInput = builder.GetMappedNode(inputId);
			mappedInput.SetOutputConnectionId(connectionId);
			mappedInput.SetOutputConnectionName("input" + index);
			mappedInput.SetOutputPortName("out");
			mappedInput.AddSuccessorNode(secondaryOutputId);
			builder.AddMappedPrimaryInput(inputId);

			// The clock / enable port is skipped and left unconnected.
			auto& mappedGate = builder.GetMappedNode(secondaryOutputId);
			mappedGate.SetInputConnectionId(index + ((index > 0u) ? 1u : 0u), connectionId);
			mappedGate.SetInputConnectionName(index + ((index > 0u) ? 1u : 0u), "input" + index);
			mappedGate.SetInputPortName(index + ((index > 0u) ? 1u : 0u), "in" + index);
			mappedGate.SetInputNode(index + ((index > 0u) ? 1u : 0u), inputId);
		}
		for (size_t index { 0u }; index < gate.outputs; index++)
		{
			auto outputId  = outputs.emplace_back(builder.EmplaceMappedNode("output" + index, CellCategory::MAIN_OUT, CellType::P_OUT, 1u));
			auto connectionId = outputConnections.emplace_back(builder.EmplaceConnection());
			auto& mappedOutput = builder.GetMappedNode(outputId);
			mappedOutput.SetInputConnectionId(0u, connectionId);
			mappedOutput.SetInputConnectionName(0u, "output" + index);
			mappedOutput.SetInputPortName(0u, "in");
			mappedOutput.SetInputNode(0u, (index == 0u) ? bufferId : inverterId);
			builder.AddMappedPrimaryOutput(outputId);

			auto& mappedGate = builder.GetMappedNode((index == 0u) ? bufferId : inverterId);
			mappedGate.SetOutputConnectionId(connectionId);
			mappedGate.SetOutputConnectionName("output" + index);
			mappedGate.SetOutputPortName("out" + index);
			mappedGate.AddSuccessorNode(outputId);
		}

		auto& secondaryInput = builder.GetMappedNode(secondaryInputId);
		auto& secondaryOutput = builder.GetMappedNode(secondaryOutputId);
		auto& buffer = builder.GetMappedNode(bufferId);
		auto& inverter = builder.GetMappedNode(inverterId);
		auto& constant = builder.GetMappedNode(constantId);
		builder.AddSecondaryInput(secondaryInputId);
		builder.AddSecondaryOutput(secondaryOutputId);
		builder.LinkSecondaryPorts(secondaryInputId, secondaryOutputId);

		auto constantConnectionId = builder.EmplaceConnection();
		constant.SetOutputConnectionId(constantConnectionId);
		constant.SetOutputConnectionName("constant");
		constant.SetOutputPortName("out");
		constant.AddSuccessorNode(secondaryOutputId);
		secondaryOutput.SetInputConnectionId(1u, constantConnectionId);
		secondaryOutput.SetInputConnectionName(1u, "constant");
		secondaryOutput.SetInputPortName(1u, "in");
		secondaryOutput.SetInputNode(1u, constantId);

		auto flipFlopConnectionId = builder.EmplaceConnection();
		secondaryInput.SetOutputConnectionId(flipFlopConnectionId);
		secondaryInput.SetOutputConnectionName("flipflop-output");
		secondaryInput.SetOutputPortName("out");
		secondaryInput.AddSuccessorNode(bufferId);
		secondaryInput.AddSuccessorNode(inverterId);
		buffer.SetInputConnectionId(0u, flipFlopConnectionId);
		buffer.SetInputConnectionName(0u, "flipflop-output");
		buffer.SetInputPortName(0u, "in");
		buffer.SetInputNode(0u, secondaryInputId);
		inverter.SetInputConnectionId(0u, flipFlopConnectionId);
		inverter.SetInputConnectionName(0u, "flipflop-output");
		inverter.SetInputPortName(0u, "in");
		inverter.SetInputNode(0u, secondaryInputId);

		Builder::BuildConfiguration config;
		auto env = builder.BuildCircuitEnvironment(config);
		const auto& mappedCircuit { env->GetMappedCircuit() };

		for (size_t value { 0u }; value < std::pow(2u, 2u * gate.inputs); value++)
		{
			TestPattern pattern(2u, gate.inputs, 1u);
			for (size_t index { 0u }; index < gate.inputs; index++)
			{
				auto inputIndex { (value >> (2u * index)) & 0x03 };
				pattern.SetPrimaryInput(0u, index, ConvertIndexToLogic(inputIndex));
			}
			LOG(INFO) << pattern;

			SimulationResult result(2u, mappedCircuit.GetNumberOfNodes());
			SimulationConfig config { MakeSimulationConfig({
				.sequentialModel = SequentialModel::Unclocked,
				.setResetModel = gate.setResetModel
			}) };
			SimulateTestPatternNaive<FaultFreeModel>(mappedCircuit, pattern, {}, result, config);

			for (size_t index { 0u }; index < gate.outputs; index++)
			{
				BOOST_CHECK_EQUAL(
					result[1u][mappedCircuit.GetPrimaryOutput(index)->GetNodeId()],
					GetLogicForCharacter(gate.values[index][value])
				);
			}
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
