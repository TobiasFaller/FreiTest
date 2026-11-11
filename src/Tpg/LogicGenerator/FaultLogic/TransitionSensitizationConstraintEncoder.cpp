#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
TransitionSensitizationConstraintEncoder<PinData, FaultModel>::TransitionSensitizationConstraintEncoder(std::shared_ptr<FaultModel> faultModel, TransitionSensitizationTimeframe targetTimeframe):
	IConstraintEncoder<PinData>(),
	_faultModel(faultModel),
	_targetTimeframe(targetTimeframe),
	_allowEmptySensitization(false),
	_sensitizationType(TransitionSensitizationType::All),
	_transitionInitialValidForTimeframe(),
	_transitionInitialValueForTimeframe(),
	_transitionSettledValidForTimeframe(),
	_transitionSettledValueForTimeframe(),
	_sensitizations()
{
}

template<typename PinData, typename FaultModel>
TransitionSensitizationConstraintEncoder<PinData, FaultModel>::~TransitionSensitizationConstraintEncoder(void) = default;

template<typename PinData, typename FaultModel>
void TransitionSensitizationConstraintEncoder<PinData, FaultModel>::SetAllowEmptySensitization(bool enable)
{
	_allowEmptySensitization = enable;
}

template<typename PinData, typename FaultModel>
const bool& TransitionSensitizationConstraintEncoder<PinData, FaultModel>::IsAllowEmptySensitization(void) const
{
	return _allowEmptySensitization;
}

template<typename PinData, typename FaultModel>
void TransitionSensitizationConstraintEncoder<PinData, FaultModel>::SetTransitionSensitizationType(TransitionSensitizationType type)
{
	_sensitizationType = type;
}

template<typename PinData, typename FaultModel>
const TransitionSensitizationType& TransitionSensitizationConstraintEncoder<PinData, FaultModel>::GetTransitionSensitizationType(void) const
{
	return _sensitizationType;
}

template<typename PinData, typename FaultModel>
std::string TransitionSensitizationConstraintEncoder<PinData, FaultModel>::GetName(void) const
{
	return "TransitionSensitizationConstraintEncoder";
}

template<typename PinData, typename FaultModel>
void TransitionSensitizationConstraintEncoder<PinData, FaultModel>::Reset(void)
{
	_transitionInitialValidForTimeframe.clear();
	_transitionInitialValueForTimeframe.clear();
	_transitionSettledValidForTimeframe.clear();
	_transitionSettledValueForTimeframe.clear();
	_sensitizations.clear();
}

template<typename PinData, typename FaultModel>
bool TransitionSensitizationConstraintEncoder<PinData, FaultModel>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if (context.IsTimeframeCompletedForTarget(timeframeId, GenerationTarget::ConstraintEncoding))
	{
		return true;
	}

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		if (timeframeId != 0)
		{
			return true;
		}

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
	auto& transitionSettledValid = _transitionSettledValidForTimeframe.emplace_back();
	auto& transitionSettledValue = _transitionSettledValueForTimeframe.emplace_back();

	std::vector<LogicContainer01> constraints;

	size_t index { 0u };
	for (auto const& transition : get_slow_transitions())
	{
		LogicConstraint initialConstraint;
		LogicConstraint settledConstraint;
		switch (transition.GetType())
		{
			case TransitionDelayFaultType::SLOW_TO_RISE:
				initialConstraint = LogicConstraint::ONLY_LOGIC_ZERO;
				settledConstraint = LogicConstraint::ONLY_LOGIC_ONE;
				break;
			case TransitionDelayFaultType::SLOW_TO_FALL:
				initialConstraint = LogicConstraint::ONLY_LOGIC_ONE;
				settledConstraint = LogicConstraint::ONLY_LOGIC_ZERO;
				break;
			case TransitionDelayFaultType::SLOW_TO_TRANSITION:
				initialConstraint = LogicConstraint::ONLY_LOGIC_01;
				settledConstraint = LogicConstraint::ONLY_LOGIC_01;
				break;
			default: Logging::Panic("Unknown transition delay fault type");
		}

		const auto& [node, port] = transition.GetNodeAndPort();
		const auto originalContainer { timeframe.template GetContainer<BadTag>(
			context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Disable) };
		const auto initialValid { encoder.EncodeLogicValueConstraintDetector(originalContainer, initialConstraint) };
		const auto initialValue { originalContainer };
		const auto settledValid { encoder.EncodeLogicValueConstraintDetector(originalContainer, settledConstraint) };
		const auto settledValue { originalContainer };

		LogicContainer01 transitionCondition;
		if (satSolver)
		{
			// Simple logic container (no initial state hacks needed)
			transitionInitialValid.push_back(initialValid);
			transitionInitialValue.push_back(initialValue);
			transitionSettledValid.push_back(settledValid);
			transitionSettledValue.push_back(settledValue);

			transitionCondition = (timeframeId == 0u)
				? encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO)
				: encoder.EncodeAnd(std::vector<LogicContainer01> {
					_transitionInitialValidForTimeframe[timeframeId - 1u][index],
					_transitionSettledValidForTimeframe[timeframeId][index],
					// This XOR is required for the slow-to-transition type
					// where the constraints don't include the inverse relation
					// of the intial and final value of the transition.
					encoder.EncodeLogicValueDetector(encoder.EncodeXor(
						_transitionInitialValueForTimeframe[timeframeId - 1u][index],
						_transitionSettledValueForTimeframe[timeframeId][index]
					), Logic::LOGIC_ONE)
				});
		}

		if (bmcSolver)
		{
			// Latches with multiplexer for initial state (No undefined initial state is allowed).
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);
			auto initialValidLatch = encoder.template NewLogicContainer<LogicContainer01>();
			auto initialValueLatch = encoder.template NewLogicContainer<get_pin_data_container_t<PinData, GoodTag>>();

			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(initialValidLatch, Logic::LOGIC_ZERO);
			encoder.EncodeLogicValue(initialValueLatch, Logic::LOGIC_ZERO);

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			bmcSolver->CommitTimeframeClause(-initialValid.l0, 0u,  initialValidLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause( initialValid.l0, 0u, -initialValidLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause(-initialValue.l0, 0u,  initialValueLatch.l0, 1u);
			bmcSolver->CommitTimeframeClause( initialValue.l0, 0u, -initialValueLatch.l0, 1u);

			transitionInitialValid.push_back(initialValidLatch);
			transitionInitialValue.push_back(initialValueLatch);
			transitionSettledValid.push_back(settledValid);
			transitionSettledValue.push_back(settledValue);

			transitionCondition = encoder.EncodeAnd(std::vector<LogicContainer01> {
				_transitionInitialValidForTimeframe[timeframeId][index],
				_transitionSettledValidForTimeframe[timeframeId][index],
				// This XOR is required for the slow-to-transition type
				// where the constraints don't include the inverse relation
				// of the intial and final value of the transition.
				encoder.EncodeLogicValueDetector(encoder.EncodeXor(
					_transitionInitialValueForTimeframe[timeframeId][index],
					_transitionSettledValueForTimeframe[timeframeId][index]
				), Logic::LOGIC_ONE)
			});
		}

		constraints.push_back(transitionCondition);
		index++;
	}

	switch (_sensitizationType)
	{
		case TransitionSensitizationType::All:
			_sensitizations.push_back(encoder.EncodeAnd(constraints));
			break;
		case TransitionSensitizationType::One:
			_sensitizations.push_back(encoder.EncodeOr(constraints));
			break;
		default: Logging::Panic("Unknown sensitization type");
	}

	return true;
}

template<typename PinData, typename FaultModel>
bool TransitionSensitizationConstraintEncoder<PinData, FaultModel>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto& encoder = context.GetEncoder();
	if (!_allowEmptySensitization)
	{
		const auto constantZero = encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
		if ((_targetTimeframe == TransitionSensitizationTimeframe::Last && _sensitizations.back() == constantZero)
			|| std::all_of(_sensitizations.cbegin(), _sensitizations.cend(), [&constantZero](const LogicContainer01& value) {
				return value == constantZero;
			}))
		{
			DVLOG(3) << "There are no gates that have a difference for the fault";
			return false;
		}
	}

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver)
	{
		DVLOG(3) << "Forcing the timeframe " << (_sensitizations.size() - 1u)
			<< " to have a difference at a the fault locations";
		if (context.IsIncremental())
		{
			auto assumption {
				encoder.EncodeLogicValueDetector(
					_targetTimeframe == TransitionSensitizationTimeframe::Last
						? _sensitizations.back()
						: encoder.EncodeOr(_sensitizations),
					Logic::LOGIC_ONE
				)
			};
			satSolver->AddAssumption(assumption.l0);
		}
		else
		{
			encoder.EncodeLogicValue(
				_targetTimeframe == TransitionSensitizationTimeframe::Last
					? _sensitizations.back()
					: encoder.EncodeOr(_sensitizations),
				Logic::LOGIC_ONE);
		}
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		auto sensitizationLatch = encoder.template NewStateContainer<LogicContainer01>();
		auto differenceOutput = (_targetTimeframe == TransitionSensitizationTimeframe::Last)
			? _sensitizations.back()
			// The sensitization does not need to happen in the last timeframe.
			// Generate OR-gate to include the previous output value of the latch.
			: encoder.EncodeOr(_sensitizations.back(), sensitizationLatch);
		encoder.EncodeNextState(sensitizationLatch, differenceOutput);
		encoder.EncodeInitialStateValue(sensitizationLatch, Logic::LOGIC_ZERO);
		encoder.EncodeTargetStateValue(sensitizationLatch, Logic::LOGIC_ONE);
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) \
	template class TransitionSensitizationConstraintEncoder<PinDataGB<CONTAINER>, Fault::SingleTransitionDelayFaultModel>; \
	template class TransitionSensitizationConstraintEncoder<PinDataGBD<CONTAINER>, Fault::SingleTransitionDelayFaultModel>; \
	template class TransitionSensitizationConstraintEncoder<PinDataGB<CONTAINER>, Fault::MultiTransitionDelayFaultModel>; \
	template class TransitionSensitizationConstraintEncoder<PinDataGBD<CONTAINER>, Fault::MultiTransitionDelayFaultModel>;
INSTANTIATE_ALL_CONTAINER

};
};
