#include "Simulation/CircuitSimulator.hpp"

#include <algorithm>
#include <cstdint>
#include <type_traits>
#include <vector>
#include <limits>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace FreiTest::Pattern;

//#define ENABLE_LOOKUP_TABLES

namespace FreiTest
{
namespace Simulation
{

template<typename FaultModel>
struct SimulationParams { };

template<> struct SimulationParams<FaultFreeModel> { };
template<> struct SimulationParams<SingleStuckAtFaultModel> { };
template<> struct SimulationParams<MultiStuckAtFaultModel> { };
template<> struct SimulationParams<SingleTransitionDelayFaultModel> {
	Logic currentValue;
	Logic previousValue;
};
template<> struct SimulationParams<MultiTransitionDelayFaultModel> {
	std::vector<Logic> currentValues;
	std::vector<Logic> previousValues;
};
template<> struct SimulationParams<CellAwareFaultModel> {
	size_t state;
	ssize_t transition;
};

static const constexpr ssize_t TRANSITION_UNKNOWN = -1;
static const constexpr ssize_t TRANSITION_NONE = -2;

static void ApplyTestPatternToTimeframe(const MappedCircuit& circuit, const TestPattern& pattern, SimulationResult& result, const SimulationConfig& config, size_t timeframe);
static void CopySecondaryOutputsFromPreviousTimeframe(const MappedCircuit& circuit, SimulationResult& result, size_t timeframeId, const SimulationConfig& config);
template<typename FaultModel>
static inline __attribute__((always_inline)) Logic SimulateGate(const MappedCircuit& circuit, const SimulationResult& result, const FaultModel& faultModel, const SimulationConfig& config,
	size_t timeframeId, size_t nodeId, SimulationParams<FaultModel>& params);

template<typename FaultModel>
static std::vector<size_t> GetFaultInputs(const FaultModel& faultModel, const MappedCircuit& circuit);
template<typename FaultModel>
static std::vector<size_t> GetFaultOutputs(const FaultModel& faultModel, const MappedCircuit& circuit);
template<typename FaultModel>
static Logic GetLogicValue(const MappedCircuit& circuit, const SimulationResult& result, const FaultModel& faultModel,
	size_t timeframeId, size_t nodeId, PortType portType, size_t portNumber, Logic originalValue, SimulationParams<FaultModel>& params);

SimulationConfig MakeSimulationConfig(Basic::SequentialConfig sequentialConfig)
{
	return {
		.sequentialConfig = sequentialConfig,
		.sequentialMode = SequentialMode::Functional
	};
}

static void ApplyTestPatternToTimeframe(const MappedCircuit& circuit, const TestPattern& pattern, SimulationResult& result, const SimulationConfig& config, size_t timeframe)
{
	for (auto [primaryInput, node] : circuit.EnumeratePrimaryInputs())
	{
		result[timeframe][node->GetNodeId()] = pattern.GetPrimaryInput(timeframe, primaryInput);
	}

	if (config.sequentialMode == SequentialMode::Independent
		|| (config.sequentialMode == SequentialMode::Functional && timeframe == 0u))
	{
		for (auto [secondaryInput, node] : circuit.EnumerateSecondaryInputs())
		{
			result[timeframe][node->GetNodeId()] = pattern.GetSecondaryInput(timeframe, secondaryInput);
		}
	}
}

static void CopySecondaryOutputsFromPreviousTimeframe(const MappedCircuit& circuit, SimulationResult& result, size_t timeframeId, const SimulationConfig& config)
{
	if (config.sequentialMode != SequentialMode::Functional)
	{
		return;
	}

	for (auto inputNode : circuit.GetSecondaryInputs())
	{
		const MappedNode* outputNode = circuit.GetSecondaryOutputForSecondaryInput(inputNode);
		result[timeframeId][inputNode->GetNodeId()] = result[timeframeId - 1u][outputNode->GetNodeId()];
	}
}

template<typename FaultModel>
static Logic GetLogicValue(const MappedCircuit& circuit, const SimulationResult& result, const FaultModel& faultModel,
	[[maybe_unused]] size_t timeframeId, [[maybe_unused]] size_t nodeId, [[maybe_unused]] PortType portType, [[maybe_unused]] size_t portNumber, Logic originalValue, SimulationParams<FaultModel>& params)
{
	if constexpr (std::is_same_v<FaultModel, Fault::FaultFreeModel>)
	{
		return originalValue;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>
		|| std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		const auto get_stuck_ats = [&]() -> std::vector<Fault::StuckAtFault> {
			if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
			{
				return { faultModel.GetFault()->GetStuckAt() };
			}
			else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
			{
				return faultModel.GetFault()->GetStuckAts();
			}
		};
		for (auto const& stuckAt : get_stuck_ats())
		{
			bool faultPin = (nodeId == stuckAt.GetNode()->GetNodeId())
				&& (portType == stuckAt.GetPort().portType)
				&& (portNumber == stuckAt.GetPort().portNumber);

			if (__builtin_expect(!faultPin, true))
			{
				continue;
			}

			return to_logic(stuckAt.GetType());
		}

		return originalValue;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>
		|| std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		const auto get_slow_transitions = [&]() -> std::vector<Fault::TransitionDelayFault> {
			if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
			{
				return { faultModel.GetFault()->GetTransitionDelay() };
			}
			else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
			{
				return faultModel.GetFault()->GetTransitionDelays();
			}
		};

		size_t indexInput { 0u };
		for (auto const& transitionDelay : get_slow_transitions())
		{
			auto const* driver { transitionDelay.GetNode()->GetDriverForPort(transitionDelay.GetPort()) };
			bool faultInputPin = (nodeId == driver->GetNodeId())
				&& (portType == PortType::Output)
				&& (portNumber == 0u);
			if (__builtin_expect(faultInputPin, false))
			{
				if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
				{
					params.previousValue = params.currentValue;
					params.currentValue = originalValue;
					//LOG(INFO) << "Update " << to_string(transitionDelay) << ", Timeframe " << timeframeId << ", Previous " << to_string(params.previousValue)
					//	<< ", Current " << to_string(params.currentValue);
				}
				else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
				{
					params.previousValues[indexInput] = params.currentValues[indexInput];
					params.currentValues[indexInput] = originalValue;
				}
			}
			indexInput++;
		}

		size_t indexOutput { 0u };
		for (auto const& transitionDelay : get_slow_transitions())
		{
			bool faultOutputPin = (nodeId == transitionDelay.GetNode()->GetNodeId())
				&& (portType == transitionDelay.GetPort().portType)
				&& (portNumber == transitionDelay.GetPort().portNumber);
			if (__builtin_expect(faultOutputPin, false))
			{
				Logic currentValue;
				Logic previousValue;
				if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
				{
					currentValue = params.currentValue;
					previousValue = params.previousValue;
					//LOG(INFO) << "Use " << to_string(transitionDelay) << ", Timeframe " << timeframeId << ", Previous " << to_string(params.previousValue)
					//	<< ", Current " << to_string(params.currentValue);
				}
				else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
				{
					currentValue = params.currentValues[indexOutput];
					previousValue = params.previousValues[indexOutput];
				}

				if ((transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_RISE && previousValue == Logic::LOGIC_ZERO && currentValue == Logic::LOGIC_ONE)
					|| (transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_FALL && previousValue == Logic::LOGIC_ONE && currentValue == Logic::LOGIC_ZERO)
					|| (transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_TRANSITION
						&& ((previousValue == Logic::LOGIC_ZERO && currentValue == Logic::LOGIC_ONE) || (previousValue == Logic::LOGIC_ONE && currentValue == Logic::LOGIC_ZERO))))
				{
					// Delay the transition by one timeframe by using the previous value.
					return previousValue;
				}
				return originalValue;
			}

			indexOutput++;
		}

		return originalValue;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		const auto& stateMachine { fault->GetStateMachine() };
		const auto& states { stateMachine.GetStates() };
		const auto& currentState { states[params.state] };

		// Only evaluate after the last input value has been settled.
		// Only used when sequential mode is false.
		if ((params.transition == TRANSITION_UNKNOWN) && (nodeId == stateMachine.GetMaxInputNodeId()))
		{
			params.transition = TRANSITION_NONE;
			size_t index { 0u };
			for (auto const& transition : currentState.transitions)
			{
				bool inputsValid = true;
				for (auto const& input: transition.inputs)
				{
					auto& [node, port] = input.nodeAndPort;
					assert (input.nodeAndPort.port.portType == PortType::Input);
					auto driverNodeId = node->GetDriverForPort(port)->GetNodeId();
					if (!IsConstraintTrueForLogic(result[timeframeId][driverNodeId], input.constraint))
					{
						inputsValid = false;
						break;
					}
				}
				if (inputsValid)
				{
					params.transition = index;
					break;
				}
				index++;
			}
		}
		else if ((params.transition >= 0)
			&& (nodeId >= stateMachine.GetMinOutputNodeId())
			&& (nodeId <= stateMachine.GetMaxOutputNodeId()))
		{
			auto const& transition = currentState.transitions[params.transition];
			for (auto const& output : transition.outputs)
			{
				auto const& [node, port] = output.nodeAndPort;
				if ((node->GetNodeId() == nodeId) && (port.portType == portType) && (port.portNumber == portNumber))
				{
					return GetLogicForConstraint(output.constraint);
				}
			}
		}

		return originalValue;
	}
	else
	{
		static_assert(std::is_void_v<FaultModel>, "This fault model is currently not supported!");
	}
}

template<typename FaultModel>
static std::vector<size_t> GetFaultInputs(const FaultModel& faultModel, const MappedCircuit& circuit)
{
	std::vector<size_t> faultInputs;
	if constexpr (std::is_same_v<FaultModel, Fault::FaultFreeModel>)
	{
		; // No inputs
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		; // No inputs
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		; // No inputs
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		auto const& transitionDelay { faultModel.GetFault()->GetTransitionDelay() };
		if (transitionDelay.GetPort().portType == PortType::Input)
		{
			auto const* driver { transitionDelay.GetNode()->GetDriverForPort(transitionDelay.GetPort()) };
			faultInputs.push_back(driver->GetNodeId());
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		for (auto const& transitionDelay : faultModel.GetFault()->GetTransitionDelays())
		{
			if (transitionDelay.GetPort().portType == PortType::Input)
			{
				auto const* driver { transitionDelay.GetNode()->GetDriverForPort(transitionDelay.GetPort()) };
				faultInputs.push_back(driver->GetNodeId());
			}
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const& stateMachine { faultModel.GetFault()->GetStateMachine() };
		faultInputs.push_back(stateMachine.GetMaxInputNodeId());
	}
	else
	{
		static_assert(std::is_void_v<FaultModel>, "This fault model is currently not supported!");
	}

	return faultInputs;
}

template<typename FaultModel>
static std::vector<size_t> GetFaultOutputs(const FaultModel& faultModel, const MappedCircuit& circuit)
{
	std::vector<size_t> faultOutputs;
	if constexpr (std::is_same_v<FaultModel, Fault::FaultFreeModel>)
	{
		; // No fault
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		auto const& stuckAt { faultModel.GetFault()->GetStuckAt() };
		faultOutputs.push_back(stuckAt.GetNode()->GetNodeId());
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		for (auto const& stuckAt : faultModel.GetFault()->GetStuckAts())
		{
			faultOutputs.push_back(stuckAt.GetNode()->GetNodeId());
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		auto const& transitionDelay { faultModel.GetFault()->GetTransitionDelay() };
		faultOutputs.push_back(transitionDelay.GetNode()->GetNodeId());
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		for (auto const& transitionDelay : faultModel.GetFault()->GetTransitionDelays())
		{
			faultOutputs.push_back(transitionDelay.GetNode()->GetNodeId());
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const& stateMachine { faultModel.GetFault()->GetStateMachine() };
		for (const auto& nodeAndPort : stateMachine.GetOutputs())
		{
			faultOutputs.push_back(nodeAndPort.node->GetNodeId());
		}
	}
	else
	{
		static_assert(std::is_void_v<FaultModel>, "This fault model is currently not supported!");
	}

	return faultOutputs;
}

#ifdef ENABLE_LOOKUP_TABLES

/**
 * @brief Converts logic 0, 1, X, U to indices 0, 1, 2, 3 (in this order).
 */
constexpr inline uint8_t ConvertLogicToIndex(Logic logic)
{
	const uint8_t value = static_cast<uint8_t>(logic);
	return (value & 0x01) | (value & 0x04) >> 1 | (value & 0x08) >> 2;
}

constexpr inline Logic Lookup(const uint8_t* table, const uint8_t value)
{
	return static_cast<Logic>(table[value]);
}

constexpr const uint8_t GATE_FFSR    [64u] = {
	'0','1','X','U', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
	'0','0','0','0', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
	'X','X','X','X', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
	'U','U','U','U', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
};
constexpr const uint8_t GATE_FFRS    [64u] = {
	'0','1','X','U', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
	'0','0','0','0', '0','0','0','0', '0','0','0','0', '0','0','0','0',
	'X','X','X','X', 'X','X','X','X', 'X','X','X','X', 'X','X','X','X',
	'U','U','U','U', 'U','U','U','U', 'U','U','U','U', 'U','U','U','U',
};
constexpr const uint8_t GATE_MUX    [64u] = {
	'0','1','X','U', '0','1','X','U', '0','1','X','U', '0','1','X','U',
	'0','0','0','0', '1','1','1','1', 'X','X','X','X', 'U','U','U','U',
	'X','X','X','X', 'X','X','X','X', 'X','X','X','X', 'X','X','X','X',
	'U','U','U','U', 'U','U','U','U', 'U','U','U','U', 'U','U','U','U',
};
constexpr const uint8_t GATE_OR     [16u] = {'0','1','X','U', '1','1','1','1', 'X','1','X','U', 'U','1','U','U'};
constexpr const uint8_t GATE_NOR    [16u] = {'1','0','X','U', '0','0','0','0', 'X','0','X','U', 'U','0','U','U'};
constexpr const uint8_t GATE_AND    [16u] = {'0','0','0','0', '0','1','X','U', '0','X','X','U', '0','U','U','U'};
constexpr const uint8_t GATE_NAND   [16u] = {'1','1','1','1', '1','0','X','U', '1','X','X','U', '1','U','U','U'};
constexpr const uint8_t GATE_XOR    [16u] = {'0','1','X','U', '1','0','X','U', 'X','X','X','U', 'U','U','U','U'};
constexpr const uint8_t GATE_XNOR   [16u] = {'1','0','X','U', '0','1','X','U', 'X','X','X','U', 'U','U','U','U'};
constexpr const uint8_t GATE_IF0    [16u] = {'0','1','X','U', 'U','U','U','U', 'U','U','U','U', 'U','U','U','U'};
constexpr const uint8_t GATE_IF1    [16u] = {'U','U','U','U', '0','1','X','U', 'U','U','U','U', 'U','U','U','U'};
constexpr const uint8_t GATE_BUF    [ 4u] = {'0','1','X','U'};
constexpr const uint8_t GATE_INV    [ 4u] = {'1','0','X','U'};
constexpr const uint8_t GATE_CONST0 [ 1u] = {'0'};
constexpr const uint8_t GATE_CONST1 [ 1u] = {'1'};
constexpr const uint8_t GATE_CONSTX [ 1u] = {'X'};
constexpr const uint8_t GATE_CONSTU [ 1u] = {'U'};

template<typename FaultModel>
static Logic SimulateGate(const MappedCircuit& circuit, const SimulationResult& result, const FaultModel& fault, const SimulationConfig& config, size_t timeframeId, size_t nodeId, SimulationParams<FaultModel>& params)
{
	const auto& timeframe { result[timeframeId] };
	const auto* node { circuit.GetNode(nodeId) };

	const auto get_input = [&](auto index) -> uint8_t {
		auto originalValue = timeframe[node->GetInput(index)->GetNodeId()];
		return ConvertLogicToIndex(GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, index, originalValue, params));
	};

	Logic resultValue = Logic::LOGIC_INVALID;
	switch (node->GetCellCategory())
	{
	case CellCategory::MAIN_IN:
		// The result has already been copied into the timeframe.
		// We do not need to apply the input fault model as the input gate does not have inputs.
		// -> Use the value out of the timeframe and apply the output fault model.
		resultValue = timeframe[nodeId];
		break;

	case CellCategory::MAIN_CONSTANT:
		switch (__builtin_expect(node->GetCellType(), CellType::PRESET_X))
		{
			case CellType::PRESET_0: resultValue = Logic::LOGIC_ZERO; break;
			case CellType::PRESET_1: resultValue = Logic::LOGIC_ONE; break;
			case CellType::PRESET_X: resultValue = Logic::LOGIC_DONT_CARE; break;
			case CellType::PRESET_U: resultValue = Logic::LOGIC_UNKNOWN; break;
			default: Logging::Panic();
		}
		break;

	case CellCategory::MAIN_OUT:
	{
		auto index = get_input(0u);
		const uint8_t* lookup;

		switch (__builtin_expect(node->GetCellType(), CellType::S_OUT))
		{
			case CellType::P_OUT: lookup = GATE_BUF; break;
			case CellType::S_OUT:
			case CellType::S_OUT_CLK:
			case CellType::S_OUT_EN:
			{
				CellType cellType;
				switch (config.sequentialConfig.sequentialModel)
				{
					case Basic::SequentialModel::None:
						Logging::Panic("No sequential model has been specified but sequential element exists in circuit.");
					case Basic::SequentialModel::Unclocked:
						cellType = CellType::S_OUT;
						break;
					case Basic::SequentialModel::FlipFlop:
						cellType = CellType::S_OUT_CLK;
						break;
					case Basic::SequentialModel::Latch:
						cellType = CellType::S_OUT_EN;
						break;
					case Basic::SequentialModel::Keep:
						cellType = node->GetCellType();
						break;
					default: Logging::Panic();
				}
				switch (cellType)
				{
					case CellType::S_OUT:
						break;
					case CellType::S_OUT_CLK:
					case CellType::S_OUT_EN:
						Logging::Panic("Not implemented");
					default:
						Logging::Panic("Unsupported cell type");
				}

				switch (config.sequentialConfig.setResetModel)
				{
					case Basic::SetResetModel::None:
						lookup = GATE_BUF;
						break;
					case Basic::SetResetModel::OnlySet:
						index |= get_input(2u) << 2u;
						lookup = GATE_FFSR;
						break;
					case Basic::SetResetModel::OnlyReset:
						index |= get_input(3u) << 4u;
						lookup = GATE_FFRS;
						break;
					case Basic::SetResetModel::SetHasPriority:
						index |= get_input(2u) << 2u;
						index |= get_input(3u) << 4u;
						lookup = GATE_FFSR;
						break;
					case Basic::SetResetModel::ResetHasPriority:
						index |= get_input(2u) << 2u;
						index |= get_input(3u) << 4u;
						lookup = GATE_FFRS;
						break;
					default: Logging::Panic();
				}
				break;
			}

			default: Logging::Panic("Unknown output type");
		}

		resultValue = Lookup(lookup, index);
		break;
	}

	case CellCategory::MAIN_BUF:
		resultValue = Lookup(GATE_BUF, get_input(0u));
		break;

	case CellCategory::MAIN_INV:
		resultValue = Lookup(GATE_INV, get_input(0u));
		break;

	case CellCategory::MAIN_AND:
	case CellCategory::MAIN_OR:
	case CellCategory::MAIN_XOR:
	case CellCategory::MAIN_NAND:
	case CellCategory::MAIN_NOR:
	case CellCategory::MAIN_XNOR:
	{
		const uint8_t* lookup;
		const uint8_t* finalLookup;
		switch (node->GetCellCategory())
		{
			case CellCategory::MAIN_AND:  lookup = GATE_AND; finalLookup = GATE_BUF; resultValue = Logic::LOGIC_ONE;  break;
			case CellCategory::MAIN_OR:   lookup = GATE_OR;  finalLookup = GATE_BUF; resultValue = Logic::LOGIC_ZERO; break;
			case CellCategory::MAIN_XOR:  lookup = GATE_XOR; finalLookup = GATE_BUF; resultValue = Logic::LOGIC_ZERO; break;
			case CellCategory::MAIN_NAND: lookup = GATE_AND; finalLookup = GATE_INV; resultValue = Logic::LOGIC_ONE;  break;
			case CellCategory::MAIN_NOR:  lookup = GATE_OR;  finalLookup = GATE_INV; resultValue = Logic::LOGIC_ZERO; break;
			case CellCategory::MAIN_XNOR: lookup = GATE_XOR; finalLookup = GATE_INV; resultValue = Logic::LOGIC_ZERO; break;
			default: Logging::Panic();
		}

		for (size_t input { 0u }; input < static_cast<size_t>(__builtin_expect(node->GetNumberOfInputs(), 2u)); input++)
		{
			resultValue = Lookup(lookup, ConvertLogicToIndex(resultValue) | get_input(input) << 2u);
		}
		resultValue = Lookup(finalLookup, ConvertLogicToIndex(resultValue));
		break;
	}

	case MAIN_BUFIF:
	case MAIN_NOTIF:
	{
		const uint8_t* lookup;
		const uint8_t* finalLookup;
		switch (node->GetCellType())
		{
			case CellType::BUFIF1: lookup = GATE_IF1; finalLookup = GATE_BUF; break;
			case CellType::BUFIF0: lookup = GATE_IF0; finalLookup = GATE_BUF; break;
			case CellType::NOTIF1: lookup = GATE_IF1; finalLookup = GATE_INV; break;
			case CellType::NOTIF0: lookup = GATE_IF0; finalLookup = GATE_INV; break;
			default: Logging::Panic();
		}

		resultValue = Lookup(lookup, get_input(0u) | get_input(1u) << 2u);
		resultValue = Lookup(finalLookup, ConvertLogicToIndex(resultValue));
		break;
	}

	case MAIN_MUX:
		resultValue = Lookup(GATE_MUX, get_input(0u) | get_input(1u) << 2u | get_input(2u) << 4u);
		break;

	default:
		// Unsupported logic gate
		Logging::Panic();
	}

	// Apply the output fault model to the computed result.
	return GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Output, 0u, resultValue, params);
}

#else

template<typename FaultModel>
__attribute__((always_inline))
static inline Logic SimulateGate(const MappedCircuit& circuit, const SimulationResult& result, const FaultModel& fault, const SimulationConfig& config, size_t timeframeId, size_t nodeId, SimulationParams<FaultModel>& params)
{
	const SimulationTimeframe& timeframe = result[timeframeId];
	const MappedNode* node = circuit.GetNode(nodeId);

	Logic resultValue = Logic::LOGIC_INVALID;
	switch (node->GetCellCategory())
	{
	case CellCategory::MAIN_IN:
		// The result has already been copied into the timeframe.
		// We do not need to apply the input fault model as the input gate does not have inputs.
		// -> Use the value out of the timeframe and apply the output fault model.
		resultValue = timeframe[nodeId];
		break;

	case CellCategory::MAIN_CONSTANT:
		switch (__builtin_expect(node->GetCellType(), CellType::PRESET_X))
		{
			case CellType::PRESET_0:
				resultValue = Logic::LOGIC_ZERO;
				break;
			case CellType::PRESET_1:
				resultValue = Logic::LOGIC_ONE;
				break;
			case CellType::PRESET_X:
				resultValue = Logic::LOGIC_DONT_CARE;
				break;
			case CellType::PRESET_U:
				resultValue = Logic::LOGIC_UNKNOWN;
				break;
			default:
				Logging::Panic();
		}
		break;

	case CellCategory::MAIN_OUT:
		resultValue = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 0u, timeframe[node->GetInput(0)->GetNodeId()], params);

		if (circuit.IsSecondaryOutput(node))
		{
			auto get_secondary_port_type = [&](Basic::SequentialModel model) -> CellType {
				switch (model)
				{
					case Basic::SequentialModel::None:
						Logging::Panic("No sequential model has been specified but sequential element exists in circuit.");
					case Basic::SequentialModel::Unclocked:
						return CellType::S_OUT;
					case Basic::SequentialModel::FlipFlop:
						return CellType::S_OUT_CLK;
					case Basic::SequentialModel::Latch:
						return CellType::S_OUT_EN;
					case Basic::SequentialModel::Keep:
						return node->GetCellType();
					default:
						Logging::Panic("Unsupported sequential model");
				}
			};

			const CellType cellType = get_secondary_port_type(config.sequentialConfig.sequentialModel);
			switch (cellType)
			{
				case CellType::S_OUT:
					// No modification of resultValue required
					break;
				case CellType::S_OUT_CLK:
				case CellType::S_OUT_EN:
					Logging::Panic("Not implemented");
					Logging::Panic("Not implemented");
				default:
					Logging::Panic("Unsupported cell type");
			}

			Logic setInput = Logic::LOGIC_ZERO;
			Logic resetInput = Logic::LOGIC_ZERO;
			switch (config.sequentialConfig.setResetModel)
			{
				case Basic::SetResetModel::None:
					// Nothing to be done here
					break;
				case Basic::SetResetModel::OnlySet:
					setInput = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 2u, timeframe[node->GetInput(2)->GetNodeId()], params);
					break;
				case Basic::SetResetModel::OnlyReset:
					resetInput = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 3u, timeframe[node->GetInput(3)->GetNodeId()], params);
					break;
				case Basic::SetResetModel::SetHasPriority:
				case Basic::SetResetModel::ResetHasPriority:
					setInput = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 2u, timeframe[node->GetInput(2)->GetNodeId()], params);
					resetInput = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 3u, timeframe[node->GetInput(3)->GetNodeId()], params);
					break;
			}

			auto get_result_on = [](Logic originalValue, Logic controlInput, Logic controlResult) -> Logic {
				if (__builtin_expect(controlInput == Logic::LOGIC_ONE, false))
				{
					return controlResult;
				}
				if (__builtin_expect(controlInput == Logic::LOGIC_DONT_CARE, false))
				{
					return Logic::LOGIC_DONT_CARE;
				}
				if (__builtin_expect(controlInput == Logic::LOGIC_UNKNOWN, false))
				{
					return Logic::LOGIC_UNKNOWN;
				}
				return originalValue;
			};
			switch (config.sequentialConfig.setResetModel)
			{
				default:
					// Nothing to be done here
					break;
				case Basic::SetResetModel::OnlySet:
					resultValue = get_result_on(resultValue, setInput, Logic::LOGIC_ONE);
					break;
				case Basic::SetResetModel::OnlyReset:
					resultValue = get_result_on(resultValue, resetInput, Logic::LOGIC_ZERO);
					break;
				case Basic::SetResetModel::ResetHasPriority:
					resultValue = get_result_on(resultValue, setInput, Logic::LOGIC_ONE);
					resultValue = get_result_on(resultValue, resetInput, Logic::LOGIC_ZERO);
					break;
				case Basic::SetResetModel::SetHasPriority:
					resultValue = get_result_on(resultValue, resetInput, Logic::LOGIC_ZERO);
					resultValue = get_result_on(resultValue, setInput, Logic::LOGIC_ONE);
					break;
			}
		}
		break;

	case CellCategory::MAIN_BUF:
	case CellCategory::MAIN_INV:
		resultValue = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 0u, timeframe[node->GetInput(0)->GetNodeId()], params);
		if (node->GetCellCategory() == MAIN_INV)
		{
			resultValue = InvertLogicValue(resultValue);
		}
		break;

	case CellCategory::MAIN_AND:
	case CellCategory::MAIN_NAND:
	case CellCategory::MAIN_OR:
	case CellCategory::MAIN_NOR:
	{
		for (auto [input, inputNode] : node->EnumerateInputs())
		{
			const Logic value = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, input, timeframe[inputNode->GetNodeId()], params);
			switch (value)
			{
				case Logic::LOGIC_ZERO:
					if (__builtin_expect(node->GetCellCategory() == CellCategory::MAIN_AND
						|| node->GetCellCategory() == CellCategory::MAIN_NAND, false))
					{
						resultValue = Logic::LOGIC_ZERO;
						goto end_controlling_and_or;
					}
					continue;

				case Logic::LOGIC_ONE:
					if (__builtin_expect(node->GetCellCategory() == CellCategory::MAIN_OR
						|| node->GetCellCategory() == CellCategory::MAIN_NOR, false))
					{
						resultValue = Logic::LOGIC_ONE;
						goto end_controlling_and_or;
					}
					continue;

#if __cplusplus > 201703L
				[[likely]]
#endif
				case Logic::LOGIC_DONT_CARE:
					if (__builtin_expect(resultValue == Logic::LOGIC_INVALID, false))
					{
						resultValue = value;
					}
					continue;

				case Logic::LOGIC_UNKNOWN:
					resultValue = value;
					continue;

				default:
					Logging::Panic();
			}
		}

		if (__builtin_expect(resultValue == Logic::LOGIC_INVALID, false))
		{
			if (node->GetCellCategory() == MAIN_AND || node->GetCellCategory() == MAIN_NAND)
			{
				// And / Nand gate: No controlling input present -> all inputs were LOGIC_ONE.
				resultValue = Logic::LOGIC_ONE;
			}
			else
			{
				// Or / Nor gate: No controlling input present -> all inputs were LOGIC_ZERO.
				resultValue = Logic::LOGIC_ZERO;
			}
		}

	end_controlling_and_or:
		// Inverting gate type is present and we need to invert the result.
		if (node->GetCellCategory() == MAIN_NAND || node->GetCellCategory() == MAIN_NOR)
		{
			resultValue = InvertLogicValue(resultValue);
		}

		break;
	}

	case CellCategory::MAIN_XOR:
	case CellCategory::MAIN_XNOR:
	{
		// Count the number of ones that are present at the gate
		size_t xorCount = 0u;
		for (auto [input, inputNode] : node->EnumerateInputs())
		{
			const Logic value = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, input, timeframe[inputNode->GetNodeId()], params);
			switch (value)
			{
#if __cplusplus > 201703L
				[[likely]]
#endif
				case Logic::LOGIC_DONT_CARE:
					resultValue = value;
					continue; // Don't break here as there might be an unknown value might overwrite the result.

				case Logic::LOGIC_UNKNOWN:
					resultValue = value;
					goto end_xor_loop; // Break here as don't care is now allowed to overwrite the result.

				case Logic::LOGIC_ZERO:
					continue;

				case Logic::LOGIC_ONE:
					xorCount++;
					continue;

				default:
					Logging::Panic();
			}
		}

	end_xor_loop:
		// If there is no don't care or unknown value present at any input
		// use the count of ones (even / odd) to determine the resulting logic value.
		if (__builtin_expect(resultValue == Logic::LOGIC_INVALID, false))
		{
			resultValue = ((xorCount & 1) == 0) ? Logic::LOGIC_ZERO : Logic::LOGIC_ONE;
			if (node->GetCellCategory() == MAIN_XNOR)
			{
				resultValue = InvertLogicValue(resultValue);
			}
		}

		break;
	}

	case MAIN_BUFIF:
	case MAIN_NOTIF:
	{
		Logic valueEn = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 1u, timeframe[node->GetInput(1)->GetNodeId()], params);
		if (node->GetCellType() == CellType::BUFIF0 || node->GetCellType() == CellType::NOTIF0)
		{
			// Enable input is active low and is inverted here for simplicity.
			valueEn = InvertLogicValue(valueEn);
		}

		switch (valueEn)
		{
			case Logic::LOGIC_ZERO:
#if __cplusplus > 201703L
			[[likely]]
#endif
			case Logic::LOGIC_DONT_CARE:
			case Logic::LOGIC_UNKNOWN:
				resultValue = Logic::LOGIC_UNKNOWN;
				break;
			case Logic::LOGIC_ONE:
				resultValue = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 0u, timeframe[node->GetInput(0)->GetNodeId()], params);
				break;
			default:
				Logging::Panic();
		}

		if (node->GetCellCategory() == MAIN_NOTIF)
		{
			resultValue = InvertLogicValue(resultValue);
		}
		break;
	}

	case MAIN_MUX:
	{
		const Logic valueSel = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 2u, timeframe[node->GetInput(2)->GetNodeId()], params);
		switch (valueSel)
		{
#if __cplusplus > 201703L
			[[likely]]
#endif
			case Logic::LOGIC_DONT_CARE:
			case Logic::LOGIC_UNKNOWN:
				resultValue = valueSel;
				break;
			case Logic::LOGIC_ZERO:
				resultValue = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 0u, timeframe[node->GetInput(0)->GetNodeId()], params);
				break;
			case Logic::LOGIC_ONE:
				resultValue = GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Input, 1u, timeframe[node->GetInput(1)->GetNodeId()], params);
				break;
			default:
				Logging::Panic();
		}
		break;
	}

	default:
		// Unsupported logic gate
		Logging::Panic();
	}

	// Apply the output fault model to the computed result.
	return GetLogicValue(circuit, result, fault, timeframeId, nodeId, PortType::Output, 0u, resultValue, params);
}

#endif

template<typename FaultModel>
void SimulateTestPatternNaive(const MappedCircuit& circuit, const TestPattern& pattern, const FaultModel& faultModel, SimulationResult& result, const SimulationConfig& config)
{
	SimulationParams<FaultModel> params { };

	if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		params.currentValue = Logic::LOGIC_INVALID;
		params.previousValue = Logic::LOGIC_INVALID;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		params.currentValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
		params.previousValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
		params.state = 0;
		params.transition = faultModel.GetFault()->IsSequentialCell()
			? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
			: TRANSITION_UNKNOWN;
	}

	for (size_t timeframeId = 0u; timeframeId < pattern.GetNumberOfTimeframes(); ++timeframeId)
	{
		SimulationTimeframe& timeframe = result[timeframeId];

[[maybe_unused]]
resimulate:
		if (timeframeId != 0u)
		{
			CopySecondaryOutputsFromPreviousTimeframe(circuit, result, timeframeId, config);
		}
		ApplyTestPatternToTimeframe(circuit, pattern, result, config, timeframeId);

		// Traverse the graph from start to end
		for (size_t nodeId = 0; nodeId < circuit.GetNumberOfNodes(); nodeId++)
		{
			timeframe[nodeId] = SimulateGate(circuit, result, faultModel, config, timeframeId, nodeId, params);
		}

		if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
			auto const& currentState { states[params.state] };
			if (faultModel.GetFault()->IsSequentialCell() && (params.transition != TRANSITION_NONE))
			{
				for (auto const& input: currentState.transitions[params.transition].inputs)
				{
					auto& [node, port] = input.nodeAndPort;
					assert (input.nodeAndPort.port.portType == PortType::Input);
					auto driverNodeId = node->GetDriverForPort(port)->GetNodeId();
					if (!IsConstraintTrueForLogic(result[timeframeId][driverNodeId], input.constraint))
					{
						params.transition = ((static_cast<size_t>(params.transition) + 1u) < currentState.transitions.size())
							? (params.transition + 1u)
							: TRANSITION_NONE; // Exhausted all transitions
						goto resimulate;
					}
				}
			}

			auto const targetState { (params.transition >= 0) ? currentState.transitions[params.transition].toState : 0 };
			params.state = targetState;
			params.transition = faultModel.GetFault()->IsSequentialCell()
				? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
				: TRANSITION_UNKNOWN;
			assert (params.state < static_cast<size_t>(states.size()));
		}
	}
}

template<typename FaultModel>
void SimulateTestPatternEventDriven(const MappedCircuit& circuit, const TestPattern& pattern, const FaultModel& faultModel, SimulationResult& result, const SimulationConfig& config)
{
	SimulationParams<FaultModel> params { };

	if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		params.currentValue = Logic::LOGIC_INVALID;
		params.previousValue = Logic::LOGIC_INVALID;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		params.currentValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
		params.previousValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
		params.state = 0;
		params.transition = faultModel.GetFault()->IsSequentialCell()
			? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
			: TRANSITION_UNKNOWN;
	}

	auto faultInputs = GetFaultInputs(faultModel, circuit);
	auto faultOutputs = GetFaultOutputs(faultModel, circuit);

	std::vector<bool> startNodes(circuit.GetNumberOfNodes(), false);
	for (auto [nodeId, node] : circuit.EnumerateNodes())
	{
		// Find the start nodes in the circuit that introduce a logic value
		if ( // Constant gates might introduce a value that is different from the current value
			__builtin_expect(node->GetCellCategory() == CellCategory::MAIN_CONSTANT, false)
			// Primary and Secondary input gates might introduce a value that is different from the current value
			|| __builtin_expect(node->GetCellCategory() == CellCategory::MAIN_IN, false))
		{
			startNodes[nodeId] = true;
		}
	}
	for (auto& nodeId : faultInputs) {
		// The fault inputs might be required to correctly evaluate the fault model
		startNodes[nodeId] = true;
	}
	for (auto& nodeId : faultOutputs) {
		// The fault outputs might produce a different value from the current value depending on the fault model
		startNodes[nodeId] = true;
	}

	std::vector<bool> activeNodes(circuit.GetNumberOfNodes(), false);
	for (size_t timeframeId = 0u; timeframeId < pattern.GetNumberOfTimeframes(); ++timeframeId)
	{
		SimulationTimeframe& timeframe = result[timeframeId];

[[maybe_unused]]
resimulate:
		if (timeframeId != 0u)
		{
			std::fill(activeNodes.begin(), activeNodes.end(), false);
			CopySecondaryOutputsFromPreviousTimeframe(circuit, result, timeframeId, config);
		}
		ApplyTestPatternToTimeframe(circuit, pattern, result, config, timeframeId);

		// Traverse the graph from start to end
		for(auto [nodeId, node] : circuit.EnumerateNodes())
		{
			if (__builtin_expect(!activeNodes[nodeId], true)
				&& __builtin_expect(!startNodes[nodeId], true))
			{
				continue;
			}

			Logic value = SimulateGate(circuit, result, faultModel, config, timeframeId, nodeId, params);
			if (__builtin_expect(value == timeframe[nodeId], true)
				&& __builtin_expect(!startNodes[nodeId], true))
			{
				continue;
			}
			timeframe[nodeId] = value;

			for (auto successor : node->GetSuccessors())
			{
				activeNodes[successor->GetNodeId()] = true;
			}
		}

		if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
			auto const& currentState { states[params.state] };
			if (faultModel.GetFault()->IsSequentialCell() && (params.transition != TRANSITION_NONE))
			{
				for (auto const& input: currentState.transitions[params.transition].inputs)
				{
					auto& [node, port] = input.nodeAndPort;
					assert (input.nodeAndPort.port.portType == PortType::Input);
					auto driverNodeId = node->GetDriverForPort(port)->GetNodeId();
					if (!IsConstraintTrueForLogic(result[timeframeId][driverNodeId], input.constraint))
					{
						params.transition = ((static_cast<size_t>(params.transition) + 1u) < currentState.transitions.size())
							? (params.transition + 1u)
							: TRANSITION_NONE; // Exhausted all transitions
						goto resimulate;
					}
				}
			}

			auto const targetState { (params.transition >= 0) ? currentState.transitions[params.transition].toState : 0 };
			params.state = targetState;
			params.transition = faultModel.GetFault()->IsSequentialCell()
				? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
				: TRANSITION_UNKNOWN;
			assert (params.state < static_cast<size_t>(states.size()));
		}
	}
}

template<typename FaultModel>
void SimulateTestPatternEventDrivenIncremental(const MappedCircuit& circuit, const TestPattern& pattern, const FaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config)
{
	SimulationParams<FaultModel> params { };

	if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		params.currentValue = Logic::LOGIC_INVALID;
		params.previousValue = Logic::LOGIC_INVALID;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		params.currentValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
		params.previousValues = std::vector(faultModel.GetFault()->GetTransitionDelays().size(), Logic::LOGIC_INVALID);
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
		params.state = 0;
		params.transition = faultModel.GetFault()->IsSequentialCell()
			? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
			: TRANSITION_UNKNOWN;
	}

	// The initial state of the flip-flops might have changed:
	// - Searching for differences here which indicate a different initial state.
	// - If the flip-flop is a fault location a recomputation of the value is required.
	std::vector<bool> activeNodes(circuit.GetNumberOfNodes(), false);
	for (auto [inputNumber, secondaryInput] : circuit.EnumerateSecondaryInputs())
	{
		const size_t nodeId = secondaryInput->GetNodeId();
		const Logic value = pattern.GetSecondaryInput(0u, inputNumber);
		if (__builtin_expect(result[0u][nodeId] != value, false))
		{
			result[0u][nodeId] = value;
			activeNodes[nodeId] = true;
		}
	}

	auto faultInputs = GetFaultInputs(faultModel, circuit);
	auto faultOutputs = GetFaultOutputs(faultModel, circuit);
	for (auto& nodeId : faultInputs) {
		// The fault inputs might be required to correctly evaluate the fault model
		activeNodes[nodeId] = true;
	}
	for (auto& nodeId : faultOutputs) {
		// The fault outputs might produce a different value from the current value depending on the fault model
		activeNodes[nodeId] = true;
	}

	for (size_t timeframeId = 0u; timeframeId < pattern.GetNumberOfTimeframes(); ++timeframeId)
	{
		const SimulationTimeframe& baseTimeframe = base[timeframeId];
		SimulationTimeframe& resultTimeframe = result[timeframeId];

[[maybe_unused]]
resimulate:
		if (timeframeId != 0u)
		{
			std::fill(activeNodes.begin(), activeNodes.end(), false);
			CopySecondaryOutputsFromPreviousTimeframe(circuit, result, timeframeId, config);

			for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
			{
				const size_t nodeId = circuit.GetSecondaryInput(secondaryInput)->GetNodeId();
				if (__builtin_expect(baseTimeframe[nodeId] != resultTimeframe[nodeId], false))
				{
					activeNodes[nodeId] = true;
				}
			}
		}
		// The primary inputs have already been applied by the reference simulation.

		for (auto& nodeId : faultInputs)
		{
			activeNodes[nodeId] = true;
		}
		for (auto& nodeId : faultOutputs)
		{
			activeNodes[nodeId] = true;
		}

		// Traverse the graph from start to end
		for (auto [nodeId, node] : circuit.EnumerateNodes())
		{
			if (__builtin_expect(!activeNodes[nodeId], true))
			{
				continue;
			}

			// Overwrite the result in any case as the fault model might change the value.
			// Then check if the base differs for the current timeframe value
			// to see if the simulation should continue.
			// It is not sufficient to first check the base timeframe, as the fault model
			// might change the gate output value back to be equivalent to the fault free value.
			//
			// Example: Flip-Flop with stuck-at at output and two timeframes
			//
			// --|---+          +-------+  +---|---+
			//  DFF  |          |  XOR  |  |  DFF  |
			//   |   | SA1 +----+ I1    |  |   |   | SA1
			//   | Q +--X--+    |     O +--+ D | Q +--X--
			//   |   |        +-+ I2    |  |   |   |
			// --|---+        | +-------+  +---|---+
			//
			//        Base  Result
			// 1) TF0 Q=0,  Q=0    Initial, Node is fault location => Activate SIN
			// 2) TF0 Q=0,  Q=1    SimulateGate, value != base => Activate XOR
			// 3) TF0 I1=0  I1=1
			//        I2=1  I2=1
			//        O=1   O=0    SimulateGate, value != base => Activate SOUT
			// 4) TF0 D=1   D=0
			// 5) TF1 Q=1   Q=1    Initial
			// 5) TF1 Q=1   Q=0    CopyFromPreviousTimeframe, Node is fault location => Activate SIN
			// 6) TF1 Q=1   Q=0    SimulateGate, value == base, but value != result
			//
			// If in step 6 only the base is checked, then the wrong value is saved in result.
			// => First store the resulting value, then check.
			auto const value = SimulateGate(circuit, result, faultModel, config, timeframeId, nodeId, params);
			resultTimeframe[nodeId] = value;
			if (__builtin_expect(value == baseTimeframe[nodeId], true))
			{
				continue;
			}

			for (auto successor : node->GetSuccessors())
			{
				activeNodes[successor->GetNodeId()] = true;
			}
		}

		if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			auto const& states { faultModel.GetFault()->GetStateMachine().GetStates() };
			auto const& currentState { states[params.state] };
			if (faultModel.GetFault()->IsSequentialCell() && (params.transition >= 0))
			{
				for (auto const& input: currentState.transitions[params.transition].inputs)
				{
					auto& [node, port] = input.nodeAndPort;
					assert (input.nodeAndPort.port.portType == PortType::Input);
					auto driverNodeId = node->GetDriverForPort(port)->GetNodeId();
					if (!IsConstraintTrueForLogic(result[timeframeId][driverNodeId], input.constraint))
					{
						// Reset result to initial good values so we can run an incremental simulation again.
						result[timeframeId] = base[timeframeId];
						params.transition = ((static_cast<size_t>(params.transition) + 1u) < currentState.transitions.size())
							? (params.transition + 1u)
							: TRANSITION_NONE; // Exhausted all transitions
						goto resimulate;
					}
				}
			}

			auto const targetState { (params.transition >= 0) ? currentState.transitions[params.transition].toState : 0 };
			params.state = targetState;
			params.transition = faultModel.GetFault()->IsSequentialCell()
				? (states[params.state].transitions.size() > 0u ? 0u : TRANSITION_NONE)
				: TRANSITION_UNKNOWN;
			assert (params.state < static_cast<size_t>(states.size()));
		}
	}
}

template void SimulateTestPatternNaive<Fault::FaultFreeModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::FaultFreeModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternNaive<Fault::SingleStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleStuckAtFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternNaive<Fault::MultiStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiStuckAtFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternNaive<Fault::SingleTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleTransitionDelayFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternNaive<Fault::MultiTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiTransitionDelayFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternNaive<Fault::CellAwareFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::CellAwareFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);

template void SimulateTestPatternEventDriven<Fault::FaultFreeModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::FaultFreeModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDriven<Fault::SingleStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleStuckAtFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDriven<Fault::MultiStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiStuckAtFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDriven<Fault::SingleTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleTransitionDelayFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDriven<Fault::MultiTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiTransitionDelayFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDriven<Fault::CellAwareFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::CellAwareFaultModel& faultModel, SimulationResult& result, const SimulationConfig& config);

template void SimulateTestPatternEventDrivenIncremental<Fault::SingleStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleStuckAtFaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDrivenIncremental<Fault::MultiStuckAtFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiStuckAtFaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDrivenIncremental<Fault::SingleTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::SingleTransitionDelayFaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDrivenIncremental<Fault::MultiTransitionDelayFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::MultiTransitionDelayFaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config);
template void SimulateTestPatternEventDrivenIncremental<Fault::CellAwareFaultModel>(const MappedCircuit& circuit, const TestPattern& pattern, const Fault::CellAwareFaultModel& faultModel, const SimulationResult& base, SimulationResult& result, const SimulationConfig& config);

};
};
