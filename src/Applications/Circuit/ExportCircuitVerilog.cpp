#include "Applications/Circuit/ExportCircuitVerilog.hpp"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>

#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/CircuitVerilogExporter/CircuitVerilogExporter.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitVerilog::ExportCircuitVerilog(void) = default;
ExportCircuitVerilog::~ExportCircuitVerilog(void) = default;

void ExportCircuitVerilog::Init(void)
{
}

bool ExportCircuitVerilog::SetSetting(std::string key, std::string value)
{
	return false;
}

void ExportCircuitVerilog::Run(void)
{
	FileHandle handleVerilog("[DataExportDirectory]/circuit.v", false);
	std::ofstream& outputVerilog = handleVerilog.GetOutStream();

	Io::CircuitVerilogExporter exporter;
	exporter.ExportCircuit(*this->circuit, outputVerilog);
}

};
};
};
