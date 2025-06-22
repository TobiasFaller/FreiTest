#include "Basic/Fault/Faults/StuckAtFault.hpp"

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Fault
{

LogicConstraint to_logic_constraint(const StuckAtFaultType& faultType)
{
	switch (faultType)
	{
		case StuckAtFaultType::STUCK_AT_0:
			return LogicConstraint::ONLY_LOGIC_ZERO;
		case StuckAtFaultType::STUCK_AT_1:
			return LogicConstraint::ONLY_LOGIC_ONE;
		case StuckAtFaultType::STUCK_AT_X:
			return LogicConstraint::ONLY_LOGIC_DONT_CARE;
		case StuckAtFaultType::STUCK_AT_U:
			return LogicConstraint::ONLY_LOGIC_UNKNOWN;
		case StuckAtFaultType::STUCK_AT_FREE:
			Logging::Panic("Stuck-At Free can not be converted to constraint");
		default:
			LOG(FATAL) << "Unknown StuckAtFaultType";
			__builtin_unreachable();
	}
}

Logic to_logic(const StuckAtFaultType& faultType)
{
	switch (faultType)
	{
		case StuckAtFaultType::STUCK_AT_0:
			return Logic::LOGIC_ZERO;
		case StuckAtFaultType::STUCK_AT_1:
			return Logic::LOGIC_ONE;
		case StuckAtFaultType::STUCK_AT_X:
			return Logic::LOGIC_DONT_CARE;
		case StuckAtFaultType::STUCK_AT_U:
			return Logic::LOGIC_UNKNOWN;
		case StuckAtFaultType::STUCK_AT_FREE:
			Logging::Panic("Stuck-At Free can not be converted to logic");
		default:
			LOG(FATAL) << "Unknown StuckAtFaultType";
			__builtin_unreachable();
	}
}

std::string to_string(const StuckAtFaultType& faultType)
{
	switch (faultType)
	{
		case StuckAtFaultType::STUCK_AT_0:
			return "Stuck-At 0";
		case StuckAtFaultType::STUCK_AT_1:
			return "Stuck-At 1";
		case StuckAtFaultType::STUCK_AT_X:
			return "Stuck-At X";
		case StuckAtFaultType::STUCK_AT_U:
			return "Stuck-At U";
		case StuckAtFaultType::STUCK_AT_FREE:
			return "Stuck-At FREE";
		default:
			return "Unknown";
	}
}

StuckAtFault::StuckAtFault(Circuit::MappedCircuit::NodeAndPort nodeAndPort, StuckAtFaultType type):
	nodeAndPort(nodeAndPort),
	type(type)
{
}

StuckAtFault::~StuckAtFault(void) = default;

const Circuit::MappedCircuit::NodeAndPort& StuckAtFault::GetNodeAndPort(void) const
{
	return nodeAndPort;
}

const Circuit::MappedNode* StuckAtFault::GetNode(void) const
{
	return nodeAndPort.node;
}

const Circuit::Port& StuckAtFault::GetPort(void) const
{
	return nodeAndPort.port;
}

const StuckAtFaultType& StuckAtFault::GetType(void) const
{
	return type;
}

StuckAtFault::operator Circuit::MappedCircuit::NodeAndPort(void) const
{
	return nodeAndPort;
}

StuckAtFault::operator StuckAtFaultType(void) const
{
	return type;
}

std::string to_string(const StuckAtFault& fault)
{
	auto [node, port] = fault.nodeAndPort;
	return to_string(fault.type) + " of "
		+ node->GetName() + "(id " + std::to_string(node->GetNodeId()) + ") port "
		+ to_string(port);
}

};
};
