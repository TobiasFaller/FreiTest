#include "Applications/Scale4Edge/TestPatternExport/TestPatternsToStatistics.hpp"

#include <boost/format.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"
#include "Io/VcdExporter/VcdModelBuilder.hpp"
#include "Io/VcdExporter/VcdExporter.hpp"
#include "Io/VcdExporter/VcdModel.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"

using namespace std;
using namespace FreiTest::Io::Vcd;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
TestPatternsToStatistics<FaultModel, FaultList>::TestPatternsToStatistics(void):
	Mixin::StatisticsMixin("Scale4Edge/TestPatternsToStatistics"),
	Mixin::FaultStatisticsMixin<FaultList>("Scale4Edge/TestPatternsToStatistics"),
	Mixin::SimulationStatisticsMixin("Scale4Edge/TestPatternsToStatistics"),
	Mixin::SolverStatisticsMixin("Scale4Edge/TestPatternsToStatistics"),
	Mixin::VcdExportMixin<FaultList>("Scale4Edge/TestPatternsToStatistics"),
	Mixin::VcmMixin("Scale4Edge/TestPatternsToStatistics"),
	AtpgBase<FaultModel, FaultList>()
{
}

template<typename FaultModel, typename FaultList>
TestPatternsToStatistics<FaultModel, FaultList>::~TestPatternsToStatistics(void) = default;

template<typename FaultModel, typename FaultList>
void TestPatternsToStatistics<FaultModel, FaultList>::Init(void)
{
	AtpgBase<FaultModel, FaultList>::Init();
}

template<typename FaultModel, typename FaultList>
void TestPatternsToStatistics<FaultModel, FaultList>::Run(void)
{
	AtpgBase<FaultModel, FaultList>::Run();
	AtpgBase<FaultModel, FaultList>::GenerateFaultList();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	FileHandle patternFileHandle("[DataImportDirectory]/patterns.json", true);
	auto patternResult = Io::ImportPatterns(patternFileHandle.GetStream(), *this->circuit);
	if (!patternResult) LOG(FATAL) << "Test patterns could not be read";

	Tpg::Vcm::VcmContext vcmContext { "pattern_generation", "Pattern Generation" };
	vcmContext.AddTags(this->vcmTags);
	vcmContext.AddVcmParameters(this->vcmParameters);
	if (auto it = this->vcmConfigurations.find(this->vcmConfiguration); it != this->vcmConfigurations.end())
	{
		vcmContext.AddTags(it->second.GetTags());
		vcmContext.AddVcmParameters(it->second.GetParameters());
	}
	auto const inputCapture = (*patternResult).GetInputCapture();
	auto const outputCapture = [&]() -> Pattern::OutputCapture {
		switch (inputCapture)
		{
		case Pattern::InputCapture::PrimaryInputsOnly: return Pattern::OutputCapture::PrimaryOutputsOnly;
		case Pattern::InputCapture::SecondaryInputsOnly: return Pattern::OutputCapture::SecondaryOutputsOnly;
		case Pattern::InputCapture::PrimaryAndSecondaryInputs: return Pattern::OutputCapture::PrimaryAndSecondaryOutputs;
		case Pattern::InputCapture::PrimaryAndInitialSecondaryInputs: return Pattern::OutputCapture::PrimaryOutputsOnly;
		default: Logging::Panic();
		}
	}();
	auto const sequentialMode = [&]() -> Simulation::SequentialMode {
		switch (inputCapture)
		{
		case Pattern::InputCapture::PrimaryInputsOnly: return Simulation::SequentialMode::Functional;
		case Pattern::InputCapture::SecondaryInputsOnly: return Simulation::SequentialMode::FullScan;
		case Pattern::InputCapture::PrimaryAndSecondaryInputs: return Simulation::SequentialMode::FullScan;
		case Pattern::InputCapture::PrimaryAndInitialSecondaryInputs: return Simulation::SequentialMode::Functional;
		default: Logging::Panic();
		}
	}();

	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	simConfig.sequentialMode = sequentialMode;
	this->faultSimulation = AtpgBase<FaultModel, FaultList>::FaultSimulation::Enabled;
	for (auto pattern : (*patternResult).GetTestPatterns())
	{
		size_t testPatternIndex = this->testPatterns.emplace_back(*pattern);
		LOG(INFO) << "Pattern "<< testPatternIndex;
		AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, std::numeric_limits<size_t>::max(), testPatternIndex, outputCapture, simConfig);
	}

	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryAndInitialSecondaryInputs);
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template<typename FaultModel, typename FaultList>
bool TestPatternsToStatistics<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/TestPatternsToStatistics/ExportThreadLimit")
	{
		return Settings::ParseSizet(value, ExportThreadLimit);
	}

	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template class TestPatternsToStatistics<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class TestPatternsToStatistics<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class TestPatternsToStatistics<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
