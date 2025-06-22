#include "Basic/Fault/Lists/CellAwareFaultList.hpp"

#include <numeric>
#include <regex>

#include "Circuit/CircuitEnvironment.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Fault
{

CellAwareMetaData::CellAwareMetaData(void):
	TargetedFaultMetaData(),
	detectingPatternId(std::numeric_limits<size_t>::max()),
	detectingNode({ nullptr, { Circuit::PortType::Output, 0u } }),
	detectingTimeframe(std::numeric_limits<size_t>::max()),
	detectingOutputGood(Logic::LOGIC_INVALID),
	detectingOutputBad(Logic::LOGIC_INVALID)
{
}

CellAwareMetaData::~CellAwareMetaData(void) = default;

CellAwareFaultListDebug::CellAwareFaultListDebug(const CellAwareFaultList &faultList, const Circuit::MappedCircuit& circuit):
	faultList(faultList),
	circuit(circuit)
{
}

CellAwareFaultListDebug::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

std::ostream& operator <<(std::ostream& stream, const CellAwareFaultListDebug& debug)
{
	stream << "FaultList: " << std::endl;
	size_t faultIndex = 0u;
	for (auto const [fault, metadata] : debug.faultList)
	{
		stream << "Fault " << std::to_string(faultIndex) << ": " << to_string(*fault) << std::endl;
		faultIndex++;
	}
	return stream;
}

CellAwareFaultListDebug to_debug(const CellAwareFaultList &faultList, const Circuit::MappedCircuit& circuit)
{
	return CellAwareFaultListDebug { faultList, circuit };
}

std::optional<CellAwareFault> MapUdfmFaultToCell(const Circuit::CircuitEnvironment& circuit, const Circuit::GroupMetaData* cell, const std::shared_ptr<Io::Udfm::UdfmFault>& fault)
{
	const auto& circuitMetaData { circuit.GetMetaData() };

	auto sourceInfo = cell->GetSourceInfo();
	ASSERT(sourceInfo.GetProperty<bool>("module-is-cell").value_or(false)) << "The provided group has to be a cell.";

	auto name = sourceInfo.GetProperty<std::string>("module-name").value_or("");
	auto type = sourceInfo.GetProperty<std::string>("module-type").value_or("");

	// Create port map to assign UDFM conditions and effects later.
	std::map<std::string, Circuit::MappedCircuit::NodeAndPort> portMap;
	for (auto port: cell->GetPorts())
	{
		const auto& portName = port.GetName();
		ASSERT(port.GetConnections().size() == 1)
			<< "Port \"" << portName << "\" in cell \"" << name << "\" has a bus with a size not equal to 1 as input / output port.";

		const auto portType = port.GetSourceInfo().GetProperty<std::string>("port-type").value_or("");
		ASSERT(portType == "input" || portType == "output")
			<< "Port \"" << portName << "\" in cell \"" << name << "\" has invalid type " << portType << ".";

		if (port.GetConnections()[0u] == nullptr)
		{
			continue;
		}

		auto const& ports = (portType == "input")
			? port.GetConnections()[0]->GetMappedSinks()
			: port.GetConnections()[0]->GetMappedSources();
		for (auto& port : ports)
		{
			if (circuitMetaData.GetGroup(port.node)->GetParent() == cell)
			{
				portMap[portName] = port;
			}
		}
	}

	auto ports_to_string = [&](auto& ports) -> std::string {
		std::string result;

		for (auto const& [portName, nodeAndPort] : ports)
		{
			if (!result.empty())
			{
				result += ", ";
			}
			result += portName;
			result += ": ";
			result += nodeAndPort.node->GetName() + " (" + to_string(nodeAndPort.node->GetCellType()) + ") ";
			result += (nodeAndPort.port.portType == Circuit::PortType::Input) ? " Input " : " Output ";
			result += std::to_string(nodeAndPort.port.portNumber);
		}

		return result;
	};

	DVLOG(6) << "Cell " << name << " with type " << type << " (" << ports_to_string(portMap) << ")";

	bool pinMappingWorked = true;
	std::vector<CellAwareAlternative> mappedAlternative;
	for (const auto& udfmAlternative : fault->GetAlternatives())
	{
		std::vector<CellAwarePort> conditions;
		std::vector<CellAwarePort> effects;

		for (const auto& [portName, portValue] : udfmAlternative->GetTestConditions())
		{
			if (__builtin_expect(portMap.find(portName) == portMap.end(), false))
			{
				LOG(WARNING) << "Was not able to find port \"" << portName << "\" in cell \"" << name << "\"! Ignoring it for fautlist generation.";
				pinMappingWorked = false;
				break;
			}

			conditions.push_back({
				portMap[portName], portName,
				udfmAlternative->GetTestConditionConstraints(portName)
			});
		}

		for (const auto& [portName, portValue] : udfmAlternative->GetTestEffects())
		{
			if (__builtin_expect(portMap.find(portName) == portMap.end(), false))
			{
				LOG(WARNING) << "Was not able to find port \"" << portName << "\" in cell \"" << name << "\"! Ignoring it for fautlist generation.";
				pinMappingWorked = false;
				break;
			}

			effects.push_back({
				portMap[portName], portName,
				udfmAlternative->GetTestEffectConstraints(portName)
			});
		}

		if (pinMappingWorked)
		{
			mappedAlternative.push_back({
				.conditions = conditions,
				.effects = effects
			});
		}
	}

	if (__builtin_expect(!pinMappingWorked, false))
	{
		LOG(WARNING) << "Condition pin mapping did not work for fault name \"" << fault->GetFaultName()
			<< "\" for cell \"" << name << "\" of type \"" << type << "\"";
		return std::nullopt;
	}

	return { CellAwareFault(fault, cell, mappedAlternative) };
}

std::vector<CellAwareFault> GenerateCellAwareFaultList(const Circuit::CircuitEnvironment& circuit, const Io::Udfm::UdfmModel& udfm)
{
	const auto& circuitMetaData { circuit.GetMetaData() };
	const auto& udfmCells { udfm.GetCells() };

	// Find cell instances and map ports
	std::vector<CellAwareFault> faults;
	for (auto const& group: circuitMetaData.GetGroups())
	{
		auto sourceInfo = group->GetSourceInfo();
		if (!sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
		{
			continue;
		}

		auto name = sourceInfo.GetProperty<std::string>("module-name").value_or("");
		auto type = sourceInfo.GetProperty<std::string>("module-type").value_or("");

		auto it = udfmCells.find(type);
		if (it == udfmCells.end())
		{
			continue;
		}

		// Generate the expanded test pattern
		auto& udfmEntry = it->second;
		for (auto& [faultName, udfmFault] : udfmEntry->GetFaults())
		{
			auto fault { MapUdfmFaultToCell(circuit, group, udfmFault) };
			if (!fault.has_value())
			{
				continue;
			}

			faults.push_back(fault.value());
		}
	}

	return faults;
}

std::pair<std::vector<CellAwareFault>, std::vector<size_t>> FilterCellAwareFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<CellAwareFault>& faultList)
{
	std::vector<bool> faultMergedOrRemoved(faultList.size(), false);
	std::vector<size_t> newFaultIndices(faultList.size());
	std::iota(newFaultIndices.begin(), newFaultIndices.end(), 0u);

	// Let an empty pattern always return the original fault list.
	if (pattern == "")
	{
		return { faultList, newFaultIndices };
	}

	std::regex patternRegex { pattern, std::regex_constants::ECMAScript };
	for (size_t faultIndex { 0 }; faultIndex < faultList.size(); faultIndex++)
	{
		auto const* group { faultList[faultIndex].GetCell() };
		auto const result { std::regex_match(group->GetHierarchyName(), patternRegex) };
		if ((type == FilterPatternType::Normal && !result)
			|| (type == FilterPatternType::Inverted && result))
		{
			faultMergedOrRemoved[faultIndex] = true;
			newFaultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	// Construct the new fault list by copying the untouched elements.
	std::vector<CellAwareFault> newFaultList;
	newFaultList.reserve(std::count_if(faultMergedOrRemoved.begin(), faultMergedOrRemoved.end(), [](bool faultMergedOrRemoved) {
		return !faultMergedOrRemoved;
	}));

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		if (!faultMergedOrRemoved[faultIndex])
		{
			// Update the fault index to the new location.
			newFaultIndices[faultIndex] = newFaultList.size();
			newFaultList.emplace_back(faultList[faultIndex]);
		}
		else if (newFaultIndices[faultIndex] != REMOVED_FAULT)
		{
			// Update the merged index to the new location.
			// This update method assumes that the referenced index has already
			// been resolved by the update above.
			newFaultIndices[faultIndex] = newFaultIndices[newFaultIndices[faultIndex]];
		}
	}

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		ASSERT(newFaultIndices[faultIndex] == REMOVED_FAULT
				|| newFaultIndices[faultIndex] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

};
};
