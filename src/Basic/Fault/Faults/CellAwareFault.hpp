#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Specification/CellAwareSpecification.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitMetaData.hpp"

namespace FreiTest
{
namespace Fault
{

class CellAwareFaultStateMachine
{
public:
	struct Port
	{
		Circuit::MappedCircuit::NodeAndPort nodeAndPort;
		Basic::LogicConstraint constraint;
	};

	enum class TransitionType { Primary, Secondary };

	struct Transition
	{
		std::vector<Port> inputs;
		std::vector<Port> outputs;
		size_t toState;
		TransitionType type;
		bool accepting;

		const std::vector<Port>& GetInputs(void) const { return inputs; }
		const std::vector<Port>& GetOutputs(void) const { return outputs; }
		size_t GetTargetState(void) const { return toState; }
		TransitionType GetTransitionType(void) const { return type; }
		bool IsAccepting(void) const { return accepting; }
	};

	struct State
	{
		std::vector<Transition> transitions;
		size_t depth;

		const std::vector<Transition>& GetTransitions(void) const { return transitions; }
		size_t GetDepth(void) const { return depth; }

		const Transition&  GetTransition(size_t id) const { return transitions[id]; }
	};

	CellAwareFaultStateMachine(std::vector<State> states);
	virtual ~CellAwareFaultStateMachine(void);

	const std::vector<State>& GetStates(void) const { return states; }
	const std::vector<Circuit::MappedCircuit::NodeAndPort>& GetInputs(void) const { return inputNodes; }
	const std::vector<Circuit::MappedCircuit::NodeAndPort>& GetOutputs(void) const { return outputNodes; }
	size_t GetTimeframes(void) const { return timeframes; }
	size_t GetMinInputNodeId(void) const { return minInputNode; }
	size_t GetMaxInputNodeId(void) const { return maxInputNode; }
	size_t GetMinOutputNodeId(void) const { return minOutputNode; }
	size_t GetMaxOutputNodeId(void) const { return maxOutputNode; }

	const State& GetState(size_t id) const { return states[id]; }

private:
	std::vector<State> states;

	// Derived data to speed up the fault model implementation:
	std::vector<Circuit::MappedCircuit::NodeAndPort> inputNodes;
	std::vector<Circuit::MappedCircuit::NodeAndPort> outputNodes;
	size_t timeframes;
	size_t minInputNode;
	size_t maxInputNode;
	size_t minOutputNode;
	size_t maxOutputNode;
};

struct CellAwareFault
{
public:
	CellAwareFault(std::string name, Circuit::GroupMetaData const* cell, CellAwareFaultStateMachine stateMachine);
	virtual ~CellAwareFault(void);

	const std::string& GetName(void) const { return name; }
	const Circuit::GroupMetaData* GetCell(void) const { return cell; }
	const CellAwareFaultStateMachine& GetStateMachine(void) const { return stateMachine; }
	bool IsSequentialCell(void) const { return sequential; }

	std::string GetCellName(void) const;
	std::string GetCellType(void) const;

private:
	std::string name;
	Circuit::GroupMetaData const* cell;
	CellAwareFaultStateMachine stateMachine;
	bool sequential;
};



std::optional<CellAwareFault> CreateCellAwareFaultForCell(const CellAwareFaultSpecification& specification, const Circuit::GroupMetaData* cell);
std::unordered_map<std::string, Circuit::MappedCircuit::NodeAndPort> CreateCellPortMapForCell(const Circuit::GroupMetaData* cell);

std::string to_string(const CellAwareFaultStateMachine& stateMachine);
std::string to_string(const CellAwareFault& fault);

};
};
