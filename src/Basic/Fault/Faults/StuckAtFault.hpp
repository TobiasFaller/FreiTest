#pragma once

#include <cstddef>
#include <string>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

enum class StuckAtFaultType
{
	STUCK_AT_0,
	STUCK_AT_1,
	STUCK_AT_X,
	STUCK_AT_U,
	STUCK_AT_FREE
};

Basic::Logic to_logic(const Fault::StuckAtFaultType& faultType);
Basic::LogicConstraint to_logic_constraint(const Fault::StuckAtFaultType& faultType);
std::string to_string(const StuckAtFaultType& faultType);

class StuckAtFault
{
public:
	StuckAtFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, StuckAtFaultType type);
	virtual ~StuckAtFault(void);

	const Circuit::MappedCircuit::NodeAndPort& GetNodeAndPort(void) const;
	const Circuit::MappedNode* GetNode(void) const;
	const Circuit::Port& GetPort(void) const;
	const StuckAtFaultType& GetType(void) const;

	operator Circuit::MappedCircuit::NodeAndPort(void) const;
	operator StuckAtFaultType(void) const;

	friend std::string to_string(const StuckAtFault& fault);

private:
	Circuit::MappedCircuit::NodeAndPort nodeAndPort;
	StuckAtFaultType type;

};

std::string to_string(const StuckAtFault& fault);

};
};
