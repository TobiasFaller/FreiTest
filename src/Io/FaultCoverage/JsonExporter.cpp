#include "Io/FaultCoverage/JsonExporter.hpp"

#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"
#include "Io/FaultCoverage/Coverage.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include <time.h>
#include <chrono>
#include <ctime>

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Io
{
namespace FaultCoverage
{

JsonExporter::JsonExporter() = default;
JsonExporter::~JsonExporter() = default;

void JsonExporter::ExportFaultCoverage(const CircuitEnvironment& circuitEnvironment, const Coverage& faultCoverage, std::string fileName)
{
	using ptree = boost::property_tree::ptree;

	const auto& metaData = circuitEnvironment.GetMetaData();

	FileHandle handle(fileName, false);
	std::ofstream& output = handle.GetOutStream();

	if (!output.is_open())
	{
		LOG(FATAL) << "The file " << fileName << " could not be opened";
	}
	ptree testpattern;
	for (size_t pattern = 0; pattern < faultCoverage.pattern.size(); pattern++)
	{
		ptree patternNode;
		for (auto [location, type]: faultCoverage.pattern[pattern])
		{
			ptree fault;
			fault.put("location", location);
			fault.put("type", type);

			patternNode.push_back(std::make_pair("", fault));
		}
		testpattern.add_child(std::to_string(pattern), patternNode);
	}

	ptree circuitNode;
	circuitNode.add_child("group", ExportGroup(circuitEnvironment, faultCoverage, metaData.GetRoot()));

	ptree json;
	json.add_child("pattern", testpattern);
	json.add_child("circuit", circuitNode);

	try
	{
		boost::property_tree::write_json(output, json);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
	}
}

boost::property_tree::ptree JsonExporter::ExportGroup(const CircuitEnvironment& circuitEnvironment, const Coverage& faultCoverage, const GroupMetaData* group)
{
	using ptree = boost::property_tree::ptree;

	auto sourceInfo = group->GetSourceInfo();
	if (sourceInfo.GetProperty<bool>("module-is-virtual").value_or(false))
	{
		return { };
	}

	ptree groupNode;
	groupNode.put("name", group->GetName());
	groupNode.put("hierarchy", group->GetHierarchyName());
	groupNode.put("type", sourceInfo.GetProperty<std::string>("module-type").value_or(""));
	groupNode.put("is-module", sourceInfo.GetProperty<bool>("module-is-module").value_or(false) ? "yes" : "no");

	ptree ports;
	for (auto port : group->GetPorts())
	{
		auto info = port.GetSourceInfo();
		auto type = info.GetProperty<std::string>("port-type").value_or("input");
		std::string fullPinName = port.GetHierarchyName();

		ptree portNode;
		portNode.put("name", port.GetName());
		portNode.put("type", type);
		portNode.put("size", port.GetSize().GetSize());
		portNode.put("bottom", port.GetSize().GetBottom());
		portNode.put("top", port.GetSize().GetTop());

		// Add the connection ids to the port
		ptree connections;
		for (auto connection: port.GetConnections()) {
			if (connection == nullptr) { continue; }

			ptree cNode;
			cNode.put("", connection->GetConnectionId());

			connections.push_back(std::make_pair("", cNode));
		}
		portNode.add_child("connections", connections);

		ports.push_back(std::make_pair("port", portNode));
	}
	groupNode.add_child("ports", ports);

	ptree wires;
	for (auto wire: group->GetWires())
	{
		ptree wireNode;
		wireNode.put("name", wire.GetName());
		wireNode.put("size", wire.GetSize().GetSize());
		wireNode.put("bottom", wire.GetSize().GetBottom());
		wireNode.put("top", wire.GetSize().GetTop());

		ptree connections;
		for (auto connection: wire.GetConnections()) {
			if (connection == nullptr) { continue; }

			ptree cNode;
			cNode.put("", connection->GetConnectionId());

			connections.push_back(std::make_pair("", cNode));
		}
		wireNode.add_child("connections", connections);
		wires.push_back(std::make_pair("wire", wireNode));
	}
	groupNode.add_child("wires", wires);

	ptree subGroups;
	for (auto subGroup: group->GetSubGroups())
	{
		auto subGroupNode = ExportGroup(circuitEnvironment, faultCoverage, subGroup);
		subGroups.push_back(std::make_pair("group", subGroupNode));
	}
	groupNode.add_child("subGroups", subGroups);
	return groupNode;
}

};
};
};
