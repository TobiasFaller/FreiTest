#include "Basic/Fault/Lists/CellAwareFaultList.hpp"

#include <numeric>
#include <random>
#include <regex>

#include "Circuit/CircuitEnvironment.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Fault
{

static std::pair<std::vector<CellAwareFault>, std::vector<size_t>> RemapFaults(const std::vector<CellAwareFault>& faultList, const std::vector<size_t>& faultIndices);

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

std::vector<CellAwareFault> GenerateCellAwareFaultList(const Circuit::CircuitEnvironment& circuit, const CellAwareModelSpecification& specification)
{
	const auto& circuitMetaData { circuit.GetMetaData() };

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

		if (!specification.HasCell(type))
		{
			continue;
		}

		// Generate the expanded test pattern
		for (auto& faultSpecification : specification.GetCell(type).GetFaults())
		{
			auto fault { CreateCellAwareFaultForCell(faultSpecification, group) };
			if (!fault.has_value())
			{
				continue;
			}

			faults.push_back(fault.value());
		}
	}

	return faults;
}

std::pair<std::vector<CellAwareFault>, std::vector<size_t>> ReduceCellAwareFaultList(const Circuit::CircuitEnvironment& circuitEnvironment, CellAwareFaultReduction reduction, const std::vector<CellAwareFault>& faultList)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	return RemapFaults(faultList, faultIndices);
}

std::pair<std::vector<CellAwareFault>, std::vector<size_t>> FilterCellAwareFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<CellAwareFault>& faultList)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	// Let an empty pattern always return the original fault list.
	if (pattern == "")
	{
		return { faultList, faultIndices };
	}

	std::regex patternRegex { pattern, std::regex_constants::ECMAScript };
	for (size_t faultIndex { 0 }; faultIndex < faultList.size(); faultIndex++)
	{
		auto const* group { faultList[faultIndex].GetCell() };
		auto const fullName { group->GetHierarchyName() + "/" + faultList[faultIndex].GetName() };
		auto const result { std::regex_match(fullName, patternRegex) };
		if ((type == FilterPatternType::Normal && !result)
			|| (type == FilterPatternType::Inverted && result))
		{
			faultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	return RemapFaults(faultList, faultIndices);
}

std::pair<std::vector<CellAwareFault>, std::vector<size_t>> SampleCellAwareFaultList(const std::vector<CellAwareFault>& faultList, size_t samples)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	if (samples == 0u)
	{
		return { faultList, faultIndices };
	}

	std::vector<size_t> sampledIndices;
	sampledIndices.reserve(samples);

	std::random_device randomDevice {};
	std::mt19937 randomSource { randomDevice() };
	std::sample(faultIndices.begin(), faultIndices.end(), std::back_inserter(sampledIndices), samples, randomSource);

	for (auto& fault : faultIndices)
	{
		fault = REMOVED_FAULT;
	}
	for (auto const& fault : sampledIndices)
	{
		faultIndices[fault] = fault;
	}

	return RemapFaults(faultList, faultIndices);
}

static std::pair<std::vector<CellAwareFault>, std::vector<size_t>> RemapFaults(const std::vector<CellAwareFault>& faultList, const std::vector<size_t>& faultIndices)
{
	ASSERT(faultList.size() == faultIndices.size())
		<< "Faults and indices list sizes have to match.";

	size_t newFaultCount { 0u };
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] == index)
		{
			newFaultCount++;
		}
	}

	std::vector<size_t> newFaultIndices { };
	std::vector<CellAwareFault> newFaultList { };

	// First re-index all the faults that have been kept.
	newFaultList.reserve(newFaultCount);
	newFaultIndices.resize(faultIndices.size(), REMOVED_FAULT);
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] == index)
		{
			newFaultIndices[index] = newFaultList.size();
			newFaultList.emplace_back(faultList[index]);
		}
	}

	// Now re-index all the faults that have been merged or removed.
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] != REMOVED_FAULT && faultIndices[index] != index)
		{
			ASSERT(newFaultIndices[faultIndices[index]] != REMOVED_FAULT);
			newFaultIndices[index] = newFaultIndices[faultIndices[index]];
		}
	}

	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		ASSERT(newFaultIndices[index] == REMOVED_FAULT
				|| newFaultIndices[index] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

CellAwareFaultReduction operator|(CellAwareFaultReduction lhs, CellAwareFaultReduction rhs)
{
	return static_cast<CellAwareFaultReduction>(
		static_cast<std::underlying_type_t<CellAwareFaultReduction>>(lhs)
		| static_cast<std::underlying_type_t<CellAwareFaultReduction>>(rhs)
	);
}

CellAwareFaultReduction operator&(CellAwareFaultReduction lhs, CellAwareFaultReduction rhs)
{
	return static_cast<CellAwareFaultReduction>(
		static_cast<std::underlying_type_t<CellAwareFaultReduction>>(lhs)
		& static_cast<std::underlying_type_t<CellAwareFaultReduction>>(rhs)
	);
}

CellAwareFaultReduction& operator&=(CellAwareFaultReduction& lhs, CellAwareFaultReduction rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

CellAwareFaultReduction& operator|=(CellAwareFaultReduction& lhs, CellAwareFaultReduction rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

};
};
