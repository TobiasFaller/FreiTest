#include "Basic/Fault/Faults/CellAwareFault.hpp"

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Circuit/CircuitMetaData.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Io::Udfm;

using NodeAndPort = FreiTest::Circuit::MappedCircuit::NodeAndPort;

namespace FreiTest
{
namespace Fault
{

std::string to_string(const std::vector<CellAwarePort>& pinAttributes);

CellAwareFault::CellAwareFault(
		const std::shared_ptr<UdfmFault> userDefinedFault,
		const Circuit::GroupMetaData* cell,
		std::vector<CellAwareAlternative> alternatives
):
	_userDefinedFault(userDefinedFault),
	_cell(cell),
	_alternatives(alternatives),
	_states(),
	_conditionNodesAndPorts(),
	_effectNodesAndPorts(),
	_timeframeSpread(0u)
{
	for (const auto& alternative : alternatives)
	{
		for (const auto& condition : alternative.conditions)
		{
			_conditionNodesAndPorts.push_back(condition.nodeAndPort);
			_timeframeSpread = std::max(_timeframeSpread, condition.logicConstraints.size());
		}
		for (const auto& effect : alternative.effects)
		{
			_effectNodesAndPorts.push_back(effect.nodeAndPort);
			_timeframeSpread = std::max(_timeframeSpread, effect.logicConstraints.size());
		}
	}

	std::sort(_conditionNodesAndPorts.begin(), _conditionNodesAndPorts.end());
	std::sort(_effectNodesAndPorts.begin(), _effectNodesAndPorts.end());
	_conditionNodesAndPorts.erase(std::unique(_conditionNodesAndPorts.begin(), _conditionNodesAndPorts.end()), _conditionNodesAndPorts.end());
	_effectNodesAndPorts.erase(std::unique(_effectNodesAndPorts.begin(), _effectNodesAndPorts.end()), _effectNodesAndPorts.end());

	// First state (epsilon).
	auto const STATE_EPSILON { 0u };
	_states.push_back(CellAwareState {
		.depth = 0u,
		.accepting = false,
		.transitions = { },
		.alternatives = { },
	});

	auto const NEW_TARGET { std::numeric_limits<size_t>::max() };
	auto const get_or_create_transition = [&](auto transitionType, ssize_t alternative, size_t state, size_t targetState, auto conditions, auto effects) -> CellAwareTransition& {
#ifndef NDEBUG
		for (auto& condition : conditions) {
			assert(condition.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_ZERO
				|| condition.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_ONE
				|| condition.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_DONT_CARE);
		}
		for (auto& effect : effects) {
			assert(effect.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_ZERO
				|| effect.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_ONE
				|| effect.logicConstraints[0] == LogicConstraint::ONLY_LOGIC_DONT_CARE);
		}
#endif

		for (auto& transition : _states[state].transitions)
		{
			const auto state_equal = [](auto& port1, auto& port2) -> bool {
				if (port1.size() != port2.size())
				{
					return false;
				}
				for (size_t index1 { 0u }; index1 < port1.size(); index1++)
				{
					bool found = false;
					for (size_t index2 { 0u }; index2 < port2.size(); index2++)
					{
						if (port1[index1].portName == port2[index2].portName)
						{
							if (port1[index1].logicConstraints != port2[index2].logicConstraints)
							{
								return false;
							}
							found = true;
						}
					}
					if (!found)
					{
						return false;
					}
				}
				return true;
			};
			if (transition.type == transitionType
				&& state_equal(transition.conditions, conditions))
			{
				std::string otherAlternatives;
				for (auto& alternativeIndex : transition.alternatives)
				{
					otherAlternatives += ", ";
					otherAlternatives += std::to_string(alternativeIndex);
				}
				LOG_IF(!state_equal(transition.effects, effects), FATAL)
					<< "The UDFM contains fault " << userDefinedFault->GetFaultName()
					<< " for cell " << userDefinedFault->GetCellName()
					<< " who two or more test alternatives ("
					<< std::to_string(alternative) << otherAlternatives
					<< ") with same prefix at depth " << std::to_string(_states[state].depth) << " but differing effects: "
					<< to_string(conditions) << " -> " << to_string(effects)
					<< " vs " << to_string(transition.conditions) << " -> " << to_string(transition.effects);
				return transition;
			}
		}

		if (targetState == NEW_TARGET) {
			_states.push_back(CellAwareState {
				.depth = _states[state].depth + 1u,
				.accepting = false,
				.transitions = { },
				.alternatives = { },
			});
			targetState = _states.size() - 1u;
		}

		return _states[state].transitions.emplace_back(CellAwareTransition {
			.type = transitionType,
			.toState = targetState,
			.conditions = conditions,
			.effects = effects,
			.alternatives = { }
		});
	};
	auto const extract_vector = [&](auto const& ports, size_t index) -> std::vector<CellAwarePort> {
		std::vector<CellAwarePort> result;
		for (auto const& port : ports)
		{
			result.push_back({
				.nodeAndPort = port.nodeAndPort,
				.portName = port.portName,
				.logicConstraints = { port.logicConstraints[index] }
			});
		}
		return result;
	};

	// Encode states for each alternative, building a prefix-code.
	for (size_t alternativeIndex { 0u }; alternativeIndex < this->_alternatives.size(); alternativeIndex++)
	{
		auto const& alternative { this->_alternatives[alternativeIndex] };
		assert (alternative.conditions.size() > 0);
		size_t currentState { STATE_EPSILON };
		for (size_t index { 0u }; index < alternative.conditions[0].logicConstraints.size(); index++)
		{
			const auto last { (index + 1u) == alternative.conditions[0].logicConstraints.size() };
			const auto conditions { extract_vector(alternative.conditions, index) };
			const auto effects { extract_vector(alternative.effects, index) };
			auto difference = false;
			for (auto& effect : effects) {
				difference |= (effect.logicConstraints[0u] == LogicConstraint::ONLY_LOGIC_ZERO);
				difference |= (effect.logicConstraints[0u] == LogicConstraint::ONLY_LOGIC_ONE);
			}

			auto& transition = get_or_create_transition(
				CellAwareTransition::TransitionType::Primary,
				alternativeIndex, currentState, NEW_TARGET, conditions, effects);
			transition.alternatives.push_back(alternativeIndex);
			_states[currentState].alternatives.push_back(alternativeIndex);
			_states[transition.toState].accepting |= (last && difference);
			currentState = transition.toState;
		}
		_states[currentState].alternatives.push_back(alternativeIndex);
	}

	// Excluding first (epsilon) state.
	for (size_t stateIndex { 1u }; stateIndex < _states.size(); stateIndex++)
	{
		for (auto& epsTrans : _states[STATE_EPSILON].transitions)
		{
			// Note 1:
			// Create a transition from each state such that it is the start state for the state machine.
			// Since we are not encoding alternatives here, the transitions might already exist.
			// Existing transitions have priority and are not considered a conflict as the test alternative's
			// behavior is expected to be different than starting without a state.
			//
			// Example:
			//  1) A=0,  B=0  -> C=X
			//  2) A=1,  B=0  -> C=X
			//  3) A=01, B=00 -> C=X1
			//
			// Transition system without continuing transitions on the states:
			//   eps -+--> 1
			//        +--> 2
			//        +--> 3 ---> 4
			//
			// Transitioning into any of these states would result in the system to get stuck.
			// Therefore, transitions equivalent to the ones from eps are inserted into each state.
			// The corner case arises then from state 3, where the conditions A=1, B=0 match the ones of alternative 2.
			// However, only the transition test creates a fault difference and has a differing fault effect.
			// The transition from 3 to 2 should not be generated and 3 to 4 is relevant instead.
			// => Therefore, transition type Secondary is used here to give it a secondary priority.
			get_or_create_transition(
				CellAwareTransition::TransitionType::Secondary,
				-1, stateIndex, epsTrans.toState, epsTrans.conditions, epsTrans.effects);
		}
	}
}

CellAwareFault::~CellAwareFault(void) = default;

const std::shared_ptr<UdfmFault>& CellAwareFault::GetUserDefinedFault(void) const
{
	return _userDefinedFault;
}

const Circuit::GroupMetaData* CellAwareFault::GetCell(void) const
{
	return _cell;
}

const std::vector<CellAwareAlternative>& CellAwareFault::GetAlternatives(void) const
{
	return _alternatives;
}

const std::vector<NodeAndPort>& CellAwareFault::GetConditionNodesAndPorts(void) const
{
	return _conditionNodesAndPorts;
}
const std::vector<NodeAndPort>& CellAwareFault::GetEffectNodesAndPorts(void) const
{
	return _effectNodesAndPorts;
}

size_t CellAwareFault::GetTimeframeSpread(void) const
{
	return _timeframeSpread;
}

const std::vector<CellAwareState>& CellAwareFault::GetStateMachine(void) const
{
	return _states;
}

std::string to_string(const CellAwareFault& fault)
{
	std::string result = "CA fault " + fault.GetUserDefinedFault()->GetFaultName() + " @ "
		+ fault.GetCell()->GetHierarchyName() + " (" + fault.GetUserDefinedFault()->GetCellName() + "): ";

	size_t index { 0u };
	for (const auto& alternative : fault.GetAlternatives())
	{
		if (index++ != 0u) result += " or ";
		result += to_string(alternative);
	}

	return result;
}

std::string to_string(const CellAwareAlternative& alternative)
{
	return to_string(alternative.conditions) + " -> " + to_string(alternative.effects);
}

std::string to_string(const std::vector<CellAwarePort>& pinAttributes)
{
	std::string result;
	size_t index { 0u };
	for (const auto& attribute : pinAttributes)
	{
		if (index++ != 0u) result += ", ";
		result += to_string(attribute);
	}
	return result;
}

std::string to_string(const CellAwarePort& attribute)
{
	return attribute.portName + "=" + to_string(attribute.logicConstraints);
}

std::string to_string(const std::vector<CellAwareState>& states)
{
	std::string result;
	for (size_t stateIndex { 0u }; stateIndex < states.size(); stateIndex++)
	{
		auto& state = states[stateIndex];
		if (stateIndex != 0u) result += "\n";

		result += "State " + std::to_string(stateIndex) + " depth=" + std::to_string(state.depth) + " (";
		for (size_t altIndex { 0u }; altIndex < state.alternatives.size(); altIndex++)
		{
			auto& alternative = state.alternatives[altIndex];
			if (altIndex != 0u) result += ", ";
			result += "alt " + std::to_string(alternative);
		}
		result += ")";

		for (size_t transIndex { 0u }; transIndex < state.transitions.size(); transIndex++)
		{
			auto& transition = state.transitions[transIndex];
			result += "\n";
			result += "    " + to_string(transition.type) + " to ";
			result += std::to_string(transition.toState);
			result += " (" + to_string(transition.conditions) + " -> " + to_string(transition.effects) + ")";
			result += ", " + std::string(states[transition.toState].accepting ? "accepting" : "not accepting");
		}
	}

	return result;
}

std::string to_string(const CellAwareTransition::TransitionType& type)
{
	switch (type)
	{
		case CellAwareTransition::TransitionType::Primary: return "primary";
		case CellAwareTransition::TransitionType::Secondary: return "secondary";
		default: __builtin_unreachable();
	}
}

};
};
