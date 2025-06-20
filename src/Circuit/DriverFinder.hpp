#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Circuit
{

struct DriverSearchConfig
{
	DriverSearchConfig():
		allowNotFound(false),
		allowUnconnected(true),
		searchPorts(true),
		searchWires(true)
	{}

	bool allowNotFound;
	bool allowUnconnected;
	bool searchPorts;
	bool searchWires;
};

std::vector<const Circuit::MappedNode*> GetDrivers(const std::string& reference, const CircuitEnvironment& circuit, const DriverSearchConfig& config = { });

};
};
