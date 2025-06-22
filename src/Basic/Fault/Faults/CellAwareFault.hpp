#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest
{
namespace Fault
{

struct CellAwarePort
{
	Circuit::MappedCircuit::NodeAndPort nodeAndPort;
	std::string portName;
	std::vector<Basic::LogicConstraint> logicConstraints;
};

struct CellAwareAlternative
{
	std::vector<CellAwarePort> conditions;
	std::vector<CellAwarePort> effects;
};

struct CellAwareTransition
{
	enum class TransitionType { Primary, Secondary };

	TransitionType type;
	size_t toState;
	std::vector<CellAwarePort> conditions;
	std::vector<CellAwarePort> effects;
	std::vector<size_t> alternatives;
};

struct CellAwareState
{
	size_t depth;
	bool accepting;
	std::vector<CellAwareTransition> transitions;
	std::vector<size_t> alternatives;
};

struct CellAwareFault
{
public:
	CellAwareFault(
		const std::shared_ptr<Io::Udfm::UdfmFault> userDefinedFault,
		const Circuit::GroupMetaData* cell,
		std::vector<CellAwareAlternative> alternatives
	);
	virtual ~CellAwareFault(void);

	const std::shared_ptr<Io::Udfm::UdfmFault>& GetUserDefinedFault(void) const;
	const Circuit::GroupMetaData* GetCell(void) const;
	const std::vector<CellAwareAlternative>& GetAlternatives(void) const;

	const std::vector<Circuit::MappedCircuit::NodeAndPort>& GetConditionNodesAndPorts(void) const;
	const std::vector<Circuit::MappedCircuit::NodeAndPort>& GetEffectNodesAndPorts(void) const;

	size_t GetTimeframeSpread(void) const;
	const std::vector<CellAwareState>& GetStateMachine(void) const;

private:
	const std::shared_ptr<Io::Udfm::UdfmFault> _userDefinedFault;
	const Circuit::GroupMetaData* _cell;
	std::vector<CellAwareAlternative> _alternatives;
	std::vector<CellAwareState> _states;

	// All NodeAndPorts that occur at at least one condition/effect in at least one test alternative; ideal for e.g. tagging
	std::vector<Circuit::MappedCircuit::NodeAndPort> _conditionNodesAndPorts;
	std::vector<Circuit::MappedCircuit::NodeAndPort> _effectNodesAndPorts;
	size_t _timeframeSpread;
};

std::string to_string(const CellAwareFault& fault);
std::string to_string(const CellAwareAlternative& alternative);
std::string to_string(const CellAwarePort& attribute);
std::string to_string(const std::vector<CellAwareState>& states);
std::string to_string(const CellAwareTransition::TransitionType& type);

};
};
