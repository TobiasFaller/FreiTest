#include "Basic/Fault/Specification/CellAwareSpecification.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>

namespace FreiTest
{
namespace Fault
{

static std::string to_string(const std::vector<CellAwareFaultSpecification::Port>& ports);

CellAwareCellSpecification::CellAwareCellSpecification(std::string name, std::vector<CellAwareFaultSpecification> faults):
	name(name),
	faults(faults.begin(), faults.end())
{
}

bool CellAwareCellSpecification::HasFault(const std::string& name) const
{
	return faults.get<name_tag>().find(name) != faults.get<name_tag>().end();
}

const CellAwareFaultSpecification& CellAwareCellSpecification::GetFault(const std::string& name) const
{
	return *faults.get<name_tag>().find(name);
}

CellAwareModelSpecification::CellAwareModelSpecification(std::vector<CellAwareCellSpecification> cells):
	cells(cells.begin(), cells.end())
{
}

bool CellAwareModelSpecification::HasCell(const std::string& name) const
{
	return cells.get<name_tag>().find(name) != cells.get<name_tag>().end();
}

const CellAwareCellSpecification& CellAwareModelSpecification::GetCell(const std::string& name) const
{
	return *cells.get<name_tag>().find(name);
}

bool CellAwareModelSpecification::HasFault(const std::string& cellName, const std::string& faultName) const
{
	if (auto it = cells.get<name_tag>().find(cellName);
		it != cells.get<name_tag>().end())
	{
		return it->HasFault(faultName);
	}

	return false;
}

const CellAwareFaultSpecification& CellAwareModelSpecification::GetFault(const std::string& cellName, const std::string& faultName) const
{
	return cells.get<name_tag>().find(cellName)->GetFault(faultName);
}

std::string to_string(const CellAwareFaultSpecification& specification)
{
	std::string result { boost::str(boost::format("CA specification %s:") % specification.GetName()) };
	for (size_t stateIndex { 0u }; stateIndex < specification.GetStates().size(); stateIndex++)
	{
		auto& state = specification.GetStates()[stateIndex];
		result += boost::str(boost::format("\n    State %d") % stateIndex);

		for (size_t transIndex { 0u }; transIndex < state.transitions.size(); transIndex++)
		{
			auto& transition = state.transitions[transIndex];
			result += "\n         to " + std::to_string(transition.toState);
			result += " (" + to_string(transition.inputs) + " -> " + to_string(transition.outputs) + ")";
			result += ", " + std::string(transition.accepting ? "accepting" : "not accepting");
		}
	}
	return result;
}

std::string to_string(const CellAwareCellSpecification& specification)
{
	std::string result = boost::str(boost::format("Cell %s:") % specification.GetName());
	for (auto const& fault : specification)
	{
		result += "\n" + to_string(fault);
	}
	return result;
}

std::string to_string(const CellAwareModelSpecification& specification)
{
	std::string result;
	for (auto const& cell : specification)
	{
		if (!result.empty()) result += "\n";
		result += to_string(cell);
	}
	return result;
}

static std::string to_string(const std::vector<CellAwareFaultSpecification::Port>& ports)
{
	std::string result;
	for (auto& port : ports)
	{
		if (result.size() != 0) result += ", ";
		result += port.name + "=" + to_string(port.constraint);
	}
	return result;
}

};
};
