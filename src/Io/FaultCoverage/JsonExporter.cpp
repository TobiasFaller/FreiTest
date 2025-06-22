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


using namespace std;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Io
{
namespace FaultCoverage
{
	JsonExporter::JsonExporter() = default;
	JsonExporter::~JsonExporter() = default;

	void JsonExporter::ExportFaultCoverage(const Circuit::CircuitEnvironment& circuitEnvironment, Coverage faultCoverage, string fileName)
	{
		using ptree = boost::property_tree::ptree;

		const auto& metaData = circuitEnvironment.GetMetaData();

		FileHandle handle(fileName, false);
		ofstream& output = handle.GetOutStream();

		if (!output.is_open())
		{
			LOG(FATAL) << "The file " << fileName << " could not be opened";
		}
		ptree testpattern;
		for (size_t pattern = 0; pattern < faultCoverage.pattern.size(); pattern++)
		{
			ptree patternNode;
			for (auto [pin, stuckAt]: faultCoverage.pattern[pattern])
			{
				ptree fault;
				fault.put("pin", pin);
				fault.put("stuckAtFault", stuckAt);

				patternNode.push_back(std::make_pair("", fault));
			}
			testpattern.add_child(to_string(pattern), patternNode);
		}

		ptree circuitNode;
		circuitNode.add_child("group", ExportGroup(circuitEnvironment, faultCoverage, metaData.GetRoot()));

		ptree json;
		json.add_child("testpattern", testpattern);
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

	boost::property_tree::ptree JsonExporter::ExportGroup(const Circuit::CircuitEnvironment& circuitEnvironment, Coverage faultCoverage, const GroupMetaData* group)
	{
		using ptree = boost::property_tree::ptree;

		auto sourceInfo = group->GetSourceInfo();
		auto name = sourceInfo.HasProperty<string>("module-name")
			? sourceInfo.GetProperty<string>("module-name")
			: sourceInfo.GetProperty<string>("primitive-name");
		auto type = sourceInfo.HasProperty<string>("module-type")
			? sourceInfo.GetProperty<string>("module-type")
			: sourceInfo.GetProperty<string>("primitive-type");

		if (name == "" || (type == "input" || type == "output"))
		{
			return { };
		}

		ptree groupNode;
		groupNode.put("name", group->GetName());
		groupNode.put("hierarchy", group->GetHierarchyName());
		groupNode.put("type", type.value_or(""));
		groupNode.put("is-module", (sourceInfo.HasProperty<string>("module-name") ? "yes" : "no"));

		ptree ports;
		for (auto port: group->GetPorts())
		{
			auto info = port.GetSourceInfo();
			auto type = info.GetProperty<string>("port-type").value_or("input");

			string fullPinName = GetFullPinName(group, port.GetName());

			ptree portNode;
			portNode.put("name", port.GetName());
			portNode.put("type", type);
			portNode.put("size", port.GetSize().GetSize());
			portNode.put("bottom", port.GetSize().GetBottom());
			portNode.put("top", port.GetSize().GetTop());

			if (faultCoverage.stuckAtOne.find(fullPinName) != faultCoverage.stuckAtOne.end())
			{
				// StuckAtOne and StuckAtZero
				ptree stuckAtOneNode;
				for (auto p: faultCoverage.stuckAtOne[fullPinName])
				{
					ptree pNode;
					pNode.put("", p);

					stuckAtOneNode.push_back(std::make_pair("", pNode));
				}

				portNode.add_child("port", stuckAtOneNode);
			}

			if (faultCoverage.stuckAtZero.find(fullPinName) != faultCoverage.stuckAtZero.end())
			{
				// StuckAtOne and StuckAtZero
				ptree stuckAtZeroNode;
				for (auto p: faultCoverage.stuckAtZero[fullPinName])
				{
					ptree pNode;
					pNode.put("", p);

					stuckAtZeroNode.push_back(std::make_pair("", pNode));
				}

				portNode.add_child("port", stuckAtZeroNode);
			}

			// Add the connection ids to the port
			ptree connections;
			for (auto connection: port.GetConnections()) {
				if (connection == nullptr) {continue; }

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
				if (connection == nullptr) {continue; }

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

	string JsonExporter::GetFullPinName(const GroupMetaData* group, string pin)
	{
		while (group != nullptr)
		{
			pin = group->GetName() + "/" + pin;
			group = group->GetParent();
		}
		return pin;
	}

	string JsonExporter::GetWireName(string wire)
	{
		vector<string> acc;
		boost::split(acc, wire, boost::is_any_of("/"));
		return acc.back();
	}

};
};
};
