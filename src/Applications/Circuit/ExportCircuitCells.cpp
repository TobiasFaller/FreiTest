#include "Applications/Circuit/ExportCircuitCells.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>

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

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitCells::ExportCircuitCells(void) = default;
ExportCircuitCells::~ExportCircuitCells(void) = default;

void ExportCircuitCells::Init(void)
{
}

bool ExportCircuitCells::SetSetting(std::string key, std::string value)
{
	return false;
}

void ExportCircuitCells::Run(void)
{
	const auto circuitName { this->circuit->GetName() };
	const auto& metaData { this->circuit->GetMetaData() };

	FileHandle handle { "[DataExportDirectory]/cells.txt", false };
	auto& out { handle.GetOutStream() };

	for (auto const* group : metaData.GetGroups())
	{
		const auto& sourceInfo { group->GetSourceInfo() };
		if (!sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
		{
			continue;
		}

		const auto cellType { sourceInfo.GetProperty<std::string>("module-type").value_or("NONE") };
		if (cellType == "input" || cellType == "output")
		{
			continue;
		}

		out << group->GetHierarchyName() << " " << cellType << std::endl;
	}
}

};
};
};
