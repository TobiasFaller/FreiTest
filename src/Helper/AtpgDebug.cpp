#include "Helper/AtpgDebug.hpp"

#include <boost/format.hpp>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Helper/FileHandle.hpp"

using namespace FreiTest::Simulation;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Helper
{

template<typename DebugType>
SimulationDebug<DebugType> DebugSimulation(
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
)
{
	return SimulationDebug<DebugType> { circuit, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
}

template<typename DebugType>
SimulationLocationDebug<DebugType> DebugSimulationLocation(
	const size_t nodeId,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
)
{
	return SimulationLocationDebug<DebugType> { nodeId, circuit, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
}

template<typename DebugType, typename FaultModel>
FaultLocationDebug<DebugType, FaultModel> DebugFaultLocation(
	const FaultModel& fault,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
)
{
	return FaultLocationDebug<DebugType, FaultModel> { fault, circuit, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
}

template<typename PinData>
EncodingDebug<PinData> DebugEncoding(Tpg::GeneratorContext<PinData>& context)
{
	return EncodingDebug<PinData> { context };
}

template<typename DebugType>
std::ostream& operator<<(std::ostream& stream, const SimulationDebug<DebugType>& debug)
{
	auto [circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther] = debug;
	auto& circuit = circuitEnvironment.GetMappedCircuit();

	for (size_t timeframeId = 0u; timeframeId < resultGoodSim.size(); timeframeId++)
	{
		stream << "Timeframe " << std::to_string(timeframeId) << std::endl;
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); nodeId++)
		{
			stream << "NodeId: " << std::to_string(nodeId) << " ("
				<< circuit.GetNode(nodeId)->GetName() << ", "
				<< to_string(circuit.GetNode(nodeId)->GetCellType()) << "): ";
			if constexpr (std::is_same_v<DebugType, DebugSimulationGood>)
			{
				stream << to_string(resultGoodSim[timeframeId][nodeId]);
			}
			else if constexpr (std::is_same_v<DebugType, DebugSimulationGoodBad>)
			{
				auto const difference = resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId];
				auto const propagation =  resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId]
					&& IsValidLogic01(resultGoodSim[timeframeId][nodeId]) && IsValidLogic01(resultBadSim[timeframeId][nodeId]);
				stream << to_string(resultGoodSim[timeframeId][nodeId]) << "/" << to_string(resultBadSim[timeframeId][nodeId])
					<< (difference ? " <-----------" : "") << (propagation ? " !!!!!" : "");
			}
			else if constexpr (std::is_same_v<DebugType, DebugSimulationVsCheck>)
			{
				auto const difference = resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId]
					|| resultGoodOther[timeframeId][nodeId] != resultBadOther[timeframeId][nodeId];
				auto const propagation = (resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId]
						&& IsValidLogic01(resultGoodSim[timeframeId][nodeId]) && IsValidLogic01(resultBadSim[timeframeId][nodeId]))
					|| (resultGoodOther[timeframeId][nodeId] != resultBadOther[timeframeId][nodeId]
						&& IsValidLogic01(resultGoodOther[timeframeId][nodeId]) && IsValidLogic01(resultBadOther[timeframeId][nodeId]));
				stream << "SIM=" << to_string(resultGoodSim[timeframeId][nodeId]) << "/" << to_string(resultBadSim[timeframeId][nodeId]) << " vs "
					<< to_string(resultGoodOther[timeframeId][nodeId]) << "/" << to_string(resultBadOther[timeframeId][nodeId]) << "=CHECK"
					<< (difference ? " <-----------" : "") << (propagation ? " !!!!!" : "");;
			}
			else if constexpr (std::is_same_v<DebugType, DebugSimulationVsAtpg>)
			{
				auto const difference = resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId]
					|| resultGoodOther[timeframeId][nodeId] != resultBadOther[timeframeId][nodeId];
				auto const propagation = (resultGoodSim[timeframeId][nodeId] != resultBadSim[timeframeId][nodeId]
						&& IsValidLogic01(resultGoodSim[timeframeId][nodeId]) && IsValidLogic01(resultBadSim[timeframeId][nodeId]))
					|| (resultGoodOther[timeframeId][nodeId] != resultBadOther[timeframeId][nodeId]
						&& IsValidLogic01(resultGoodOther[timeframeId][nodeId]) && IsValidLogic01(resultBadOther[timeframeId][nodeId]));
				stream << "SIM=" << to_string(resultGoodSim[timeframeId][nodeId]) << "/" << to_string(resultBadSim[timeframeId][nodeId]) << " vs "
					<< to_string(resultGoodOther[timeframeId][nodeId]) << "/" << to_string(resultBadOther[timeframeId][nodeId]) << "=ATPG"
					<< (difference ? " <-----------" : "") << (propagation ? " !!!!!" : "");;
			}
			else
			{
				Logging::Panic("Not implemented");
			}
			stream << std::endl;
		}
	}
	return stream;
}

struct LogicDebug
{
	Basic::Logic value;
	bool marked;
};

std::ostream& operator<<(std::ostream& stream, const LogicDebug& debug)
{
	return stream << (debug.marked ? ">" : " ") << to_string(debug.value) << (debug.marked ? "<" : " ");
}

template<typename DebugType>
std::ostream& operator<<(std::ostream& stream, const SimulationLocationDebug<DebugType>& debug)
{
	auto [nodeId, circuit, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther] = debug;

	auto const print_line = [&](auto const& name, auto const& func) -> auto {
		stream << name;
		for (size_t timeframe = 0u; timeframe < resultGoodSim.GetNumberOfTimeframes(); timeframe++)
		{
			stream << func(timeframe) << " | ";
		}
		stream << std::endl;
	};

	print_line("timeframe:  ", [&](auto const& timeframe) { return boost::str(boost::format("% 3d") % timeframe); });
	if constexpr (std::is_same_v<DebugType, DebugSimulationGood>)
	{
		print_line("sim good:  ", [&](auto const& timeframe) { return LogicDebug { resultGoodSim[timeframe][nodeId], false }; });
	}
	else if constexpr (std::is_same_v<DebugType, DebugSimulationGoodBad>)
	{
		print_line("sim good:   ", [&](auto const& timeframe) { return LogicDebug { resultGoodSim[timeframe][nodeId], false }; });
		print_line("sim bad:    ", [&](auto const& timeframe) {
			auto const difference = resultGoodSim[timeframe][nodeId] != resultBadSim[timeframe][nodeId];
			return LogicDebug { resultBadOther[timeframe][nodeId], difference };
		});
		print_line("difference: ", [&](auto const& timeframe) {
			auto const difference = resultGoodSim[timeframe][nodeId] != resultBadSim[timeframe][nodeId];
			return difference ? "DIF" : "___";
		});
	}
	else if constexpr (std::is_same_v<DebugType, DebugSimulationVsCheck>)
	{
		print_line("sim good:   ", [&](auto const& timeframe) { return LogicDebug { resultGoodSim[timeframe][nodeId], false }; });
		print_line("sim bad:    ", [&](auto const& timeframe) { return LogicDebug { resultBadSim[timeframe][nodeId], false }; });
		print_line("check good: ", [&](auto const& timeframe) {
			auto const difference = resultGoodSim[timeframe][nodeId] != resultGoodOther[timeframe][nodeId];
			return LogicDebug { resultGoodOther[timeframe][nodeId], difference };
		});
		print_line("check bad:  ", [&](auto const& timeframe) {
			auto const difference = resultBadSim[timeframe][nodeId] != resultBadOther[timeframe][nodeId];
			return LogicDebug { resultBadSim[timeframe][nodeId], difference };
		});
		print_line("mismatch:   ", [&](auto const& timeframe) {
			auto const mismatch = resultGoodSim[timeframe][nodeId] != resultGoodOther[timeframe][nodeId]
				|| resultBadSim[timeframe][nodeId] != resultBadOther[timeframe][nodeId];
			return mismatch ? "ERR" : "___";
		});
	}
	else if constexpr (std::is_same_v<DebugType, DebugSimulationVsAtpg>)
	{
		print_line("sim good:   ", [&](auto const& timeframe) { return LogicDebug { resultGoodSim[timeframe][nodeId], false }; });
		print_line("sim bad:    ", [&](auto const& timeframe) { return LogicDebug { resultBadSim[timeframe][nodeId], false }; });
		print_line("atpg good:  ", [&](auto const& timeframe) {
			auto const difference = resultGoodSim[timeframe][nodeId] != resultGoodOther[timeframe][nodeId];
			return LogicDebug { resultGoodOther[timeframe][nodeId], difference };
		});
		print_line("atpg bad:   ", [&](auto const& timeframe) {
			auto const difference = resultBadSim[timeframe][nodeId] != resultBadOther[timeframe][nodeId];
			return LogicDebug { resultBadOther[timeframe][nodeId], difference };
		});
		print_line("mismatch:   ", [&](auto const& timeframe) {
			auto const mismatch = resultGoodSim[timeframe][nodeId] != resultGoodOther[timeframe][nodeId]
				|| resultBadSim[timeframe][nodeId] != resultBadOther[timeframe][nodeId];
			auto const difference = resultGoodSim[timeframe][nodeId] != resultBadSim[timeframe][nodeId]
				|| resultGoodOther[timeframe][nodeId] != resultBadOther[timeframe][nodeId];
			return mismatch ? "ERR" : (difference ? "DIF" :  "___");
		});
	}
	else
	{
		Logging::Panic("Not Implemented");
	}

	return stream;
}

template<typename DebugType, typename FaultModel>
std::ostream& operator<<(std::ostream& stream, const FaultLocationDebug<DebugType, FaultModel>& debug)
{
	auto const [faultModel, circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther] = debug;
	auto const& circuit = circuitEnvironment.GetMappedCircuit();

	auto const print_line = [&](auto const& name, auto const& func) -> auto {
		stream << name;
		for (size_t timeframe = 0u; timeframe < resultGoodSim.GetNumberOfTimeframes(); timeframe++)
		{
			stream << func(timeframe) << " | ";
		}
		stream << std::endl;
	};

	std::vector<bool> faultActivated(resultGoodSim.GetNumberOfTimeframes(), false);
	std::vector<bool> faultPropagatedToPrimary(resultGoodSim.GetNumberOfTimeframes(), false);
	std::vector<bool> faultPropagatedToSecondary(resultGoodSim.GetNumberOfTimeframes(), false);
	auto const print_activation_and_propagation = [&]() {
		print_line("timeframe:  ", [&](auto const& timeframe) { return boost::str(boost::format("% 3d") % timeframe); });
		print_line("activated:  ", [&](auto const& timeframe) { return faultActivated[timeframe] ? "ACT" : "___"; });
		print_line("prop. PO:   ", [&](auto const& timeframe) { return faultPropagatedToPrimary[timeframe] ? "PRP" : "___"; });
		print_line("prop. SO:   ", [&](auto const& timeframe) { return faultPropagatedToSecondary[timeframe] ? "PRP" : "___"; });
	};

	for (size_t timeframe { 0u }; timeframe < resultGoodSim.GetNumberOfTimeframes(); timeframe++)
	{
		for (auto const primaryOutput : circuit.GetPrimaryOutputs())
		{
			bool propagated = false;
			if constexpr (std::is_same_v<DebugType, DebugSimulationGoodBad>
				|| std::is_same_v<DebugType, DebugSimulationVsAtpg>
				|| std::is_same_v<DebugType, DebugSimulationVsCheck>)
			{
				auto const& goodValue { resultGoodSim[timeframe][primaryOutput->GetNodeId()] };
				auto const& badValue { resultBadSim[timeframe][primaryOutput->GetNodeId()] };
				propagated |= (goodValue == Basic::Logic::LOGIC_ZERO && badValue == Basic::Logic::LOGIC_ONE)
					|| (goodValue == Basic::Logic::LOGIC_ONE && badValue == Basic::Logic::LOGIC_ZERO);
			}
			if constexpr (std::is_same_v<DebugType, DebugSimulationVsAtpg>
				|| std::is_same_v<DebugType, DebugSimulationVsCheck>)
			{
				auto const& goodValue { resultGoodOther[timeframe][primaryOutput->GetNodeId()] };
				auto const& badValue { resultBadOther[timeframe][primaryOutput->GetNodeId()] };
				propagated |= (goodValue == Basic::Logic::LOGIC_ZERO && badValue == Basic::Logic::LOGIC_ONE)
					|| (goodValue == Basic::Logic::LOGIC_ONE && badValue == Basic::Logic::LOGIC_ZERO);
			}
			faultPropagatedToPrimary[timeframe] = faultPropagatedToPrimary[timeframe] | propagated;
		}

		for (auto const secondaryOutput : circuit.GetSecondaryOutputs())
		{
			bool propagated = false;
			if constexpr (std::is_same_v<DebugType, DebugSimulationGoodBad>
				|| std::is_same_v<DebugType, DebugSimulationVsAtpg>
				|| std::is_same_v<DebugType, DebugSimulationVsCheck>)
			{
				auto const& goodValue { resultGoodSim[timeframe][secondaryOutput->GetNodeId()] };
				auto const& badValue { resultBadSim[timeframe][secondaryOutput->GetNodeId()] };
				propagated |= (goodValue == Basic::Logic::LOGIC_ZERO && badValue == Basic::Logic::LOGIC_ONE)
					|| (goodValue == Basic::Logic::LOGIC_ONE && badValue == Basic::Logic::LOGIC_ZERO);
			}
			if constexpr (std::is_same_v<DebugType, DebugSimulationVsAtpg>
				|| std::is_same_v<DebugType, DebugSimulationVsCheck>)
			{
				auto const& goodValue { resultGoodOther[timeframe][secondaryOutput->GetNodeId()] };
				auto const& badValue { resultBadOther[timeframe][secondaryOutput->GetNodeId()] };
				propagated |= (goodValue == Basic::Logic::LOGIC_ZERO && badValue == Basic::Logic::LOGIC_ONE)
					|| (goodValue == Basic::Logic::LOGIC_ONE && badValue == Basic::Logic::LOGIC_ZERO);
			}
			faultPropagatedToSecondary[timeframe] = faultPropagatedToSecondary[timeframe] | propagated;
		}
	}

	auto const compute_stuck_at_fault_activation = [&](auto stuckAt) -> void {
		auto const driver { circuit.GetDriverForPort(stuckAt.GetNodeAndPort()) };
		for (size_t timeframe { 0u }; timeframe < resultGoodSim.GetNumberOfTimeframes(); timeframe++)
		{
			Basic::LogicConstraint constraint;
			switch (stuckAt.GetType())
			{
				case Fault::StuckAtFaultType::STUCK_AT_0: constraint = Basic::LogicConstraint::ONLY_LOGIC_ONE; break;
				case Fault::StuckAtFaultType::STUCK_AT_1: constraint = Basic::LogicConstraint::ONLY_LOGIC_ZERO; break;
				case Fault::StuckAtFaultType::STUCK_AT_X: constraint = Basic::LogicConstraint::ALWAYS_FALSE; break;
				case Fault::StuckAtFaultType::STUCK_AT_U: constraint = Basic::LogicConstraint::ALWAYS_FALSE; break;
				case Fault::StuckAtFaultType::STUCK_AT_FREE: constraint = Basic::LogicConstraint::ALWAYS_FALSE; break;
				default: Logging::Panic();
			}
			faultActivated[timeframe] = faultActivated[timeframe]
				| Basic::IsConstraintTrueForLogic(resultGoodSim[timeframe][driver->GetNodeId()], constraint);
		}
	};
	auto const compute_transition_delay_fault_activation = [&](auto transitionDelay) -> void {
		auto const driver { circuit.GetDriverForPort(transitionDelay.GetNodeAndPort()) };
		for (size_t timeframe { 0u }; timeframe + 1u < resultGoodSim.GetNumberOfTimeframes(); timeframe++)
		{
			bool activated = false;
			switch (transitionDelay.GetType())
			{
				case Fault::TransitionDelayFaultType::SLOW_TO_RISE:
					activated = (resultGoodSim[timeframe][driver->GetNodeId()] == Basic::Logic::LOGIC_ZERO)
						&& (resultGoodSim[timeframe + 1u][driver->GetNodeId()] == Basic::Logic::LOGIC_ONE);
					break;
				case Fault::TransitionDelayFaultType::SLOW_TO_FALL:
					activated = (resultGoodSim[timeframe][driver->GetNodeId()] == Basic::Logic::LOGIC_ONE)
						&& (resultGoodSim[timeframe + 1u][driver->GetNodeId()] == Basic::Logic::LOGIC_ZERO);
					break;
				case Fault::TransitionDelayFaultType::SLOW_TO_TRANSITION:
					activated = ((resultGoodSim[timeframe][driver->GetNodeId()] == Basic::Logic::LOGIC_ONE)
							&& (resultGoodSim[timeframe + 1u][driver->GetNodeId()] == Basic::Logic::LOGIC_ZERO))
						|| ((resultGoodSim[timeframe][driver->GetNodeId()] == Basic::Logic::LOGIC_ZERO)
							&& (resultGoodSim[timeframe + 1u][driver->GetNodeId()] == Basic::Logic::LOGIC_ONE));
					break;
				default: Logging::Panic();
			}
			faultActivated[timeframe + 1u] = faultActivated[timeframe + 1u] | activated;
		}
	};
	auto const NO_TRANSITION = std::numeric_limits<size_t>::max();
	auto const compute_cell_aware_fault_activation = [&](auto stateMachine, auto const& sim) -> auto {
		std::vector<size_t> statesOut;
		std::vector<size_t> transitionsOut;

		size_t currentState { 0u };
		for (size_t timeframe { 0u }; timeframe < sim.GetNumberOfTimeframes(); timeframe++)
		{
			size_t nextState { 0u };

			auto const& transitions { stateMachine.GetState(currentState).GetTransitions() };
			for (size_t transitionId { 0u }; transitionId < transitions.size(); transitionId++)
			{
				auto const& transition { transitions[transitionId] };

				bool satisfied = true;
				for (auto& [nodeAndPort, constraint] : transition.GetInputs())
				{
					auto const targetNodeId { circuit.GetDriverForPort(nodeAndPort)->GetNodeId() };
					auto const value { sim[timeframe][targetNodeId] };
					satisfied &= IsConstraintTrueForLogic(value, constraint);
				}
				if (satisfied)
				{
					transitionsOut.push_back(transitionId);
					nextState = transition.GetTargetState();
					faultActivated[timeframe] = faultActivated[timeframe] | transition.IsAccepting();
					break;
				}
			}
			statesOut.push_back(currentState);
			transitionsOut.resize(statesOut.size(), NO_TRANSITION);
			currentState = nextState;
		}

		return std::make_tuple(statesOut, transitionsOut);
	};

	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		auto const fault = faultModel.GetFault();
		auto const stuckAt = fault->GetStuckAt();
		compute_stuck_at_fault_activation(stuckAt);

		stream << "Fault: " << to_string(*fault) << std::endl;
		print_activation_and_propagation();

		auto const& [faultNode, faultPort] = stuckAt.GetNodeAndPort();
		for (size_t inputIndex { 0u }; inputIndex < faultNode->GetNumberOfInputs(); inputIndex++) {
			auto const driver { circuit.GetDriverForPort({ faultNode, { Circuit::PortType::Input, inputIndex } }) };
			stream << "Input " << std::to_string(inputIndex) << " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
		}
		stream << "Output:" << std::endl;
		stream << SimulationLocationDebug<DebugType>
				{ faultNode->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
			<< std::endl;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		auto const fault = faultModel.GetFault();
		for (auto const& stuckAt : fault->GetStuckAts())
		{
			compute_stuck_at_fault_activation(stuckAt);
		}

		stream << "Fault: " << to_string(*fault) << std::endl;
		print_activation_and_propagation();

		for (auto const& stuckAt : fault->GetStuckAts())
		{
			auto const& [faultNode, faultPort] = stuckAt.GetNodeAndPort();
			for (size_t inputIndex { 0u }; inputIndex < faultNode->GetNumberOfInputs(); inputIndex++) {
				auto const driver { circuit.GetDriverForPort({ faultNode, { Circuit::PortType::Input, inputIndex } }) };
				stream << "Input " << std::to_string(inputIndex) << " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
				stream << SimulationLocationDebug<DebugType>
						{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
			}
			stream << "Output:" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ faultNode->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
				<< std::endl;
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		auto const fault = faultModel.GetFault();
		auto const transitionDelay = fault->GetTransitionDelay();
		compute_transition_delay_fault_activation(transitionDelay);

		stream << "Fault: " << to_string(*fault) << std::endl;
		print_activation_and_propagation();

		auto const& [faultNode, faultPort] = transitionDelay.GetNodeAndPort();
		for (size_t inputIndex { 0u }; inputIndex < faultNode->GetNumberOfInputs(); inputIndex++) {
			auto const driver { circuit.GetDriverForPort({ faultNode, { Circuit::PortType::Input, inputIndex } }) };
			stream << "Input " << std::to_string(inputIndex) << " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
		}
		stream << "Output:" << std::endl;
		stream << SimulationLocationDebug<DebugType>
				{ faultNode->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
			<< std::endl;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		auto const fault = faultModel.GetFault();
		for (auto const& transitionDelay : fault->GetTransitionDelays())
		{
			compute_transition_delay_fault_activation(transitionDelay);
		}

		stream << "Fault: " << to_string(*fault) << std::endl;
		print_activation_and_propagation();

		for (auto const& transitionDelay : fault->GetTransitionDelays())
		{
			auto const& [faultNode, faultPort] = transitionDelay.GetNodeAndPort();
			for (size_t inputIndex { 0u }; inputIndex < faultNode->GetNumberOfInputs(); inputIndex++) {
				auto const driver { circuit.GetDriverForPort({ faultNode, { Circuit::PortType::Input, inputIndex } }) };
				stream << "Input " << std::to_string(inputIndex) << " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
				stream << SimulationLocationDebug<DebugType>
						{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther };
			}
			stream << "Output:" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ faultNode->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
				<< std::endl;
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto const fault = faultModel.GetFault();
		const auto& stateMachine = fault->GetStateMachine();
		const auto [goodSimStates, goodSimTransitions] = compute_cell_aware_fault_activation(stateMachine, resultGoodSim);
		const auto [badSimStates, badSimTransitions] = compute_cell_aware_fault_activation(stateMachine, resultBadSim);
		const auto [goodOtherStates, goodOtherTransitions] = compute_cell_aware_fault_activation(stateMachine, resultGoodOther);
		const auto [badOtherStates, badOtherTransitions] = compute_cell_aware_fault_activation(stateMachine, resultBadOther);

		stream << "Fault: " << to_string(*fault) << std::endl;
		stream << "State Machine: " << to_string(stateMachine) << std::endl << std::endl;
		print_activation_and_propagation();
		print_line("gsim state: ", [&](auto timeframe) { return boost::str(boost::format("% 3d") % goodSimStates[timeframe]); });
		print_line("gsim tran.: ", [&](auto timeframe) {
			return goodSimTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % goodSimTransitions[timeframe]) : "   ";
		});
		if constexpr (std::is_same_v<DebugType, DebugSimulationGoodBad>
			|| std::is_same_v<DebugType, DebugSimulationVsAtpg>
			|| std::is_same_v<DebugType, DebugSimulationVsCheck>)
		{
			print_line("bsim state: ", [&](auto timeframe) { return boost::str(boost::format("% 3d") % badSimStates[timeframe]); });
			print_line("bsim tran.: ", [&](auto timeframe) {
				return badSimTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % badSimTransitions[timeframe]) : "   ";
			});
		}
		if constexpr (std::is_same_v<DebugType, DebugSimulationVsAtpg>)
		{
			print_line("g atpg state:", [&](auto timeframe) { return boost::str(boost::format("% 3d") % goodOtherStates[timeframe]); });
			print_line("g atpg tran.:", [&](auto timeframe) {
				return goodOtherTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % goodOtherTransitions[timeframe]) : "   ";
			});
			print_line("b atpg state:", [&](auto timeframe) { return boost::str(boost::format("% 3d") % badOtherStates[timeframe]); });
			print_line("b atpg tran.:", [&](auto timeframe) {
				return badOtherTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % badOtherTransitions[timeframe]) : "   ";
			});
		}
		if constexpr (std::is_same_v<DebugType, DebugSimulationVsCheck>)
		{
			print_line("g chck state:", [&](auto timeframe) { return boost::str(boost::format("% 3d") % goodOtherStates[timeframe]); });
			print_line("g chck tran.:", [&](auto timeframe) {
				return goodOtherTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % goodOtherTransitions[timeframe]) : "   ";
			});
			print_line("b chck state:", [&](auto timeframe) { return boost::str(boost::format("% 3d") % badOtherStates[timeframe]); });
			print_line("b chck tran.:", [&](auto timeframe) {
				return badOtherTransitions[timeframe] != NO_TRANSITION ? boost::str(boost::format("% 3d") % badOtherTransitions[timeframe]) : "   ";
			});
		}
		stream << std::endl;

		stream << "Fault Inputs:" << std::endl;
		for (const auto& [inputNode, inputPort] : stateMachine.GetInputs())
		{
			auto const driver { circuit.GetDriverForPort({ inputNode, inputPort }) };
			stream << "Input @node id " << std::to_string(inputNode->GetNodeId()) << " @" << to_string(inputPort)
				<< " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
				<< std::endl;
		}

		stream << "Fault Outputs:" << std::endl;
		for (const auto& [outputNode, outputPort] : stateMachine.GetOutputs())
		{
			auto const driver { circuit.GetDriverForPort({ outputNode, outputPort }) };
			stream << "Output @node id " << std::to_string(outputNode->GetNodeId()) + " @" + to_string(outputPort)
				<< " (driver is node id " << std::to_string(driver->GetNodeId()) << "):" << std::endl;
			stream << SimulationLocationDebug<DebugType>
					{ driver->GetNodeId(), circuitEnvironment, resultGoodSim, resultBadSim, resultGoodOther, resultBadOther }
				<< std::endl;
		}
	}
	else
	{
		LOG(FATAL) << "Fault model not implemented";
		__builtin_unreachable();
	}

	return stream;
}

template<typename PinData>
std::ostream& operator<<(std::ostream& stream, const EncodingDebug<PinData>& debug)
{
	auto const [context] = debug;
	auto const& circuit { context.GetCircuitEnvironment() };
	auto const& mappedCircuit { context.GetMappedCircuit() };
	auto const& metaData { context.GetCircuitMetaData() };

	auto const convert_container = [&](auto container) -> std::string {
		if (!container.IsSet())
		{
			return "--";
		}

		if constexpr (is_01_encoding_v<decltype(container)>)
		{
			return std::to_string(container.l0.GetSigned());
		}
		else if constexpr (is_01X_encoding_v<decltype(container)>
			|| is_01F_encoding_v<decltype(container)>)
		{
			return std::to_string(container.l0.GetSigned())
				+ " " + std::to_string(container.l1.GetSigned());
		}
		else if constexpr (is_U01X_encoding_v<decltype(container)>
			|| is_U01F_encoding_v<decltype(container)>)
		{
			return std::to_string(container.l0.GetSigned())
				+ " " + std::to_string(container.l1.GetSigned());
		}
	};
	auto const convert_pin_data = [&](auto const& timeframe, auto nodeId, auto portType, auto portIndex) -> std::string {
		std::string result;
		if constexpr (has_good_encoding_v<PinData>)
		{
			const auto container { timeframe.template GetContainer<GoodTag>(
				context, nodeId, portType, portIndex) };
			if (!result.empty()) result += " / ";
			result += convert_container(container);
		}
		if constexpr (has_bad_encoding_v<PinData>)
		{
			const auto container { timeframe.template GetContainer<BadTag>(
				context, nodeId, portType, portIndex) };
			if (!result.empty()) result += " / ";
			result += convert_container(container);
		}
		if constexpr (has_diff_encoding_v<PinData>)
		{
			const auto container { timeframe.template GetContainer<DiffTag>(
				context, nodeId, portType, portIndex) };
			if (!result.empty()) result += " / ";
			result += convert_container(container);
		}
		return result;
	};

	stream << (boost::format("Circuit: %s") % circuit->GetName()) << std::endl;
	stream << (boost::format("    Primary Inputs: %d (Normal Inputs)") % mappedCircuit.GetNumberOfPrimaryInputs()) << std::endl;
	stream << (boost::format("    Primary Outputs: %d (Normal Outputs)") % mappedCircuit.GetNumberOfPrimaryOutputs()) << std::endl;
	stream << (boost::format("    Sequential Elements: %d (Flip-Flops, Latches)") % mappedCircuit.GetNumberOfSecondaryInputs()) << std::endl;
	stream << std::endl;

	const auto timeframes { context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitEncoding) };
	for (size_t timeframeIndex { 0u }; timeframeIndex < timeframes; timeframeIndex++)
	{
		auto const& timeframe { context.GetTimeframe(timeframeIndex) };
		stream << (boost::format("Timeframe: %d") % timeframeIndex) << std::endl;

		// Primary inputs and outputs
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumeratePrimaryInputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ inputNode, { Circuit::PortType::Output, 0u } }) };
			stream
				<< (boost::format("    PI %s: %s") % friendlyName
					% convert_pin_data(timeframe, inputNode->GetNodeId(), Circuit::PortType::Output, 0u))
				<< std::endl;
		}
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumeratePrimaryOutputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			stream
				<< (boost::format("    PO %s: %s") % friendlyName
					% convert_pin_data(timeframe, outputNode->GetNodeId(), Circuit::PortType::Input, 0u))
				<< std::endl;
		}

		// Secondary inputs and outputs (sequential elements)
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumerateSecondaryInputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ inputNode->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }) };
			stream
				<< (boost::format("    SI %s: %s") % friendlyName
					% convert_pin_data(timeframe, inputNode->GetNodeId(), Circuit::PortType::Output, 0u))
				<< std::endl;
		}
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumerateSecondaryOutputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			stream
				<< (boost::format("    SO %s: %s") % friendlyName
					% convert_pin_data(timeframe, outputNode->GetNodeId(), Circuit::PortType::Input, 0u))
				<< std::endl;
		}
		stream << "    --" << std::endl;

		auto const walk_groups = [&](auto const& self, auto const* group) -> void {
			const auto& sourceInfo { group->GetSourceInfo() };
			const auto isCell { sourceInfo.template GetProperty<bool>("module-is-cell").value_or(false) };
			const auto isPrimitive { sourceInfo.template GetProperty<bool>("module-is-primitive").value_or(false) };
			if (isCell || isPrimitive)
			{
				for (auto& port: group->GetPorts())
				{
					const auto portType { port.GetSourceInfo().template GetProperty<std::string>("port-type").value_or("") };
					if (portType != "input" && portType != "output")
					{
						continue;
					}

					const auto friendlyName { port.GetHierarchyName() };
					for (auto index : port.GetSize().GetIndicesBottomToTop())
					{
						if (auto const* connection { port.GetConnectionForIndex(index) };
							connection != nullptr && connection->GetMappedSources().size() == 1u)
						{
							auto const& nodeAndPort { connection->GetMappedSources()[0] };
							stream
								<< (boost::format("    GT %s [%i]: %s") % friendlyName % index
									% convert_pin_data(timeframe, nodeAndPort.node->GetNodeId(), nodeAndPort.port.portType, nodeAndPort.port.portNumber))
								<< std::endl;
						}
					}
				}
			}

			for (auto const* subGroup : group->GetSubGroups())
			{
				self(self, subGroup);
			}
		};
		walk_groups(walk_groups, metaData.GetRoot());
		stream << std::endl;
	}

	return stream;
}

void DebugExportSimulation(
	const std::string& name,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& result
)
{
	FileHandle handleTrace { boost::str(boost::format("%s-trace.txt") % name), false };
	FileHandle handleCompact { boost::str(boost::format("%s-compact.txt") % name), false };
	handleTrace.GetOutStream() << to_string(result, circuit.GetMappedCircuit());
	handleCompact.GetOutStream() << to_string(result);
}

#define SIMULATION_DEBUG(TAG) \
	template SimulationDebug<TAG> DebugSimulation(const Circuit::CircuitEnvironment&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&); \
	template std::ostream& operator<<(std::ostream& stream, const SimulationDebug<TAG>& debug);

#define SIMULATION_LOCATION_DEBUG(TAG) \
	template SimulationLocationDebug<TAG> DebugSimulationLocation(const size_t, const Circuit::CircuitEnvironment&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&); \
	template std::ostream& operator<<(std::ostream& stream, const SimulationLocationDebug<TAG>& debug);

#define FAULT_LOCATION_DEBUG(TAG, FAULT_MODEL) \
	template FaultLocationDebug<TAG, FAULT_MODEL> DebugFaultLocation(const FAULT_MODEL&, const Circuit::CircuitEnvironment&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&, const Simulation::SimulationResult&); \
	template std::ostream& operator<<(std::ostream& stream, const FaultLocationDebug<TAG, FAULT_MODEL>& debug);

#define INSTANTIATE_FOR_TAG(TAG) \
	SIMULATION_DEBUG(TAG) \
	SIMULATION_LOCATION_DEBUG(TAG) \
	FAULT_LOCATION_DEBUG(TAG, Fault::FaultFreeModel) \
	FAULT_LOCATION_DEBUG(TAG, Fault::SingleStuckAtFaultModel) \
	FAULT_LOCATION_DEBUG(TAG, Fault::MultiStuckAtFaultModel) \
	FAULT_LOCATION_DEBUG(TAG, Fault::SingleTransitionDelayFaultModel) \
	FAULT_LOCATION_DEBUG(TAG, Fault::MultiTransitionDelayFaultModel) \
	FAULT_LOCATION_DEBUG(TAG, Fault::CellAwareFaultModel)

INSTANTIATE_FOR_TAG(DebugSimulationGood)
INSTANTIATE_FOR_TAG(DebugSimulationGoodBad)
INSTANTIATE_FOR_TAG(DebugSimulationVsCheck)
INSTANTIATE_FOR_TAG(DebugSimulationVsAtpg)

#define FOR_PINDATA(PINDATA) \
	template EncodingDebug<PINDATA> DebugEncoding(Tpg::GeneratorContext<PINDATA>&); \
	template std::ostream& operator<<(std::ostream& stream, const EncodingDebug<PINDATA>& debug);
INSTANTIATE_FOR_ALL_PINDATA

}
}
