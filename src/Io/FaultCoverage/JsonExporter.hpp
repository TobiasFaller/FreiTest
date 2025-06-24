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

	void ExportFaultCoverage(const Circuit::CircuitEnvironment& CircuitEnvironment, const Coverage& faultCoverage, std::string fileName);

private:
	boost::property_tree::ptree ExportGroup(const Circuit::CircuitEnvironment& circuitEnvironment, const Coverage& faultCoverage, const Circuit::GroupMetaData* group);

};

};
};
};
