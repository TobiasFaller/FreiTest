#pragma once

#include <cstddef>
#include <string>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

enum class TransitionDelayFaultType
{
	SLOW_TO_RISE,
	SLOW_TO_FALL,
	SLOW_TO_TRANSITION
};

std::string to_string(const TransitionDelayFaultType& faultType);

class TransitionDelayFault
{
public:
	TransitionDelayFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, TransitionDelayFaultType type);
	virtual ~TransitionDelayFault(void);

	const Circuit::MappedCircuit::NodeAndPort& GetNodeAndPort(void) const;
	const Circuit::MappedNode* GetNode(void) const;
	const Circuit::Port& GetPort(void) const;
	const TransitionDelayFaultType& GetType(void) const;

	operator Circuit::MappedCircuit::NodeAndPort(void) const;
	operator TransitionDelayFaultType(void) const;

	friend std::string to_string(const TransitionDelayFault& fault);

private:
	Circuit::MappedCircuit::NodeAndPort nodeAndPort;
	TransitionDelayFaultType type;

};

std::string to_string(const TransitionDelayFault& fault);

};
};
