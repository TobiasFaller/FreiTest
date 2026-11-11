#include "Io/UserDefinedFaultModel/UdfmToSpecification.hpp"

#include <cassert>
#include <limits>

#include "Basic/Logging.hpp"

using namespace FreiTest::Fault;

namespace FreiTest
{
namespace Io
{
namespace Udfm
{

static CellAwareFaultSpecification ConvertToSpecification(const Io::Udfm::UdfmFault& udfm);
static std::string to_string(const CellAwareFaultSpecification::Port& port);
static std::string to_string(const std::vector<CellAwareFaultSpecification::Port>& list);

CellAwareModelSpecification ConvertToSpecification(const Io::Udfm::UdfmModel& udfm)
{
	std::vector<CellAwareCellSpecification> cells;
	for (auto& [cellName, cell] : udfm.GetCells())
	{
		std::vector<CellAwareFaultSpecification> faults;
		for (auto& [faultName, fault] : cell->GetFaults())
		{
			faults.push_back(ConvertToSpecification(*fault));
		}
		cells.emplace_back(cellName, faults);
	}
	return { cells };
}

static CellAwareFaultSpecification ConvertToSpecification(const Io::Udfm::UdfmFault& udfmFault)
{
	using State = CellAwareFaultSpecification::State;
	using Transition = CellAwareFaultSpecification::Transition;
	using Port = CellAwareFaultSpecification::Port;

	std::vector<State> states;
	std::vector<std::vector<std::vector<size_t>>> transitionAlternatives;

	// First state (epsilon).
	auto const STATE_EPSILON { 0u };
	states.push_back(State {
		.transitions = { },
	});
	transitionAlternatives.push_back({ });

	auto const NEW_TARGET { std::numeric_limits<size_t>::max() };
	auto const get_or_create_transition = [&](ssize_t alternative, size_t state, size_t targetState, auto inputs, auto outputs)
		-> std::tuple<Transition&, size_t>
	{
#ifndef NDEBUG
		for (auto& input : inputs) {
			assert(input.constraint == Basic::LogicConstraint::NO_CONSTRAINT
				|| input.constraint == Basic::LogicConstraint::ONLY_LOGIC_ZERO
				|| input.constraint == Basic::LogicConstraint::ONLY_LOGIC_ONE);
		}
		for (auto& output : outputs) {
			assert(output.constraint == Basic::LogicConstraint::NO_CONSTRAINT
				|| output.constraint == Basic::LogicConstraint::ONLY_LOGIC_ZERO
				|| output.constraint == Basic::LogicConstraint::ONLY_LOGIC_ONE
				|| output.constraint == Basic::LogicConstraint::ONLY_LOGIC_DONT_CARE);
		}
#endif

		for (size_t transitionIndex { 0u }; transitionIndex < states[state].transitions.size(); transitionIndex++)
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
						if (port1[index1].name == port2[index2].name)
						{
							if (port1[index1].constraint != port2[index2].constraint)
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

			auto& transition = states[state].transitions[transitionIndex];
			if (state_equal(transition.inputs, inputs))
			{
				std::string otherAlternatives;
				for (auto& alternativeIndex : transitionAlternatives[state][transitionIndex])
				{
					otherAlternatives += ", ";
					otherAlternatives += std::to_string(alternativeIndex);
				}
				LOG_IF(!state_equal(transition.outputs, outputs), FATAL)
					<< "The UDFM contains fault " << udfmFault.GetFaultName()
					<< " for cell " << udfmFault.GetCellName()
					<< " where two or more test alternatives ("
					<< std::to_string(alternative) << otherAlternatives
					<< ") with same prefix but differing effects: "
					<< to_string(inputs) << " -> " << to_string(outputs)
					<< " vs " << to_string(transition.inputs) << " -> " << to_string(transition.outputs);
				return { transition, transitionIndex };
			}
		}

		if (targetState == NEW_TARGET) {
			states.push_back(State {
				.transitions = { },
			});
			transitionAlternatives.push_back({ });
			targetState = states.size() - 1u;
		}

		auto& transition = states[state].transitions.emplace_back(Transition {
			.inputs = inputs,
			.outputs = outputs,
			.toState = targetState
		});
		transitionAlternatives[state].push_back({ });
		return { transition, transitionAlternatives[state].size() - 1u };
	};
	auto const extract_vector = [&](auto const& ports, size_t index) -> std::vector<Port> {
		std::vector<Port> result;
		for (auto const& [portName, constraints] : ports)
		{
			result.push_back({
				.name = portName,
				.constraint = constraints[index]
			});
		}
		return result;
	};

	// Encode states for each alternative, building a prefix-code.
	for (size_t alternativeIndex { 0u }; alternativeIndex < udfmFault.GetAlternatives().size(); alternativeIndex++)
	{
		auto const& alternative { *udfmFault.GetAlternative(alternativeIndex) };

		size_t timeframes { 0u };
		for (auto& [port, constraints] : alternative.GetTestConditions())
		{
			assert(timeframes == 0u || constraints.size() == timeframes);
			timeframes = constraints.size();
		}
		for (auto& [port, constraints] : alternative.GetTestEffects())
		{
			assert(timeframes == 0u || constraints.size() == timeframes);
			timeframes = constraints.size();
		}

		size_t currentState { STATE_EPSILON };
		for (size_t index { 0u }; index < timeframes; index++)
		{
			const auto last { (index + 1u) == timeframes };
			const auto conditions { extract_vector(alternative.GetTestConditions(), index) };
			const auto effects { extract_vector(alternative.GetTestEffects(), index) };
			auto difference = false;
			for (auto& effect : effects) {
				difference |= (effect.constraint == Basic::LogicConstraint::ONLY_LOGIC_ZERO);
				difference |= (effect.constraint == Basic::LogicConstraint::ONLY_LOGIC_ONE);
			}

			auto [transition, transitionIndex] = get_or_create_transition(
				alternativeIndex, currentState, NEW_TARGET, conditions, effects);
			transitionAlternatives[currentState][transitionIndex].push_back(alternativeIndex);
			transition.accepting |= (last && difference);
			currentState = transition.toState;
		}
	}

	return CellAwareFaultSpecification(udfmFault.GetFaultName(), states);
}

static std::string to_string(const CellAwareFaultSpecification::Port& port)
{
	return port.name + "=" + to_string(port.constraint);
}

static std::string to_string(const std::vector<CellAwareFaultSpecification::Port>& list)
{
	std::string result;
	size_t index { 0u };
	for (const auto& port : list)
	{
		if (index++ != 0u) result += ", ";
		result += to_string(port);
	}
	return result;
}

};
};
};
