#include "Applications/BaseApplication.hpp"

#include <cassert>
#include <iostream>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"

#include "Applications/Utility/GateLogicOptimization.hpp"
#include "Applications/Circuit/ExportCircuitNodes.hpp"
#include "Applications/Circuit/ExportCircuitHierarchy.hpp"
#include "Applications/Circuit/ExportCircuitStructuralDependence.hpp"
#include "Applications/Circuit/ExportCircuitLogicalDependence.hpp"
#include "Applications/Circuit/ExportCircuitPorts.hpp"
#include "Applications/Circuit/ExportCircuitVerilog.hpp"
#include "Applications/Circuit/ExportCircuitCells.hpp"
#include "Applications/Circuit/ExportCircuitCnf.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/SatFullScanAtpg.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/SatFullScanFuzzing.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/SatSequentialAtpg.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/BmcSequentialAtpg.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/BmcSequentialFuzzing.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/LfsrAtpg.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/RandomAtpg.hpp"
#include "Applications/Scale4Edge/TestPatternExport/TestPatternsToVcd.hpp"
#include "Applications/Scale4Edge/TestPatternExport/TestPatternsToStatistics.hpp"
#include "Applications/Scale4Edge/FaultCompaction/GreedyStaticFaultCompaction.hpp"
#include "Applications/Scale4Edge/FaultCompaction/SatStaticFaultCompaction.hpp"
#include "Applications/Scale4Edge/FaultCoverageExport/FaultCoverageExport.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"


using namespace std;

namespace FreiTest
{
namespace Application
{

BaseApplication::BaseApplication() = default;
BaseApplication::~BaseApplication() = default;

void BaseApplication::PreInit(void)
{
}

void BaseApplication::PostInit(void)
{
}

void BaseApplication::PreRun(void)
{
}

void BaseApplication::PostRun(void)
{
}

bool BaseApplication::SetSetting(string key, string value)
{
	return false;
}

Basic::ApplicationStatistics BaseApplication::GetStatistics(void)
{
	return {};
}

unique_ptr<BaseApplication> BaseApplication::Create(std::string application)
{
	if (application.empty())
	{
		LOG(FATAL) << "No application has been specified!";
		return {};
	}

	if (application == "None")
	{
		LOG(WARNING) << "Explicitly creating no application! Probably setting this configuration option was forgotten.";
		return {};
	}

	if (application == "UTILITY_GATE_LOGIC_OPTIMIZATION")
		return std::make_unique<FreiTest::Application::Utility::GateLogicOptimization>();

	if (application == "CIRCUIT_EXPORT_NODES")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitNodes>();
	if (application == "CIRCUIT_EXPORT_HIERARCHY")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitHierarchy>();
	if (application == "CIRCUIT_EXPORT_PORTS")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitPorts>();
	if (application == "CIRCUIT_EXPORT_VERILOG")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitVerilog>();
	if (application == "CIRCUIT_EXPORT_CELLS")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitCells>();
	if (application == "CIRCUIT_EXPORT_STRUCTURAL_DEPENDENCE")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitStructuralDependence>();
	if (application == "CIRCUIT_EXPORT_LOGICAL_DEPENDENCE")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitLogicalDependence>();
	if (application == "CIRCUIT_EXPORT_CNF")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitCnf>();

	if (application == "SCALE4EDGE_SAT_FULLSCAN_STUCK_AT_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_SAT_SEQUENTIAL_STUCK_AT_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatSequentialAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_STUCK_AT_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_RANDOM_STUCK_AT_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::RandomAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_LFSR_STUCK_AT_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::LfsrAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_VCD_STUCK_AT")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToVcd<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_STATISTICS_STUCK_AT")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToStatistics<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_SAT_FULLSCAN_STUCK_AT_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanFuzzing<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_STUCK_AT_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialFuzzing<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_GREEDY_STATIC_FAULT_COMPACTION_STUCK_AT")
		return std::make_unique<FreiTest::Application::Scale4Edge::GreedyStaticFaultCompaction<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_SAT_STATIC_FAULT_COMPACTION_STUCK_AT")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatStaticFaultCompaction<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();
	if (application == "SCALE4EDGE_FAULT_COVERAGE_EXPORT_STUCK_AT")
		return std::make_unique<FreiTest::Application::Scale4Edge::FaultCoverageExport<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>>();

	if (application == "SCALE4EDGE_SAT_FULLSCAN_TRANSITION_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_SAT_SEQUENTIAL_TRANSITION_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatSequentialAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_TRANSITION_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_RANDOM_TRANSITION_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::RandomAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_LFSR_TRANSITION_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::LfsrAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_VCD_TRANSITION")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToVcd<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_STATISTICS_TRANSITION")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToStatistics<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_SAT_FULLSCAN_TRANSITION_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanFuzzing<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_TRANSITION_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialFuzzing<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_GREEDY_STATIC_FAULT_COMPACTION_TRANSITION")
		return std::make_unique<FreiTest::Application::Scale4Edge::GreedyStaticFaultCompaction<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_SAT_STATIC_FAULT_COMPACTION_TRANSITION")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatStaticFaultCompaction<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();
	if (application == "SCALE4EDGE_FAULT_COVERAGE_EXPORT_TRANSITION")
		return std::make_unique<FreiTest::Application::Scale4Edge::FaultCoverageExport<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>>();

	if (application == "SCALE4EDGE_SAT_FULLSCAN_CELL_AWARE_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_SAT_SEQUENTIAL_CELL_AWARE_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatSequentialAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_CELL_AWARE_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_RANDOM_CELL_AWARE_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::RandomAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_LFSR_CELL_AWARE_ATPG")
		return std::make_unique<FreiTest::Application::Scale4Edge::LfsrAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_VCD_CELL_AWARE")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToVcd<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_PATTERNS_TO_STATISTICS_CELL_AWARE")
		return std::make_unique<FreiTest::Application::Scale4Edge::TestPatternsToStatistics<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_SAT_FULLSCAN_CELL_AWARE_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatFullScanFuzzing<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_BMC_SEQUENTIAL_CELL_AWARE_FUZZ")
		return std::make_unique<FreiTest::Application::Scale4Edge::BmcSequentialFuzzing<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_GREEDY_STATIC_FAULT_COMPACTION_CELL_AWARE")
		return std::make_unique<FreiTest::Application::Scale4Edge::GreedyStaticFaultCompaction<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_SAT_STATIC_FAULT_COMPACTION_CELL_AWARE")
		return std::make_unique<FreiTest::Application::Scale4Edge::SatStaticFaultCompaction<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();
	if (application == "SCALE4EDGE_FAULT_COVERAGE_EXPORT_CELL_AWARE")
		return std::make_unique<FreiTest::Application::Scale4Edge::FaultCoverageExport<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>>();


	LOG(FATAL) << "The application " << application << " was not found!";
	return {};
}

};
};
