#pragma once

#include <boost/property_tree/ptree.hpp>

#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/FaultCoverage/Coverage.hpp"

#include <iostream>
#include <cstdlib>
#include <string>

namespace FreiTest
{
namespace Io
{
namespace FaultCoverage
{

class JsonExporter
{
public:
	JsonExporter();
	virtual ~JsonExporter();

	void ExportFaultCoverage(const Circuit::CircuitEnvironment& CircuitEnvironment, Coverage faultCoverage, std::string fileName);

private:
	boost::property_tree::ptree ExportGroup(const Circuit::CircuitEnvironment& circuitEnvironment, Coverage faultCoverage, const Circuit::GroupMetaData* group);

	std::string GetFullPinName(const Circuit::GroupMetaData* group, std::string pin);
	std::string GetWireName(std::string wire);

};

};
};
};
