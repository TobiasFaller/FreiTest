#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"

#include <numeric>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace FreiTest::Io::Udfm;
using namespace SolverProxy::Sat;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
CellAwareFaultEncoder<PinData>::CellAwareFaultEncoder(std::shared_ptr<CellAwareFaultModel> faultModel):
	_faultModel(faultModel),
	_noAlternativeEffect(CellAwareNoAlternativeEffect::FaultFree),
	_badContainerForTimeframe(),
	_diffContainerForTimeframe(),
	_states()
{
}

template<typename PinData>
CellAwareFaultEncoder<PinData>::~CellAwareFaultEncoder(void) = default;

template<typename PinData>
void CellAwareFaultEncoder<PinData>::SetEffectWhenNoAlternativeFound(CellAwareNoAlternativeEffect effect)
{
	_noAlternativeEffect = effect;
}

template<typename PinData>
const CellAwareNoAlternativeEffect& CellAwareFaultEncoder<PinData>::GetEffectWhenNoAlternativeFound(void) const
{
	return _noAlternativeEffect;
}

template<typename PinData>
std::string CellAwareFaultEncoder<PinData>::GetName(void) const
{
	return "CellAwareFaultEncoder";
}

template<typename PinData>
void CellAwareFaultEncoder<PinData>:: Reset(void)
{
	_badContainerForTimeframe.clear();
	_diffContainerForTimeframe.clear();
	_states.clear();
}

template<typename PinData>
bool CellAwareFaultEncoder<PinData>::PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	const auto& fault { _faultModel->GetFault() };
	auto& encoder { context.GetEncoder() };

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	while (_badContainerForTimeframe.size() <= timeframeId)
	{
		auto& badMap = _badContainerForTimeframe.emplace_back();
		auto& diffMap = _diffContainerForTimeframe.emplace_back();
		for (auto& nodeAndPort : fault->GetEffectNodesAndPorts())
		{
			badMap[nodeAndPort] = encoder.template NewLogicContainer<BadContainer>();
			diffMap[nodeAndPort] = DiffContainer();
		}
	}

	return true;
}

template<typename PinData>
bool CellAwareFaultEncoder<PinData>::PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
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
		_noTransitions.resize(timeframeId + 1);
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
				std::vector<LogicContainer01> effects;
				for (auto const& condition : transition.conditions)
				{
					const auto& [node, port] = condition.nodeAndPort;
					const auto& logicConstraint { condition.logicConstraints[0u] };
					const auto container { timeframe.template GetContainer<BadTag>(
						context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
					conditions.push_back(encoder.EncodeLogicValueConstraintDetector(container, logicConstraint));
				}
				for (auto const& effect : transition.effects)
				{
					const auto& [node, port] = effect.nodeAndPort;
					const auto& logicConstraint { effect.logicConstraints[0u] };
					const auto container { timeframe.template GetContainer<BadTag>(
						context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
					effects.push_back(encoder.EncodeLogicValueConstraintDetector(container, logicConstraint));
				}

				auto transitionTaken {
					encoder.EncodeAnd(
						encoder.EncodeAnd(transitionAllowed, _states[timeframeId][stateIndex]),
						encoder.EncodeAnd(conditions))
				};
				encoder.EncodeImplication(transitionTaken, encoder.EncodeAnd(effects));
				for (size_t index { 0u }; index < stateMachine.size(); index++)
				{
					encoder.EncodeImplication(transitionTaken, encoder.EncodeLogicValueDetector(_nextStates[timeframeId][index],
						(index == transition.toState) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO));
				}
				transitionsTaken.push_back(transitionTaken);
				_transitions[timeframeId][stateIndex].push_back(transitionTaken);
			}
		}
	}

	std::vector<LogicContainer01> noAlternativeEffects;
	for (const auto& [node, port] : fault->GetEffectNodesAndPorts())
	{
		LogicConstraint constraint = LogicConstraint::NO_CONSTRAINT;
		switch (_noAlternativeEffect)
		{
			case CellAwareNoAlternativeEffect::FaultFree:
			{
				const auto badContainer { timeframe.template GetContainer<BadTag>(
					context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
				const auto originalContainer { timeframe.template GetContainer<BadTag>(
					context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Disable) };
				noAlternativeEffects.push_back(encoder.EncodeLogicEquivalenceDetector(badContainer, originalContainer));
				continue;
			}

			case CellAwareNoAlternativeEffect::FixedOne: constraint = LogicConstraint::ONLY_LOGIC_ONE; break;
			case CellAwareNoAlternativeEffect::FixedZero: constraint = LogicConstraint::ONLY_LOGIC_ZERO; break;
			case CellAwareNoAlternativeEffect::FixedDontCare: constraint = LogicConstraint::ONLY_LOGIC_DONT_CARE; break;
			case CellAwareNoAlternativeEffect::FixedUnknown: constraint = LogicConstraint::ONLY_LOGIC_UNKNOWN; break;
			case CellAwareNoAlternativeEffect::Unconstrained: constraint = LogicConstraint::NO_CONSTRAINT; break;

			default: Logging::Panic("Unknown effect when no alternative is active");
		}

		// The logic value can be constrained by using a constraint detector.
		const auto container { timeframe.template GetContainer<BadTag>(
			context, node->GetNodeId(), port.portType, port.portNumber, AllowContainerInterceptor::Enable) };
		noAlternativeEffects.push_back(encoder.EncodeLogicValueConstraintDetector(container, constraint));
	}

	auto noTransition { -encoder.EncodeOr(transitionsTaken) };
	encoder.EncodeImplication(noTransition, encoder.EncodeAnd(noAlternativeEffects));
	for (size_t nextIndex { 0u }; nextIndex < stateMachine.size(); nextIndex++)
	{
		encoder.EncodeImplication(noTransition, encoder.EncodeLogicValueDetector(_nextStates[timeframeId][nextIndex],
			(nextIndex == 0) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO));
	}
	_noTransitions[timeframeId] = noTransition;

	return true;
}

template<typename PinData>
bool CellAwareFaultEncoder<PinData>::PostEncodeCircuit(GeneratorContext<PinData>& context)
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
const void* CellAwareFaultEncoder<PinData>::GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframeId, size_t nodeId, PortType portType, size_t portId)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	if constexpr (has_bad_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(BadTag))
		{
			MappedCircuit::NodeAndPort nodeAndPort { context.GetMappedCircuit().GetNode(nodeId), { portType, portId } };
			if (auto it = _badContainerForTimeframe[timeframeId].find(nodeAndPort);
				it != _badContainerForTimeframe[timeframeId].end())
			{
				return &it->second;
			}
		}
	}

	if constexpr (has_diff_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(DiffTag))
		{
			// The difference container has to be overriden by the fault injector too.
			// This is because otherwise the D-Chain will link the inputs of the fault location
			// to the outputs even though the cell-aware fault should break the implication chain.
			MappedCircuit::NodeAndPort nodeAndPort { context.GetMappedCircuit().GetNode(nodeId), { portType, portId } };
			if (auto it = _diffContainerForTimeframe[timeframeId].find(nodeAndPort);
				it != _diffContainerForTimeframe[timeframeId].end())
			{
				if (!it->second.IsSet())
					it->second = context.GetEncoder().EncodeLogic01DifferenceDetector(
						context.GetTimeframe(timeframeId).template GetContainer<GoodTag>(
							context, nodeId, portType, portId, AllowContainerInterceptor::Enable),
						context.GetTimeframe(timeframeId).template GetContainer<BadTag>(
							context, nodeId, portType, portId, AllowContainerInterceptor::Enable)
					);
				return &it->second;
			}
		}
	}

	return nullptr;
}

template<typename PinData>
const std::vector<std::vector<LogicContainer01>>& CellAwareFaultEncoder<PinData>::GetStates() const
{
	return _states;
}

template<typename PinData>
const std::vector<std::vector<LogicContainer01>>& CellAwareFaultEncoder<PinData>::GetNextStates() const
{
	return _nextStates;
}

template<typename PinData>
const std::vector<std::vector<std::vector<LogicContainer01>>>& CellAwareFaultEncoder<PinData>::GetTransitions() const
{
	return _transitions;
}

template<typename PinData>
const std::vector<LogicContainer01>& CellAwareFaultEncoder<PinData>::GetNoTransitions() const
{
	return _noTransitions;
}

#define FOR_CONTAINER(CONTAINER) \
	template class CellAwareFaultEncoder<PinDataGB<CONTAINER>>; \
	template class CellAwareFaultEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};

