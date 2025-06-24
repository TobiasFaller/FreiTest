#include "Applications/Scale4Edge/FaultCoverageExport/FaultCoverageExport.hpp"

#include <string>
#include <vector>

#include "Basic/CpuClock.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Helper/FileHandle.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"
#include "Io/FaultCoverage/Coverage.hpp"
#include "Io/FaultCoverage/JsonExporter.hpp"
// freitest:private begin
#ifdef HAS_STAR_VISION
#include "Io/FaultCoverage/ZdbExporter.hpp"
#endif
// freitest:private end

#include <boost/algorithm/string.hpp>

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel, typename FaultList>
FaultCoverageExport<FaultModel, FaultList>::FaultCoverageExport(void):
	Mixin::StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	faultCoverageFileName(),
	exportThreadLimit(0),
	simulationThreadLimit(0),
	parallelMutex()
{
}

template <typename FaultModel, typename FaultList>
FaultCoverageExport<FaultModel, FaultList>::~FaultCoverageExport(void) = default;

template <typename FaultModel, typename FaultList>
void FaultCoverageExport<FaultModel, FaultList>::Run(void)
{
	FileHandle patternFileHandle("[DataImportDirectory]/patterns.json", true);
	auto patternResult = Io::ImportPatterns(patternFileHandle.GetStream(), *this->circuit);
	if (!patternResult) LOG(FATAL) << "Test patterns could not be read";

	FileHandle faultFileHandle("[DataImportDirectory]/faults.json", true);
	auto faultResult = [&]() -> std::optional<Io::FaultListExchangeFormat<FaultList>> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>
			|| std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return Io::ImportFaults<FaultList>(faultFileHandle.GetStream(), *this->circuit);
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			ASSERT(this->GetUdfm()) << "No User-Defined Fault Model (UDFM) was loaded";
			return Io::ImportFaults<FaultList>(faultFileHandle.GetStream(), *this->circuit, *this->GetUdfm());
		}
		else
		{
			Logging::Panic("Unknown fault model");
		}
	}();
	if (!faultResult) LOG(FATAL) << "Fault list could not be read";

	// Make sure to pre-allocate all data structures as they are concurrently accesses
	// and a reallocation would case a race-condition.
	Io::FaultCoverage::Coverage faultCoverage {};
	faultCoverage.pattern.resize(patternResult->GetNumberOfPatterns(), {});
	faultCoverage.input.resize(patternResult->GetNumberOfPatterns(), {});

	const Pattern::InputCapture capture = patternResult->GetInputCapture();
	Parallel::ExecuteParallel(0u, patternResult->GetNumberOfPatterns(), Parallel::Arena::General, Parallel::Order::Parallel, [&](size_t patternIndex) {
		LOG(INFO) << "    ... simulating testpattern " << (patternIndex + 1u) << "(" << patternResult->GetNumberOfPatterns() << ")";
		auto const& pattern { patternResult->GetPattern(patternIndex) };
		auto const coverage { Simulate(*pattern, capture, faultResult->GetFaults()) };
		for (size_t faultIndex { 0u }; faultIndex < faultResult->GetNumberOfFaults(); faultIndex++)
		{
			if (coverage[faultIndex])
			{
				auto [fault, metaData] = faultResult->GetFaults()[faultIndex];
				auto faultInfo = GetFaultInformation({ fault });

				std::scoped_lock lock { parallelMutex };
				faultCoverage.fault[faultInfo].push_back(patternIndex);
				faultCoverage.pattern[patternIndex].push_back(faultInfo);
			}
		}
	});
	LOG(INFO) << "Fault Coverage has been recorded: " << patternResult->GetNumberOfPatterns() << " test patterns have been simulated";

// freitest:private begin
#ifdef HAS_STAR_VISION
	LOG(INFO) << "Exporting the fault coverage as ZDB file ...";
	Io::FaultCoverage::ZdbExporter zdb;
	zdb.ExportFaultCoverage(*this->circuit, faultCoverage, faultCoverageFileName + ".zdb");
#endif
// freitest:private end

	LOG(INFO) << "Exporting the fault coverage as JSON file ...";
	Io::FaultCoverage::JsonExporter json;
	json.ExportFaultCoverage(*this->circuit, faultCoverage, faultCoverageFileName + ".json");
}

template <typename FaultModel, typename FaultList>
std::vector<bool> FaultCoverageExport<FaultModel, FaultList>::Simulate(const Pattern::TestPattern& testPattern, Pattern::InputCapture capture, const FaultList& faultList)
{
	const auto& circuit = this->circuit->GetMappedCircuit();
	std::vector<bool> result(faultList.size(), false);

	const Pattern::OutputCapture outputCapture = [&](){
		switch (capture)
		{
			case Pattern::InputCapture::PrimaryInputsOnly:
			case Pattern::InputCapture::PrimaryAndInitialSecondaryInputs:
				return Pattern::OutputCapture::PrimaryOutputsOnly;
			case Pattern::InputCapture::SecondaryInputsOnly:
				return Pattern::OutputCapture::SecondaryOutputsOnly;
			case Pattern::InputCapture::PrimaryAndSecondaryInputs:
				return Pattern::OutputCapture::PrimaryAndSecondaryOutputs;
			default:
				Logging::Panic("Unsupported capture operation mode");
		}
	}();

	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	Simulation::SimulationResult goodResult(testPattern.GetNumberOfTimeframes(), circuit.GetNumberOfNodes());
	Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(circuit, testPattern, {}, goodResult, simConfig);

	const auto& simulate_for_fault_range = [&](size_t startIndex, size_t endIndex) {
		Simulation::SimulationResult badResult(testPattern.GetNumberOfTimeframes(), circuit.GetNumberOfNodes(), Basic::Logic::LOGIC_UNKNOWN);

		for (size_t faultIndex = startIndex; faultIndex != endIndex; ++faultIndex)
		{
			// Check for already detected faults if not SimulateAllFaults option is enabled
			auto [fault, metadata] = faultList[faultIndex];
			DVLOG(5) << "Checking if test pattern finds " << to_string(*fault);

			// Do a pre-check if the fault is sensitized in at least one timeframe.
			// If this is not the case then we can skip the simulation as no fault propagation is possible.
			if (!AtpgBase<FaultModel, FaultList>::CheckSensitization({ fault }, goodResult))
			{
				// Fault is not sensitized -> test next fault
				continue;
			}

			badResult.ReplaceWith(goodResult);
			Simulation::SimulateTestPatternEventDrivenIncremental<FaultModel>(circuit, testPattern, { fault }, std::as_const(goodResult), badResult, simConfig);

			const auto [testPrimaryOutputs, testSecondaryOutputs ] = Pattern::GetCaptureOutputs(outputCapture);
			for (size_t timeframe = 0u; timeframe < testPattern.GetNumberOfTimeframes(); ++timeframe)
			{
				for (size_t index = 0u; testPrimaryOutputs && index < circuit.GetNumberOfPrimaryOutputs(); ++index)
				{
					const auto* primaryOutput = circuit.GetPrimaryOutput(index);
					const auto good = goodResult.GetOutputLogic(primaryOutput, timeframe);
					const auto bad = badResult.GetOutputLogic(primaryOutput, timeframe);
					const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);

					DVLOG(9) << "Primary Output " << index << ": " << "Good " << good << " <-> " << bad << " Bad"
						<< (difference ? " (Difference detected)" : "");
					if (__builtin_expect(difference, false))
					{
						result[faultIndex] = true;
						goto nextFault;
					}
				}

				for (size_t index = 0; testSecondaryOutputs && index < circuit.GetNumberOfSecondaryOutputs(); ++index)
				{
					const auto* secondaryOutput = circuit.GetSecondaryOutput(index);
					const auto good = goodResult.GetOutputLogic(secondaryOutput, timeframe);
					const auto bad = badResult.GetOutputLogic(secondaryOutput, timeframe);
					const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && bad != good;

					DVLOG(9) << "Secondary Output " << index << ": " << "Good " << good << " <-> " << bad << " Bad"
						<< (difference ? " (Difference detected)" : "");
					if (__builtin_expect(difference, false))
					{
						result[faultIndex] = true;
						goto nextFault;
					}
				}
			}

			nextFault:
				continue;
		}
	};

	Parallel::ExecuteParallelInBlocks(0, faultList.size(), 256u, Parallel::Arena::FaultSimulation, Parallel::Order::Parallel, simulate_for_fault_range);
	return result;
}

template <typename FaultModel, typename FaultList>
Basic::ApplicationStatistics FaultCoverageExport<FaultModel, FaultList>::GetStatistics(void)
{
	return std::move(statistics);
}

template <typename FaultModel, typename FaultList>
bool FaultCoverageExport<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/FaultCoverageExport/FaultCoverageFileName")
	{
		faultCoverageFileName = value;
		return true;
	}
	if (key == "Scale4Edge/FaultCoverageExport/SimulationThreadLimit")
	{
		return Settings::ParseSizet(value, simulationThreadLimit);
	}
	if (key == "Scale4Edge/FaultCoverageExport/ExportThreadLimit")
	{
		return Settings::ParseSizet(value, exportThreadLimit);
	}

	return false;
}

template <typename FaultModel, typename FaultList>
Io::FaultCoverage::FaultInformation FaultCoverageExport<FaultModel, FaultList>::GetFaultInformation(const FaultModel& faultModel)
{
	const auto& metaData = this->circuit->GetMetaData();
	const auto& fault = faultModel.GetFault();
	if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
	{
		auto location { metaData.GetFriendlyName(fault->GetNodeAndPort()) };
		switch (fault->GetType())
		{
			case Fault::StuckAtFaultType::STUCK_AT_0: return { location, "StuckAt0" };
			case Fault::StuckAtFaultType::STUCK_AT_1: return { location, "StuckAt1" };
			case Fault::StuckAtFaultType::STUCK_AT_U: return { location, "StuckAtU" };
			case Fault::StuckAtFaultType::STUCK_AT_X: return { location, "StuckAtX" };
			default: __builtin_unreachable();
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
	{
		auto location { metaData.GetFriendlyName(fault->GetNodeAndPort()) };
		switch (fault->GetType())
		{
			case Fault::TransitionDelayFaultType::SLOW_TO_RISE: return { location, "SlowToRise" };
			case Fault::TransitionDelayFaultType::SLOW_TO_FALL: return { location, "SlowToFall" };
			case Fault::TransitionDelayFaultType::SLOW_TO_TRANSITION: return { location, "SlowToTransition" };
			default: __builtin_unreachable();
		}
	}
	else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		auto location { fault->GetCell()->GetHierarchyName() };
		auto type { fault->GetUserDefinedFault()->GetFaultName() };
		return { location, type };
	}
}

template class FaultCoverageExport<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class FaultCoverageExport<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class FaultCoverageExport<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
