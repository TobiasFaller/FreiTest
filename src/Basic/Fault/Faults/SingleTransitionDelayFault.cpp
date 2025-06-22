#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/SingleTransitionDelayFault.hpp"
#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

SingleTransitionDelayFault::SingleTransitionDelayFault(MappedCircuit::NodeAndPort nodeAndPort, TransitionDelayFaultType type):
	transitionDelay(nodeAndPort, type)
{
}

SingleTransitionDelayFault::SingleTransitionDelayFault(TransitionDelayFault transitionDelay):
	transitionDelay(transitionDelay)
{
}

SingleTransitionDelayFault::~SingleTransitionDelayFault(void) = default;

const TransitionDelayFault& SingleTransitionDelayFault::GetTransitionDelay(void) const
{
	return transitionDelay;
}

const Circuit::MappedCircuit::NodeAndPort& SingleTransitionDelayFault::GetNodeAndPort(void) const
{
	return transitionDelay.GetNodeAndPort();
}

const Circuit::MappedNode* SingleTransitionDelayFault::GetNode(void) const
{
	return transitionDelay.GetNodeAndPort().node;
}

const Circuit::Port& SingleTransitionDelayFault::GetPort(void) const
{
	return transitionDelay.GetNodeAndPort().port;
}

const TransitionDelayFaultType& SingleTransitionDelayFault::GetType(void) const
{
	return transitionDelay.GetType();
}

SingleTransitionDelayFault::operator Circuit::MappedCircuit::NodeAndPort(void) const
{
	return transitionDelay.GetNodeAndPort();
}

SingleTransitionDelayFault::operator TransitionDelayFaultType(void) const
{
	return transitionDelay.GetType();
}

std::string to_string(const SingleTransitionDelayFault& fault)
{
	return "Single Transition Delay Fault[" + to_string(fault.transitionDelay) + "]";
}

};
};
