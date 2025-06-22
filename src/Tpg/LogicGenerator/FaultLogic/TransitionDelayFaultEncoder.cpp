#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
TransitionDelayFaultEncoder<PinData, FaultModel>::TransitionDelayFaultEncoder(std::shared_ptr<FaultModel> faultModel):
	_faultModel(faultModel),
	_badContainerForTimeframe(),
	_diffContainerForTimeframe()
{
}

template<typename PinData, typename FaultModel>
TransitionDelayFaultEncoder<PinData, FaultModel>::~TransitionDelayFaultEncoder(void) = default;

template<typename PinData, typename FaultModel>
std::string TransitionDelayFaultEncoder<PinData, FaultModel>::GetName(void) const
{
	return "TransitionDelayFaultEncoder";
}

template<typename PinData, typename FaultModel>
void TransitionDelayFaultEncoder<PinData, FaultModel>::Reset(void)
{
	_badContainerForTimeframe = { };
	_diffContainerForTimeframe = { };
}

template<typename PinData, typename FaultModel>
bool TransitionDelayFaultEncoder<PinData, FaultModel>::PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	auto const& fault { _faultModel->GetFault() };
	auto& encoder { context.GetEncoder() };

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	auto const get_slow_transition_count = [&]() -> size_t {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return 1u;
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
		{
			return fault->GetTransitionDelays().size();
		}
	};

	while (_badContainerForTimeframe.size() <= timeframeId)
	{
		auto& badContainers = _badContainerForTimeframe.emplace_back();
		auto& diffContainers = _diffContainerForTimeframe.emplace_back();
		for (size_t index { 0u }; index < get_slow_transition_count(); index++)
		{
			badContainers.push_back(encoder.template NewLogicContainer<BadContainer>());
			diffContainers.push_back(DiffContainer());
		}
	}

	return true;
}

template<typename PinData, typename FaultModel>
bool TransitionDelayFaultEncoder<PinData, FaultModel>::PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	const auto& fault { _faultModel->GetFault() };
	const auto& timeframe { context.GetTimeframe(timeframeId) };
	auto& encoder { context.GetEncoder() };

	auto get_slow_transitions = [&]() -> std::vector<Fault::TransitionDelayFault> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return { fault->GetTransitionDelay() };
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
		{
			return fault->GetTransitionDelays();
		}
	};

	auto& transitionInitialValid = _transitionInitialValidForTimeframe.emplace_back();
	auto& transitionInitialValue = _transitionInitialValueForTimeframe.emplace_back();

	size_t index { 0u };
	for (auto const& transition : get_slow_transitions())
	{
		LogicConstraint initialConstraint;
		LogicConstraint transitionConstraint;
		switch (transition.GetType())
		{
			case Fault::TransitionDelayFaultType::SLOW_TO_RISE:
				initialConstraint = LogicConstraint::ONLY_LOGIC_ZERO;
				transitionConstraint = LogicConstraint::ONLY_LOGIC_ONE;
				break;
			case Fault::TransitionDelayFaultType::SLOW_TO_FALL:
				initialConstraint = LogicConstraint::ONLY_LOGIC_ONE;
				transitionConstraint = LogicConstraint::ONLY_LOGIC_ZERO;
				break;
			case Fault::TransitionDelayFaultType::SLOW_TO_TRANSITION:
				initialConstraint = LogicConstraint::ONLY_LOGIC_01;
				transitionConstraint = LogicConstraint::ONLY_LOGIC_01;
				break;
			default: Logging::Panic("Unknown transition delay fault type");
		}

		const auto& [node, port] = transition.GetNodeAndPort();
		auto const badContainer { timeframe.template GetContainer<BadTag>(
			context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
		const auto initialValueValid { encoder.EncodeLogicValueConstraintDetector(badContainer, initialConstraint) };
		const auto initialValue { encoder.EncodeLogicValueDetector(badContainer, Logic::LOGIC_ONE) };

		const auto originalContainer { timeframe.template GetContainer<BadTag>(
			context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Disable) };
		const auto transitionValueValid { encoder.EncodeLogicValueConstraintDetector(originalContainer, transitionConstraint) };
		const auto transitionValue { encoder.EncodeLogicValueDetector(originalContainer, Logic::LOGIC_ONE) };

		LogicContainer01 transitionCondition;
		if (satSolver)
		{
			// Simple logic container (no initial state hacks needed)
			transitionInitialValid.push_back(initialValueValid);
			transitionInitialValue.push_back(initialValue);

			transitionCondition = (timeframeId == 0u)
				? encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO)
				: encoder.EncodeAnd(std::vector<LogicContainer01> {
					_transitionInitialValidForTimeframe[timeframeId - 1u][index],
					transitionValueValid,
					// This XOR is required for the slow-to-transition type
					// where the constraints don't include the inverse relation
					// of the intial and final value of the transition.
					encoder.EncodeXor(
						_transitionInitialValueForTimeframe[timeframeId - 1u][index],
						transitionValue
					)
				});
		}

		if (bmcSolver)
		{
			// Latches with multiplexer for initial state (No undefined initial state is allowed).
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);
			auto activationLatch = encoder.template NewLogicContainer<LogicContainer01>();
			auto activationValueLatch = encoder.template NewLogicContainer<LogicContainer01>();

			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(activationLatch, Logic::LOGIC_ZERO);
			encoder.EncodeLogicValue(activationValueLatch, Logic::LOGIC_ZERO);

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			bmcSolver->CommitTimeframeClause(-initialValueValid.l0, 0u,  activationLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause( initialValueValid.l0, 0u, -activationLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause(-initialValue.l0, 0u,  activationValueLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause( initialValue.l0, 0u, -activationValueLatch.l0, 1u);

			transitionInitialValid.push_back(activationLatch);
			transitionInitialValue.push_back(activationValueLatch);

			transitionCondition = encoder.EncodeAnd(std::vector<LogicContainer01> {
				activationLatch,
				transitionValueValid,
				// This XOR is required for the slow-to-transition type
				// where the constraints don't include the inverse relation
				// of the intial and final value of the transition.
				encoder.EncodeXor(
					activationValueLatch,
					transitionValue
				)
			});
		}

		encoder.EncodeImplication( transitionCondition, encoder.EncodeLogicEquivalenceDetector(badContainer, -originalContainer));
		encoder.EncodeImplication(-transitionCondition, encoder.EncodeLogicEquivalenceDetector(badContainer,  originalContainer));
		index++;
	}

	return true;
}

template<typename PinData, typename FaultModel>
const void* TransitionDelayFaultEncoder<PinData, FaultModel>::GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframeId, size_t nodeId, PortType portType, size_t portId)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	const auto get_slow_transitions = [&]() -> std::vector<Fault::TransitionDelayFault> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return { _faultModel->GetFault()->GetTransitionDelay() };
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
		{
			return _faultModel->GetFault()->GetTransitionDelays();
		}
	};

	if constexpr (has_bad_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(BadTag))
		{
			size_t index { 0u };
			for (auto const& transitionDelay : get_slow_transitions())
			{
				if (transitionDelay.GetNode()->GetNodeId() == nodeId
					&& transitionDelay.GetPort().portType == portType
					&& transitionDelay.GetPort().portNumber == portId)
				{
					return &_badContainerForTimeframe[timeframeId][index];
				}

				index++;
			}
		}
	}

	if constexpr (has_diff_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(DiffTag))
		{
			// The difference container has to be overriden by the fault injector too.
			// This is because otherwise the D-Chain will link the inputs of the fault location
			// to the outputs even though the transition fault should break the implication chain.
			size_t index { 0u };
			for (auto const& transitionDelay : get_slow_transitions())
			{
				if (transitionDelay.GetNode()->GetNodeId() == nodeId
					&& transitionDelay.GetPort().portType == portType
					&& transitionDelay.GetPort().portNumber == portId)
				{
					if (!_diffContainerForTimeframe[timeframeId][index].IsSet())
					{
						_diffContainerForTimeframe[timeframeId][index] = context.GetEncoder().EncodeLogic01DifferenceDetector(
							context.GetTimeframe(timeframeId).template GetContainer<GoodTag>(
								context, nodeId, portType, portId, AllowContainerInterceptor::Enable),
							context.GetTimeframe(timeframeId).template GetContainer<BadTag>(
								context, nodeId, portType, portId, AllowContainerInterceptor::Enable)
						);
					}
					return &_diffContainerForTimeframe[timeframeId][index];
				}

				index++;
			}
		}
	}

	return nullptr;
}

#define FOR_CONTAINER(CONTAINER) \
	template class TransitionDelayFaultEncoder<PinDataGB<CONTAINER>, Fault::SingleTransitionDelayFaultModel>; \
	template class TransitionDelayFaultEncoder<PinDataGBD<CONTAINER>, Fault::SingleTransitionDelayFaultModel>; \
	template class TransitionDelayFaultEncoder<PinDataGB<CONTAINER>, Fault::MultiTransitionDelayFaultModel>; \
	template class TransitionDelayFaultEncoder<PinDataGBD<CONTAINER>, Fault::MultiTransitionDelayFaultModel>;
INSTANTIATE_ALL_CONTAINER

};
};
