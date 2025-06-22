#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/SingleStuckAtFault.hpp"
#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

SingleStuckAtFault::SingleStuckAtFault(MappedCircuit::NodeAndPort nodeAndPort, StuckAtFaultType type):
	stuckAt(nodeAndPort, type)
{
}

SingleStuckAtFault::SingleStuckAtFault(StuckAtFault stuckAt):
	stuckAt(stuckAt)
{
}

SingleStuckAtFault::~SingleStuckAtFault(void) = default;

const StuckAtFault& SingleStuckAtFault::GetStuckAt(void) const
{
	return stuckAt;
}

const Circuit::MappedCircuit::NodeAndPort& SingleStuckAtFault::GetNodeAndPort(void) const
{
	return stuckAt.GetNodeAndPort();
}

const Circuit::MappedNode* SingleStuckAtFault::GetNode(void) const
{
	return stuckAt.GetNodeAndPort().node;
}

const Circuit::Port& SingleStuckAtFault::GetPort(void) const
{
	return stuckAt.GetNodeAndPort().port;
}

const StuckAtFaultType& SingleStuckAtFault::GetType(void) const
{
	return stuckAt.GetType();
}

SingleStuckAtFault::operator Circuit::MappedCircuit::NodeAndPort(void) const
{
	return stuckAt.GetNodeAndPort();
}

SingleStuckAtFault::operator StuckAtFaultType(void) const
{
	return stuckAt.GetType();
}

std::string to_string(const SingleStuckAtFault& fault)
{
	return "Single Stuck-At Fault[" + to_string(fault.stuckAt) + "]";
}

};
};
