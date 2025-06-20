#pragma once

#include <string>

namespace FreiTest
{
namespace Circuit
{

enum class PortType {
	Input, Output
};

struct Port {
	PortType portType;
	size_t portNumber;

	// Compare in the following order: portType (input < output), portNumber
	inline bool operator<(const Port& comp) const
	{
		return (portType < comp.portType) || (portType == comp.portType && portNumber < comp.portNumber);
	}

	inline bool operator==(const Port& comp) const
	{
		return portType == comp.portType && portNumber == comp.portNumber;
	}
};

std::string to_string(const PortType& portType);
std::string to_string(const Port& port);

};
};
