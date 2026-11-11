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
#include <unordered_set>

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
#include "Basic/Fault/Specification/CellAwareSpecification.hpp"
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
FuzzingBase<FaultModel, FaultList>::FuzzingBase(std::string configPrefix):
	Mixin::StatisticsMixin(configPrefix),
	Mixin::FaultStatisticsMixin<FaultList>(configPrefix),
	Mixin::SimulationStatisticsMixin(configPrefix),
	Mixin::SolverStatisticsMixin(configPrefix),
	Mixin::VcdExportMixin<FaultList>(configPrefix),
	Mixin::VcmMixin(configPrefix),
	AtpgBase<FaultModel, FaultList>(configPrefix),
	configPrefix(configPrefix)
{
}

template <typename FaultModel, typename FaultList>
FuzzingBase<FaultModel, FaultList>::~FuzzingBase(void) = default;

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::Init(void)
{
	if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		this->cellAwareFaultModelSource
			= AtpgBase<FaultModel, FaultList>::CellAwareFaultModelSource::Custom;
	}
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::Run(void)
{
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::GenerateCircuit(size_t seed)
{
	this->cells.clear();
	this->inputs.clear();
	this->outputs.clear();

	using GenRand = std::uniform_int_distribution<size_t>;
	using RangeIt = IntegerIterator<size_t>;

	std::mt19937_64 random { seed };
	auto const circuitName { boost::str(boost::format("C%016x") % seed) };
	auto const inputCount { (GenRand { configNumInputsMin, configNumInputsMax })(random) };
	auto const outputCount { (GenRand { configNumOutputsMin, configNumOutputsMax })(random) };
	auto const cellCount { (GenRand { configNumCellsMin, configNumCellsMax })(random) };

	Builder::CircuitBuilder builder;
	builder.SetName(circuitName);
	auto const circuitGroupId { builder.EmplaceGroup(circuitName) };

	struct Output {
		Builder::MappedNodeId nodeId;
		Builder::ConnectionId connId;
	};
	std::vector<Output> inputs { };
	std::vector<Output> cells { };
	std::vector<Output> outputs { };

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
		this->inputs.push_back("I" +std::to_string(index + 1u));
	}

	// Cells
	for (size_t index { 0u }; index < cellCount; index++) {
		auto uniquify_connection = [&, uniqueIndex=0u] (const Output& connection) mutable -> Output {
			auto const name { "uniquify_" + std::to_string(index) + "_" + std::to_string(uniqueIndex) };
			auto const gateId { builder.EmplaceMappedNode(name, CellCategory::MAIN_BUF, CellType::BUF, 1u) };
			auto const groupId { builder.EmplaceGroup(name) };
			auto const connId { builder.EmplaceConnection() };
			auto const sourceInfoId { builder.AddSourceInfo({}) };
			auto const portInputId { builder.GetGroup(groupId).EmplacePort("GI", Builder::PortType::Input) };
			builder.GetMappedNode(gateId).SetInputNode(0u, connection.nodeId);
			builder.GetMappedNode(gateId).SetInputConnectionId(0u, connection.connId);
			builder.GetMappedNode(gateId).SetInputConnectionName(0u, "conn_" + std::to_string(connection.connId));
			builder.GetMappedNode(gateId).SetInputPortName(0u, "GI" );
			builder.GetMappedNode(connection.nodeId).AddSuccessorNode(gateId);
			builder.GetGroup(groupId).GetPort(portInputId).SetConnections({ connection.connId });
			auto const portOutputId { builder.GetGroup(groupId).EmplacePort("GO", Builder::PortType::Output) };
			builder.GetMappedNode(gateId).SetOutputConnectionId(connId);
			builder.GetMappedNode(gateId).SetOutputConnectionName("conn_" + std::to_string(connId));
			builder.GetMappedNode(gateId).SetOutputPortName("GO");
			builder.GetGroup(groupId).GetPort(portOutputId).SetConnections({ connId });
			builder.GetGroup(circuitGroupId).AddGroup(groupId);
			builder.GetGroup(groupId).SetParent(circuitGroupId);
			builder.GetGroup(groupId).AddMappedNode(gateId);
			builder.GetGroup(groupId).SetSourceInfo(sourceInfoId);
			builder.GetSourceInfo(sourceInfoId).sourceType = "unique";
			builder.GetSourceInfo(sourceInfoId).AddProperty<bool>("module-is-virtual", true);
			builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-name", name);
			builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("module-type", "BUF");
			builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("virtual-name", name);
			builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("virtual-type", "BUF");
			builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("virtual-category", "unique");
			return  { gateId, connId };
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
		builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("cell-name", name);
		builder.GetSourceInfo(sourceInfoId).AddProperty<std::string>("cell-type", name);

		// Generate inputs for cell and add duplicate inputs if required.
		std::vector<size_t> cellInputIndices;
		auto const cellInputCount { (GenRand { configNumCellInputsMin, configNumCellInputsMax })(random) };
		while (cellInputIndices.size() < cellInputCount) {
			std::sample(RangeIt { 0u }, RangeIt { inputs.size() + cells.size() },
				std::back_inserter(cellInputIndices), cellInputCount - cellInputIndices.size(), random);
		}
		std::vector<Output> cellInputs;
		std::transform(cellInputIndices.begin(), cellInputIndices.end(), std::back_inserter(cellInputs),
			[&](auto index) {
				return (index >= inputs.size()) ? cells[index - inputs.size()] : inputs[index];
			});

		std::unordered_set<size_t> connectionSet;
		for (auto& cellInput : cellInputs) {
			if (auto [it, inserted] = connectionSet.emplace(cellInput.connId); !inserted) {
				cellInput = uniquify_connection(cellInput);
			}
		}

		const size_t GATES_WITH_ONE_INPUT = 4; // Keep in sync with the gate list below.
		const std::vector<std::tuple<std::string, CellCategory, CellType, size_t>> gateTypes {
			// One-input gates (need to be first in list)
			{ "SKIP", CellCategory::MAIN_UNKNOWN, CellType::UNDEFTYPE, 1u },
			{ "DFF",  CellCategory::MAIN_UNKNOWN, CellType::UNDEFTYPE, 1u },
			{ "BUF",  CellCategory::MAIN_BUF,     CellType::BUF,       1u },
			{ "INV",  CellCategory::MAIN_INV,     CellType::INV,       1u },
			// Two-input gates
			{ "AND",  CellCategory::MAIN_AND,     CellType::AND,       2u },
			{ "NAND", CellCategory::MAIN_NAND,    CellType::NAND,      2u },
			{ "OR",   CellCategory::MAIN_OR,      CellType::OR,        2u },
			{ "NOR",  CellCategory::MAIN_NOR,     CellType::NOR,       2u },
			{ "XOR",  CellCategory::MAIN_XOR,     CellType::XOR,       2u },
			{ "XNOR", CellCategory::MAIN_XNOR,    CellType::XNOR,      2u },
		};

		// Reduce the output to single connection and make sure the output has a unique connectionId.
		size_t gateIndex { 0u };
		size_t flipFlopCount { 0u };
		std::vector<std::string> logic;
		std::vector<Output> cellOutputs;
		std::copy(cellInputs.begin(), cellInputs.end(), std::back_inserter(cellOutputs));
		while (cellOutputs.size() > 1u || connectionSet.find(cellOutputs[0].connId) != connectionSet.end()) {
			std::vector<Output> newOutputs;
			for (size_t i { 0u }; i < cellOutputs.size(); ) {
				auto const gateTypeMax { (i + 1 < cellOutputs.size()) ? (gateTypes.size() - 1u) : GATES_WITH_ONE_INPUT - 1u };
				auto const gateTypeIndex { (GenRand { 0, gateTypeMax })(random) };
				auto const [gateName, gateCategory, gateType, gateSize] = gateTypes[gateTypeIndex];
				logic.push_back(gateName);

				if (gateName == "SKIP") {
					newOutputs.push_back(cellOutputs[i]);
				} else if (gateName == "DFF") {
					auto const nameSi { "gate_" + std::to_string(index) + "_" + std::to_string(gateIndex + 0u) };
					auto const nameSo { "gate_" + std::to_string(index) + "_" + std::to_string(gateIndex + 1u) };
					auto const nameC0 { "gate_" + std::to_string(index) + "_" + std::to_string(gateIndex + 2u) };
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
					gateIndex += 3;
					flipFlopCount++;
				} else {
					auto const name { "gate_" + std::to_string(index) + "_" + std::to_string(gateIndex) };
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
					gateIndex++;
				}
				i += gateSize;
			}
			cellOutputs = std::move(newOutputs);
		}
		cells.push_back({ cellOutputs[0u].nodeId, cellOutputs[0u].connId });

		std::vector<std::string> inputNames;
		for (size_t i { 0u }; i < cellInputs.size(); i++) {
			auto const [gateId, connId] = cellInputs[i];
			auto const portId { builder.GetGroup(cellGroupId).EmplacePort(
				"I" + std::to_string(i + 1u), Builder::PortType::Input) };
			auto const portInfoId { builder.AddSourceInfo({}) };
			builder.GetGroup(cellGroupId).GetPort(portId).SetConnections({ connId });
			builder.GetGroup(cellGroupId).GetPort(portId).SetSourceInfo(portInfoId);
			builder.GetSourceInfo(portInfoId).AddProperty<std::string>("port-type", "input");
			inputNames.push_back("I" + std::to_string(i + 1u));
		}

		std::vector<std::string> outputNames;
		auto const outputPortId { builder.GetGroup(cellGroupId).EmplacePort("O", Builder::PortType::Output) };
		auto const outputPortInfoId { builder.AddSourceInfo({}) };
		builder.GetGroup(cellGroupId).GetPort(outputPortId).SetConnections({ cellOutputs[0u].connId });
		builder.GetGroup(cellGroupId).GetPort(outputPortId).SetSourceInfo(outputPortInfoId);
		builder.GetSourceInfo(outputPortInfoId).AddProperty<std::string>("port-type", "output");
		outputNames.push_back("O");

		this->cells.push_back(Cell {
			.name = name,
			.inputs = inputNames,
			.outputs = outputNames,
			.logic = logic,
			.flipFlopCount = flipFlopCount,
		});
	}

	// Outputs
	for (size_t index { 0u }; index < outputCount; index++) {
		auto const name { "output_" + std::to_string(index) };
		auto const gateId { builder.EmplaceMappedNode(name, CellCategory::MAIN_OUT, CellType::P_OUT, 1u) };
		auto const groupId { builder.EmplaceGroup(name) };
		auto const connId { builder.EmplaceConnection() };
		auto const sourceId { builder.AddSourceInfo({}) };
		auto const inputPortId { builder.GetGroup(groupId).EmplacePort("I", Builder::PortType::Input) };
		auto const outputPortId { builder.GetGroup(groupId).EmplacePort("O", Builder::PortType::Output) };
		auto const circuitPortId { builder.GetGroup(circuitGroupId).EmplacePort("O" + std::to_string(index + 1), Builder::PortType::Output) };
		auto const circuitPortSourceId { builder.AddSourceInfo({}) };
		auto const [inputGateId, inputConnId] = cells[(GenRand { 0u, cells.size() - 1u })(random)];
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
		this->outputs.push_back("O" +std::to_string(index + 1u));
	}

	Builder::BuildConfiguration buildConfig { };
	buildConfig.mappedDisallowCycles = true;
	buildConfig.mappedRemoveUnconnectedGates = true;
	buildConfig.mappedDisallowUnconnectedInputs = true;
	this->circuit = builder.BuildCircuitEnvironment(buildConfig);
}

template <typename FaultModel, typename FaultList>
void FuzzingBase<FaultModel, FaultList>::GenerateFaultModel(size_t seed)
{
	using GenRand = std::uniform_int_distribution<size_t>;
	using RangeIt = IntegerIterator<size_t>;

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

		std::mt19937_64 random { seed };
		auto const build_tree = [&random](auto const branches, auto const maxBranchesPerNode) -> auto {
			// Initial state (epsilon) always exists.
			std::vector<std::vector<size_t>> tree { {} };
			std::vector<size_t> virtualToTree { 0u };
			std::vector<size_t> incomplete { 0u };

			// Build a tree by adding branches randomly.
			for (size_t branch { 0u }; branch < branches; branch++) {
				auto const incompleteIndex { (GenRand { 0u, incomplete.size() - 1u })(random) };
				auto const toExtend { incomplete[incompleteIndex] };

				// We want to add a transition to the toExtend node.
				// If the node is virtual (doesn't exist yet), create it now
				// and update the node id in the id map for correct mapping.
				if (toExtend != 0 && virtualToTree[toExtend] == 0u) {
					auto const createNode = tree.size();
					virtualToTree[toExtend] = createNode;
					tree.push_back({ });
				}

				// Create a new virtual node for the branch destination.
				auto const branchDestination = virtualToTree.size();
				virtualToTree.push_back(0);
				incomplete.push_back(branchDestination);
				tree[virtualToTree[toExtend]].push_back(branchDestination);

				// If the number of branches have been exhausted delist the tree node
				if (tree[virtualToTree[toExtend]].size() >= maxBranchesPerNode) {
					incomplete.erase(incomplete.begin() + incompleteIndex);
				}
			}
			// Replace virtual ids with tree ids.
			// Nodes that are virtual will lead back to epsilon.
			for (auto& node : tree) {
				for (auto& branch : node) {
					branch = virtualToTree[branch];
				}
			}
			return tree;
		};

		std::vector<Fault::CellAwareCellSpecification> cellSpecifications;
		for (auto& cell : this->cells) {
			std::vector<Fault::CellAwareFaultSpecification> faultSpecifications;
			auto const faultCount { (GenRand { configNumFaultsMin, configNumFaultsMax })(random) };
			for (size_t faultIndex { 0u }; faultIndex < faultCount; faultIndex++) {
				auto const branchCount { (GenRand { configNumBranchesMin, configNumBranchesMax })(random) };
				auto const maxBranchesPerNode { 1u << cell.inputs.size() };
				auto const tree { build_tree(branchCount, maxBranchesPerNode) };

				std::vector<std::string> dffStates(tree.size(), std::string(cell.flipFlopCount, 'X'));
				std::vector<Fault::CellAwareFaultSpecification::State> faultStates(tree.size());
				for (size_t index { 0u }; index < tree.size(); index++) {
					// Sample number-of-children values out of all possible input assignments.
					std::vector<size_t> childInputs;
					std::sample(RangeIt { 0u }, RangeIt { maxBranchesPerNode },
						std::back_inserter(childInputs), tree[index].size(), random);

					auto const to_binary_string = [](auto value, auto const& bits) {
						std::string result;
						for (size_t bit { 0u }; bit < bits; bit++) {
							result += ((value & 1) ? "1" : "0");
							value >>= 1;
						}
						return result;
					};

					for (size_t child { 0u }; child < tree[index].size(); child++) {
						auto const inputValues { to_binary_string(childInputs[child], cell.inputs.size()) };
						auto [outputValues, nextDffStates] = simulate_cell(cell, inputValues, dffStates[index]);

						auto const transitionTo { tree[index][child] };
						auto const isLeafTransition { transitionTo == 0u };

						if (isLeafTransition) {
							for (auto& value : outputValues) {
								// Invert output for fault propagation
								value = (value == 'X') ? 'X' : ((value == '0') ? '1' : '0');
							}
						} else {
							// Propagate internal state for child
							dffStates[tree[index][child]] = nextDffStates;
						}

						bool accepting = false;
						for (auto& value : outputValues) {
							auto const makeDontCare { (GenRand { 0u, configNumDontCare })(random) };
							if (makeDontCare == configNumDontCare) value = 'X'; // Undefined output
							accepting |= (value == '0' || value == '1');
						}

						std::vector<Fault::CellAwareFaultSpecification::Port> inputs;
						std::vector<Fault::CellAwareFaultSpecification::Port> outputs;
						for (size_t i { 0u }; i < cell.inputs.size(); i++) {
							inputs.push_back({ cell.inputs[i], Basic::GetLogicConstraintForCharacter(inputValues[i]) });
						}
						for (size_t i { 0u }; i < cell.outputs.size(); i++) {
							outputs.push_back({ cell.outputs[i], Basic::GetLogicConstraintForCharacter(outputValues[i]) });
						}
						faultStates[index].transitions.push_back({
							.inputs = inputs,
							.outputs = outputs,
							.toState = transitionTo,
							.accepting = accepting
						});
					}
				}
				faultSpecifications.emplace_back("fault_" + std::to_string(faultIndex), faultStates);
			}
			cellSpecifications.emplace_back(cell.name, faultSpecifications);
		}

		auto specification { std::make_shared<Fault::CellAwareModelSpecification>(cellSpecifications) };
		Mixin::CellAwareSpecificationMixin::SetCellAwareModelSpecification(specification);
		DVLOG(3) << to_string(*specification);
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
		this->faultList = FaultList(Fault::GenerateCellAwareFaultList(*this->circuit,
			*this->GetCellAwareModelSpecification()));
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
