#include "Applications/Mixins/Vcd/VcdExportMixin.hpp"

#include <boost/format.hpp>

#include <string>
#include <utility>
#include <limits>

#include "Basic/Settings.hpp"
#include "Basic/StaticAssert.hpp"
#include "Basic/Statistic/Sampling.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/CircuitVerilogExporter/CircuitVerilogExporter.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

template<typename FaultList>
VcdExportMixin<FaultList>::VcdExportMixin(std::string configPrefix):
	exportId(0u),
	vcdModelBuilder(),
	vcdExport(VcdExport::Disabled),
	vcdExportDirectory("[DataExportDirectory]/vcds"),
	vcdExportSamples(0u),
	vcdExportCircuitPath("[DataExportDirectory]/vcds/circuit.v"),
	vcdConfigPrefix(configPrefix)
{
}

template<typename FaultList>
VcdExportMixin<FaultList>::~VcdExportMixin(void) = default;

template<typename FaultList>
void VcdExportMixin<FaultList>::Init(void)
{
	vcdModelBuilder.Initialize(*this->circuit);
}

template<typename FaultList>
void VcdExportMixin<FaultList>::Run(void)
{
}

template<typename FaultList>
void VcdExportMixin<FaultList>::PreRun(void)
{
	ExportCircuitForVisualisation();
}

template<typename FaultList>
void VcdExportMixin<FaultList>::PostRunVcd(const FaultList& faultList)
{
	if (!__builtin_expect(vcdExport == VcdExport::Enabled, false))
	{
		return;
	}

	std::vector<bool> faultCoverage;
	faultCoverage.reserve(faultList.size());
	for (size_t index = 0u; index < faultList.size(); ++index)
	{
		auto [fault, metaData] = faultList[index];
		faultCoverage[index] = (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED);
	}
	ExportVcdForFaultCoverage(faultList, faultCoverage);
}

template<typename FaultList>
bool VcdExportMixin<FaultList>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "VcdExport", vcdConfigPrefix))
	{
		return Settings::ParseEnum(value, vcdExport, {
			{ "Disabled", VcdExport::Disabled },
			{ "Enabled", VcdExport::Enabled },
		});
	}
	if (Settings::IsOption(key, "VcdExportDirectory", vcdConfigPrefix))
	{
		vcdExportDirectory = value;
		return true;
	}
	if (Settings::IsOption(key, "VcdExportSamples", vcdConfigPrefix))
	{
		return Settings::ParseSizet(value, vcdExportSamples);
	}
	if (Settings::IsOption(key, "VcdExportCircuitName", vcdConfigPrefix))
	{
		vcdExportCircuitPath = value;
		return true;
	}

	return false;
}

template<typename FaultList>
void VcdExportMixin<FaultList>::ExportCircuitForVisualisation(void) const
{
	if (!__builtin_expect(vcdExport == VcdExport::Enabled, false))
	{
		return;
	}

	if (vcdExportCircuitPath.empty())
	{
		return;
	}

	// Copy settings and modify for VCM
	FileHandle handle(vcdExportCircuitPath, false);
	std::ofstream& verilogCircuitStream = handle.GetOutStream();

	Io::CircuitVerilogExporter exporter;
	exporter.ExportCircuit(*this->circuit, verilogCircuitStream);
}

template<typename FaultList>
void VcdExportMixin<FaultList>::ExportVcdForGoodSimulation(const PatternInfo& pattern, const Simulation::SimulationResult &goodResult) const
{
	if (!__builtin_expect(vcdExport == VcdExport::Enabled, false))
	{
		return;
	}

	VcdInfo info { GetGoodSimulationVcdInfo(*this->circuit, pattern) };
	auto modelGood { vcdModelBuilder.BuildVcdModel(goodResult, *this->circuit, info.header) };
	Io::Vcd::ExportVcd(modelGood, vcdExportDirectory + "/" + info.fileName);
}

template<typename FaultList>
void VcdExportMixin<FaultList>::ExportVcdForBadSimulation(const FaultList& faultList, const PatternInfo& pattern, const FaultInfo& faultInfo, const Simulation::SimulationResult &badResult) const
{
	if (!__builtin_expect(vcdExport == VcdExport::Enabled, false))
	{
		return;
	}
	if (!__builtin_expect(Statistic::IsSampleUsed(faultInfo.faultId, faultList.size(), vcdExportSamples), false))
	{
		return;
	}

	VcdInfo info { GetBadSimulationVcdInfo(*this->circuit, pattern, faultInfo) };
	auto modelBad { vcdModelBuilder.BuildVcdModel(badResult, *this->circuit, info.header) };
	Io::Vcd::ExportVcd(modelBad, vcdExportDirectory + "/" + info.fileName);
}

template<typename FaultList>
void VcdExportMixin<FaultList>::ExportVcdForFaultCoverage(const FaultList& faultList, const std::vector<bool>& coverage) const
{
	if (__builtin_expect(vcdExport != VcdExport::Enabled, false))
	{
		return;
	}

	Simulation::SimulationResult faultCoverage(1u, this->circuit->GetMappedCircuit().GetNumberOfNodes(), Logic::LOGIC_ZERO);
	for (size_t faultIndex = 0u; faultIndex < faultList.size(); ++faultIndex)
	{
		if (coverage[faultIndex])
		{
			auto [fault, metaData] = faultList[faultIndex];

			if constexpr (std::is_same_v<FaultList, Fault::SingleStuckAtFaultList>)
			{
				faultCoverage[0][fault->GetNode()->GetNodeId()] = Logic::LOGIC_ONE;
			}
			else if constexpr (std::is_same_v<FaultList, Fault::SingleTransitionDelayFaultList>)
			{
				faultCoverage[0][fault->GetNode()->GetNodeId()] = Logic::LOGIC_ONE;
			}
			else if constexpr (std::is_same_v<FaultList, Fault::CellAwareFaultList>)
			{
				auto& gatesOfCell = fault->GetCell()->GetSubGroups();

				for (auto& gate : gatesOfCell)
				{
					auto& mappedNodes = gate->GetMappedNodes();
					for (auto& node : mappedNodes)
					{
						faultCoverage[0][node->GetNodeId()] = Logic::LOGIC_ONE;
					}
				}
			}
			else
			{
				NotImplemented<FaultList>();
			}
		}
	}

	VcdInfo info { GetFaultCoverageVcdInfo(*this->circuit) };
	auto modelCoverage { vcdModelBuilder.BuildVcdModel(faultCoverage, *this->circuit, info.header) };
	Io::Vcd::ExportVcd(modelCoverage, vcdExportDirectory + "/" + info.fileName);
}

template<typename FaultList>
void VcdExportMixin<FaultList>::ExportVcdForPatternFaultCoverage(const FaultList& faultList, const std::vector<bool>& coverage, const PatternInfo& pattern) const
{
	if (__builtin_expect(vcdExport != VcdExport::Enabled, false))
	{
		return;
	}

	Simulation::SimulationResult faultCoverage(1u, this->circuit->GetMappedCircuit().GetNumberOfNodes(), Logic::LOGIC_ZERO);
	for (size_t faultIndex = 0u; faultIndex < faultList.size(); ++faultIndex)
	{
		if (coverage[faultIndex])
		{
			auto [fault, metaData] = faultList[faultIndex];

			if constexpr (std::is_same_v<FaultList, Fault::SingleStuckAtFaultList>)
			{
				faultCoverage[0][fault->GetNode()->GetNodeId()] = Logic::LOGIC_ONE;
			}
			else if constexpr (std::is_same_v<FaultList, Fault::SingleTransitionDelayFaultList>)
			{
				faultCoverage[0][fault->GetNode()->GetNodeId()] = Logic::LOGIC_ONE;
			}
			else if constexpr (std::is_same_v<FaultList, Fault::CellAwareFaultList>)
			{
				auto& gatesOfCell = fault->GetCell()->GetSubGroups();
				for (auto& gate : gatesOfCell)
				{
					auto& mappedNodes = gate->GetMappedNodes();
					for (auto& node : mappedNodes)
					{
						faultCoverage[0][node->GetNodeId()] = Logic::LOGIC_ONE;
					}
				}
			}
			else
			{
				NotImplemented<FaultList>();
			}
		}
	}

	VcdInfo info { GetPatternFaultCoverageVcdInfo(*this->circuit, pattern) };
	auto modelCoverage { vcdModelBuilder.BuildVcdModel(faultCoverage, *this->circuit, info.header) };
	Io::Vcd::ExportVcd(modelCoverage, vcdExportDirectory + "/" + info.fileName);
}

template<typename FaultList>
typename VcdExportMixin<FaultList>::VcdInfo VcdExportMixin<FaultList>::GetFaultCoverageVcdInfo(const CircuitEnvironment& circuitEnvironment) const
{
	std::string fileName = "00_faultCoverage.vcd";
	std::string header = "Fault Coverage!";

	return { std::numeric_limits<size_t>::max(), fileName, header };
}

template<typename FaultList>
typename VcdExportMixin<FaultList>::VcdInfo VcdExportMixin<FaultList>::GetPatternFaultCoverageVcdInfo(const CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern) const
{
	std::string fileName = std::to_string(pattern.pattern.GetNumberOfTimeframes()) + "TF-" + std::to_string(pattern.patternId) + "-faultCoverage.vcd";
	std::string header = "Pattern Fault Coverage!";

	return { std::numeric_limits<size_t>::max(), fileName, header };
}

template<typename FaultList>
typename VcdExportMixin<FaultList>::VcdInfo VcdExportMixin<FaultList>::GetGoodSimulationVcdInfo(const CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern) const
{
	std::string fileName = boost::str(boost::format("%d-faultFree.vcd") % pattern.patternId);
	std::string header = "Generated by the FreiTest project! @University Freiburg";

	return { std::numeric_limits<size_t>::max(), fileName, header };
}

template<typename FaultList>
typename VcdExportMixin<FaultList>::VcdInfo VcdExportMixin<FaultList>::GetBadSimulationVcdInfo(const CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern, const FaultInfo& faultInfo) const
{
	const auto& metaData = circuitEnvironment.GetMetaData();
	const auto& fault = faultInfo.fault;

	const size_t vcdId = exportId.fetch_add(1);

	if constexpr (std::is_same_v<FaultList, Fault::SingleStuckAtFaultList>)
	{
		auto const stuckAt = fault.GetStuckAt();
		auto const faultName = to_string(to_logic(stuckAt.GetType()));
		auto const connectionId = stuckAt.GetNode()->GetPortConnectionId(stuckAt.GetPort());

		auto const* group = metaData.GetGroup(stuckAt.GetNode());
		auto const& groupInfo = group->GetSourceInfo();
		auto const* cell = group->GetParent();
		auto const& cellInfo = cell->GetSourceInfo();
		ASSERT(cellInfo.template GetProperty<bool>("module-is-cell").value_or(false)) << "Stuck-At fault is at unknown location";

		bool portIsTopLevel = (groupInfo.template GetProperty<std::string>("primitive-category").value_or("gate") == "port");
		if (portIsTopLevel)
		{
			// The parent module (the circuit top level module) contains the correct port names.
			cell = cell->GetParent();
		}

		bool connectedToPort = false;
		std::string portName;
		for (auto port : cell->GetPorts())
		{
			for (const auto index : port.GetSize().GetIndicesTopToBottom())
			{
				const auto* connection = port.GetConnectionForIndex(index);
				if (connection == nullptr)
				{
					continue;
				}

				if (connection->GetConnectionId() == connectionId)
				{
					connectedToPort = true;
					portName = (port.GetSize().GetSize() == 1u)
						? port.GetHierarchyName()
						: boost::str(boost::format("%s [%d]") % port.GetHierarchyName() % index);
					break;
				}
			}
		}
		ASSERT(connectedToPort) << "Stuck-At fault is not at cell port.";

		std::string header = boost::str(boost::format("%s-SA%s") % portName % faultName);
		std::string file = boost::str(boost::format("%d-SA%s-%s-%d.vcd") % pattern.patternId % faultName % portName % vcdId);

		size_t pos;
		while ((pos = file.find("[")) != std::string::npos) { file.replace(pos, 1, ""); }
		while ((pos = file.find("]")) != std::string::npos) { file.replace(pos, 1, ""); }
		while ((pos = file.find("/")) != std::string::npos) { file.replace(pos, 1, "'"); }

		DVLOG(3) << "Filename: " << file;
		DVLOG(3) << "Header: " << header;

		return { vcdId, file, header };
	}
	else if constexpr (std::is_same_v<FaultList, Fault::SingleTransitionDelayFaultList>)
	{
		auto const transitionDelay = fault.GetTransitionDelay();
		auto const faultName = to_string(transitionDelay.GetType());
		auto const connectionId = transitionDelay.GetNode()->GetPortConnectionId(transitionDelay.GetPort());

		auto const* group = metaData.GetGroup(transitionDelay.GetNode());
		auto const& groupInfo = group->GetSourceInfo();
		auto const* cell = group->GetParent();
		auto const& cellInfo = cell->GetSourceInfo();
		ASSERT(cellInfo.template GetProperty<bool>("module-is-cell").value_or(false)) << "Transition delay fault is at unknown location";

		bool portIsTopLevel = (groupInfo.template GetProperty<std::string>("primitive-category").value_or("gate") == "port");
		if (portIsTopLevel)
		{
			// The parent module (the circuit top level module) contains the correct port names.
			cell = cell->GetParent();
		}

		bool connectedToPort = false;
		std::string portName;
		for (auto port : cell->GetPorts())
		{
			for (const auto index : port.GetSize().GetIndicesTopToBottom())
			{
				const auto* connection = port.GetConnectionForIndex(index);
				if (connection == nullptr)
				{
					continue;
				}

				if (connection->GetConnectionId() == connectionId)
				{
					connectedToPort = true;
					portName = (port.GetSize().GetSize() == 1u)
						? port.GetHierarchyName()
						: boost::str(boost::format("%s [%d]") % port.GetHierarchyName() % index);
					break;
				}
			}
		}
		ASSERT(connectedToPort) << "Stuck-At fault is not at cell port.";

		std::string header = boost::str(boost::format("%s-SA%s") % portName % faultName);
		std::string file = boost::str(boost::format("%d-SA%s-%s-%d.vcd") % pattern.patternId % faultName % portName % vcdId);

		size_t pos;
		while ((pos = file.find("[")) != std::string::npos) { file.replace(pos, 1, ""); }
		while ((pos = file.find("]")) != std::string::npos) { file.replace(pos, 1, ""); }
		while ((pos = file.find("/")) != std::string::npos) { file.replace(pos, 1, "'"); }

		DVLOG(3) << "Filename: " << file;
		DVLOG(3) << "Header: " << header;

		return { vcdId, file, header };
	}
	else if constexpr (std::is_same_v<FaultList, Fault::CellAwareFaultList>)
	{
		std::string file;
		std::string header;

		// Get cell information of the cell aware fault
		auto cell = fault.GetCell();

		// If a UDFM entry has different alternatives to test for a fault,
		// it is enough to note some information once
		int numberOfEntries = 0;

		// testAlternative is a User Defined Fault Entry
		for (auto& testAlternative : fault.GetUserDefinedFault()->GetAlternatives())
		{
			auto faultname = fault.GetUserDefinedFault()->GetFaultName();

			// Returns a PortMapType (string, error) of Conditions for UDFM Entry
			std::string conditionForHeader;

			std::string cellHierarchyname = cell->GetHierarchyName();
			for (auto& [portname, portvalue] : testAlternative->GetTestConditions())
			{
				// Get the key of the UDFMs condition map and get the port of the specific cell
				auto port = cell->GetPort(portname);
				conditionForHeader += port->GetHierarchyName() + ":" + to_string(portvalue) + "|";
			}

			// Doing the same (see above) for the effects of a fault
			std::string effectsForHeader;
			for (auto& [portname, portvalue] : testAlternative->GetTestEffects())
			{
				auto port = cell->GetPort(portname);
				effectsForHeader += port->GetHierarchyName() + ":" + to_string(portvalue) +"|";
			}

			// like here: faultname is only needed once
			if (numberOfEntries < 1)
			{
				header += faultname + " " + conditionForHeader + " " + effectsForHeader + ";";
			}
			else
			{
				header += conditionForHeader + " " + effectsForHeader + ";";
			}

			size_t pos;
			while ((pos = cellHierarchyname.find("/")) != std::string::npos)
			{
				cellHierarchyname.replace(pos, 1, "'");
			}

			// here again
			if (numberOfEntries < 1)
			{
				// to match the fault and fault free vcds
				file += std::to_string(pattern.pattern.GetNumberOfTimeframes()) + "TF-" + std::to_string(pattern.patternId)
			+ "-Inst-" + faultname + "_in_" + cellHierarchyname + "cell.vcd";
			}

			numberOfEntries++;
		}
		return {vcdId, file, header};
	}
	else
	{
		NotImplemented<FaultList>();
	}
}

template class VcdExportMixin<Fault::SingleStuckAtFaultList>;
template class VcdExportMixin<Fault::SingleTransitionDelayFaultList>;
template class VcdExportMixin<Fault::CellAwareFaultList>;

};
};
};
