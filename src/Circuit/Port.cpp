#include "Circuit/Port.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Circuit
{

std::string to_string(const PortType& portType)
{
	switch(portType)
	{
	case PortType::Input:
		return "input";
	case PortType::Output:
		return "output";
	default:
		LOG(FATAL) << "Unknown PortType";
	}
	__builtin_unreachable();
}

std::string to_string(const Port& port)
{
	return "portType: " + to_string(port.portType) + ", portNumber: " + std::to_string(port.portNumber);
}

};
};
