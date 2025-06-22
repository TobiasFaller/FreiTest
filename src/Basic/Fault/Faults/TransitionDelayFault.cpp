#include "Basic/Fault/Faults/TransitionDelayFault.hpp"

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Fault
{

std::string to_string(const TransitionDelayFaultType& faultType)
{
	switch (faultType)
	{
		case TransitionDelayFaultType::SLOW_TO_RISE:
			return "Slow-to-Rise";
		case TransitionDelayFaultType::SLOW_TO_FALL:
			return "Slow-to-Fall";
		case TransitionDelayFaultType::SLOW_TO_TRANSITION:
			return "Slow-to-Transition";
		default:
			return "Unknown";
	}
}

TransitionDelayFault::TransitionDelayFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, TransitionDelayFaultType type):
	nodeAndPort(nodeAndPort),
	type(type)
{
}

TransitionDelayFault::~TransitionDelayFault(void) = default;

const Circuit::MappedCircuit::NodeAndPort& TransitionDelayFault::GetNodeAndPort(void) const
{
	return nodeAndPort;
}

const Circuit::MappedNode* TransitionDelayFault::GetNode(void) const
{
	return nodeAndPort.node;
}

const Circuit::Port& TransitionDelayFault::GetPort(void) const
{
	return nodeAndPort.port;
}

const TransitionDelayFaultType& TransitionDelayFault::GetType(void) const
{
	return type;
}

TransitionDelayFault::operator Circuit::MappedCircuit::NodeAndPort(void) const
{
	return nodeAndPort;
}

TransitionDelayFault::operator TransitionDelayFaultType(void) const
{
	return type;
}

std::string to_string(const TransitionDelayFault& fault)
{
	auto [node, port] = fault.nodeAndPort;
	return to_string(fault.type) + " of "
		+ node->GetName() + "(id " + std::to_string(node->GetNodeId()) + ") port "
		+ to_string(port);
}

};
};
