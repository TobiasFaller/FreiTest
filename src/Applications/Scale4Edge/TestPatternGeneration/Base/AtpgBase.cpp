#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <atomic>
#include <cstdint>
#include <cstring>
#include <execution>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>
#include <random>
#include <thread>
#include <utility>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPatternList.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/DriverFinder.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Io/StilExporter/StilExporter.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"
#include "Io/VcdExporter/VcdExporter.hpp"
#include "Io/VcdExporter/VcdModel.hpp"
#include "Io/UserDefinedFaultModel/UdfmParser.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace FreiTest::Fault;
using namespace FreiTest::Simulation;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename PinData> struct AtpgConfig { };
template<typename PinData> struct AtpgConfig<Fault::SingleStuckAtFaultModel, PinData> {
	using FaultGenerator = Tpg::StuckAtFaultEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitization = Tpg::StuckAtSensitizationConstraintEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitizationTimeframe = Tpg::StuckAtSensitizationTimeframe;
	using FaultTagger = Tpg::StuckAtFaultCircuitTagger<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultTaggerMode = Tpg::StuckAtFaultTaggingMode;
	static size_t GetRequiredTimeframeCount(const Fault::SingleStuckAtFaultModel& faultModel) { return 1u; }
};
template<typename PinData> struct AtpgConfig<Fault::SingleTransitionDelayFaultModel, PinData> {
	using FaultGenerator = Tpg::TransitionDelayFaultEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitization = Tpg::TransitionSensitizationConstraintEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitizationTimeframe = Tpg::TransitionSensitizationTimeframe;
	using FaultTagger = Tpg::TransitionDelayFaultCircuitTagger<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultTaggerMode = Tpg::TransitionDelayFaultTaggingMode;
	static size_t GetRequiredTimeframeCount(const Fault::SingleTransitionDelayFaultModel& faultModel) { return 2u; }
};
template<typename PinData> struct AtpgConfig<Fault::CellAwareFaultModel, PinData> {
	using FaultGenerator = Tpg::CellAwareFaultEncoder<PinData>;
	using FaultSensitization = Tpg::CellAwareSensitizationConstraintEncoder<PinData>;
	using FaultSensitizationTimeframe = Tpg::CellAwareSensitizationTimeframe;
	using FaultTagger = Tpg::CellAwareFaultCircuitTagger<PinData>;
	using FaultTaggerMode = Tpg::CellAwareFaultTaggingMode;
	static size_t GetRequiredTimeframeCount(const Fault::CellAwareFaultModel& faultModel) { return faultModel.GetFault()->GetTimeframeSpread(); }
};

AtpgData<SingleStuckAtFaultModel>::AtpgData(std::string configPrefix):
	faultListReduction(FaultListReduction::RemoveEquivalent),
	configPrefix(configPrefix)
{
}
AtpgData<SingleStuckAtFaultModel>::~AtpgData(void) = default;

bool AtpgData<SingleStuckAtFaultModel>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "FaultListReduction", configPrefix))
	{
		return Settings::ParseEnum(value, faultListReduction, {
			{ "Original", FaultListReduction::Original },
			{ "RemoveEquivalent", FaultListReduction::RemoveEquivalent },
		});
	}

	return false;
}

void AtpgData<SingleStuckAtFaultModel>::Init(void)
{
}

void AtpgData<SingleStuckAtFaultModel>::Run(void)
{
}

AtpgData<SingleTransitionDelayFaultModel>::AtpgData(std::string configPrefix):
	faultListReduction(FaultListReduction::RemoveEquivalent),
	configPrefix(configPrefix)
{
}
AtpgData<SingleTransitionDelayFaultModel>::~AtpgData(void) = default;

bool AtpgData<SingleTransitionDelayFaultModel>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "FaultListReduction", configPrefix))
	{
		return Settings::ParseEnum(value, faultListReduction, {
			{ "Original", FaultListReduction::Original },
			{ "RemoveEquivalent", FaultListReduction::RemoveEquivalent },
		});
	}

	return false;
}

void AtpgData<SingleTransitionDelayFaultModel>::Init(void)
{
}

void AtpgData<SingleTransitionDelayFaultModel>::Run(void)
{
}

AtpgData<CellAwareFaultModel>::AtpgData(std::string configPrefix):
	Mixin::UdfmMixin(configPrefix)
{
}
AtpgData<CellAwareFaultModel>::~AtpgData(void) = default;

bool AtpgData<CellAwareFaultModel>::SetSetting(std::string key, std::string value)
{
	return UdfmMixin::SetSetting(key, value);
}

void AtpgData<CellAwareFaultModel>::Init(void)
{
	UdfmMixin::Init();
}

void AtpgData<CellAwareFaultModel>::Run(void)
{
	UdfmMixin::Run();
}

template <typename FaultModel, typename FaultList>
AtpgBase<FaultModel, FaultList>::AtpgBase(std::string configPrefix):
	Mixin::StatisticsMixin(configPrefix),
	Mixin::FaultStatisticsMixin<FaultList>(configPrefix),
	Mixin::SimulationStatisticsMixin(configPrefix),
	Mixin::SolverStatisticsMixin(configPrefix),
	Mixin::VcdExportMixin<FaultList>(configPrefix),
	Mixin::VcmMixin(configPrefix),
	AtpgData<FaultModel>(configPrefix),
	faultListSource(FaultListSource::FreiTest),
	faultListFile("NOT_SPECIFIED"),
	faultList(),
	fullFaultList(),
	faultMapping(),
	faultListFilter(".*"),
	faultListExclude(""),
	testPatterns(),
	statPatternsGenerated(),
	printFaultListReport(PrintFaultListReport::PrintSummary),
	faultSimulation(FaultSimulation::Enabled),
	testPatternExport(TestPatternExport::Enabled),
	printPatternReport(PrintTestPatternReport::PrintSummary),
	simulateAllFaults(SimulateAllFaults::Disabled),
	checkSimulation(CheckSimulation::Disabled),
	checkSimulationInitialState(CheckSimulationInitialState::Disabled),
	checkSimulationInputs(CheckSimulationInputs::Disabled),
	checkSimulationFlipFlops(CheckSimulationFlipFlops::Disabled),
	checkAtpgResult(CheckAtpgResult::Disabled),
	checkMaxIterationCovered(CheckMaxIterationCovered::Disabled),
	incrementalSimulation(IncrementalSimulation::Enabled),
	patternGenerationThreadLimit(0u),
	solverThreadLimit(1u),
	solverTimeout(10u * 60u),
	solverUntestabilityTimeout(3u * 60u),
	simulationThreadLimit(0u),
	faultListBegin(0u),
	faultListEnd(std::numeric_limits<size_t>::max()),
	parallelMutex(),
	vcdDebugExportId(0u),
	configPrefix(configPrefix)
{
	statPatternsGenerated.SetCollectValues(true);
}

template <typename FaultModel, typename FaultList>
AtpgBase<FaultModel, FaultList>::~AtpgBase(void) = default;

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::Init(void)
{
	AtpgData<FaultModel>::Init();
	Mixin::StatisticsMixin::Init();
	Mixin::FaultStatisticsMixin<FaultList>::Init();
	Mixin::SimulationStatisticsMixin::Init();
	Mixin::SolverStatisticsMixin::Init();
	Mixin::VcdExportMixin<FaultList>::Init();
	Mixin::VcmMixin::Init();
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::Run(void)
{
	Parallel::SetThreads(Parallel::Arena::General, 0u);
	Parallel::SetThreads(Parallel::Arena::PatternGeneration, patternGenerationThreadLimit);
	Parallel::SetThreads(Parallel::Arena::FaultSimulation, simulationThreadLimit);

	AtpgData<FaultModel>::Run();
	Mixin::StatisticsMixin::Run();
	Mixin::FaultStatisticsMixin<FaultList>::Run();
	Mixin::SimulationStatisticsMixin::Run();
	Mixin::SolverStatisticsMixin::Run();
	Mixin::VcdExportMixin<FaultList>::Run();
	Mixin::VcmMixin::Run();
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::PreRun(void)
{
	Mixin::StatisticsMixin::PreRun();
	Mixin::FaultStatisticsMixin<FaultList>::PreRun();
	Mixin::SimulationStatisticsMixin::PreRun();
	Mixin::SolverStatisticsMixin::PreRun();
	Mixin::VcdExportMixin<FaultList>::PreRun();
	Mixin::VcmMixin::PreRun();
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::PostRun(void)
{
	Mixin::StatisticsMixin::PostRun();
	Mixin::FaultStatisticsMixin<FaultList>::PostRun();
	Mixin::SimulationStatisticsMixin::PostRun();
	Mixin::SolverStatisticsMixin::PostRun();
	Mixin::VcdExportMixin<FaultList>::PostRunVcd(faultList);
	Mixin::VcmMixin::PostRun();
}

template <typename FaultModel, typename FaultList>
bool AtpgBase<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "FaultListSource", configPrefix))
	{
		return Settings::ParseEnum(value, faultListSource, {
			{ "FreiTest", FaultListSource::FreiTest },
			{ "File", FaultListSource::File },
		});
	}
	if (Settings::IsOption(key, "FaultListFile", configPrefix))
	{
		faultListFile = value;
		return true;
	}
	if (Settings::IsOption(key, "FaultSimulation", configPrefix))
	{
		return Settings::ParseEnum(value, faultSimulation, {
			{ "Disabled", FaultSimulation::Disabled },
			{ "Enabled", FaultSimulation::Enabled },
		});
	}
	if (Settings::IsOption(key, "TestPatternExport", configPrefix))
	{
		return Settings::ParseEnum(value, testPatternExport, {
			{ "Disabled", TestPatternExport::Disabled },
			{ "Enabled", TestPatternExport::Enabled },
		});
	}
	if (Settings::IsOption(key, "PatternGenerationThreadLimit", configPrefix))
	{
		return Settings::ParseSizet(value, patternGenerationThreadLimit);
	}
	if (Settings::IsOption(key, "SolverThreadLimit", configPrefix))
	{
		return Settings::ParseSizet(value, solverThreadLimit);
	}
	if (Settings::IsOption(key, "SolverTimeout", configPrefix))
	{
		return Settings::ParseSizet(value, solverTimeout);
	}
	if (Settings::IsOption(key, "SolverTimeoutUntestability", configPrefix))
	{
		return Settings::ParseSizet(value, solverUntestabilityTimeout);
	}
	if (Settings::IsOption(key, "SimulationThreadLimit", configPrefix))
	{
		return Settings::ParseSizet(value, simulationThreadLimit);
	}
	if (Settings::IsOption(key, "FaultStartIndex", configPrefix))
	{
		return Settings::ParseSizet(value, faultListBegin);
	}
	if (Settings::IsOption(key, "FaultEndIndex", configPrefix))
	{
		if (Settings::ParseSizet(value, faultListEnd))
		{
			faultListEnd++;
			return true;
		}
		else
		{
			return false;
		}
	}
	if (Settings::IsOption(key, "FaultListFilter", configPrefix))
	{
		faultListFilter = value;
		return true;
	}
	if (Settings::IsOption(key, "FaultListExclude", configPrefix))
	{
		faultListExclude = value;
		return true;
	}
	if (Settings::IsOption(key, "SimulateAllFaults", configPrefix))
	{
		return Settings::ParseEnum(value, simulateAllFaults, {
			{ "Disabled", SimulateAllFaults::Disabled },
			{ "Enabled", SimulateAllFaults::Enabled },
		});
	}
	if (Settings::IsOption(key, "CheckSimulation", configPrefix))
	{
		return Settings::ParseEnum(value, checkSimulation, {
			{ "Disabled", CheckSimulation::Disabled },
			{ "Enabled", CheckSimulation::Enabled },
		});
	}
	if (Settings::IsOption(key, "CheckSimulationInitialState", configPrefix))
	{
		return Settings::ParseEnum(value, checkSimulationInitialState, {
			{ "Disabled", CheckSimulationInitialState::Disabled },
			{ "CheckEqual", CheckSimulationInitialState::CheckEqual },
		});
	}
	if (Settings::IsOption(key, "CheckSimulationInputs", configPrefix))
	{
		return Settings::ParseEnum(value, checkSimulationInputs, {
			{ "Disabled", CheckSimulationInputs::Disabled },
			{ "CheckEqual", CheckSimulationInputs::CheckEqual },
		});
	}
	if (Settings::IsOption(key, "CheckSimulationFlipFlops", configPrefix))
	{
		return Settings::ParseEnum(value, checkSimulationFlipFlops, {
			{ "Disabled", CheckSimulationFlipFlops::Disabled },
			{ "CheckEqual", CheckSimulationFlipFlops::CheckEqual },
		});
	}
	if (Settings::IsOption(key, "CheckAtpgResult", configPrefix))
	{
		return Settings::ParseEnum(value, checkAtpgResult, {
			{ "Disabled", CheckAtpgResult::Disabled },
			{ "CheckEqual", CheckAtpgResult::CheckEqual },
			{ "CheckInitial", CheckAtpgResult::CheckInitial },
		});
	}
	if (Settings::IsOption(key, "CheckMaxIterationCovered", configPrefix))
	{
		return Settings::ParseEnum(value, checkMaxIterationCovered, {
			{ "Disabled", CheckMaxIterationCovered::Disabled },
			{ "Enabled", CheckMaxIterationCovered::Enabled },
		});
	}
	if (Settings::IsOption(key, "IncrementalSimulation", configPrefix))
	{
		return Settings::ParseEnum(value, incrementalSimulation, {
			{ "Disabled", IncrementalSimulation::Disabled },
			{ "Enabled", IncrementalSimulation::Enabled },
		});
	}
	if (Settings::IsOption(key, "PrintTestPatternReport", configPrefix))
	{
		return Settings::ParseEnum(value, printPatternReport, {
			{ "PrintDetail", PrintTestPatternReport::PrintDetail },
			{ "PrintSummary", PrintTestPatternReport::PrintSummary },
			{ "PrintNothing", PrintTestPatternReport::PrintNothing },
		});
	}
	if (Settings::IsOption(key, "PrintFaultListReport", configPrefix))
	{
		return Settings::ParseEnum(value, printFaultListReport, {
			{ "PrintDetail", PrintFaultListReport::PrintDetail },
			{ "PrintSummary", PrintFaultListReport::PrintSummary },
			{ "PrintNothing", PrintFaultListReport::PrintNothing },
		});
	}

	return AtpgData<FaultModel>::SetSetting(key, value)
		|| Mixin::StatisticsMixin::SetSetting(key, value)
		|| Mixin::FaultStatisticsMixin<FaultList>::SetSetting(key, value)
		|| Mixin::SimulationStatisticsMixin::SetSetting(key, value)
		|| Mixin::SolverStatisticsMixin::SetSetting(key, value)
		|| Mixin::VcdExportMixin<FaultList>::SetSetting(key, value)
		|| Mixin::VcmMixin::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
Basic::ApplicationStatistics AtpgBase<FaultModel, FaultList>::GetStatistics(void)
{
	ApplicationStatistics combinedStatistics;
	combinedStatistics.Merge(Mixin::StatisticsMixin::GetStatistics());
	combinedStatistics.Merge(Mixin::FaultStatisticsMixin<FaultList>::GetStatistics());
	combinedStatistics.Merge(Mixin::SimulationStatisticsMixin::GetStatistics());
	combinedStatistics.Merge(Mixin::SolverStatisticsMixin::GetStatistics());
	combinedStatistics.Merge(Mixin::VcdExportMixin<FaultList>::GetStatistics());
	combinedStatistics.Merge(Mixin::VcmMixin::GetStatistics());
	return combinedStatistics;
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::GenerateFaultList(void)
{
	auto& metaData { this->circuit->GetMetaData() };

	CpuClock generationTimer;
	CpuClock reductionTimer;

	if (faultListSource == AtpgBase<FaultModel, FaultList>::FaultListSource::File)
	{
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			std::vector<SingleStuckAtFault> allFaults { };

			generationTimer.SetTimeReference();
			ASSERT(faultListFile != "") << "No fault list file has been specified";
			FileHandle fileHandle(faultListFile, true);
			auto& input = fileHandle.GetStream();
			while (!input.eof())
			{
				std::string port;
				std::string type;
				input >> port;
				input >> type;
				port = this->circuit->GetName() + "/" + port;

				const auto* portMetaData { metaData.GetPort(port) };
				ASSERT(portMetaData != nullptr) << "Port " << port << " could not been found.";
				ASSERT(portMetaData->GetSize().GetSize() == 1u) << "Port " << port << " has size greater than 1.";
				ASSERT(portMetaData->GetSize().GetBottom() == 0u) << "Port " << port << " doesn't start at index 0.";

				const auto* gateMetaData { portMetaData->GetGroup() };
				const auto& gateSourceInfo { gateMetaData->GetSourceInfo() };
				const auto& portSourceInfo { portMetaData->GetSourceInfo() };
				ASSERT(gateSourceInfo.template GetProperty<bool>("module-is-primitive").value_or(false)) << "The gate has to have the primitive attribute.";

				auto cellType = gateSourceInfo.template GetProperty<std::string>("module-type").value_or("");
				ASSERT(cellType != "") << "The cell type attribute has to be defined.";

				auto portType = portSourceInfo.template GetProperty<std::string>("port-type").value_or("");
				ASSERT(portType != "") << "The port has no port-type attribute.";

				const auto& mappedNodes { gateMetaData->GetMappedNodes() };
				const auto* connection { portMetaData->GetConnectionForIndex(0u) };

				const std::vector<Circuit::MappedCircuit::NodeAndPort>* nodes;
				if (portType == "input")
				{
					nodes = &connection->GetMappedSinks();
				}
				else if (portType == "output")
				{
					nodes = &connection->GetMappedSources();
				}
				else
				{
					Logging::Panic("Unknown port type " + portType);
				}

				std::vector<Circuit::MappedCircuit::NodeAndPort> faultLocation;
				for (auto& [node, port] : *nodes)
				{
					if (std::find(mappedNodes.begin(), mappedNodes.end(), node) != mappedNodes.end())
					{
						faultLocation.push_back({ node, port });
					}
				}
				ASSERT(faultLocation.size() == 1u) << "Could not find unqiue fault location.";

				if (type == "SA0")
				{
					allFaults.push_back({ faultLocation[0u], StuckAtFaultType::STUCK_AT_0 });
				}
				else if (type == "SA1")
				{
					allFaults.push_back({ faultLocation[0u], StuckAtFaultType::STUCK_AT_1 });
				}
				else
				{
					Logging::Panic("Unknown fault type " + type);
				}
			}
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [filteredFaultList, fullToFilteredFaults] = Fault::FilterStuckAtFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, allFaults);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterStuckAtFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			std::vector<SingleTransitionDelayFault> allFaults { };

			generationTimer.SetTimeReference();
			ASSERT(faultListFile != "") << "No fault list file has been specified";
			FileHandle fileHandle(faultListFile, true);
			auto& input = fileHandle.GetStream();
			while (!input.eof())
			{
				std::string port;
				std::string type;
				input >> port;
				input >> type;
				port = this->circuit->GetName() + "/" + port;

				const auto* portMetaData { metaData.GetPort(port) };
				ASSERT(portMetaData != nullptr) << "Port " << port << " could not been found.";
				ASSERT(portMetaData->GetSize().GetSize() == 1u) << "Port " << port << " has size greater than 1.";
				ASSERT(portMetaData->GetSize().GetBottom() == 0u) << "Port " << port << " doesn't start at index 0.";

				const auto* gateMetaData { portMetaData->GetGroup() };
				const auto& gateSourceInfo { gateMetaData->GetSourceInfo() };
				const auto& portSourceInfo { portMetaData->GetSourceInfo() };
				ASSERT(gateSourceInfo.template GetProperty<bool>("module-is-primitive").value_or(false)) << "The gate has to have the primitive attribute.";

				auto cellType = gateSourceInfo.template GetProperty<std::string>("module-type").value_or("");
				ASSERT(cellType != "") << "The cell type attribute has to be defined.";

				auto portType = portSourceInfo.template GetProperty<std::string>("port-type").value_or("");
				ASSERT(portType != "") << "The port has no port-type attribute.";

				const auto& mappedNodes { gateMetaData->GetMappedNodes() };
				const auto* connection { portMetaData->GetConnectionForIndex(0u) };

				const std::vector<Circuit::MappedCircuit::NodeAndPort>* nodes;
				if (portType == "input")
				{
					nodes = &connection->GetMappedSinks();
				}
				else if (portType == "output")
				{
					nodes = &connection->GetMappedSources();
				}
				else
				{
					Logging::Panic("Unknown port type " + portType);
				}

				std::vector<Circuit::MappedCircuit::NodeAndPort> faultLocation;
				for (auto& [node, port] : *nodes)
				{
					if (std::find(mappedNodes.begin(), mappedNodes.end(), node) != mappedNodes.end())
					{
						faultLocation.push_back({ node, port });
					}
				}
				ASSERT(faultLocation.size() == 1u) << "Could not find unqiue fault location.";

				if (type == "STR")
				{
					allFaults.push_back({ faultLocation[0u], TransitionDelayFaultType::SLOW_TO_RISE });
				}
				else if (type == "STF")
				{
					allFaults.push_back({ faultLocation[0u], TransitionDelayFaultType::SLOW_TO_FALL });
				}
				else if (type == "STT")
				{
					allFaults.push_back({ faultLocation[0u], TransitionDelayFaultType::SLOW_TO_TRANSITION });
				}
				else
				{
					Logging::Panic("Unknown fault type " + type);
				}
			}
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [filteredFaultList, fullToFilteredFaults] = Fault::FilterTransitionDelayFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, allFaults);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterTransitionDelayFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			std::vector<CellAwareFault> allFaults { };

			generationTimer.SetTimeReference();
			ASSERT(faultListFile != "") << "No fault list file has been specified";
			FileHandle fileHandle(faultListFile, true);
			auto& input = fileHandle.GetStream();

			std::string line;
			while (getline(input, line))
			{
				boost::trim(line);
				if (line.empty())
				{
					continue;
				}

				auto it { line.find(' ') };
				ASSERT(it != std::string::npos) << "Could not find separator in line \"" << line << "\"";
				auto cellName { line.substr(0, it) };
				auto faultName { line.substr(it + 1u) };
				boost::trim(cellName);
				boost::trim(faultName);
				cellName = this->circuit->GetName() + "/" + cellName;

				auto const* cellMetaData = metaData.GetGroup(cellName);
				ASSERT(cellMetaData != nullptr) << "Cell " << cellName << " not found in circuit.";

				const auto& sourceInfo { cellMetaData->GetSourceInfo() };
				ASSERT(sourceInfo.template GetProperty<bool>("module-is-cell").value_or(false)) << "The cell has to have the cell attribute.";

				auto cellType = sourceInfo.template GetProperty<std::string>("module-type").value_or("");
				ASSERT(cellType != "") << "The cell type attribute has to be defined.";

				auto udfmFault { this->GetUdfm()->GetFault(cellType, faultName) };
				ASSERT(udfmFault) << "Could not find UDFM fault " << faultName << " for cell " << cellType << " in the UDFM.";

				auto cellAwareFault { MapUdfmFaultToCell(*this->circuit, cellMetaData, udfmFault) };
				ASSERT(cellAwareFault.has_value()) << "Could not map cell-aware fault to cell";
				allFaults.push_back(cellAwareFault.value());
			}
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [filteredFaultList, fullToFilteredFaults] = Fault::FilterCellAwareFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, allFaults);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterCellAwareFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else
		{
			LOG(FATAL) << "Fault model not implemented";
			__builtin_unreachable();
		}
	}
	else if (faultListSource == AtpgBase<FaultModel, FaultList>::FaultListSource::FreiTest)
	{
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			auto reductions = Fault::StuckAtFaultReduction::RemoveCellInternal
				| Fault::StuckAtFaultReduction::RemoveSequentialClock
				| Fault::StuckAtFaultReduction::RemoveSequentialSetReset
				| Fault::StuckAtFaultReduction::RemoveConnectedToDontCare
				| Fault::StuckAtFaultReduction::RemoveConnectedToUnknown;
			if (this->faultListReduction == AtpgData<FaultModel>::FaultListReduction::RemoveEquivalent)
			{
				reductions |= Fault::StuckAtFaultReduction::RemoveEquivalent;
			}

			generationTimer.SetTimeReference();
			auto allFaults = Fault::GenerateStuckAtFaultList(*this->circuit);
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [collapsedFaultList, fullToCollapsedFaults] = Fault::ReduceStuckAtFaultList(*this->circuit, reductions, allFaults);
			auto [filteredFaultList, collapsedToFilteredFaults] = Fault::FilterStuckAtFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, collapsedFaultList);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterStuckAtFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.EquivalentFaults", allFaults.size() - collapsedFaultList.size(), "Fault(s)", "The number of faults which were removed due to being equivalent");
			statistics.Add("FaultList.RemainingFaults", collapsedFaultList.size(), "Fault(s)", "The number of faults after removing equivalent faults");
			statistics.Add("FaultList.CollapseRatio", collapsedFaultList.size() * 100.0f / allFaults.size(), "Percent", "The ratio of the collapsed fault list compared to the original fault list");
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToCollapsedFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = collapsedToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			auto reductions = Fault::TransitionDelayFaultReduction::RemoveCellInternal
				| Fault::TransitionDelayFaultReduction::RemoveSequentialClock
				| Fault::TransitionDelayFaultReduction::RemoveSequentialSetReset
				| Fault::TransitionDelayFaultReduction::RemoveConnectedToDontCare
				| Fault::TransitionDelayFaultReduction::RemoveConnectedToUnknown;
			if (this->faultListReduction == AtpgData<FaultModel>::FaultListReduction::RemoveEquivalent)
			{
				reductions |= Fault::TransitionDelayFaultReduction::RemoveEquivalent;
			}

			generationTimer.SetTimeReference();
			auto allFaults = Fault::GenerateTransitionDelayFaultList(*this->circuit);
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [collapsedFaultList, fullToCollapsedFaults] = Fault::ReduceTransitionDelayFaultList(*this->circuit, reductions, allFaults);
			auto [filteredFaultList, collapsedToFilteredFaults] = Fault::FilterTransitionDelayFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, collapsedFaultList);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterTransitionDelayFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.EquivalentFaults", allFaults.size() - collapsedFaultList.size(), "Fault(s)", "The number of faults which were removed due to being equivalent");
			statistics.Add("FaultList.RemainingFaults", collapsedFaultList.size(), "Fault(s)", "The number of faults after removing equivalent faults");
			statistics.Add("FaultList.CollapseRatio", collapsedFaultList.size() * 100.0f / allFaults.size(), "Percent", "The ratio of the collapsed fault list compared to the original fault list");
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToCollapsedFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = collapsedToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			generationTimer.SetTimeReference();
			auto allFaults = Fault::GenerateCellAwareFaultList(*this->circuit, *this->GetUdfm());
			generationTimer.Stop();

			reductionTimer.SetTimeReference();
			auto [filteredFaultList, fullToFilteredFaults] = Fault::FilterCellAwareFaultListByPattern(*this->circuit, faultListFilter, Fault::FilterPatternType::Normal, allFaults);
			auto [includedFaultList, filteredToIncludedFaults] = Fault::FilterCellAwareFaultListByPattern(*this->circuit, faultListExclude, Fault::FilterPatternType::Inverted, filteredFaultList);
			reductionTimer.Stop();

			// Collect statistics
			statistics.Add("FaultList.FilteredFaults", filteredFaultList.size(), "Fault(s)", "The number of faults which matched the fault list filter");
			statistics.Add("FaultList.IncludedFaults", includedFaultList.size(), "Fault(s)", "The number of faults which are not excluded");

			fullFaultList = FaultList(allFaults);
			faultList = FaultList(includedFaultList);
			faultMapping = std::vector<size_t>(fullFaultList.size(), 0u);

			std::iota(faultMapping.begin(), faultMapping.end(), 0u);
			for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); faultIndex++)
			{
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = fullToFilteredFaults[faultMapping[faultIndex]];
				}
				if (faultMapping[faultIndex] != Fault::REMOVED_FAULT)
				{
					faultMapping[faultIndex] = filteredToIncludedFaults[faultMapping[faultIndex]];
				}
			}
		}
		else
		{
			LOG(FATAL) << "Fault model not implemented";
			__builtin_unreachable();
		}
	}

	LOG(INFO) << "Generated initial fault list with " << fullFaultList.size() << " faults";
	LOG(INFO) << "Generated final fault list with " << faultList.size()
		<< " (" << std::setprecision(2) << (faultList.size() * 100.0f / fullFaultList.size()) << " %) of originally "
		<< fullFaultList.size() << " faults";

	statistics.Add("FaultList.InitialFaults", fullFaultList.size(), "Fault(s)", "The initial number of faults");
	statistics.Add("FaultList.FinalFaults", faultList.size(), "Fault(s)", "The final number of faults");
	statistics.Add("FaultList.FinalRatio", faultList.size() * 100.0f / fullFaultList.size(), "Percent", "The ratio of the final fault list compared to the initial fault list");
	statistics.Add("FaultList.GenerationTime", generationTimer.RunTimeSinceReference(), "Second(s)", "The time for generating the fault list");
	statistics.Add("FaultList.ReductionTime", reductionTimer.RunTimeSinceReference(), "Second(s)", "The time for reducing and filtering the fault list");

	if (printFaultListReport == PrintFaultListReport::PrintDetail
		|| printFaultListReport == PrintFaultListReport::PrintSummary)
	{
		size_t collapsedFaults { 0u };
		size_t removedFaults { 0u };

		for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); ++faultIndex)
		{
			if (faultMapping[faultIndex] == Fault::REMOVED_FAULT)
			{
				removedFaults++;
			}
			else if (faultIndex != (faultMapping[faultIndex] + removedFaults))
			{
				collapsedFaults++;
			}
		}

		LOG(INFO) << "Total faults: " << fullFaultList.size();
		LOG(INFO) << "    Collapsed faults: " << collapsedFaults;
		LOG(INFO) << "    Removed faults: " << removedFaults;
		LOG(INFO) << "    Final faults: " << faultList.size();
	}

	if (printFaultListReport == PrintFaultListReport::PrintDetail)
	{
		for (size_t faultIndex = 0u; faultIndex < fullFaultList.size(); ++faultIndex)
		{
			LOG(INFO) << "Fault " << faultIndex << ": " << to_string(*fullFaultList.GetFault(faultIndex));
		}
		for (size_t faultIndex { 0u }; faultIndex < fullFaultList.size(); ++faultIndex)
		{
			if (faultMapping[faultIndex] == Fault::REMOVED_FAULT)
			{
				LOG(INFO) << "Fault " << faultIndex << ": Has been removed by one of the filters";
			}
			else
			{
				auto& [fault, metaData] = faultList[faultMapping[faultIndex]];
				LOG(INFO) << "Fault " << faultIndex << ": Mapped to fault " << faultMapping[faultIndex] << " which is " << to_string(*fault);
			}
		}
	}

	faultListBegin = std::min(faultList.size(), faultListBegin);
	faultListEnd = (faultListEnd == std::numeric_limits<size_t>::max()) ? faultList.size() : std::min(faultList.size(), faultListEnd);

	LOG(INFO) << "Considering faults from " << std::to_string(faultListBegin + 1u) << " to " << std::to_string(faultListEnd);
	Logging::SetFaultLimits(faultListBegin, faultListEnd);
	ResetStatistics();
}

template <typename FaultModel, typename FaultList>
template <typename PinData>
void AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinData>& logicGenerator, const SimulationConfig& simConfig) const
{
	const auto& mappedCircuit = this->circuit->GetMappedCircuit();
	const auto& [fault, metaData] = faultList[faultIndex];
	SimulationResult atpgGoodResult = Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator.GetContext());
	SimulationResult atpgBadResult = Tpg::Extractor::ExtractSimulationResult<PinData, BadTag>(logicGenerator.GetContext());

	LOG_IF(!CheckSensitization(FaultModel(fault), atpgGoodResult), FATAL) << "\n"
		<< "The ATPG result does not sensitize the fault " << faultIndex << " (" + to_string(*fault) << ")\n"
		<< "Result ATPG Good: " << to_string(atpgGoodResult) << "\n"
		<< "Result ATPG Bad:  " << to_string(atpgBadResult) << "\n"
		<< "Pattern: " << to_string(pattern);

	const auto [testPrimaryOutputs, testSecondaryOutputs ] = Pattern::GetCaptureOutputs(capture);
	bool propagated = false;
	for (size_t timeframe = 0u; timeframe < atpgGoodResult.GetNumberOfTimeframes(); ++timeframe)
	{
		if (testPrimaryOutputs)
		{
			for (auto const& primaryOutput : mappedCircuit.GetPrimaryOutputs())
			{
				const Logic good = atpgGoodResult.GetOutputLogic(primaryOutput, timeframe);
				const Logic bad = atpgBadResult.GetOutputLogic(primaryOutput, timeframe);
				const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);

				propagated |= difference;
			}
		}
		if (testSecondaryOutputs)
		{
			for (auto const& secondaryOutput : mappedCircuit.GetSecondaryOutputs())
			{
				const Logic good = atpgGoodResult.GetOutputLogic(secondaryOutput, timeframe);
				const Logic bad = atpgBadResult.GetOutputLogic(secondaryOutput, timeframe);
				const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);

				propagated |= difference;
			}
		}
	}
	LOG_IF(!propagated, FATAL) << "The ATPG result does not propagate the fault " << faultIndex	<< " (" + to_string(*fault) << ")";

	SimulationResult simGoodResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
	SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, pattern, {}, simGoodResult, simConfig);

	SimulationResult simBadResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
	SimulateTestPatternEventDriven<FaultModel>(mappedCircuit, pattern, { fault }, simBadResult, simConfig);

	if (this->vcdExport == Mixin::VcdExportMixin<FaultList>::VcdExport::Enabled)
	{
		const auto exportId = vcdDebugExportId.fetch_add(1u, std::memory_order_acq_rel);
		const auto exportInfo = this->GetBadSimulationVcdInfo(*this->circuit, { .patternId = exportId, .pattern = pattern }, { .faultId = faultIndex, .fault = *fault });

		Io::Vcd::ExportVcd(this->vcdModelBuilder.BuildVcdModel(atpgGoodResult, *this->circuit, exportInfo.header), this->vcdExportDirectory + "/" + std::to_string(exportId) + "-atpg-good.vcd");
		Io::Vcd::ExportVcd(this->vcdModelBuilder.BuildVcdModel(atpgBadResult, *this->circuit, exportInfo.header), this->vcdExportDirectory + "/" + std::to_string(exportId) + "-atpg-bad.vcd");
		Io::Vcd::ExportVcd(this->vcdModelBuilder.BuildVcdModel(simGoodResult, *this->circuit, exportInfo.header), this->vcdExportDirectory + "/" + std::to_string(exportId) + "-sim-good.vcd");
		Io::Vcd::ExportVcd(this->vcdModelBuilder.BuildVcdModel(simBadResult, *this->circuit, exportInfo.header), this->vcdExportDirectory + "/" + std::to_string(exportId) + "-sim-bad.vcd");
	}

	for (size_t timeframe = 0u; timeframe < atpgGoodResult.GetNumberOfTimeframes(); ++timeframe)
	{
		for (auto [nodeId, node] : mappedCircuit.EnumerateNodes())
		{
			ASSERT((atpgGoodResult[timeframe][nodeId] == Logic::LOGIC_INVALID || atpgGoodResult[timeframe][nodeId] == simGoodResult[timeframe][nodeId])
					&& (atpgBadResult[timeframe][nodeId] == Logic::LOGIC_INVALID ||  atpgBadResult[timeframe][nodeId] == simBadResult[timeframe][nodeId]))
				<< "\nThe good or bad fault simulation did not match the ATPG at node "
				<< node->GetName() << " in timeframe " << timeframe << " for nodeId " << nodeId << "\n"
				<< "Result ATPG: " << to_string(atpgGoodResult[timeframe][nodeId])
				<< "/" << to_string(atpgBadResult[timeframe][nodeId])
				<< " and Sim " << to_string(simGoodResult[timeframe][nodeId])
				<< "/" << to_string(simBadResult[timeframe][nodeId]) << "\n"
				<< "Pattern: " << to_string(pattern) << "\n"
				<< "\n" << to_string(*fault) << "\n\n"
				<< "Comparison:\n"
				<< to_string(atpgGoodResult, atpgBadResult, simGoodResult, simBadResult, mappedCircuit) << "\n"
				<< DebugFaultLocation(atpgGoodResult, atpgBadResult, simGoodResult, simBadResult, faultIndex);
		}
	}
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::RunFaultSimulation(const Tpg::Vcm::VcmContext& context, size_t targetFaultIndex, size_t patternIndex, Pattern::OutputCapture capture, const SimulationConfig& simConfig)
{
	Tpg::Vcm::VcmContext simulationContext { context, context.GetTarget(), context.GetDisplayName() };
	simulationContext.SetVcmParameter("is_simulation", true);

	ASSERT(patternIndex < testPatterns.size()) << "Pattern index is larger than list size";

	const auto testPattern = testPatterns[patternIndex];
	const auto& mappedCircuit = this->circuit->GetMappedCircuit();

	SimulationResult goodResult(testPattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());

	CpuClock totalSimulationClock;
	totalSimulationClock.SetTimeReference();
#ifndef NDEBUG
	std::atomic<size_t> faultsCoveredBySimulation { 0u };
	CpuClock initialFaultFreeSimulationClock;
	initialFaultFreeSimulationClock.SetTimeReference();
#endif

	SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, *testPattern, {}, goodResult, simConfig);

#ifndef NDEBUG
	initialFaultFreeSimulationClock.Stop();
	{
		std::scoped_lock lock { parallelMutex };
		initialFaultFreeSimulationTime.AddValue(initialFaultFreeSimulationClock.TotalRunTime());
	}
#endif

	Mixin::VcdExportMixin<FaultList>::ExportVcdForGoodSimulation({ patternIndex, *testPattern }, goodResult);

	const auto [testPrimary, testSecondary] = Pattern::GetCaptureOutputs(capture);
	const auto simulate_for_fault_range = [&, testPrimaryOutputs=testPrimary, testSecondaryOutputs=testSecondary](size_t begin, size_t end) {

	#ifndef NDEBUG
		CpuClock simulateForFaultClock;
		simulateForFaultClock.SetTimeReference();
	#endif

		for (size_t faultIndex = begin; faultIndex < end; ++faultIndex)
		{
			// Check for already detected faults if not SimulateAllFaults option is enabled
			auto [fault, metaData] = faultList[faultIndex];
			if (simulateAllFaults != SimulateAllFaults::Enabled
				&& metaData->faultStatus != Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
			{
				continue;
			}

			DVLOG(5) << "Checking if test pattern finds " << to_string(*fault);

			// Do a pre-check if the fault is sensitized in at least one timeframe.
			// If this is not the case then we can skip the simulation as no fault propagation is possible.
			if (!CheckSensitization(FaultModel(fault), goodResult))
			{
				// Fault is not sensitized -> test next fault
				ASSERT(faultIndex != targetFaultIndex) << "The test pattern " << patternIndex << " does not sensitize the fault " << faultIndex
						<< " (" + to_string(*fault) << ")";
				continue;
			}

			// Start simulation
			SimulationResult badResult(testPattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			switch (incrementalSimulation)
			{
				case IncrementalSimulation::Disabled:
				{
					VLOG(9) << "Incremental simulation disabled! This will slow down the simulation!";

				#ifndef NDEBUG
					CpuClock eventDrivenSimulationClock;
					eventDrivenSimulationClock.SetTimeReference();
				#endif

					SimulateTestPatternEventDriven<FaultModel>(mappedCircuit, *testPattern, { fault }, badResult, simConfig);

				#ifndef NDEBUG
					eventDrivenSimulationClock.Stop();
					std::scoped_lock lock { parallelMutex };
					eventDrivenSimulationTime.AddValue(eventDrivenSimulationClock.TotalRunTime());
				#endif

					break;
				}
				case IncrementalSimulation::Enabled:
				{

				#ifndef NDEBUG
					CpuClock eventDrivenSimulationIncrementalClock;
					eventDrivenSimulationIncrementalClock.SetTimeReference();
				#endif

					badResult.ReplaceWith(goodResult);
					SimulateTestPatternEventDrivenIncremental<FaultModel>(mappedCircuit, *testPattern, { fault }, std::as_const(goodResult), badResult, simConfig);

				#ifndef NDEBUG
					eventDrivenSimulationIncrementalClock.Stop();
					std::scoped_lock lock { parallelMutex };
					eventDrivenSimulationIncrementalTime.AddValue(eventDrivenSimulationIncrementalClock.TotalRunTime());
				#endif

					break;
				}
			}

			if (__builtin_expect(checkSimulation == CheckSimulation::Enabled, false))
			{
				SimulationResult checkResult(testPattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
				SimulateTestPatternNaive<FaultModel>(mappedCircuit, *testPattern, { fault }, checkResult, simConfig);
				for (size_t timeframe = 0u; timeframe < testPattern->GetNumberOfTimeframes(); ++timeframe)
				{
					for (size_t nodeId = 0u; nodeId < mappedCircuit.GetNumberOfNodes(); ++nodeId)
					{
						ASSERT(badResult[timeframe][nodeId] == checkResult[timeframe][nodeId])
							<< "The event-based and naive simulation result did not match for gate "
							<< mappedCircuit.GetNode(nodeId)->GetName() << ". Good: "
							<< to_string(goodResult[timeframe][nodeId]) << ", Bad: "
							<< to_string(badResult[timeframe][nodeId]) << ", Bad (Check): "
							<< to_string(checkResult[timeframe][nodeId]);
					}
				}
			}

			if (__builtin_expect(checkSimulationInitialState == CheckSimulationInitialState::CheckEqual, false))
			{
				for (auto [inputNumber, secondaryInput] : mappedCircuit.EnumerateSecondaryInputs())
				{
					if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
					{
						// This produces false positives in the case that the fault is located at this node.
						// E.g. a stuck-at 0 fault at the output of flip-flop.
						// => Skip all fault locations as we expect a different output value
						if (secondaryInput->GetNodeId() == fault->GetNode()->GetNodeId())
						{
							continue;
						}
					}
					else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
					{
						// TODO: Implement
						LOG(FATAL) << "MultiStuckAtFaultModel not implemented";
						__builtin_unreachable();
					}
					else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
					{
						const auto& effectNodeAndPorts = fault->GetEffectNodesAndPorts();

						// This produces false positives in the case that the fault is located at this node.
						// => Skip all fault locations as we expect a different output value
						if (std::find_if(effectNodeAndPorts.begin(), effectNodeAndPorts.end(),
							[&] (const Circuit::MappedCircuit::NodeAndPort& nap) { return nap.node->GetNodeId() == secondaryInput->GetNodeId(); } ) != effectNodeAndPorts.end())
						{
							continue;
						}
					}
					else
					{
						LOG(FATAL) << "Fault model not implemented";
						__builtin_unreachable();
					}

					ASSERT(goodResult[0u][secondaryInput->GetNodeId()] == badResult[0u][secondaryInput->GetNodeId()])
						<< "The initial state does not match for the good and bad simulation for gate "
						<< secondaryInput->GetName() << ". Good: "
						<< to_string(goodResult[0u][secondaryInput->GetNodeId()]) << ", Bad: "
						<< to_string(badResult[0u][secondaryInput->GetNodeId()]);
				}
			}

			if (__builtin_expect(checkSimulationInputs == CheckSimulationInputs::CheckEqual, false))
			{
				// TODO: Rearrange for loops like below? (Should be better performance for CA and MultiStuckAt)
				for (size_t timeframe = 0u; timeframe < testPattern->GetNumberOfTimeframes(); ++timeframe)
				{
					for (auto [inputNumber, primaryInput] : mappedCircuit.EnumeratePrimaryInputs())
					{
						if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
						{
							// This produces false positives in the case that the fault is located at this node.
							// E.g. a stuck-at 0 fault at this input port.
							// => Skip all fault locations as we expect a different output value
							if (primaryInput->GetNodeId() == fault->GetNode()->GetNodeId())
							{
								continue;
							}
						}
						else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
						{
							// TODO: Implement
							LOG(FATAL) << "MultiStuckAtFaultModel not implemented";
							__builtin_unreachable();
						}
						else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
						{
							const auto& effectNodeAndPorts = fault->GetEffectNodesAndPorts();

							// This produces false positives in the case that the fault is located at this node.
							// E.g. a fault at the input port.
							// => Skip all fault locations as we expect a different output value
							if (std::find_if(effectNodeAndPorts.begin(), effectNodeAndPorts.end(),
								[&] (const Circuit::MappedCircuit::NodeAndPort& nap) { return nap.node->GetNodeId() == primaryInput->GetNodeId(); } ) != effectNodeAndPorts.end())
							{
								continue;
							}
						}
						else
						{
							LOG(FATAL) << "Fault model not implemented";
							__builtin_unreachable();
						}

						ASSERT(goodResult[timeframe][primaryInput->GetNodeId()] == badResult[timeframe][primaryInput->GetNodeId()])
							<< "The circuit input does not match for the good and bad simulation for input gate "
							<< primaryInput->GetName() << ". Good: "
							<< to_string(goodResult[timeframe][primaryInput->GetNodeId()]) << ", Bad: "
							<< to_string(badResult[timeframe][primaryInput->GetNodeId()]);
					}
				}
			}

			if (vcmEnable == VcmMixin::VcmEnable::Enabled)
			{
				Pattern::TestPattern vcmPattern = VcmMixin::GetVcmPatternForSimulationResult(simulationContext, goodResult, badResult);
				if (!VcmMixin::SimulateVcmPattern(simulationContext, vcmPattern))
				{
					if (faultIndex == targetFaultIndex)
					{
						LOG(FATAL) << "The test pattern " << patternIndex << " does not test the fault " << faultIndex
							<< " (" + to_string(*fault) << ") according to VCM";
					}

					goto nextFault;
				}
			}

			if (__builtin_expect(checkSimulationFlipFlops == CheckSimulationFlipFlops::CheckEqual, false))
			{
				for (auto [inputNumber, secondaryInput] : mappedCircuit.EnumerateSecondaryInputs())
				{
					if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
					{
						// This produces false positives in the case that the fault is located at this node.
						// E.g. a stuck-at 0 fault at this flip-flop.
						// => Skip all fault locations as we expect a different output value
						if (secondaryInput->GetNodeId() == fault->GetNode()->GetNodeId())
						{
							continue;
						}
					}
					else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
					{
						// TODO: Implement
						LOG(FATAL) << "MultiStuckAtFaultModel not implemented";
						__builtin_unreachable();
					}
					else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
					{
						// This produces false positives in the case that the fault is located at this node.
						// => Skip all fault locations as we expect a different output value
						bool isFaultLocation = false;
						for (const auto& nodeAndPort : fault->GetEffectNodesAndPorts())
						{
							if (__builtin_expect(secondaryInput->GetNodeId() == nodeAndPort.node->GetNodeId(), false))
							{
								isFaultLocation = true;
								break;
							}
						}

						if (__builtin_expect(isFaultLocation, false))
						{
							continue;
						}
					}
					else
					{
						LOG(FATAL) << "Fault model not implemented";
						__builtin_unreachable();
					}

					for (size_t timeframe = 0u; timeframe < testPattern->GetNumberOfTimeframes(); ++timeframe)
					{
						ASSERT(goodResult[timeframe][secondaryInput->GetNodeId()] == testPattern->GetSecondaryInput(timeframe, inputNumber))
							<< "The flip-flop content of node " << mappedCircuit.GetNode(secondaryInput->GetNodeId())->GetName()
							<< " does not match between simulation and the generated test pattern. Good: "
							<< to_string(goodResult[timeframe][secondaryInput->GetNodeId()]) << ", Pattern: "
							<< to_string(testPattern->GetSecondaryInput(timeframe, inputNumber));
					}
				}
			}

			for (size_t timeframe = 0u; timeframe < testPattern->GetNumberOfTimeframes(); ++timeframe)
			{
				for (size_t index = 0u; testPrimaryOutputs && index < mappedCircuit.GetNumberOfPrimaryOutputs(); ++index)
				{
					const auto* primaryOutput = mappedCircuit.GetPrimaryOutput(index);
					const Logic good = goodResult.GetOutputLogic(primaryOutput, timeframe);
					const Logic bad = badResult.GetOutputLogic(primaryOutput, timeframe);
					const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);

					DVLOG(9) << "Primary Output " << index << ": " << "Good " << good << " <-> " << bad << " Bad"
						<< (difference ? " (Difference detected)" : "");
					if (__builtin_expect(difference, false))
					{
						{
							std::scoped_lock lock { parallelMutex };
							if (metaData->faultStatus != Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
							{
								if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNDETECTED)
								{
									LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
										<< " with status " << to_string(metaData->faultStatus) << "/" << to_string(metaData->targetedFaultStatus)
										<< " was found by test pattern " << patternIndex << "!";
								}
								goto nextFault;
							}

							if (checkMaxIterationCovered == CheckMaxIterationCovered::Enabled
								&& metaData->targetedFaultStatus == Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS)
							{
								LOG(FATAL) << "Fault " << faultIndex << " " << to_string(*fault)
									<< " with status " << to_string(metaData->faultStatus) << "/" << to_string(metaData->targetedFaultStatus)
									<< " was found by test pattern " << patternIndex << "!";
							}

							Mixin::FaultStatisticsMixin<FaultList>::SetFaultStatus(faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_DETECTED,
								Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE);

							metaData->detectingPatternId = patternIndex;
							metaData->detectingTimeframe = timeframe;
							metaData->detectingNode = { primaryOutput, { Circuit::PortType::Input, 0u } };
							metaData->detectingOutputGood = good;
							metaData->detectingOutputBad = bad;
						}

					#ifndef NDEBUG
						if (__builtin_expect(faultIndex != targetFaultIndex, true))
						{
							faultsCoveredBySimulation += 1u;
						}
					#endif

						Mixin::VcdExportMixin<FaultList>::ExportVcdForBadSimulation(faultList, { patternIndex, *testPattern }, { faultIndex, *fault }, badResult);
						goto nextFault;
					}
				}

				for (size_t index = 0; testSecondaryOutputs && index < mappedCircuit.GetNumberOfSecondaryOutputs(); ++index)
				{
					const auto* secondaryOutput = mappedCircuit.GetSecondaryOutput(index);
					const Logic good = goodResult.GetOutputLogic(secondaryOutput, timeframe);
					const Logic bad = badResult.GetOutputLogic(secondaryOutput, timeframe);
					const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && bad != good;

					DVLOG(9) << "Secondary Output " << index << ": " << "Good " << good << " <-> " << bad << " Bad"
						<< (difference ? " (Difference detected)" : "");
					if (__builtin_expect(difference, false))
					{
						{
							std::scoped_lock lock { parallelMutex };
							if (metaData->faultStatus != Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
							{
								if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNDETECTED)
								{
									LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
										<< " with status " << to_string(metaData->faultStatus) << "/" << to_string(metaData->targetedFaultStatus)
										<< " was found by test pattern " << patternIndex << "!";
								}
								goto nextFault;
							}

							if (checkMaxIterationCovered == CheckMaxIterationCovered::Enabled
								&& metaData->targetedFaultStatus == Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS)
							{
								LOG(FATAL) << "Fault " << faultIndex << " " << to_string(*fault)
									<< " with status " << to_string(metaData->faultStatus) << "/" << to_string(metaData->targetedFaultStatus)
									<< " was found by test pattern " << patternIndex << "!";
							}

							Mixin::FaultStatisticsMixin<FaultList>::SetFaultStatus(faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_DETECTED,
								Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE);

							metaData->detectingPatternId = patternIndex;
							metaData->detectingTimeframe = timeframe;
							metaData->detectingNode = { secondaryOutput, { Circuit::PortType::Input, 0u } };
							metaData->detectingOutputGood = good;
							metaData->detectingOutputBad = bad;
						}

					#ifndef NDEBUG
						if (__builtin_expect(faultIndex != targetFaultIndex, true))
						{
							faultsCoveredBySimulation += 1u;
						}
					#endif

						Mixin::VcdExportMixin<FaultList>::ExportVcdForBadSimulation(faultList, { patternIndex, *testPattern }, { faultIndex, *fault }, badResult);
						goto nextFault;
					}
				}
			}

			if (faultIndex == targetFaultIndex)
			{
				LOG(FATAL) << "The test pattern " << patternIndex << " does not propagate the fault " << faultIndex
					<< " (" + to_string(*fault) << ")";
			}

			nextFault:
				continue;
		}

	#ifndef NDEBUG
		simulateForFaultClock.Stop();
		std::scoped_lock lock { parallelMutex };
		simulateForFaultTime.AddValue(simulateForFaultClock.TotalRunTime());
	#endif
	};

	if (faultSimulation == FaultSimulation::Enabled)
	{
		Parallel::ExecuteParallelInBlocks(faultListBegin, faultListEnd, 256u, Parallel::Arena::FaultSimulation, Parallel::Order::Parallel, simulate_for_fault_range);
	}
	else
	{
		simulate_for_fault_range(targetFaultIndex, targetFaultIndex + 1u);
	}

	totalSimulationClock.Stop();
	std::scoped_lock lock { parallelMutex };
	totalSimulationTime.AddValue(totalSimulationClock.TotalRunTime());
#ifndef NDEBUG
	faultsCoveredBySimulationStat.AddValue(faultsCoveredBySimulation);
#endif
}

template <typename FaultModel, typename FaultList>
bool AtpgBase<FaultModel, FaultList>::CheckSensitization(const FaultModel& faultModel, const Simulation::SimulationResult& goodResult) const
{
	const auto& mappedCircuit { this->circuit->GetMappedCircuit() };

	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		const auto& stuckAt { fault->GetStuckAt() };
		const size_t nodeId { mappedCircuit.GetDriverForPort(stuckAt)->GetNodeId() };
		for (size_t timeframeId = 0u; timeframeId < goodResult.GetNumberOfTimeframes(); ++timeframeId)
		{
			const Logic& value = goodResult[timeframeId][nodeId];
			if (__builtin_expect(stuckAt.GetType() == StuckAtFaultType::STUCK_AT_1 && value == Logic::LOGIC_ZERO, false)
				|| __builtin_expect(stuckAt.GetType() == StuckAtFaultType::STUCK_AT_0 && value == Logic::LOGIC_ONE, false))
			{
				return true;
			}
		}
		return false;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		for (const auto& stuckAt : fault.GetStuckAts())
		{
			const size_t nodeId = mappedCircuit.GetDriverForPort(stuckAt)->GetNodeId();
			for (size_t timeframeId = 0u; timeframeId < goodResult.GetNumberOfTimeframes(); ++timeframeId)
			{
				const Logic& value = goodResult[timeframeId][nodeId];
				if (__builtin_expect(stuckAt.GetType() == StuckAtFaultType::STUCK_AT_1 && value == Logic::LOGIC_ZERO, false)
					|| __builtin_expect(stuckAt.GetType() == StuckAtFaultType::STUCK_AT_0 && value == Logic::LOGIC_ONE, false))
				{
					return true;
				}
			}
		}
		return false;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		const auto& transitionDelay { fault->GetTransitionDelay() };
		const size_t nodeId { mappedCircuit.GetDriverForPort(transitionDelay)->GetNodeId() };
		for (size_t timeframeId = 0u; timeframeId + 1u < goodResult.GetNumberOfTimeframes(); ++timeframeId)
		{
			const Logic& initialValue = goodResult[timeframeId][nodeId];
			const Logic& finalValue = goodResult[timeframeId + 1u][nodeId];
			if (__builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_RISE && initialValue == Logic::LOGIC_ZERO && finalValue == Logic::LOGIC_ONE, false)
				|| __builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_FALL && initialValue == Logic::LOGIC_ONE && finalValue == Logic::LOGIC_ZERO, false)
				|| __builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_TRANSITION
					&& ((initialValue == Logic::LOGIC_ZERO && finalValue == Logic::LOGIC_ONE)
						|| (initialValue == Logic::LOGIC_ONE && finalValue == Logic::LOGIC_ZERO)), false))
			{
				return true;
			}
		}
		return false;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiTransitionDelayFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		for (const auto& transitionDelay : fault.GetTransitionDelays())
		{
			const size_t nodeId = mappedCircuit.GetDriverForPort(transitionDelay)->GetNodeId();
			for (size_t timeframeId = 0u; timeframeId + 1u < goodResult.GetNumberOfTimeframes(); ++timeframeId)
			{
				const Logic& initialValue = goodResult[timeframeId][nodeId];
				const Logic& finalValue = goodResult[timeframeId + 1u][nodeId];
				if (__builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_RISE && initialValue == Logic::LOGIC_ZERO && finalValue == Logic::LOGIC_ONE, false)
					|| __builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_FALL && initialValue == Logic::LOGIC_ONE && finalValue == Logic::LOGIC_ZERO, false)
					|| __builtin_expect(transitionDelay.GetType() == TransitionDelayFaultType::SLOW_TO_TRANSITION
						&& ((initialValue == Logic::LOGIC_ZERO && finalValue == Logic::LOGIC_ONE)
							|| (initialValue == Logic::LOGIC_ONE && finalValue == Logic::LOGIC_ZERO)), false))
				{
					return true;
				}
			}
		}
		return false;
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		const auto& fault { faultModel.GetFault() };
		for (const auto& alternative : fault->GetAlternatives())
		{
			for (size_t timeframeId { 0u }; timeframeId < goodResult.GetNumberOfTimeframes(); timeframeId++)
			{
				// Not enough timeframes left to be able to fulfill conditions
				bool satisfied { (timeframeId + alternative.conditions[0].logicConstraints.size()) <= goodResult.GetNumberOfTimeframes() };
				for (const auto& condition : alternative.conditions)
				{
					if (!satisfied) { break; }
					for (size_t index { 0u }; index < alternative.conditions[0].logicConstraints.size(); index++)
					{
						if (!satisfied) { break; }
						auto const targetNodeId { mappedCircuit.GetDriverForPort(condition.nodeAndPort)->GetNodeId() };
						auto const goodValue { goodResult[timeframeId + index][targetNodeId] };
						satisfied &= IsConstraintTrueForLogic(goodValue, condition.logicConstraints[index]);
					}
				}
				if (satisfied) { return true; }
			}
		}
		return false;
	}
	else
	{
		LOG(FATAL) << "Fault model not implemented";
		__builtin_unreachable();
	}
}

template <typename FaultModel, typename FaultList>
std::vector<Fault::FaultStatus> AtpgBase<FaultModel, FaultList>::CheckCombinationalUntestability(const FaultList& faultList, const std::vector<bool>& faultMask)
{
	using LogicContainer = LogicContainer01X;
	using PinData = Tpg::PinDataGB<LogicContainer>;
	using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
	using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
	using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;
	using FaultTagger = typename AtpgConfig<FaultModel, PinData>::FaultTagger;
	using FaultTaggerMode = typename AtpgConfig<FaultModel, PinData>::FaultTaggerMode;

	statistics.Add("Atpg.Encoding.CombinationalUntestability.LogicContainer", std::string("LogicContainer") + get_logic_container_name<LogicContainer>, "Type", "LogicContainer used");
	statistics.Add("Atpg.Encoding.CombinationalUntestability.PinData", std::string("PinData") + get_pin_data_name_v<PinData>, "Type", "PinData used");

	std::atomic<Fault::FaultStatus> faultStatus[faultList.size()];
	for (size_t index { 0u }; index < faultList.size(); ++index)
	{
		new (&faultStatus[index]) std::atomic<Fault::FaultStatus>(Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED);
	}

	std::atomic<size_t> testableFaults { 0u };
	std::atomic<size_t> untestableFaults { 0u };
	std::atomic<size_t> timeouts { 0u };

	for (size_t index { faultListBegin }; index != faultListEnd; ++index)
	{
		if (faultMask[index])
		{
			std::atomic_store_explicit(&faultStatus[index], Fault::FaultStatus::FAULT_STATUS_UNDETECTED, std::memory_order_release);
			std::atomic_fetch_add_explicit(&untestableFaults, 1u, std::memory_order_release);
		}
		else
		{
			std::atomic_store_explicit(&faultStatus[index], Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED, std::memory_order_release);
		}
	}

	const auto print_status = [faults = faultList.size(), &testableFaults, &untestableFaults, &timeouts](void) -> void {
		const auto total = testableFaults + untestableFaults + timeouts;
		LOG(INFO) << "Simple Combinational Testability Analysis: "
			<< testableFaults << " Testable (" << std::setw(2) << std::setprecision(4) << std::fixed << (100.0 * testableFaults / faults) << " %), "
			<< untestableFaults << " Untestable (" << std::setw(2) << std::setprecision(4) << std::fixed << (100.0 * untestableFaults / faults) << " %), "
			<< timeouts << " Skipped (" << std::setw(2) << std::setprecision(4) << std::fixed << (100.0 * timeouts / faults) << " %), "
			<< total << " / " << faults << " (" << std::setw(2) << std::setprecision(4) << std::fixed << (100.0 * total / faults) << " %) completed";
	};
	print_status();

	const auto check_testability = [&](size_t faultIndex) -> void {
		if (std::atomic_load_explicit(&faultStatus[faultIndex], std::memory_order_acquire) == Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			return;
		}

		auto [fault, metaData] = faultList[faultIndex];

		std::shared_ptr<Sat::SatSolverProxy> satSolver = Sat::SatSolverProxy::CreateSatSolver(Settings::GetInstance()->SatSolver);
		ASSERT(satSolver) << "Could not initialize SAT-Solver!";
		satSolver->SetSolverTimeout(solverUntestabilityTimeout);

		auto faultModel = std::make_shared<FaultModel>(fault);
		auto timeframes = AtpgConfig<FaultModel, PinData>::GetRequiredTimeframeCount(*faultModel);
		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);

		logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
		auto faultTagger = logicGenerator->template EmplaceModule<FaultTagger>(faultModel);
		faultTagger->SetTaggingMode(FaultTaggerMode::FullScan);

		// Basic circuit encoding for single timeframe of combinational testability check.
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, BadTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();

		// Additional constraints for test generation where the fault effect has to be sensitized (difference at fault location)
		// and propagated (difference at primary or secondary circuit output).
		logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
		logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
		logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryAndSecondaryOutputs, Tpg::FaultPropagationTimeframe::Last);

		logicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
		if (!logicGenerator->GenerateCircuitLogic())
		{
			Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
			if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_UNDETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				std::atomic_fetch_add_explicit(&untestableFaults, 1u, std::memory_order_acq_rel);
			}
			print_status();
			return;
		}

		const auto& result = satSolver->Solve();
		switch (result)
		{
		case Sat::SatResult::SAT:
		{
			Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
			if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_DETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				std::atomic_fetch_add_explicit(&testableFaults, 1u, std::memory_order_acq_rel);
			}

			auto const& mappedCircuit = this->circuit->GetMappedCircuit();
			auto pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndSecondaryInputs);

			SimulationConfig simConfig { MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
			simConfig.sequentialMode = Simulation::SequentialMode::FullScan;

			SimulationResult goodResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
			SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, pattern, {}, goodResult, simConfig);

			const auto& simulate_for_fault_range = [&](size_t begin, size_t end) {
				SimulationResult badResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);

				for (size_t faultIndex = begin; faultIndex != end; ++faultIndex)
				{
					// Check for already detected faults if not SimulateAllFaults option is enabled
					if (std::atomic_load_explicit(&faultStatus[faultIndex], std::memory_order_acquire) == Fault::FaultStatus::FAULT_STATUS_DETECTED)
					{
						continue;
					}

					auto [fault, metaData] = faultList[faultIndex];

					// Do a pre-check if the fault is sensitized in at least one timeframe.
					// If this is not the case then we can skip the simulation as no fault propagation is possible.
					if (!CheckSensitization(FaultModel(fault), goodResult))
					{
						// Fault is not sensitized -> test next fault
						continue;
					}

					// Start simulation
					badResult.ReplaceWith(goodResult);
					SimulateTestPatternEventDrivenIncremental<FaultModel>(mappedCircuit, pattern, { fault }, std::as_const(goodResult), badResult, simConfig);

					auto has_difference = [&](const Circuit::MappedNode* node, size_t timeframe) -> bool {
						const Logic good = goodResult.GetOutputLogic(node, timeframe);
						const Logic bad = badResult.GetOutputLogic(node, timeframe);
						return IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);
					};

					for (size_t timeframe = 0u; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
					{
						for (size_t index = 0u; index < mappedCircuit.GetNumberOfPrimaryOutputs(); ++index)
						{
							if (__builtin_expect(has_difference(mappedCircuit.GetPrimaryOutput(index), timeframe), false))
							{
								Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
								if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_DETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
								{
									std::atomic_fetch_add_explicit(&testableFaults, 1u, std::memory_order_acq_rel);
								}

								expected = Fault::FaultStatus::FAULT_STATUS_EXTENDED;
								if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_DETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
								{
									std::atomic_fetch_sub_explicit(&timeouts, 1u, std::memory_order_acq_rel);
									std::atomic_fetch_add_explicit(&testableFaults, 1u, std::memory_order_acq_rel);
								}
								goto nextFault;
							}
						}

						for (size_t index = 0; index < mappedCircuit.GetNumberOfSecondaryOutputs(); ++index)
						{
							if (__builtin_expect(has_difference(mappedCircuit.GetSecondaryOutput(index), timeframe), false))
							{
								Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
								if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_DETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
								{
									std::atomic_fetch_add_explicit(&testableFaults, 1u, std::memory_order_acq_rel);
								}

								expected = Fault::FaultStatus::FAULT_STATUS_EXTENDED;
								if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_DETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
								{
									std::atomic_fetch_sub_explicit(&timeouts, 1u, std::memory_order_acq_rel);
									std::atomic_fetch_add_explicit(&testableFaults, 1u, std::memory_order_acq_rel);
								}
								goto nextFault;
							}
						}
					}

					nextFault:
						continue;
				}
			};

			Parallel::ExecuteParallelInBlocks(faultListBegin, faultListEnd, 256u, Parallel::Arena::FaultSimulation, Parallel::Order::Parallel, simulate_for_fault_range);
			print_status();
			return;
		}

		case Sat::SatResult::UNKNOWN:
		{
			Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
			if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_EXTENDED, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				std::atomic_fetch_add_explicit(&timeouts, 1u, std::memory_order_acq_rel);
			}
			print_status();
			return;
		}

		case Sat::SatResult::UNSAT:
		{
			Fault::FaultStatus expected = Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED;
			if (std::atomic_compare_exchange_strong_explicit(&faultStatus[faultIndex], &expected, Fault::FaultStatus::FAULT_STATUS_UNDETECTED, std::memory_order_acq_rel, std::memory_order_acquire))
			{
				std::atomic_fetch_add_explicit(&untestableFaults, 1u, std::memory_order_acq_rel);
			}
			print_status();
			return;
		}

		}
	};

	Parallel::ExecuteParallel(faultListBegin, faultListEnd, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, check_testability);

	std::vector<Fault::FaultStatus> result(faultList.size(), Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED);
	for (size_t index { faultListBegin }; index != faultListEnd; ++index)
	{
		auto status = std::atomic_load_explicit(&faultStatus[index], std::memory_order_acquire);
		if (status == Fault::FaultStatus::FAULT_STATUS_DETECTED || status == Fault::FaultStatus::FAULT_STATUS_UNDETECTED)
		{
			result[index] = status;
		}
	}

	for (size_t index { 0u }; index < faultList.size(); ++index)
	{
		faultStatus[index].~atomic();
	}
	return result;
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::ResetStatistics(void)
{
	statPatternsGenerated.AddValue(testPatterns.size());
	Mixin::FaultStatisticsMixin<FaultList>::ResetStatistics(faultList.size());
	PrintStatistics();
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::SnapshotStatisticsForIteration(void)
{
	statPatternsGenerated.AddValue(testPatterns.size());
	Mixin::FaultStatisticsMixin<FaultList>::SnapshotStatisticsForIteration();
	PrintStatistics();
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::PrintStatistics(void)
{
	Mixin::FaultStatisticsMixin<FaultList>::PrintStatistics("Patterns: " + std::to_string(testPatterns.size()));
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::ExportStatistics(void)
{
	Mixin::FaultStatisticsMixin<FaultList>::ExportStatistics(faultList);
	Mixin::SimulationStatisticsMixin::ExportStatistics();
	Mixin::SolverStatisticsMixin::ExportStatistics();

	size_t totalLength = 0;
	for (const auto pattern : testPatterns)
	{
		totalLength += pattern->GetNumberOfTimeframes();
	}
	const float averageLength = (static_cast<float>(totalLength)
		/ static_cast<float>(testPatterns.size()));

	switch (printPatternReport)
	{
		case PrintTestPatternReport::PrintDetail:
		{
			LOG(INFO) << "Test patterns generated: " << testPatterns.size();
			LOG(INFO) << "    Total length: " << totalLength << " timeframes";
			LOG(INFO) << "    Average length: " << averageLength << " timeframes";
			size_t patternIndex { 0u };
			for (const auto pattern : testPatterns)
			{
				LOG(INFO) << "    Test pattern " << patternIndex << ": " << to_string(*pattern);
				patternIndex++;
			}
			break;
		}

		case PrintTestPatternReport::PrintSummary:
		{
			LOG(INFO) << "Test patterns generated: " << testPatterns.size();
			LOG(INFO) << "    Total length: " << totalLength << " timeframes";
			LOG(INFO) << "    Average length: " << averageLength << " timeframes";
			break;
		}

		case PrintTestPatternReport::PrintNothing:
			break;
	}

	statistics.Add("Atpg.Patterns.Generated", testPatterns.size(), "Test Pattern(s)", "The number of test patterns generated by a SAT run");
	statistics.Add("Atpg.Patterns.LengthTotal", totalLength, "Timeframe(s)", "The length of all generated test patterns combined");
	statistics.Add("Atpg.Patterns.LengthAverage", averageLength, "Timeframe(s)", "The average length of the generated test patterns");

	Mixin::StatisticsMixin::ExportStatistics("plots/PatternsGenerated", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Patterns",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Generated Testpatterns",
			.yAxisMin = 0.0,
			.yAxisMax = static_cast<double>(testPatterns.size())
		}, statPatternsGenerated);
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture capture) const
{
	if (testPatternExport == TestPatternExport::Disabled)
	{
		return;
	}

	FileHandle jsonPatternsHandle("[DataExportDirectory]/patterns.json", false);
	FileHandle stilPatternsHandle("[DataExportDirectory]/patterns.stil", false);
	Io::TestPatternExchangeFormat testPatternExport(*this->circuit, testPatterns, capture);
	Io::ExportPatterns(jsonPatternsHandle.GetOutStream(), testPatternExport);
	Io::ExportStilPatterns(*this->circuit, testPatterns,
		(capture == Pattern::InputCapture::PrimaryAndSecondaryInputs)
			? Io::StilPatternType::FullScan : Io::StilPatternType::Sequential,
		stilPatternsHandle.GetOutStream());
}

template <typename FaultModel, typename FaultList>
void AtpgBase<FaultModel, FaultList>::ExportFaultList(void) const
{
	FaultList faultListWithEquivalent;
	for (size_t index = 0; index < fullFaultList.size(); index++)
	{
		auto faultIndex = faultMapping[index];
		if (faultIndex != Fault::REMOVED_FAULT)
		{
			faultListWithEquivalent.push_back(fullFaultList[index]);
		}
	}

	FileHandle faultListHandle("[DataExportDirectory]/faults.json", false);
	FileHandle faultListWithEquivalentHandle("[DataExportDirectory]/faults.all.json", false);
	Io::FaultListExchangeFormat<FaultList> faultListExport { *this->circuit, faultList };
	Io::FaultListExchangeFormat<FaultList> faultListWithEquivalentExport { *this->circuit, faultListWithEquivalent };
	Io::ExportFaults(faultListHandle.GetOutStream(), faultListExport);
	Io::ExportFaults(faultListWithEquivalentHandle.GetOutStream(), faultListWithEquivalentExport);
}

template <typename FaultModel, typename FaultList>
std::string AtpgBase<FaultModel, FaultList>::DebugFaultLocation(const SimulationResult& atpgGoodResult, const SimulationResult& atpgBadResult, const SimulationResult& simGoodResult, const SimulationResult& simBadResult, size_t faultIndex) const
{
	// This function can be used to print additional information about the fault
	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		return "";
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
	{
		return "";
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		const auto& [fault, metaData] = faultList[faultIndex];

		std::string retStr = "Fault Condition(s):\n";
		for (const auto& [conditionNode, conditionPort] : fault->GetConditionNodesAndPorts())
		{
			size_t nodeId = conditionNode->GetDriverForPort(conditionPort)->GetNodeId();
			retStr += "Condition @node id " + std::to_string(conditionNode->GetNodeId()) + " @" + to_string(conditionPort) + " (driver is node id " + std::to_string(nodeId) + "):\n";
			std::string timeframeString = "timeframe:  ";
			std::string atpgGoodString 	= "atpg good: ";
			std::string atpgBadString 	= "atpg bad:  ";
			std::string simGoodString 	= "sim good:  ";
			std::string simBadString 	= "sim bad:   ";
			std::string missMatchString = "missmatch: ";
			for (size_t timeframe = 0u; timeframe < atpgGoodResult.GetNumberOfTimeframes(); ++timeframe)
			{
				bool missmatchGood = atpgGoodResult[timeframe][nodeId] != simGoodResult[timeframe][nodeId];
				bool missmatchBad = atpgBadResult[timeframe][nodeId] != simBadResult[timeframe][nodeId];
				timeframeString += std::to_string(timeframe) + " | ";
				atpgGoodString += 	(missmatchGood 	? ">" : " ") + to_string(atpgGoodResult[timeframe][nodeId]) 	+ (missmatchGood 	? "<" : " ") + "|";
				atpgBadString += 	(missmatchBad 	? ">" : " ") + to_string(atpgBadResult[timeframe][nodeId]) 	+ (missmatchBad 	? "<" : " ") + "|";
				simGoodString += 	(missmatchGood 	? ">" : " ") + to_string(simGoodResult[timeframe][nodeId]) 	+ (missmatchGood 	? "<" : " ") + "|";
				simBadString += 	(missmatchBad 	? ">" : " ") + to_string(simBadResult[timeframe][nodeId]) 	+ (missmatchBad 	? "<" : " ") + "|";
				missMatchString += std::string((missmatchGood || missmatchBad) ? "!!!" : "___") + "|";
			}
			retStr += timeframeString + "\n" + atpgGoodString + "\n" + atpgBadString + "\n" + simGoodString + "\n" + simBadString + "\n" + missMatchString + "\n\n";
		}

		retStr += "\nFault Location(s):\n";
		for (const auto& [effectNode, effectPort] : fault->GetEffectNodesAndPorts())
		{
			size_t nodeId = effectNode->GetNodeId();
			retStr += "Effect node @node " + std::to_string(nodeId) + " @" + to_string(effectPort) + ":\n";
			std::string timeframeString = "timeframe:  ";
			std::string atpgGoodString 	= "atpg good: ";
			std::string atpgBadString 	= "atpg bad:  ";
			std::string simGoodString 	= "sim good:  ";
			std::string simBadString 	= "sim bad:   ";
			std::string missMatchString = "missmatch: ";
			for (size_t timeframe = 0u; timeframe < atpgGoodResult.GetNumberOfTimeframes(); ++timeframe)
			{
				bool missmatchGood = atpgGoodResult[timeframe][nodeId] != simGoodResult[timeframe][nodeId];
				bool missmatchBad = atpgBadResult[timeframe][nodeId] != simBadResult[timeframe][nodeId];
				timeframeString += std::to_string(timeframe) + " | ";
				atpgGoodString += 	(missmatchGood 	? ">" : " ") + to_string(atpgGoodResult[timeframe][nodeId]) 	+ (missmatchGood 	? "<" : " ") + "|";
				atpgBadString += 	(missmatchBad 	? ">" : " ") + to_string(atpgBadResult[timeframe][nodeId]) 	+ (missmatchBad 	? "<" : " ") + "|";
				simGoodString += 	(missmatchGood 	? ">" : " ") + to_string(simGoodResult[timeframe][nodeId]) 	+ (missmatchGood 	? "<" : " ") + "|";
				simBadString += 	(missmatchBad 	? ">" : " ") + to_string(simBadResult[timeframe][nodeId]) 	+ (missmatchBad 	? "<" : " ") + "|";
				missMatchString += std::string((missmatchGood || missmatchBad)
										? "!!!"	: (atpgGoodResult[timeframe][nodeId] != atpgBadResult[timeframe][nodeId]) ? "EFF" : "___") + "|";
			}
			retStr += timeframeString + "\n" + atpgGoodString + "\n" + atpgBadString + "\n" + simGoodString + "\n" + simBadString + "\n" + missMatchString + "\n\n";
		}
		return retStr;
	}
	else
	{
		LOG(FATAL) << "Fault model not implemented";
		__builtin_unreachable();
	}
}

template class AtpgBase<SingleStuckAtFaultModel, SingleStuckAtFaultList>;
template class AtpgBase<SingleTransitionDelayFaultModel, SingleTransitionDelayFaultList>;
template class AtpgBase<CellAwareFaultModel, CellAwareFaultList>;

#define FOR_CONTAINER(CONTAINER) \
	template void AtpgBase<SingleStuckAtFaultModel, SingleStuckAtFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGB<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const; \
	template void AtpgBase<SingleStuckAtFaultModel, SingleStuckAtFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGBD<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const; \
	template void AtpgBase<SingleTransitionDelayFaultModel, SingleTransitionDelayFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGB<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const; \
	template void AtpgBase<SingleTransitionDelayFaultModel, SingleTransitionDelayFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGBD<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const; \
	template void AtpgBase<CellAwareFaultModel, CellAwareFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGB<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const; \
	template void AtpgBase<CellAwareFaultModel, CellAwareFaultList>::ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinDataGBD<CONTAINER>>& logicGenerator, const SimulationConfig& simConfig) const;
INSTANTIATE_ALL_CONTAINER

};
};
};
