#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"

#include <numeric>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace FreiTest::Io::Udfm;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
CellAwareSensitizationConstraintEncoder<PinData>::CellAwareSensitizationConstraintEncoder(std::shared_ptr<Fault::CellAwareFaultModel> faultModel, CellAwareSensitizationTimeframe targetTimeframe):
	IConstraintEncoder<PinData>(),
	_faultModel(faultModel),
	_targetTimeframe(targetTimeframe),
	_allowEmptySensitization(false),
	_states(),
	_sensitizations()
{
}

template<typename PinData>
CellAwareSensitizationConstraintEncoder<PinData>::~CellAwareSensitizationConstraintEncoder(void) = default;


template<typename PinData>
void CellAwareSensitizationConstraintEncoder<PinData>::SetAllowEmptySensitization(bool enable)
{
	_allowEmptySensitization = enable;
}

template<typename PinData>
bool CellAwareSensitizationConstraintEncoder<PinData>::IsAllowEmptySensitization(void) const
{
	return _allowEmptySensitization;
}

template<typename PinData>
std::string CellAwareSensitizationConstraintEncoder<PinData>::GetName(void) const
{
	return "CellAwareSensitizationConstraintEncoder";
}

template<typename PinData>
void CellAwareSensitizationConstraintEncoder<PinData>::Reset(void)
{
	_states.clear();
	_nextStates.clear();
	_sensitizations.clear();
}

template<typename PinData>
bool CellAwareSensitizationConstraintEncoder<PinData>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if (context.IsTimeframeCompletedForTarget(timeframeId, GenerationTarget::ConstraintEncoding))
	{
		return true;
	}

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());

	const auto& fault { _faultModel->GetFault() };
	const auto& stateMachine { fault->GetStateMachine() };
	const auto& timeframe { context.GetTimeframe(timeframeId) };
	auto& encoder { context.GetEncoder() };

	if (_states.size() <= timeframeId)
	{
		_states.resize(timeframeId + 1);
		_nextStates.resize(timeframeId + 1);
		_transitions.resize(timeframeId + 1);
		_transitions[timeframeId].resize(stateMachine.size());
		_sensitizations.resize(timeframeId + 1);
	}
	for (size_t stateIndex { 0u }; stateIndex < stateMachine.size(); stateIndex++)
	{
		if (bmcSolver)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);
		}
		_states[timeframeId].emplace_back(encoder.template NewLogicContainer<LogicContainer01>());
		if (bmcSolver)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		}
		_nextStates[timeframeId].emplace_back(encoder.template NewLogicContainer<LogicContainer01>());
	}

	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	std::vector<LogicContainer01> sensitized { encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO) };
	std::vector<LogicContainer01> transitionsTaken { encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO) };
	for (auto transitionType : std::vector<CellAwareTransition::TransitionType> {
			// First encode primary, then secondary transitions which have a lower priority.
			CellAwareTransition::TransitionType::Primary,
			CellAwareTransition::TransitionType::Secondary
		})
	{
		// This will result in LOGIC_ONE for primary transitions.
		// For secondary transitions, this will only be LOGIC_ONE
		// if no primary transitions were taken.
		auto transitionAllowed = -encoder.EncodeOr(transitionsTaken);
		for (size_t stateIndex { 0u }; stateIndex < stateMachine.size(); stateIndex++)
		{
			for (auto const& transition : stateMachine[stateIndex].transitions)
			{
				if (transition.type != transitionType)
				{
					continue;
				}

				std::vector<LogicContainer01> conditions;
				for (auto const& condition : transition.conditions)
				{
					const auto& [node, port] = condition.nodeAndPort;
					const auto& logicConstraint { condition.logicConstraints[0u] };
					const auto container { timeframe.template GetContainer<BadTag>(
						context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
					conditions.push_back(encoder.EncodeLogicValueConstraintDetector(container, logicConstraint));
				}

				auto transitionTaken {
					encoder.EncodeAnd(
						encoder.EncodeAnd(transitionAllowed, _states[timeframeId][stateIndex]),
						encoder.EncodeAnd(conditions))
				};
				for (size_t index { 0u }; index < stateMachine.size(); index++)
				{
					encoder.EncodeImplication(transitionTaken, encoder.EncodeLogicValueDetector(_nextStates[timeframeId][index],
						(index == transition.toState) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO));
				}
				transitionsTaken.push_back(transitionTaken);
				if (stateMachine[transition.toState].accepting) {
					sensitized.push_back(transitionTaken);
				}
				_transitions[timeframeId][stateIndex].push_back(transitionTaken);
			}
		}
	}

	auto noTransition { -encoder.EncodeOr(transitionsTaken) };
	for (size_t nextIndex { 0u }; nextIndex < stateMachine.size(); nextIndex++)
	{
		encoder.EncodeImplication(noTransition, encoder.EncodeLogicValueDetector(_nextStates[timeframeId][nextIndex],
			(nextIndex == 0) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO));
	}
	_sensitizations[timeframeId] = encoder.EncodeOr(sensitized);

	return true;
}

template<typename PinData>
bool CellAwareSensitizationConstraintEncoder<PinData>::PostConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto& encoder = context.GetEncoder();
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (satSolver)
	{
		assert (_states.size() > 0u);
		for (size_t index { 0u }; index < _states[0u].size(); index++)
		{
			encoder.EncodeLogicValue(_states[0u][index], (index == 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);
		}
		for (size_t index { 1u }; index < _states.size(); index++)
		{
			encoder.EncodeLogicEquivalenceConstraint(_nextStates[index - 1u], _states[index]);
		}
	}
	if (bmcSolver)
	{
		assert (_states.size() == 1u);
		for (size_t index { 0u }; index < _states[0u].size(); index++)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(_states[0u][index], (index == 0) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			bmcSolver->CommitTimeframeClause(-_nextStates[0u][index].l0, 0u,  _states[0u][index].l0, 1u);
			bmcSolver->CommitTimeframeClause( _nextStates[0u][index].l0, 0u, -_states[0u][index].l0, 1u);
		}
	}

	return true;
}

template<typename PinData>
bool CellAwareSensitizationConstraintEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	DASSERT(context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) > 0u) << "No timeframes have been generated";
	DASSERT(_sensitizations.size() == context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		<< "The fault sensitization module was not invoked for all timeframes";

	auto& encoder = context.GetEncoder();
	if (!_allowEmptySensitization)
	{
		const LogicContainer01 constantZero = encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
		if ((_targetTimeframe == CellAwareSensitizationTimeframe::Last && _sensitizations.back() == constantZero)
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
					_targetTimeframe == CellAwareSensitizationTimeframe::Last
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
				_targetTimeframe == CellAwareSensitizationTimeframe::Last
					? _sensitizations.back()
					: encoder.EncodeOr(_sensitizations),
				Logic::LOGIC_ONE);
		}
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Latch);
		auto sensitizationLatch = encoder.template NewLogicContainer<LogicContainer01>();

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		auto differenceOutput = (_targetTimeframe == CellAwareSensitizationTimeframe::Last)
			? _sensitizations.back()
			// The sensitization does not need to happen in the last timeframe.
			// Generate OR-gate to include the previous output value of the latch.
			: encoder.EncodeOr(_sensitizations.back(), sensitizationLatch);

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		bmcSolver->CommitTimeframeClause( differenceOutput.l0, 0u, -sensitizationLatch.l0, 1u);
		bmcSolver->CommitTimeframeClause(-differenceOutput.l0, 0u,  sensitizationLatch.l0, 1u);

		// Initialize fault sensitization to 0
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ZERO);

		// Require fault sensitization to be 1
		bmcSolver->SetTargetClauseType(ClauseType::Target);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ONE);
	}

	return true;
}

template<typename PinData>
const std::vector<std::vector<LogicContainer01>>& CellAwareSensitizationConstraintEncoder<PinData>::GetStates() const
{
	return _states;
}

template<typename PinData>
const std::vector<std::vector<LogicContainer01>>& CellAwareSensitizationConstraintEncoder<PinData>::GetNextStates() const
{
	return _nextStates;
}

template<typename PinData>
const std::vector<std::vector<std::vector<LogicContainer01>>>& CellAwareSensitizationConstraintEncoder<PinData>::GetTransitions() const
{
	return _transitions;
}

template<typename PinData>
const std::vector<LogicContainer01>& CellAwareSensitizationConstraintEncoder<PinData>::GetSensitizations() const
{
	return _sensitizations;
}

#define FOR_CONTAINER(CONTAINER) \
	template class CellAwareSensitizationConstraintEncoder<PinDataGB<CONTAINER>>; \
	template class CellAwareSensitizationConstraintEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
