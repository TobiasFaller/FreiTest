#include "Applications/Scale4Edge/FaultCompaction/SatStaticFaultCompaction.hpp"

#include <boost/format.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"

using namespace SolverProxy;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

SatCompactionData<Fault::SingleStuckAtFaultModel>::SatCompactionData(std::string configPrefix)
{
}
SatCompactionData<Fault::SingleStuckAtFaultModel>::~SatCompactionData(void) = default;

bool SatCompactionData<Fault::SingleStuckAtFaultModel>::SetSetting(std::string key, std::string value)
{
	return false;
}

void SatCompactionData<Fault::SingleStuckAtFaultModel>::Init(void)
{
}

void SatCompactionData<Fault::SingleStuckAtFaultModel>::Run(void)
{
}

SatCompactionData<Fault::SingleTransitionDelayFaultModel>::SatCompactionData(std::string configPrefix)
{
}
SatCompactionData<Fault::SingleTransitionDelayFaultModel>::~SatCompactionData(void) = default;

bool SatCompactionData<Fault::SingleTransitionDelayFaultModel>::SetSetting(std::string key, std::string value)
{
	return false;
}

void SatCompactionData<Fault::SingleTransitionDelayFaultModel>::Init(void)
{
}

void SatCompactionData<Fault::SingleTransitionDelayFaultModel>::Run(void)
{
}

SatCompactionData<Fault::CellAwareFaultModel>::SatCompactionData(std::string configPrefix):
	Mixin::UdfmMixin(configPrefix)
{
}
SatCompactionData<Fault::CellAwareFaultModel>::~SatCompactionData(void) = default;

bool SatCompactionData<Fault::CellAwareFaultModel>::SetSetting(std::string key, std::string value)
{
	return UdfmMixin::SetSetting(key, value);
}

void SatCompactionData<Fault::CellAwareFaultModel>::Init(void)
{
	UdfmMixin::Init();
}

void SatCompactionData<Fault::CellAwareFaultModel>::Run(void)
{
	UdfmMixin::Run();
}

template <typename FaultModel, typename FaultList>
SatStaticFaultCompaction<FaultModel, FaultList>::SatStaticFaultCompaction(void):
	BaseApplication(),
	SatCompactionData<FaultModel>("Scale4Edge/FaultCompaction")
{
}

template <typename FaultModel, typename FaultList>
SatStaticFaultCompaction<FaultModel, FaultList>::~SatStaticFaultCompaction(void) = default;

template <typename FaultModel, typename FaultList>
void SatStaticFaultCompaction<FaultModel, FaultList>::Init(void)
{
}

template <typename FaultModel, typename FaultList>
void SatStaticFaultCompaction<FaultModel, FaultList>::Run(void)
{
	LOG(INFO) << "Importing test patterns";

	FileHandle patternFileHandle("[DataImportDirectory]/patterns.json", true);
	auto patternResult = Io::ImportPatterns(patternFileHandle.GetStream(), *this->circuit);
	if (!patternResult) LOG(FATAL) << "Test patterns could not be read";

	FileHandle importMetaDataHandle("[DataImportDirectory]/faults.json", true);
	auto faultResult = [&]() -> std::optional<Io::FaultListExchangeFormat<FaultList>> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>
			|| std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return Io::ImportFaults<FaultList>(importMetaDataHandle.GetStream(), *this->circuit);
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			ASSERT(this->GetUdfm()) << "No User-Defined Fault Model (UDFM) was loaded";
			return Io::ImportFaults<FaultList>(importMetaDataHandle.GetStream(), *this->circuit, *this->GetUdfm());
		}
		else
		{
			Logging::Panic("Unknown fault model");
		}
	}();
	if (!faultResult) LOG(FATAL) << "Fault data could not be read";

	auto importedPatterns { patternResult->GetTestPatterns() };
	auto capture { patternResult->GetInputCapture() };

	LOG(INFO) << "Imported " << importedPatterns.size() << " patterns";
	for (size_t pattern { 0u }; pattern < importedPatterns.size(); ++pattern)
	{
		LOG(INFO) << "Imported Pattern " << pattern << ": " << to_string(*importedPatterns[pattern]);
	}

	if (capture == Pattern::InputCapture::PrimaryAndSecondaryInputs
		|| capture == Pattern::InputCapture::SecondaryInputsOnly)
	{
		LOG_IF(std::any_of(importedPatterns.begin(), importedPatterns.end(), [](auto pattern) {
				return pattern->GetNumberOfTimeframes() > 1u;
			}), FATAL) << "There exist test patterns with more than one timeframe. Use the sequential mode for multi-timeframe patterns.";
	}
	else if (capture == Pattern::InputCapture::PrimaryInputsOnly
		|| capture == Pattern::InputCapture::PrimaryAndInitialSecondaryInputs)
	{
		LOG_IF(std::none_of(importedPatterns.begin(), importedPatterns.end(), [](auto pattern) {
				return pattern->GetNumberOfTimeframes() > 1u;
			}), WARNING) << "There is no test pattern with more than one timeframe. Use the full-scan mode for single-timeframe full-scan patterns";
	}
	else
	{
		Logging::Panic("Capture mode is currently not fully supported");
	}

	auto exportedPatterns { CompactPatternList(importedPatterns, faultResult->GetFaults(), capture) };
	LOG(INFO) << "Exporting " << importedPatterns.size() << " patterns";
	for (size_t pattern { 0u }; pattern < exportedPatterns.size(); ++pattern)
	{
		LOG(INFO) << "Exporting Pattern " << pattern << ": " << to_string(*exportedPatterns[pattern]);
	}

	LOG(INFO) << "Exporting test patterns";
	FileHandle exportPatternHandle("[DataExportDirectory]/patterns.json", false);
	Io::ExportPatterns(exportPatternHandle.GetOutStream(), { *this->circuit, exportedPatterns, capture });

	FileHandle exportMetaDataHandle("[DataExportDirectory]/faults.json", false);
	Io::FaultListExchangeFormat<FaultList> faultData { *this->circuit, faultResult->GetFaults() };
	Io::ExportFaults(exportMetaDataHandle.GetOutStream(), faultData);
}

template <typename FaultModel, typename FaultList>
Pattern::TestPatternList SatStaticFaultCompaction<FaultModel, FaultList>::CompactPatternList(Pattern::TestPatternList &patternList, const FaultList &faultList, Pattern::InputCapture capture)
{
	std::shared_ptr<Sat::MaxSatSolverProxy> solver = Sat::MaxSatSolverProxy::CreateMaxSatSolver(Sat::SatSolver::PROD_MAX_SAT_PACOSE);
	Tpg::LogicEncoder encoder { solver };

	LOG(INFO) << "Generating the pattern literals";
	std::vector<Tpg::LogicContainer01> patternLiterals;
	std::vector<std::vector<bool>> patternFaultCoverage;
	for (auto const& pattern : patternList)
	{
		patternLiterals.push_back(encoder.NewLogicContainer<Tpg::LogicContainer01>());
		patternFaultCoverage.emplace_back(faultList.size(), false);
	}

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

	LOG(INFO) << "Computing coverage matrix";
	// Create the fault coverage matrix
	Parallel::ExecuteParallel(0, patternList.size(), Parallel::Arena::FaultSimulation, Parallel::Order::Parallel, [&](size_t index) {
		RunFaultSimulation(patternList[index], faultList, patternFaultCoverage[index], outputCapture);
	});

	LOG(INFO) << "Encoding coverage matrix";
	// Convert the coverage matrix into a SAT problem where
	// all testable faults are required to be detected.
	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		auto [fault, metadata] = faultList[faultIndex];

		std::vector<Tpg::LogicContainer01> detectingPatterns;
		for (size_t patternIndex { 0u }; patternIndex < patternList.size(); ++patternIndex)
		{
			if (patternFaultCoverage[patternIndex][faultIndex])
			{
				detectingPatterns.push_back(patternLiterals[patternIndex]);
			}
		}

		if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			ASSERT(detectingPatterns.size() == 0u) << "The fault " << faultIndex << " was marked undetectable, but is detected by a test pattern.";
			continue;
		}

		ASSERT(detectingPatterns.size() > 0u) << "The fault " << faultIndex << " was marked testable, but was not detected by a test pattern.";
		if (detectingPatterns.size() == 0u)
		{
			continue;
		}

		// Require the fault to be detected by at least one pattern.
		auto detectionLiteral = encoder.EncodeOr(detectingPatterns);
		encoder.EncodeLogicValue(detectionLiteral, Basic::Logic::LOGIC_ONE);
	}

	LOG(INFO) << "Encoding maximization literals";
	// Now add all pattern literals as maximization literal to
	// maximize the number of patterns that are not required.
	for (auto& patternLiteral : patternLiterals)
	{
		auto zeroDetector { encoder.EncodeLogicValueDetector(patternLiteral, Basic::Logic::LOGIC_ZERO) };
		solver->CommitSoftClause(zeroDetector.l0);
	}

	LOG(INFO) << "Solving problem instance";
	auto solverResult = solver->MaxSolve();
	ASSERT(solverResult == Sat::SatResult::SAT) << "The solver returned an invalid result (" << solverResult << ")!";

	Pattern::TestPatternList resultList;
	for (size_t patternIndex = 0u; patternIndex < patternList.size(); ++patternIndex)
	{
		auto value = encoder.GetSolvedLogicValue(patternLiterals[patternIndex]);
		ASSERT (value == Basic::Logic::LOGIC_ZERO || value == Basic::Logic::LOGIC_ONE) << "Solver returned invalid result for pattern " << patternIndex;

		LOG(INFO) << "Test pattern " << patternIndex << " is " << ((value == Basic::Logic::LOGIC_ZERO) ? "not " : "") << "required";
		if (value == Basic::Logic::LOGIC_ONE)
		{
			resultList.push_back(patternList[patternIndex]);
		}
	}

	return resultList;
}

template <typename FaultModel, typename FaultList>
void SatStaticFaultCompaction<FaultModel, FaultList>::RunFaultSimulation(std::shared_ptr<Pattern::TestPattern> pattern, const FaultList &faultList, std::vector<bool>& faultCoverage, Pattern::OutputCapture capture)
{
	const Circuit::MappedCircuit& circuit { this->circuit->GetMappedCircuit() };

	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	Simulation::SimulationResult goodResult { pattern->GetNumberOfTimeframes(), circuit.GetNumberOfNodes() };
	Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(circuit, *pattern, { }, goodResult, simConfig);

	Simulation::SimulationResult badResult { pattern->GetNumberOfTimeframes(), circuit.GetNumberOfNodes() };
	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		auto [fault, metadata] = faultList[faultIndex];

		badResult.ReplaceWith(goodResult);
		Simulation::SimulateTestPatternEventDrivenIncremental<FaultModel>(circuit, *pattern, { fault }, std::as_const(goodResult), badResult, simConfig);

		const auto [testPrimaryOutputs, testSecondaryOutputs ] = Pattern::GetCaptureOutputs(capture);
		for (size_t timeframe = 0u; timeframe < pattern->GetNumberOfTimeframes(); ++timeframe)
		{
			for (size_t index = 0u; testPrimaryOutputs && index < circuit.GetNumberOfPrimaryOutputs(); ++index)
			{
				const auto* primaryOutput = circuit.GetPrimaryOutput(index);
				const auto good = goodResult.GetOutputLogic(primaryOutput, timeframe);
				const auto bad = badResult.GetOutputLogic(primaryOutput, timeframe);
				const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);

				if (__builtin_expect(difference, false))
				{
					faultCoverage[faultIndex] = true;
					goto nextFault;
				}
			}

			for (size_t index = 0; testSecondaryOutputs && index < circuit.GetNumberOfSecondaryOutputs(); ++index)
			{
				const auto* secondaryOutput = circuit.GetSecondaryOutput(index);
				const auto good = goodResult.GetOutputLogic(secondaryOutput, timeframe);
				const auto bad = badResult.GetOutputLogic(secondaryOutput, timeframe);
				const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && bad != good;

				if (__builtin_expect(difference, false))
				{
					faultCoverage[faultIndex] = true;
					goto nextFault;
				}
			}
		}

	nextFault:
		;
	}
}

template class SatStaticFaultCompaction<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class SatStaticFaultCompaction<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class SatStaticFaultCompaction<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
