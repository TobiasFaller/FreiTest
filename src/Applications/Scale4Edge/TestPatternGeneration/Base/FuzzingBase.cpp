#include "Applications/Scale4Edge/TestPatternGeneration/Base/FuzzingBase.hpp"

#include <boost/format.hpp>

#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>
#include <iomanip>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Io/CircuitVerilogExporter/CircuitVerilogExporter.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/Sat/Glucose421ParallelSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel, typename FaultList>
FuzzingBase<FaultModel, FaultList>::FuzzingBase(void):
	Mixin::StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>()
{
}

template <typename FaultModel, typename FaultList>
FuzzingBase<FaultModel, FaultList>::~FuzzingBase(void) = default;

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::Init(void)
{
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::Run(void)
{
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::GenerateCircuit(size_t seed)
{
	struct Output {
		Builder::MappedNodeId nodeId;
		Builder::ConnectionId connId;
	};

	auto const get_hex_string = [](size_t value) {
		std::stringstream stream;
		stream << std::setfill ('0') << std::setw(sizeof(size_t)*2) << std::hex << value;
		return stream.str();
	};

	Builder::CircuitBuilder builder;
	builder.SetName("circuit_" + get_hex_string(seed));
	auto const circuitGroupId { builder.EmplaceGroup("circuit_" + get_hex_string(seed)) };

	std::mt19937_64 random { seed };
	size_t inputCount { (std::uniform_int_distribution<size_t> { configNumInputsMin, configNumInputsMax })(random) };
	size_t outputCount { (std::uniform_int_distribution<size_t> { configNumOutputsMin, configNumOutputsMax })(random) };
	size_t cellCount { (std::uniform_int_distribution<size_t> { configNumCellsMin, configNumCellsMax })(random) };
	std::vector<Output> inputs { };
	std::vector<Output> cells { };
	std::vector<Output> outputs { };

	LOG(INFO) << "Generating circuit " << get_hex_string(seed)
		<< " (I: " << inputCount << " / O: " << outputCount << " / C: " << cellCount << ")";

	// Inputs
	for (size_t index { 0u }; index < inputCount; index++) {
		auto const name { "input_" + std::to_string(index) };
		auto const gateId { builder.EmplaceMappedNode(name, CellCategory::MAIN_IN, CellType::P_IN, 0u) };
		auto const groupId { builder.EmplaceGroup(name) };
		auto const connId { builder.EmplaceConnection() };
		auto const portId { builder.GetGroup(groupId).EmplacePort("O", Builder::PortType::Output) };
		auto const sourceId { builder.AddSourceInfo({}) };
		auto const circuitPortId { builder.GetGroup(circuitGroupId).EmplacePort("I" + std::to_string(index + 1), Builder::PortType::Input) };
		auto const circuitPortSourceId { builder.AddSourceInfo({}) };
		builder.GetMappedNode(gateId).SetOutputConnectionId(connId);
		builder.GetMappedNode(gateId).SetOutputConnectionName("conn_" + std::to_string(connId));
		builder.GetMappedNode(gateId).SetOutputPortName("O");
		builder.GetGroup(circuitGroupId).AddGroup(groupId);
		builder.GetGroup(circuitGroupId).GetPort(circuitPortId).SetConnections({ connId });
		builder.GetGroup(circuitGroupId).GetPort(circuitPortId).SetSourceInfo(circuitPortSourceId);
		builder.GetGroup(groupId).SetParent(circuitGroupId);
		builder.GetGroup(groupId).AddMappedNode(gateId);
		builder.GetGroup(groupId).GetPort(portId).SetConnections({ connId });
		builder.GetGroup(groupId).SetSourceInfo(sourceId);
		builder.AddMappedPrimaryInput(gateId);
		builder.GetSourceInfo(circuitPortSourceId).AddProperty<std::string>("port-type", "input");
		builder.GetSourceInfo(sourceId).sourceType = "port";
		inputs.push_back({ gateId, connId });
	}

	// Cells
	this->cells.clear();
	for (size_t index { 0u }; index < cellCount; index++) {
		auto const get_random_connections = [&](size_t count) {
			auto const available { inputs.size() + cells.size() - 1u };
			std::vector<size_t> indices;
			for (size_t connection { 0u }; (connection <= available) && (connection < count); connection++) {
				auto chosen { (std::uniform_int_distribution<size_t> { 0u, (available - connection) })(random) };
				// Skip gaps of taken values (assumes connections are sorted)
				for (auto& index : indices) {
					if (chosen >= index) chosen++;
				}
				indices.push_back(chosen);
				std::sort(indices.begin(), indices.end());
			}
			// Only allow duplicates if necessary
			while (indices.size() < count) {
				auto chosen { (std::uniform_int_distribution<size_t> { 0u, available })(random) };
				indices.push_back(chosen);
			}
			std::vector<Output> connections;
			for (auto& index : indices) {
				connections.push_back((index >= inputs.size())
					? cells[index - inputs.size()] : inputs[index]);
			}
			return connections;
		};

		auto const name { "cell_" + std::to_string(index) };
		auto const cellGroupId { builder.EmplaceGroup(name) };
		auto const sourceInfoId { builder.AddSourceInfo({ }) };
		builder.GetGroup(circuitGroupId).AddGroup(cellGroupId);
		builder.GetGroup(cellGroupId).SetParent(circuitGroupId);
		builder.GetGroup(cellGroupId).SetSourceInfo(sourceInfoId);
		builder.GetSourceInfo(sourceInfoId).sourceType = "module";
		builder.GetSourceInfo(sourceInfoId).AddProperty<bool>("module-is-cell", true);
		builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-name", name);
		builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-type", name);

		// Generate inputs for cell
		std::vector<Output> cellInputs;
		size_t cellInputCount { (std::uniform_int_distribution<size_t> { configNumCellInputsMin, configNumCellInputsMax })(random) };
		auto cellInputConnections { get_random_connections(cellInputCount) };
		for (size_t i { 0u }; i < cellInputCount; i++) {
			auto const [gateId, connId] = cellInputConnections[i];
			auto const portId { builder.GetGroup(cellGroupId).EmplacePort(
				"I" + std::to_string(i + 1u), Builder::PortType::Input) };
			auto const portInfoId { builder.AddSourceInfo({}) };
			builder.GetGroup(cellGroupId).GetPort(portId).SetConnections({ connId });
			builder.GetGroup(cellGroupId).GetPort(portId).SetSourceInfo(portInfoId);
			builder.GetSourceInfo(portInfoId).AddProperty<std::string>("port-type", "input");
			cellInputs.push_back(Output { gateId, connId });
		}

		const std::vector<std::tuple<std::string, CellCategory, CellType, size_t>> gateTypes {
			{ "SKIP", CellCategory::MAIN_UNKNOWN, CellType::UNDEFTYPE, 1u },
			{ "DFF",  CellCategory::MAIN_UNKNOWN, CellType::UNDEFTYPE, 1u },
			{ "BUF",  CellCategory::MAIN_BUF,     CellType::BUF,       1u },
			{ "INV",  CellCategory::MAIN_INV,     CellType::INV,       1u },
			{ "AND",  CellCategory::MAIN_AND,     CellType::AND,       2u },
			{ "NAND", CellCategory::MAIN_NAND,    CellType::NAND,      2u },
			{ "OR",   CellCategory::MAIN_OR,      CellType::OR,        2u },
			{ "NOR",  CellCategory::MAIN_NOR,     CellType::NOR,       2u },
			{ "XOR",  CellCategory::MAIN_XOR,     CellType::XOR,       2u },
			{ "XNOR", CellCategory::MAIN_XNOR,    CellType::XNOR,      2u },
		};
		size_t cellIndex { 0u };
		size_t flipFlopCount { 0u };
		std::vector<std::string> logic;
		std::vector<Output> cellOutputs;
		std::copy(cellInputs.begin(), cellInputs.end(), std::back_inserter(cellOutputs));
		while (cellOutputs.size() > 1u) {
			std::vector<Output> newOutputs;
			for (size_t i { 0u }; i < cellOutputs.size(); ) {
				auto const gateTypeMax { (i + 1 < cellOutputs.size()) ? (gateTypes.size() - 1u) : 3u };
				auto const gateTypeIndex { (std::uniform_int_distribution<size_t> { 0, gateTypeMax })(random) };
				auto const [gateName, gateCategory, gateType, gateSize] = gateTypes[gateTypeIndex];
				logic.push_back(gateName);

				if (gateName == "SKIP") {
					newOutputs.push_back(cellOutputs[i]);
				} else if (gateName == "DFF") {
					auto const nameSi { "gate_" + std::to_string(index) + "_" + std::to_string(cellIndex + 0u) };
					auto const nameSo { "gate_" + std::to_string(index) + "_" + std::to_string(cellIndex + 1u) };
					auto const nameC0 { "gate_" + std::to_string(index) + "_" + std::to_string(cellIndex + 2u) };
					auto const gateSiId { builder.EmplaceMappedNode(nameSi, CellCategory::MAIN_IN,       CellType::S_IN,     0u) };
					auto const gateSoId { builder.EmplaceMappedNode(nameSo, CellCategory::MAIN_OUT,      CellType::S_OUT,    4u) };
					auto const gateC0Id { builder.EmplaceMappedNode(nameC0, CellCategory::MAIN_CONSTANT, CellType::PRESET_0, 0u) };
					auto const groupSiId { builder.EmplaceGroup(nameSi) };
					auto const groupSoId { builder.EmplaceGroup(nameSo) };
					auto const groupC0Id { builder.EmplaceGroup(nameC0) };
					auto const connSiId { builder.EmplaceConnection() };
					auto const connSoId { builder.EmplaceConnection() };
					auto const connC0Id { builder.EmplaceConnection() };
					auto const sourceInfoSiId { builder.AddSourceInfo({}) };
					auto const sourceInfoSoId { builder.AddSourceInfo({}) };
					auto const sourceInfoC0Id { builder.AddSourceInfo({}) };
					auto const portInputDSoId { builder.GetGroup(groupSoId).EmplacePort("GI_D", Builder::PortType::Input) };
					auto const portInputCSoId { builder.GetGroup(groupSoId).EmplacePort("GI_C", Builder::PortType::Input) };
					auto const portInputSSoId { builder.GetGroup(groupSoId).EmplacePort("GI_S", Builder::PortType::Input) };
					auto const portInputRSoId { builder.GetGroup(groupSoId).EmplacePort("GI_R", Builder::PortType::Input) };
					auto const portOutputSiId { builder.GetGroup(groupSiId).EmplacePort("GO", Builder::PortType::Output) };
					auto const portOutputSoId { builder.GetGroup(groupSoId).EmplacePort("GO", Builder::PortType::Output) };
					auto const [inputGateId, inputConnId] = cellOutputs[i];
					builder.GetMappedNode(gateSoId).SetInputNode(0u, inputGateId);
					builder.GetMappedNode(gateSoId).SetInputConnectionId(0u, inputConnId);
					builder.GetMappedNode(gateSoId).SetInputConnectionName(0u, "conn_" + std::to_string(inputConnId));
					builder.GetMappedNode(gateSoId).SetInputPortName(0u, "GI_D");
					builder.GetMappedNode(inputGateId).AddSuccessorNode(gateSoId);
					builder.GetGroup(groupSoId).GetPort(portInputDSoId).SetConnections({ inputConnId });
					builder.GetMappedNode(gateSoId).SetInputNode(1u, gateC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionId(1u, connC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionName(1u, "conn_" + std::to_string(connC0Id));
					builder.GetMappedNode(gateSoId).SetInputPortName(1u, "GI_C");
					builder.GetMappedNode(gateC0Id).AddSuccessorNode(gateSoId);
					builder.GetGroup(groupSoId).GetPort(portInputCSoId).SetConnections({ connC0Id });
					builder.GetMappedNode(gateSoId).SetInputNode(2u, gateC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionId(2u, connC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionName(2u, "conn_" + std::to_string(connC0Id));
					builder.GetMappedNode(gateSoId).SetInputPortName(2u, "GI_S");
					builder.GetMappedNode(gateC0Id).AddSuccessorNode(gateSoId);
					builder.GetGroup(groupSoId).GetPort(portInputSSoId).SetConnections({ connC0Id });
					builder.GetMappedNode(gateSoId).SetInputNode(3u, gateC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionId(3u, connC0Id);
					builder.GetMappedNode(gateSoId).SetInputConnectionName(3u, "conn_" + std::to_string(connC0Id));
					builder.GetMappedNode(gateSoId).SetInputPortName(3u, "GI_R");
					builder.GetMappedNode(gateC0Id).AddSuccessorNode(gateSoId);
					builder.GetGroup(groupSoId).GetPort(portInputRSoId).SetConnections({ connC0Id });
					builder.GetMappedNode(gateSoId).SetOutputConnectionId(connSoId);
					builder.GetMappedNode(gateSoId).SetOutputConnectionName("conn_" + std::to_string(connSoId));
					builder.GetMappedNode(gateSoId).SetOutputPortName("GO");
					builder.GetGroup(groupSoId).GetPort(portOutputSoId).SetConnections({ connSoId });
					builder.GetMappedNode(gateSiId).SetOutputConnectionId(connSiId);
					builder.GetMappedNode(gateSiId).SetOutputConnectionName("conn_" + std::to_string(connSiId));
					builder.GetMappedNode(gateSiId).SetOutputPortName("GO");
					builder.GetGroup(groupSiId).GetPort(portOutputSiId).SetConnections({ connSiId });
					builder.GetGroup(cellGroupId).AddGroup(groupSiId);
					builder.GetGroup(cellGroupId).AddGroup(groupSoId);
					builder.GetGroup(cellGroupId).AddGroup(groupC0Id);
					builder.GetGroup(groupSiId).SetParent(cellGroupId);
					builder.GetGroup(groupSoId).SetParent(cellGroupId);
					builder.GetGroup(groupC0Id).SetParent(cellGroupId);
					builder.GetGroup(groupSiId).AddMappedNode(gateSiId);
					builder.GetGroup(groupSoId).AddMappedNode(gateSoId);
					builder.GetGroup(groupC0Id).AddMappedNode(gateC0Id);
					builder.GetGroup(groupSiId).SetSourceInfo(sourceInfoSiId);
					builder.GetGroup(groupSoId).SetSourceInfo(sourceInfoSoId);
					builder.GetGroup(groupC0Id).SetSourceInfo(sourceInfoC0Id);
					builder.GetSourceInfo(sourceInfoSiId).sourceType = "module";
					builder.GetSourceInfo(sourceInfoSiId).AddProperty<bool>("module-is-primitive", true);
					builder.GetSourceInfo(sourceInfoSiId).AddProperty<std::string>("module-name", nameSi);
					builder.GetSourceInfo(sourceInfoSiId).AddProperty<std::string>("module-type", nameSi);
					builder.GetSourceInfo(sourceInfoSoId).sourceType = "module";
					builder.GetSourceInfo(sourceInfoSoId).AddProperty<bool>("module-is-primitive", true);
					builder.GetSourceInfo(sourceInfoSoId).AddProperty<std::string>("module-name", nameSo);
					builder.GetSourceInfo(sourceInfoSoId).AddProperty<std::string>("module-type", nameSo);
					builder.GetSourceInfo(sourceInfoC0Id).sourceType = "module";
					builder.GetSourceInfo(sourceInfoC0Id).AddProperty<bool>("module-is-primitive", true);
					builder.GetSourceInfo(sourceInfoC0Id).AddProperty<std::string>("module-name", nameC0);
					builder.GetSourceInfo(sourceInfoC0Id).AddProperty<std::string>("module-type", nameC0);
					builder.AddSecondaryInput(gateSiId);
					builder.AddSecondaryOutput(gateSoId);
					builder.LinkSecondaryPorts(gateSiId, gateSoId);
					newOutputs.push_back(Output { gateSiId, connSiId });
					cellIndex += 3;
					flipFlopCount++;
				} else {
					auto const name { "gate_" + std::to_string(index) + "_" + std::to_string(cellIndex) };
					auto const gateId { builder.EmplaceMappedNode(name, gateCategory, gateType, gateSize) };
					auto const groupId { builder.EmplaceGroup(name) };
					auto const connId { builder.EmplaceConnection() };
					auto const sourceInfoId { builder.AddSourceInfo({}) };
					for (size_t input { 0u }; input < gateSize; input++) {
						auto const portId { builder.GetGroup(groupId).EmplacePort("GI" + std::to_string(input + 1), Builder::PortType::Input) };
						auto const [inputGateId, inputConnId] = cellOutputs[i + input];
						builder.GetMappedNode(gateId).SetInputNode(input, inputGateId);
						builder.GetMappedNode(gateId).SetInputConnectionId(input, inputConnId);
						builder.GetMappedNode(gateId).SetInputConnectionName(input, "conn_" + std::to_string(inputConnId));
						builder.GetMappedNode(gateId).SetInputPortName(input, "GI" + std::to_string(input + 1));
						builder.GetMappedNode(inputGateId).AddSuccessorNode(gateId);
						builder.GetGroup(groupId).GetPort(portId).SetConnections({ inputConnId });
					}
					auto const portOutputId { builder.GetGroup(groupId).EmplacePort("GO", Builder::PortType::Output) };
					builder.GetMappedNode(gateId).SetOutputConnectionId(connId);
					builder.GetMappedNode(gateId).SetOutputConnectionName("conn_" + std::to_string(connId));
					builder.GetMappedNode(gateId).SetOutputPortName("GO");
					builder.GetGroup(groupId).GetPort(portOutputId).SetConnections({ connId });
					builder.GetGroup(cellGroupId).AddGroup(groupId);
					builder.GetGroup(groupId).SetParent(cellGroupId);
					builder.GetGroup(groupId).AddMappedNode(gateId);
					builder.GetGroup(groupId).SetSourceInfo(sourceInfoId);
					builder.GetSourceInfo(sourceInfoId).sourceType = "module";
					builder.GetSourceInfo(sourceInfoId).AddProperty<bool>("module-is-primitive", true);
					builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-name", name);
					builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-type", gateName);
					newOutputs.push_back(Output { gateId, connId });
					cellIndex++;
				}
				i += gateSize;
			}
			cellOutputs = std::move(newOutputs);
		}
		auto const portId { builder.GetGroup(cellGroupId).EmplacePort("O", Builder::PortType::Output) };
		auto const portInfoId { builder.AddSourceInfo({}) };
		builder.GetGroup(cellGroupId).GetPort(portId).SetConnections({ cellOutputs[0u].connId });
		builder.GetGroup(cellGroupId).GetPort(portId).SetSourceInfo(portInfoId);
		builder.GetSourceInfo(portInfoId).AddProperty<std::string>("port-type", "output");
		cells.push_back({ cellOutputs[0u].nodeId, cellOutputs[0u].connId });

		std::vector<std::string> inputs;
		for (size_t index { 0u }; index < cellInputCount; index++) {
			inputs.push_back("I" + std::to_string(index + 1u));
		}
		this->cells.push_back(Cell {
			.name = name,
			.inputs = inputs,
			.outputs = { "O" },
			.logic = logic,
			.flipFlopCount = flipFlopCount,
		});
	}

	// Outputs
	for (size_t index { 0u }; index < outputCount; index++) {
		auto const get_random_connection = [&]() {
			std::uniform_int_distribution<size_t> randomInput { 0u, cells.size() - 1u };
			return cells[randomInput(random)];
		};

		auto const name { "output_" + std::to_string(index) };
		auto const gateId { builder.EmplaceMappedNode(name, CellCategory::MAIN_OUT, CellType::P_OUT, 1u) };
		auto const groupId { builder.EmplaceGroup(name) };
		auto const connId { builder.EmplaceConnection() };
		auto const sourceId { builder.AddSourceInfo({}) };
		auto const inputPortId { builder.GetGroup(groupId).EmplacePort("I", Builder::PortType::Input) };
		auto const outputPortId { builder.GetGroup(groupId).EmplacePort("O", Builder::PortType::Output) };
		auto const circuitPortId { builder.GetGroup(circuitGroupId).EmplacePort("O" + std::to_string(index + 1), Builder::PortType::Output) };
		auto const circuitPortSourceId { builder.AddSourceInfo({}) };
		auto const [inputGateId, inputConnId] = get_random_connection();
		builder.GetMappedNode(gateId).SetInputNode(0u, inputGateId);
		builder.GetMappedNode(gateId).SetInputConnectionId(0u, inputConnId);
		builder.GetMappedNode(gateId).SetInputConnectionName(0u, "conn_" + std::to_string(inputConnId));
		builder.GetMappedNode(gateId).SetInputPortName(0u, "I");
		builder.GetMappedNode(gateId).SetOutputConnectionId(connId);
		builder.GetMappedNode(gateId).SetOutputConnectionName("conn_" + std::to_string(connId));
		builder.GetMappedNode(gateId).SetOutputPortName("O");
		builder.GetMappedNode(inputGateId).AddSuccessorNode(gateId);
		builder.GetGroup(circuitGroupId).AddGroup(groupId);
		builder.GetGroup(circuitGroupId).GetPort(circuitPortId).SetConnections({ inputConnId });
		builder.GetGroup(circuitGroupId).GetPort(circuitPortId).SetSourceInfo(circuitPortSourceId);
		builder.GetGroup(groupId).SetParent(circuitGroupId);
		builder.GetGroup(groupId).AddMappedNode(gateId);
		builder.GetGroup(groupId).GetPort(inputPortId).SetConnections({ inputConnId });
		builder.GetGroup(groupId).GetPort(outputPortId).SetConnections({ connId });
		builder.GetGroup(groupId).SetSourceInfo(sourceId);
		builder.AddMappedPrimaryOutput(gateId);
		builder.GetSourceInfo(circuitPortSourceId).AddProperty<std::string>("port-type", "output");
		builder.GetSourceInfo(sourceId).sourceType = "port";
		outputs.push_back(Output { gateId, connId });
	}

	Builder::BuildConfiguration buildConfig { };
	buildConfig.mappedDisallowCycles = true;
	buildConfig.mappedRemoveUnconnectedGates = true;
	buildConfig.mappedDisallowUnconnectedInputs = true;
	this->circuit = builder.BuildCircuitEnvironment(buildConfig);
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::GenerateFaultModel(size_t seed, UdfmType udfmType)
{
	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		// Nothing to do
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		// Nothing to do
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		std::mt19937_64 random { seed };
		auto const simulate_cell = [](auto const& cell, auto const& input, auto const& state) -> auto {
			auto const get_index = [](auto const& value) -> size_t {
				return (value == 'X') ? 2u : (value - '0');
			};
			std::string output = input;
			size_t logicIndex { 0u };
			size_t dffIndex { 0u };
			std::string newState;
			while (output.size() > 1u) {
				std::string newOutputs { };
				for (size_t outputIndex { 0u }; outputIndex < output.size();) {
					if (cell.logic[logicIndex] == "SKIP") {
						newOutputs += output[outputIndex];
						outputIndex += 1u;
					} else if (cell.logic[logicIndex] == "DFF") {
						newState += output[outputIndex];
						newOutputs += state[dffIndex++];
						outputIndex += 1u;
					} else if (cell.logic[logicIndex] == "BUF" || cell.logic[logicIndex] == "INV") {
						auto const index { get_index(output[outputIndex + 0u]) };
						std::string lookup;
						if (cell.logic[logicIndex] == "BUF") lookup = "01X";
						if (cell.logic[logicIndex] == "INV") lookup = "10X";
						newOutputs += lookup.substr(index, 1u);
						outputIndex += 1u;
					} else {
						auto const index { get_index(output[outputIndex + 0u]) + 3 * get_index(output[outputIndex + 1u]) };
						std::string lookup;
						if (cell.logic[logicIndex] == "AND")  lookup = "00001X0XX";
						if (cell.logic[logicIndex] == "NAND") lookup = "11110X1XX";
						if (cell.logic[logicIndex] == "OR")   lookup = "01X111X1X";
						if (cell.logic[logicIndex] == "NOR")  lookup = "10X000X0X";
						if (cell.logic[logicIndex] == "XOR")  lookup = "01X10XXXX";
						if (cell.logic[logicIndex] == "XNOR") lookup = "10X01XXXX";
						newOutputs += lookup.substr(index, 1u);
						outputIndex += 2u;
					}
					logicIndex++;
				}
				output = newOutputs;
			}
			return std::make_tuple(output, newState);
		};
		auto const build_tree = [&random](auto const leaves, auto const maxBranches) -> auto {
			std::vector<std::vector<size_t>> tree { {} };
			std::vector<size_t> incomplete { 0u };
			for (size_t index { 0u }; index < leaves; index++) {
				auto const incompleteIndex { (std::uniform_int_distribution<size_t> { 0u, incomplete.size() - 1u })(random) };
				auto const toExtend { incomplete[incompleteIndex] };
				// If there is no branch (it is a leaf) insert first one
				if (tree[toExtend].size() == 0u && toExtend != 0u) {
					tree.push_back({ });
					tree[toExtend].push_back(tree.size() - 1u);
					incomplete.push_back(tree.size() - 1u);
				}
				// Insert new branch
				tree.push_back({ });
				tree[toExtend].push_back(tree.size() - 1u);
				incomplete.push_back(tree.size() - 1u);
				// If the number of branches have been exhausted delist the tree node
				if (tree[toExtend].size() >= maxBranches) {
					incomplete.erase(std::find(incomplete.begin(), incomplete.end(), toExtend));
				}
			}
			return tree;
		};
		auto const get_parents = [](auto const& tree) -> auto {
			std::vector<size_t> parents(tree.size(), std::numeric_limits<size_t>::max());
			for (size_t index { 0u }; index < tree.size(); index++) {
				for (auto const& child : tree[index]) {
					parents[child] = index;
				}
			}
			return parents;
		};
		auto const to_binary_string = [](auto value, auto const& bits) {
			std::string result;
			for (size_t bit { 0u }; bit < bits; bit++) {
				result += ((value & 1) ? "1" : "0");
				value >>= 1;
			}
			return result;
		};

		this->udfm = std::make_unique<Io::Udfm::UdfmModel>();
		for (auto& cell : this->cells) {
			// We can not test cells that are themselves part of the scan chain.
			if (udfmType == UdfmType::FullScan && cell.flipFlopCount > 0) {
				continue;
			}
			auto udfmCell { this->udfm->AddCell(cell.name) };
			size_t faultCount { (std::uniform_int_distribution<size_t> { configNumUdfmFaultsMin, configNumUdfmFaultsMax })(random) };
			for (size_t fault { 0u }; fault < faultCount; fault++) {
				auto const alternativeCount { (std::uniform_int_distribution<size_t> { configNumUdfmAltsMin, configNumUdfmAltsMax })(random) };
				auto const maxBranches { 1u << cell.inputs.size() };
				auto const tree { build_tree(alternativeCount, maxBranches) };
				auto const parents { get_parents(tree) };

				std::vector<std::vector<std::string>> allConditions(tree.size(), std::vector<std::string> { });
				std::vector<std::vector<std::string>> allEffects(tree.size(), std::vector<std::string> { });
				std::vector<std::string> allStates(tree.size(), std::string { }); // For D-Flip-Flops in cells

				for (size_t flipFlop { 0u }; flipFlop < cell.flipFlopCount; flipFlop++) {
					// For full-scan we require specific flip-flop value
					if (udfmType == UdfmType::FullScan) {
						auto value { (std::uniform_int_distribution<size_t> { 0u, 1u })(random) };
						allStates[0u] += '0' + value;
					} else {
						allStates[0u] += 'X';
					}
				}

				for (size_t index { 0u }; index < tree.size(); index++) {
					std::vector<size_t> transitionConditions;
					for (size_t child { 0u }; child < tree[index].size(); child++) {
						auto condition { (std::uniform_int_distribution<size_t> { 0u, maxBranches - 1u - child })(random) };
						// Skip gaps of taken values (assumes conditions are sorted)
						for (auto& transitionCondition : transitionConditions) {
							if (condition >= transitionCondition) condition++;
						}
						transitionConditions.push_back(condition);
						std::sort(transitionConditions.begin(), transitionConditions.end());
					}
					// Convert binary value to input string
					for (size_t child { 0u }; child < tree[index].size(); child++) {
						auto const condition { to_binary_string(transitionConditions[child], cell.inputs.size()) };
						auto const effectTransformation { (std::uniform_int_distribution<size_t> { 0u, configNumUdfmDontCare })(random) };
						auto const isLeafTransition { tree[tree[index][child]].size() == 0u };
						auto [effect, state] = simulate_cell(cell, condition, allStates[index]);
						if (isLeafTransition) effect = (effect == "X") ? "X" : ((effect == "0") ? "1" : "0"); // Invert effect
						if (effectTransformation == configNumUdfmDontCare) effect = "X"; // No effect
						allConditions[index].push_back(condition);
						allEffects[index].push_back(effect);
						allStates[tree[index][child]] = state;
					}
				}

				auto udfmFault { udfmCell->AddFault("fault_" + std::to_string(fault), "pseudo-fault") };
				for (size_t index { 0u }; index < tree.size(); index++) {
					// Only include leaves
					if (tree[index].size() != 0u) {
						continue;
					}

					// Trace back the leaf to the root ("record" conditions and effects in reverse order)
					std::vector<std::string> conditions(cell.inputs.size(), std::string { });
					std::vector<std::string> effects(cell.outputs.size(), std::string { });
					size_t current = index;
					do {
						auto const parent { parents[current] };
						auto const offset { std::find(tree[parent].begin(), tree[parent].end(), current) };
						ASSERT(offset != tree[parent].end());
						auto const index { offset - tree[parent].begin() };
						for (size_t i { 0u }; i < cell.inputs.size(); i++) {
							conditions[i] += allConditions[parent][index][i];
						}
						for (size_t i { 0u }; i < cell.outputs.size(); i++) {
							effects[i] += allEffects[parent][index][i];
						}
						current = parents[current];
					} while (current != 0u);

					// Convert to UDFM format
					Io::Udfm::UdfmPortMap mappedConditions { };
					Io::Udfm::UdfmPortMap mappedEffects { };
					for (size_t i { 0u }; i < cell.inputs.size(); i++) {
						std::reverse(conditions[i].begin(), conditions[i].end()); // Transform to forward direction
						mappedConditions[cell.inputs[i]] = Basic::GetLogicConstraintsForString(conditions[i]);
					}
					for (size_t i { 0u }; i < cell.outputs.size(); i++) {
						std::reverse(effects[i].begin(), effects[i].end()); // Transform to forward direction
						mappedEffects[cell.outputs[i]] = Basic::GetLogicConstraintsForString(effects[i]);
					}
					udfmFault->AddAlternative("dynamic-test", mappedConditions, mappedEffects);
				}
			}
		}
		DVLOG(3) << *this->udfm;
	}
	else
	{
		LOG(FATAL) << "Fault model not implemented";
		__builtin_unreachable();
	}
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::GenerateFaultList(size_t seed)
{
	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		this->faultList = FaultList(Fault::GenerateStuckAtFaultList(*this->circuit));
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		this->faultList = FaultList(Fault::GenerateTransitionDelayFaultList(*this->circuit));
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		this->faultList = FaultList(Fault::GenerateCellAwareFaultList(*this->circuit, *this->udfm));
	}
	else
	{
		LOG(FATAL) << "Fault model not implemented";
		__builtin_unreachable();
	}
	this->faultListBegin = 0;
	this->faultListEnd = this->faultList.size();
}

template class FuzzingBase<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class FuzzingBase<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class FuzzingBase<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
