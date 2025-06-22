#include "Applications/Circuit/ExportCircuitNodes.hpp"

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
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Helper/FileHandle.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitNodes::ExportCircuitNodes(void) = default;
ExportCircuitNodes::~ExportCircuitNodes(void) = default;

void ExportCircuitNodes::Init(void)
{
}

bool ExportCircuitNodes::SetSetting(std::string key, std::string value)
{
	if (key == "Utility/NodeFilter")
	{
		optionFilter = value;
		return true;
	}

	return false;
}

void ExportCircuitNodes::Run(void)
{
	std::vector<std::string> unmappedName;
	std::vector<std::string> unmappedType;
	std::vector<std::string> unmappedGateId;
	std::vector<std::vector<std::string>> unmappedInputPins;
	std::vector<std::vector<std::string>> unmappedInputPinIds;
	std::vector<std::vector<std::string>> unmappedInputWires;
	std::vector<std::vector<std::string>> unmappedOutputPins;
	std::vector<std::vector<std::string>> unmappedOutputPinIds;
	std::vector<std::vector<std::string>> unmappedOutputWires;
	std::vector<std::vector<std::string>> mappedNames;
	std::vector<std::vector<std::string>> mappedTypes;
	std::vector<std::vector<std::string>> mappedGateIds;
	std::vector<std::vector<std::string>> mappedSignalName;

	auto& unmappedCircuit { this->circuit->GetUnmappedCircuit() };
	auto& mappedCircuit { this->circuit->GetMappedCircuit() };
	auto& metaData { this->circuit->GetMetaData() };

	unmappedName.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedType.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedGateId.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedInputPins.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedInputWires.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedOutputPins.reserve(unmappedCircuit.GetNumberOfNodes());
	unmappedOutputWires.reserve(unmappedCircuit.GetNumberOfNodes());
	mappedNames.reserve(unmappedCircuit.GetNumberOfNodes());
	mappedTypes.reserve(unmappedCircuit.GetNumberOfNodes());
	mappedGateIds.reserve(unmappedCircuit.GetNumberOfNodes());
	mappedSignalName.reserve(unmappedCircuit.GetNumberOfNodes());

	// ------------------------------------------------------------------------
	// Collect gate information
	// ------------------------------------------------------------------------

	std::regex filterRegex { optionFilter, std::regex_constants::ECMAScript };
	for (size_t unmappedNodeId = 0u; unmappedNodeId < unmappedCircuit.GetNumberOfNodes(); ++unmappedNodeId)
	{
		const auto* unmappedNode = unmappedCircuit.GetNode(unmappedNodeId);
		if (!std::regex_match(metaData.GetGroup(unmappedNode)->GetHierarchyName(), filterRegex))
		{
			continue;
		}

		unmappedName.emplace_back(unmappedNode->GetName());
		unmappedType.emplace_back(unmappedNode->GetType());
		unmappedGateId.emplace_back(std::to_string(unmappedNodeId));

		unmappedInputPins.emplace_back();
		unmappedInputPinIds.emplace_back();
		unmappedInputWires.emplace_back();
		for (size_t inputId = 0u; inputId < unmappedNode->GetNumberOfInputs(); ++inputId)
		{
			unmappedInputPins.back().emplace_back(unmappedNode->GetInputPinData(inputId)->PinName);
			unmappedInputPinIds.back().emplace_back(std::to_string(unmappedNode->GetInputPinData(inputId)->PinSort));
			unmappedInputWires.back().emplace_back(unmappedNode->GetInputPinData(inputId)->PinWireName);
		}

		unmappedOutputPins.emplace_back();
		unmappedOutputPinIds.emplace_back();
		unmappedOutputWires.emplace_back();
		for (size_t outputId = 0u; outputId < unmappedNode->GetNumberOfOutputs(); ++outputId)
		{
			unmappedOutputPins.back().emplace_back(unmappedNode->GetOutputPinData(outputId)->PinName);
			unmappedOutputPinIds.back().emplace_back(std::to_string(unmappedNode->GetOutputPinData(outputId)->PinSort));
			unmappedOutputWires.back().emplace_back(unmappedNode->GetOutputPinData(outputId)->PinWireName);
		}

		mappedNames.emplace_back();
		mappedTypes.emplace_back();
		mappedGateIds.emplace_back();
		mappedSignalName.emplace_back();

		for (int64_t mappedNodeId : this->circuit->GetMappedNodeIds(unmappedNodeId))
		{
			const auto* mappedNode = mappedCircuit.GetNode(mappedNodeId);

			mappedNames.back().emplace_back(mappedNode->GetName());
			mappedTypes.back().emplace_back(boost::str(boost::format("%s %s") % to_string(mappedNode->GetCellCategory()) % to_string(mappedNode->GetCellType())));
			mappedGateIds.back().emplace_back(std::to_string(mappedNodeId));
			mappedSignalName.back().emplace_back(mappedNode->GetOutputSignalName());
		}
	}

	// ------------------------------------------------------------------------
	// Prepare layout
	// ------------------------------------------------------------------------

	// Find the maximum string length of a (nested) vector.
	auto vector_max = [](auto& vector) -> size_t {
		using type = std::remove_reference_t<decltype(vector)>;

		size_t max = 0u;

		if constexpr (std::is_same_v<type, std::string>)
		{
			max = vector.size();
		}
		else if constexpr (std::is_same_v<type, std::vector<std::string>>)
		{
			for (auto& element : vector)
				max = std::max(max, element.size());
		}
		else if constexpr (std::is_same_v<type, std::vector<std::vector<std::string>>>)
		{
			for (auto& vec1 : vector)
				for (auto& element : vec1)
					max = std::max(max, element.size());
		}
		else if constexpr (std::is_same_v<type, std::vector<std::vector<std::vector<std::string>>>>)
		{
			for (auto& vec1 : vector)
				for (auto& vec2 : vec1)
					for (auto& element : vec2)
						max = std::max(max, element.size());
		}
		else
		{
			// Print the type in an error message
			typename decltype(vector)::type::invalid unsupported_type;
		}

		return max;
	};

	std::vector<std::string> columnHeaders {
		"Unmapped Name",
		"Unmapped Type",
		"Unmapped Gate Id",
		"Unmapped Input Pins",
		"Unmapped Input Wires",
		"Unmapped Output Pins",
		"Unmapped Output Wires",
		"Mapped Names",
		"Mapped Types",
		"Mapped Gate Ids",
		"Mapped Output Signal",
	};
	std::vector<size_t> columnSizes {
		vector_max(unmappedName),
		vector_max(unmappedType),
		vector_max(unmappedGateId),
		vector_max(unmappedInputPins),
		vector_max(unmappedInputWires),
		vector_max(unmappedOutputPins),
		vector_max(unmappedOutputWires),
		vector_max(mappedNames),
		vector_max(mappedTypes),
		vector_max(mappedGateIds),
		vector_max(mappedSignalName),
	};
	std::vector<std::string> separators {
		"+|",
		"+|",
		"+|",
		"+|",
		"+|",
		"+|",
		"+|",
		"+ +| |",
		"+|",
		"+|",
		"+|",
		"+|"
	};
	const auto get_separator = [&separators](size_t column, bool divider) -> std::string_view {
		const auto& separator = separators[column];
		const auto offset = divider ? 0u : (separator.size() / 2u);
		return std::string_view(separator).substr(offset, separator.size() / 2u);
	};

	// Fix the size of the column if the header is larger.
	for (size_t column = 0u; column < columnHeaders.size(); ++column)
		columnSizes[column] = std::max(columnHeaders[column].size(), columnSizes[column]);

	// ------------------------------------------------------------------------
	// Print the table
	// ------------------------------------------------------------------------

	FileHandle handle("[DataExportDirectory]/nodes.txt", false);
	std::ofstream& output = handle.GetOutStream();

	// Print header
	for (size_t column = 0u; column < columnHeaders.size(); ++column)
		output << get_separator(column, true) << std::string(columnSizes[column] + 2u, '-');
	output << get_separator(columnHeaders.size(), true) << std::endl;

	for (size_t column = 0u; column < columnHeaders.size(); ++column)
		output << get_separator(column, false) << " " << std::setw(columnSizes[column]) << std::left << columnHeaders[column] << " ";
	output << get_separator(columnHeaders.size(), false) << std::endl;

	for (size_t column = 0u; column < columnHeaders.size(); ++column)
		output << get_separator(column, true) << std::string(columnSizes[column] + 2u, '-');
	output << get_separator(columnHeaders.size(), true) << std::endl;

	// Returns a lambda that:
	// - Accepts the loop indices as input
	// - Returns the value for the corresponding indices
	// - Returns true for each dimension that has been exhausted
	auto build_vector_iterator = [](auto& vector) -> std::function<std::tuple<std::string, bool, bool, bool>(size_t, size_t, size_t)> {
		using type = std::remove_reference_t<decltype(vector)>;

		if constexpr (std::is_same_v<type, std::string>)
		{
			return [&vector](size_t index1, size_t index2, size_t index3) -> std::tuple<std::string, bool, bool, bool> {
				return {
					(index1 > 0u || index2 > 0u || index3 > 0u) ? "" : vector,
					true,
					true,
					true
				};
			};
		}
		else  if constexpr (std::is_same_v<type, std::vector<std::string>>)
		{
			return [&vector](size_t index1, size_t index2, size_t index3) -> std::tuple<std::string, bool, bool, bool> {
				return {
					(index1 >= vector.size() || index2 > 0u || index3 > 0u) ? "" : vector[index1],
					(index1 + 1u >= vector.size()),
					true,
					true
				};
			};
		}
		else if constexpr (std::is_same_v<type, std::vector<std::vector<std::string>>>)
		{
			return [&vector](size_t index1, size_t index2, size_t index3) -> std::tuple<std::string, bool, bool, bool> {
				return {
					(index1 >= vector.size() || index2 >= vector[index1].size() || index3 > 0u) ? "" : vector[index1][index2],
					(index1 + 1u >= vector.size()),
					(index1 >= vector.size()) || (index2 + 1u >= vector[index1].size()),
					true
				};
			};
		}
		else if constexpr (std::is_same_v<type, std::vector<std::vector<std::vector<std::string>>>>)
		{
			return [&vector](size_t index1, size_t index2, size_t index3) -> std::tuple<std::string, bool, bool, bool> {
				return {
					(index1 >= vector.size() || index2 >= vector[index1].size() || index3 >= vector[index1][index2].size()) ? "" : vector[index1][index2][index3],
					(index1 + 1u >= vector.size()),
					(index1 >= vector.size()) || (index2 + 1u >= vector[index1].size()),
					(index1 >= vector.size()) || (index2 >= vector[index1].size()) || (index3 + 1u >= vector[index1][index2].size())
				};
			};
		}
		else
		{
			// Print the type in an error message
			typename decltype(vector)::type::invalid unsupported_type;
		}
	};

	// Print content
	for (size_t index = 0u; index < unmappedName.size(); ++index)
	{
		std::vector<std::function<std::tuple<std::string, bool, bool, bool>(size_t, size_t, size_t)>> rowIterators {
			build_vector_iterator(unmappedName[index]),
			build_vector_iterator(unmappedType[index]),
			build_vector_iterator(unmappedGateId[index]),
			build_vector_iterator(unmappedInputPins[index]),
			build_vector_iterator(unmappedInputWires[index]),
			build_vector_iterator(unmappedOutputPins[index]),
			build_vector_iterator(unmappedOutputWires[index]),
			build_vector_iterator(mappedNames[index]),
			build_vector_iterator(mappedTypes[index]),
			build_vector_iterator(mappedGateIds[index]),
			build_vector_iterator(mappedSignalName[index])
		};

		for (size_t index1 = 0u;; ++index1)
		{
			for (size_t index2 = 0u;; ++index2)
			{
				for (size_t index3 = 0u;; ++index3)
				{
					bool index1ex = true;
					bool index2ex = true;
					bool index3ex = true;
					for (size_t column = 0u; column < columnHeaders.size(); ++column)
					{
						auto [value, exhausted1, exhausted2, exhausted3] = rowIterators[column](index1, index2, index3);
						index1ex &= exhausted1;
						index2ex &= exhausted2;
						index3ex &= exhausted3;

						output << get_separator(column, false) << " " << std::setw(columnSizes[column]) << std::left << value << " ";
					}
					output << get_separator(columnHeaders.size(), false) << std::endl;

					if (index1ex & index2ex & index3ex) goto exit_loop;
					if (index2ex & index3ex) goto continue_index_1;
					if (index3ex) goto continue_index_2;
				}

			continue_index_2:
				continue;
			}

		continue_index_1:
			continue;

		exit_loop:
			break;
		}

		for (size_t column = 0u; column < columnHeaders.size(); ++column)
			output << get_separator(column, true) << std::string(columnSizes[column] + 2u, '-');
		output << get_separator(columnHeaders.size(), true) << std::endl;
	}

}

};
};
};
