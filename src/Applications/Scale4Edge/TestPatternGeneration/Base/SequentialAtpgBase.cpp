#include "Applications/Scale4Edge/TestPatternGeneration/Base/SequentialAtpgBase.hpp"

#include <boost/tokenizer.hpp>

#include <memory>
#include <string>

#include "Basic/CpuClock.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Simulation/CircuitSimulator.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Pattern;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
SequentialAtpgBase<FaultModel, FaultList>::SequentialAtpgBase(void):
	Mixin::StatisticsMixin("UNINITIALIZED"),
	Mixin::FaultStatisticsMixin<FaultList>("UNINITIALIZED"),
	Mixin::SimulationStatisticsMixin("UNINITIALIZED"),
	Mixin::SolverStatisticsMixin("UNINITIALIZED"),
	Mixin::VcdExportMixin<FaultList>("UNINITIALIZED"),
	Mixin::VcmMixin("UNINITIALIZED"),
	AtpgBase<FaultModel, FaultList>(),
	settingsCombinationalTestabilityCheck(CombinationalTestabilityCheck::Enabled),
	settingsMaximumTimeframes(10u)
{
}

template<typename FaultModel, typename FaultList>
SequentialAtpgBase<FaultModel, FaultList>::~SequentialAtpgBase(void) = default;

template<typename FaultModel, typename FaultList>
bool SequentialAtpgBase<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/TestPatternGeneration/CombinationalTestabilityCheck")
	{
		return Settings::ParseEnum(value, settingsCombinationalTestabilityCheck, {
			{ "Disabled", CombinationalTestabilityCheck::Disabled },
			{ "Enabled", CombinationalTestabilityCheck::Enabled },
		});
	}
	if (key == "Scale4Edge/TestPatternGeneration/SolverMaximumTimeframes")
	{
		return Settings::ParseSizet(value, settingsMaximumTimeframes);
	}

	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template<typename FaultModel, typename FaultList>
bool SequentialAtpgBase<FaultModel, FaultList>::SimulateTestPattern(size_t faultIndex, const TestPattern& pattern)
{
	const MappedCircuit &circuit = this->circuit->GetMappedCircuit();
	const auto [fault, metadata] = this->faultList[faultIndex];

	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	Simulation::SimulationResult goodResult(pattern.GetNumberOfTimeframes(), circuit.GetNumberOfNodes());
	Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(circuit, pattern, { }, goodResult, simConfig);

	Simulation::SimulationResult badResult(pattern.GetNumberOfTimeframes(), circuit.GetNumberOfNodes());
	switch(this->incrementalSimulation)
	{
		case AtpgBase<FaultModel, FaultList>::IncrementalSimulation::Disabled:
			badResult.Fill(Logic::LOGIC_DONT_CARE);
			Simulation::SimulateTestPatternEventDriven<FaultModel>(circuit, pattern, { fault }, badResult, simConfig);
			break;
		case AtpgBase<FaultModel, FaultList>::IncrementalSimulation::Enabled:
			badResult.ReplaceWith(goodResult);
			Simulation::SimulateTestPatternEventDrivenIncremental<FaultModel>(circuit, pattern, { fault }, std::as_const(goodResult), badResult, simConfig);
			break;
	}

	if (__builtin_expect(this->checkSimulation == AtpgBase<FaultModel, FaultList>::CheckSimulation::Enabled, false))
	{
		Simulation::SimulationResult checkResult(pattern.GetNumberOfTimeframes(), circuit.GetNumberOfNodes());
		Simulation::SimulateTestPatternNaive<FaultModel>(circuit, pattern, { fault }, checkResult, simConfig);
		for (size_t timeframe = 0u; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
		{
			for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
			{
				ASSERT(badResult[timeframe][nodeId] == checkResult[timeframe][nodeId])
					<< "The event-based and naive simulation result did not match for gate "
					<< circuit.GetNode(nodeId)->GetName() << ". Good: "
					<< to_string(goodResult[timeframe][nodeId]) << ", Bad: "
					<< to_string(badResult[timeframe][nodeId]) << ", Bad (Check): "
					<< to_string(checkResult[timeframe][nodeId]);
			}
		}
	}

	DVLOG(9) << "Good result: " << to_string(goodResult, circuit);
	DVLOG(9) << "Bad result: " << to_string(badResult, circuit);

	for (size_t timeframe = 0u; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
	{
		for (size_t index = 0u; index < circuit.GetNumberOfPrimaryOutputs(); ++index)
		{
			const MappedNode *primaryOutput = circuit.GetPrimaryOutput(index);
			const Logic good = goodResult.GetOutputLogic(primaryOutput, timeframe);
			const Logic bad = badResult.GetOutputLogic(primaryOutput, timeframe);
			const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);
			if (difference)
			{
				return true;
			}
		}
	}

	return false;
}

template class SequentialAtpgBase<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class SequentialAtpgBase<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class SequentialAtpgBase<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
