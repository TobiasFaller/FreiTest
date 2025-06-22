#pragma once

#include <tuple>
#include <string>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/TransitionDelayFault.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

struct SingleTransitionDelayFault
{
public:
	SingleTransitionDelayFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, TransitionDelayFaultType type);
	SingleTransitionDelayFault(TransitionDelayFault fault);
	virtual ~SingleTransitionDelayFault(void);

	const TransitionDelayFault& GetTransitionDelay(void) const;
	const Circuit::MappedCircuit::NodeAndPort& GetNodeAndPort(void) const;
	const Circuit::MappedNode* GetNode(void) const;
	const Circuit::Port& GetPort(void) const;
	const TransitionDelayFaultType& GetType(void) const;

	operator Circuit::MappedCircuit::NodeAndPort(void) const;
	operator TransitionDelayFaultType(void) const;

	friend std::string to_string(const SingleTransitionDelayFault& fault);

private:
	TransitionDelayFault transitionDelay;

};

std::string to_string(const SingleTransitionDelayFault& fault);

};
};
