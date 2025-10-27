#include "Applications/Fuzzing/FaultSimulationFuzzing.hpp"

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Parallel.hpp"
#include "Simulation/CircuitSimulator.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Fault;

namespace FreiTest
{
namespace Application
{
namespace Fuzzing
{

const std::string optionPrefix = "Fuzzing";

template<typename FaultModel, typename FaultList>
FaultSimulationFuzzing<FaultModel, FaultList>::FaultSimulationFuzzing(void):
	Mixin::StatisticsMixin(optionPrefix),
	Mixin::FaultStatisticsMixin<FaultList>(optionPrefix),
	Mixin::SimulationStatisticsMixin(optionPrefix),
	Mixin::SolverStatisticsMixin(optionPrefix),
	Mixin::VcdExportMixin<FaultList>(optionPrefix),
	Mixin::VcmMixin(optionPrefix),
	Scale4Edge::AtpgBase<FaultModel, FaultList>(optionPrefix),
	seed(0u),
	timeframes(100u),
	iterations(5u),
	stopped(false)
{
}

template<typename FaultModel, typename FaultList>
FaultSimulationFuzzing<FaultModel, FaultList>::~FaultSimulationFuzzing(void) = default;

template<typename FaultModel, typename FaultList>
bool FaultSimulationFuzzing<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
    if (Settings::IsOption(key, "Seed", optionPrefix))
    {
		return Settings::ParseSizet(value, seed);
    }
    if (Settings::IsOption(key, "Timeframes", optionPrefix))
    {
        return Settings::ParseSizet(value, timeframes);
    }
    if (Settings::IsOption(key, "Iterations", optionPrefix))
    {
        return Settings::ParseSizet(value, iterations);
    }

    return Scale4Edge::AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template<typename FaultModel, typename FaultList>
void FaultSimulationFuzzing<FaultModel, FaultList>::Init(void)
{
	Scale4Edge::AtpgBase<FaultModel, FaultList>::Init();
}

template<typename FaultModel, typename FaultList>
void FaultSimulationFuzzing<FaultModel, FaultList>::Run(void)
{
	Scale4Edge::AtpgBase<FaultModel, FaultList>::Run();
	Scale4Edge::AtpgBase<FaultModel, FaultList>::GenerateFaultList();

	auto const& mappedCircuit { this->circuit->GetMappedCircuit() };
	auto const simulate_for_index = [&](size_t index) -> void
	{
		if (stopped.load())
		{
			return;
		}

		auto [fault, metadata] = this->faultList[index];
		LOG(INFO) << "Simulating " << to_string(*fault);

		std::mt19937 random { seed };
		for (size_t iteration { 0u }; iteration < iterations; iteration++)
		{
			Pattern::TestPattern pattern { timeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs(), Logic::LOGIC_DONT_CARE };
			for (size_t input { 0u }; input < mappedCircuit.GetNumberOfSecondaryInputs(); input++)
			{
				pattern.SetSecondaryInput(0u, input, (random() % 2u) ? Logic::LOGIC_ZERO : Logic::LOGIC_ONE);
			}
			for (size_t timeframe { 0u }; timeframe < timeframes; timeframe++)
			{
				for (size_t input { 0u }; input < mappedCircuit.GetNumberOfPrimaryInputs(); input++)
				{
					pattern.SetPrimaryInput(timeframe, input, (random() % 2u) ? Logic::LOGIC_ZERO : Logic::LOGIC_ONE);
				}
			}

			Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
			Simulation::SimulationResult goodNativeResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			Simulation::SimulationResult goodEventResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, pattern, { }, goodNativeResult, simConfig);
			Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, pattern, { }, goodEventResult, simConfig);
			ReportIfInequal(goodNativeResult, goodEventResult, "good native", "good event");

			Simulation::SimulationResult badNativeResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			Simulation::SimulationResult badEventResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			Simulation::SimulationResult badIncrementalResult(pattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			badIncrementalResult.ReplaceWith(goodNativeResult);
			Simulation::SimulateTestPatternNaive<FaultModel>(mappedCircuit, pattern, { fault }, badNativeResult, simConfig);
			Simulation::SimulateTestPatternEventDriven<FaultModel>(mappedCircuit, pattern, { fault }, badEventResult, simConfig);
			Simulation::SimulateTestPatternEventDrivenIncremental<FaultModel>(mappedCircuit, pattern, { fault }, std::as_const(goodNativeResult), badIncrementalResult, simConfig);
			ReportIfInequal(badNativeResult, badEventResult, "bad native", "bad event");
			ReportIfInequal(badNativeResult, badIncrementalResult, "bad native", "bad incremental");
		}
	};

	Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd, Parallel::Arena::FaultSimulation, Parallel::Order::Parallel, simulate_for_index);
	Logging::ClearCurrentFault();
}

template<typename FaultModel, typename FaultList>
void FaultSimulationFuzzing<FaultModel, FaultList>::ReportIfInequal(const Simulation::SimulationResult& resultA, const Simulation::SimulationResult& resultB, const std::string& nameA, const std::string& nameB)
{
	auto const& mappedCircuit { this->circuit->GetMappedCircuit() };
	for (size_t timeframe { 0u }; timeframe < resultA.GetNumberOfTimeframes(); timeframe++)
	{
		for (size_t nodeId { 0u }; nodeId < mappedCircuit.GetNumberOfNodes(); nodeId++)
		{
			if (resultA[timeframe][nodeId] == resultB[timeframe][nodeId])
			{
				continue;
			}

			bool expected = false;
			if (!stopped.compare_exchange_strong(expected, true))
			{
				// Do not report as another thread has found and printed an issue before.
				return;
			}

			LOG(FATAL) << "The " << nameA << " vs " << nameB << " simulation result did not match for gate "
				<< mappedCircuit.GetNode(nodeId)->GetName() << ", timeframe " << timeframe << ". "
				<< nameA << ": " << to_string(resultA[timeframe][nodeId]) << ", "
				<< nameB << ": " << to_string(resultB[timeframe][nodeId]) << std::endl
				<< nameA << ": " << to_string(resultA) << std::endl
				<< nameB << ": " << to_string(resultB);
		}
	}
}

template class FaultSimulationFuzzing<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class FaultSimulationFuzzing<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class FaultSimulationFuzzing<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
