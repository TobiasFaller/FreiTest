#include "Applications/Circuit/ExportCircuitPorts.hpp"

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

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/CircuitGuard/CircuitGuard.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitPorts::ExportCircuitPorts(void) = default;
ExportCircuitPorts::~ExportCircuitPorts(void) = default;

void ExportCircuitPorts::Init(void)
{
}

bool ExportCircuitPorts::SetSetting(std::string key, std::string value)
{
	return false;
}

void ExportCircuitPorts::Run(void)
{
	auto const& metaData { this->circuit->GetMetaData() };
	auto const& mappedCircuit { this->circuit->GetMappedCircuit() };

	// ------------------------------------------------------------------------
	// Export in text form (combined)
	// ------------------------------------------------------------------------

	FileHandle handleTxt("[DataExportDirectory]/ports.txt", false);
	std::ofstream& outputTxt = handleTxt.GetOutStream();

	outputTxt << "Name: " << this->circuit->GetName() << std::endl;
	outputTxt << std::endl;

	outputTxt << "Primary Inputs: " << std::to_string(mappedCircuit.GetNumberOfPrimaryInputs()) << std::endl;
	for (auto [inputIndex, inputNode] : mappedCircuit.EnumeratePrimaryInputs())
	{
		outputTxt << '\t' << metaData.GetFriendlyName({ inputNode, { Circuit::PortType::Output, 0u } }) << std::endl;
	}
	outputTxt << std::endl;

	outputTxt << "Primary Outputs: " << std::to_string(mappedCircuit.GetNumberOfPrimaryOutputs()) << std::endl;
	for (auto [outputIndex, outputNode] : mappedCircuit.EnumeratePrimaryOutputs())
	{
		outputTxt << '\t' << metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) << std::endl;
	}
	outputTxt << std::endl;

	outputTxt << "Secondary Inputs: " << std::to_string(mappedCircuit.GetNumberOfSecondaryInputs()) << std::endl;
	for (auto [inputIndex, inputNode] : mappedCircuit.EnumerateSecondaryInputs())
	{
		outputTxt << '\t' << metaData.GetFriendlyName({ inputNode->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }) << std::endl;
	}
	outputTxt << std::endl;

	outputTxt << "Secondary Outputs: " << std::to_string(mappedCircuit.GetNumberOfSecondaryOutputs()) << std::endl;
	for (auto [outputIndex, outputNode] : mappedCircuit.EnumerateSecondaryOutputs())
	{
		outputTxt << '\t' << metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) << std::endl;
	}
	outputTxt << std::endl;

	// ------------------------------------------------------------------------
	// Export in text form (separately)
	// ------------------------------------------------------------------------

	{
		FileHandle handleTxt("[DataExportDirectory]/primary-inputs.txt", false);
		std::ofstream& primaryInputs = handleTxt.GetOutStream();
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumeratePrimaryInputs())
		{
			auto name { metaData.GetFriendlyName({ inputNode, { Circuit::PortType::Output, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Primary input is missing circuit name";
			primaryInputs << name.substr(name.find("/") + 1u) << std::endl;
		}
	}

	{
		FileHandle handleTxt("[DataExportDirectory]/primary-outputs.txt", false);
		std::ofstream& primaryOutputs = handleTxt.GetOutStream();
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumeratePrimaryOutputs())
		{
			auto name { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Primary output is missing circuit name";
			primaryOutputs << name.substr(name.find("/") + 1u) << std::endl;
		}
	}

	{
		FileHandle handleTxt("[DataExportDirectory]/secondary-inputs.txt", false);
		std::ofstream& secondaryInputs = handleTxt.GetOutStream();
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumerateSecondaryInputs())
		{
			auto name { metaData.GetFriendlyName({ inputNode->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Secondary input is missing circuit name";
			secondaryInputs << name.substr(name.find("/") + 1u) << std::endl;
		}
	}

	{
		FileHandle handleTxt("[DataExportDirectory]/secondary-outputs.txt", false);
		std::ofstream& secondaryOutputs = handleTxt.GetOutStream();
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumerateSecondaryOutputs())
		{
			auto name { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Secondary output is missing circuit name";
			secondaryOutputs << name.substr(name.find("/") + 1u) << std::endl;
		}
	}

	// ------------------------------------------------------------------------
	// Export in JSON form
	// ------------------------------------------------------------------------

	FileHandle handleJson("[DataExportDirectory]/ports.json", false);
	std::ofstream& outputJson = handleJson.GetOutStream();

	try
	{
		auto const circuit = Io::CreateCircuitGuard(*this->circuit);
		boost::property_tree::write_json(outputJson, circuit);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
	}
}

};
};
};
