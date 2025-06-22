#include "Applications/Circuit/ExportCircuitHierarchy.hpp"

#include <boost/format.hpp>

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
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitHierarchy::ExportCircuitHierarchy(void) = default;
ExportCircuitHierarchy::~ExportCircuitHierarchy(void) = default;

void ExportCircuitHierarchy::Init(void)
{
}

bool ExportCircuitHierarchy::SetSetting(std::string key, std::string value)
{
	if (key == "Utility/NameFilter")
	{
		optionFilter = value;
		return true;
	}

	return false;
}

void ExportCircuitHierarchy::Run(void)
{
	FileHandle handle("[DataExportDirectory]/hierarchy.txt", false);
	std::ofstream& output = handle.GetOutStream();

	auto metaData = this->circuit->GetMetaData();
	ExportHierarchy(0u, metaData.GetRoot(), output);
}

void ExportCircuitHierarchy::ExportHierarchy(size_t level, const Circuit::GroupMetaData* group, std::ostream& out)
{
	std::regex filterRegex { optionFilter, std::regex_constants::ECMAScript };
	if (!std::regex_match(group->GetHierarchyName(), filterRegex))
	{
		return;
	}

	auto convert_to_string = [](std::any value) -> std::string
	{
		if (value.type() == typeid(size_t))
		{
			return std::to_string(std::any_cast<size_t>(value));
		}
		else if (value.type() == typeid(bool))
		{
			return std::any_cast<bool>(value) ? "yes" : "no";
		}
		else if (value.type() == typeid(std::string))
		{
			return std::any_cast<std::string>(value);
		}
		else if (value.type() == typeid(std::vector<std::string>))
		{
			auto vec = std::any_cast<std::vector<std::string>>(value);
			std::string result = "";
			for (size_t index = 0u; index < vec.size(); ++index)
			{
				if (index != 0u) result += ", ";
				result += vec[index];
			}
			return result;
		}

		return "Not implemented yet";
	};
	auto connections_to_string = [](const std::vector<const Circuit::ConnectionMetaData*>& connections)
	{
		std::string result;
		for (auto const& connection : connections)
		{
			if (result.size() > 0u) result += ", ";
			result += (connection != nullptr) ? ("C" + std::to_string(connection->GetConnectionId())) : "NULL";
		}
		return result;
	};

	out << std::string(level, '\t') << "Group: " << group->GetName() << std::endl;
	out << std::string(level + 1u, '\t') << "Source File: " << group->GetSourceInfo().sourceFile << std::endl;
	out << std::string(level + 1u, '\t') << "Source Location: " << group->GetSourceInfo().sourceLocation << std::endl;
	out << std::string(level + 1u, '\t') << "Source Name: " << group->GetSourceInfo().sourceName << std::endl;
	out << std::string(level + 1u, '\t') << "Source Type: " << group->GetSourceInfo().sourceType << std::endl;
	for (auto [name, value] : group->GetSourceInfo().properties)
	{
		out << std::string(level + 1u, '\t') << "Source Info: " << name << "=" << convert_to_string(value) << std::endl;
	}

	for (auto& port : group->GetPorts())
	{
		out << std::string(level + 1u, '\t') << "Port: " << port.GetName() << std::endl;
		out << std::string(level + 2u, '\t') << "Source File: " << port.GetSourceInfo().sourceFile << std::endl;
		out << std::string(level + 2u, '\t') << "Source Location: " << port.GetSourceInfo().sourceLocation << std::endl;
		out << std::string(level + 2u, '\t') << "Source Name: " << port.GetSourceInfo().sourceName << std::endl;
		out << std::string(level + 2u, '\t') << "Source Type: " << port.GetSourceInfo().sourceType << std::endl;
		for (auto [name, value] : port.GetSourceInfo().properties)
		{
			out << std::string(level + 2u, '\t') << "Source Info: " << name << "=" << convert_to_string(value) << std::endl;
		}
		out << std::string(level + 2u, '\t') << "Connections: " << connections_to_string(port.GetConnections()) << std::endl;
	}

	for (auto& wire : group->GetWires())
	{
		out << std::string(level + 1u, '\t') << "Wire: " << wire.GetName() << std::endl;
		out << std::string(level + 2u, '\t') << "Source File: " << wire.GetSourceInfo().sourceFile << std::endl;
		out << std::string(level + 2u, '\t') << "Source Location: " << wire.GetSourceInfo().sourceLocation << std::endl;
		out << std::string(level + 2u, '\t') << "Source Name: " << wire.GetSourceInfo().sourceName << std::endl;
		out << std::string(level + 2u, '\t') << "Source Type: " << wire.GetSourceInfo().sourceType << std::endl;
		for (auto [name, value] : wire.GetSourceInfo().properties)
		{
			out << std::string(level + 2u, '\t') << "Source Info: " << name << "=" << convert_to_string(value) << std::endl;
		}
		out << std::string(level + 2u, '\t') << "Connections: " << connections_to_string(wire.GetConnections()) << std::endl;
	}

	for (auto& unmappedNode : group->GetUnmappedNodes())
	{
		out << std::string(level + 1u, '\t') << "Unmapped Gate: " << unmappedNode->GetName() << std::endl;
		out << std::string(level + 2u, '\t') << "Type: " << unmappedNode->GetType() << std::endl;
		for (size_t input = 0u; input < unmappedNode->GetNumberOfInputs(); ++input)
		{
			out << std::string(level + 2u, '\t') << "Input " << std::to_string(input + 1u) << ":" << std::endl;
			out << std::string(level + 3u, '\t') << "Name: " << unmappedNode->GetInputPinData(input)->PinName << std::endl;
			out << std::string(level + 3u, '\t') << "ID: " << unmappedNode->GetInputPinData(input)->PinSort << std::endl;
			out << std::string(level + 3u, '\t') << "Signal: "
				<< ((unmappedNode->GetInputPinData(input)->PinWireId != Circuit::MappedCircuit::NO_CONNECTION)
					? unmappedNode->GetInputPinData(input)->PinWireName : "None")
				<< std::endl;
			out << std::string(level + 3u, '\t') << "Connection: "
				<< ((unmappedNode->GetInputPinData(input)->PinWireId != Circuit::MappedCircuit::NO_CONNECTION)
					? ("C" + std::to_string(unmappedNode->GetInputPinData(input)->PinWireId)) : "X")
				<< std::endl;
		}
		for (size_t output = 0u; output < unmappedNode->GetNumberOfOutputs(); ++output)
		{
			out << std::string(level + 2u, '\t') << "Output " << std::to_string(output + 1u) << ":" << std::endl;
			out << std::string(level + 3u, '\t') << "Name: " << unmappedNode->GetOutputPinData(output)->PinName << std::endl;
			out << std::string(level + 3u, '\t') << "ID: " << unmappedNode->GetOutputPinData(output)->PinSort << std::endl;
			out << std::string(level + 3u, '\t') << "Signal: "
				<< ((unmappedNode->GetOutputPinData(output)->PinWireId != Circuit::UnmappedCircuit::NO_CONNECTION)
					? unmappedNode->GetOutputPinData(output)->PinWireName : "None")
				<< std::endl;
			out << std::string(level + 3u, '\t') << "Connection: "
				<< ((unmappedNode->GetOutputPinData(output)->PinWireId != Circuit::UnmappedCircuit::NO_CONNECTION)
					? ("C" + std::to_string(unmappedNode->GetOutputPinData(output)->PinWireId)) : "X")
				<< std::endl;
		}
	}

	for (auto& mappedNode : group->GetMappedNodes())
	{
		out << std::string(level + 1u, '\t') << "Mapped Gate: " << mappedNode->GetName() << std::endl;
		out << std::string(level + 2u, '\t') << "Cell Category: " << to_string(mappedNode->GetCellCategory()) << std::endl;
		out << std::string(level + 2u, '\t') << "Cell Type: " << to_string(mappedNode->GetCellType()) << std::endl;
		for (size_t input = 0u; input < mappedNode->GetNumberOfInputs(); ++input)
		{
			out << std::string(level + 2u, '\t') << "Input " << std::to_string(input + 1u) << ": "
				<< ((mappedNode->GetInputConnectionId(input) != Circuit::MappedCircuit::NO_CONNECTION)
					? ("C" + std::to_string(mappedNode->GetInputConnectionId(input))) : "X")
				<< std::endl;
		}
		out << std::string(level + 2u, '\t') << "Output: "
				<< ((mappedNode->GetOutputConnectionId() != Circuit::MappedCircuit::NO_CONNECTION)
					? ("C" + std::to_string(mappedNode->GetOutputConnectionId())) : "X")
				<< std::endl;
	}

	for (auto& group : group->GetSubGroups())
	{
		ExportHierarchy(level + 1u, group, out);
	}
}

};
};
};
