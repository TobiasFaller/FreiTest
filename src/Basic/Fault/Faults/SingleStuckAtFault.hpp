#pragma once

#include <tuple>
#include <string>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/StuckAtFault.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

struct SingleStuckAtFault
{
public:
	SingleStuckAtFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, StuckAtFaultType type);
	SingleStuckAtFault(StuckAtFault fault);
	virtual ~SingleStuckAtFault(void);

	const StuckAtFault& GetStuckAt(void) const;
	const Circuit::MappedCircuit::NodeAndPort& GetNodeAndPort(void) const;
	const Circuit::MappedNode* GetNode(void) const;
	const Circuit::Port& GetPort(void) const;
	const StuckAtFaultType& GetType(void) const;

	operator Circuit::MappedCircuit::NodeAndPort(void) const;
	operator StuckAtFaultType(void) const;

	friend std::string to_string(const SingleStuckAtFault& fault);

private:
	StuckAtFault stuckAt;

};

std::string to_string(const SingleStuckAtFault& fault);

};
};
