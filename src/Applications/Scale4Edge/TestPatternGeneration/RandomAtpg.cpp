#include "Applications/Scale4Edge/TestPatternGeneration/RandomAtpg.hpp"

#include <boost/format.hpp>

#include <cstdint>
#include <execution>
#include <iostream>
#include <map>
#include <memory>
#include <thread>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/CircuitEnvironment.hpp"

using namespace std;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace FreiTest::Circuit;
using namespace FreiTest::Pattern;
using namespace FreiTest::Application::Mixin;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel, typename FaultList>
RandomAtpg<FaultModel, FaultList>::RandomAtpg():
	StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(),
	_numberOfPatterns(15u),
	_numberOfTimeframes(50u),
	_injectDontCares(false)
{
}

template <typename FaultModel, typename FaultList>
RandomAtpg<FaultModel, FaultList>::~RandomAtpg(void) = default;

template <typename FaultModel, typename FaultList>
void RandomAtpg<FaultModel, FaultList>::Init(void)
{
	AtpgBase<FaultModel, FaultList>::Init();
}

template <typename FaultModel, typename FaultList>
void RandomAtpg<FaultModel, FaultList>::Run(void)
{
	AtpgBase<FaultModel, FaultList>::Run();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	AtpgBase<FaultModel, FaultList>::GenerateFaultList();
	VLOG(6) << to_debug(this->faultList, this->circuit->GetMappedCircuit());

	LOG(INFO) << "Generating random test patterns for the given circuit";
	Parallel::ExecuteParallel(0u, _numberOfPatterns, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t iteration) {
		GeneratePattern(iteration);
	});

	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryInputsOnly);
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template <typename FaultModel, typename FaultList>
bool RandomAtpg<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/TestPatternGeneration/NumberOfPatterns")
	{
		return Settings::ParseSizet(value, _numberOfPatterns);
	}
	if (key == "Scale4Edge/TestPatternGeneration/NumberOfTimeframes")
	{
		return Settings::ParseSizet(value, _numberOfTimeframes);
	}
	if (key == "Scale4Edge/TestPatternGeneration/InjectDontCare")
	{
		_injectDontCares = (value == "Yes");
		return true;
	}

	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
Logic RandomAtpg<FaultModel, FaultList>::IntToLogic(int value)
{
	switch (value)
	{
	case 0u:
		return Logic::LOGIC_ZERO;
	case 1u:
		return Logic::LOGIC_ONE;
	case 2u:
		return Logic::LOGIC_DONT_CARE;
	}

	Logging::Panic();
}

template <typename FaultModel, typename FaultList>
void RandomAtpg<FaultModel, FaultList>::GeneratePattern(size_t iteration)
{
	const auto& mappedCircuit = this->circuit->GetMappedCircuit();
	const auto numberOfPrimaryInputs = this->circuit->GetMappedCircuit().GetNumberOfPrimaryInputs();

	VLOG(3) << "Generating test pattern in iteration: " << iteration;

	mt19937 mt_rand(iteration);

	TestPattern pattern(_numberOfTimeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs(), Logic::LOGIC_DONT_CARE);
	for (size_t secondaryInput { 0u }; secondaryInput < mappedCircuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
	{
		pattern.SetSecondaryInput(0u, secondaryInput, Logic::LOGIC_ZERO);
	}

	for (size_t timeframe { 0u }; timeframe < _numberOfTimeframes; ++timeframe)
	{
		for (size_t primaryInput { 0u }; primaryInput < numberOfPrimaryInputs; primaryInput++)
		{
			auto intValue = mt_rand() % (_injectDontCares ? 3u : 2u);
			auto logicValue = IntToLogic(intValue);
			pattern.SetPrimaryInput(timeframe, primaryInput, logicValue);
		}
	}

	VLOG(3) << "Test pattern: " << to_string(pattern);
	size_t testPatternIndex = this->testPatterns.emplace_back(pattern);

	Tpg::Vcm::VcmContext vcmContext { "pattern_generation", "Pattern Generation" };
	vcmContext.SetTargetStartState(std::vector<Basic::Logic>(this->circuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_DONT_CARE));
	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled)
	{
		vcmContext.SetVcmStartState(std::vector<Basic::Logic>(this->vcmCircuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_ZERO));
	}
	vcmContext.AddTags(this->vcmTags);
	vcmContext.AddVcmParameters(this->vcmParameters);
	if (auto it = this->vcmConfigurations.find(this->vcmConfiguration); it != this->vcmConfigurations.end())
	{
		vcmContext.AddTags(it->second.GetTags());
		vcmContext.AddVcmParameters(it->second.GetParameters());
	}

	LOG(INFO) << "Simulating test pattern " << iteration;
	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, std::numeric_limits<size_t>::max(), testPatternIndex, Pattern::OutputCapture::PrimaryOutputsOnly, simConfig);
}

template class RandomAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class RandomAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class RandomAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
