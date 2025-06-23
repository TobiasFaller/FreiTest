#include "Applications/Scale4Edge/TestPatternGeneration/LfsrAtpg.hpp"

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
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
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
LfsrAtpg<FaultModel, FaultList>::LfsrAtpg(void):
	StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>()
{
}

template <typename FaultModel, typename FaultList>
LfsrAtpg<FaultModel, FaultList>::~LfsrAtpg(void) = default;

template <typename FaultModel, typename FaultList>
void LfsrAtpg<FaultModel, FaultList>::Init(void)
{
	AtpgBase<FaultModel, FaultList>::Init();

	_maxNumLfsrs = 10;
	_maxTimeFrames = 32;
	this->vcmEnable = VcmMixin::VcmEnable::Disabled;
}

template <typename FaultModel, typename FaultList>
void LfsrAtpg<FaultModel, FaultList>::Run(void)
{
	AtpgBase<FaultModel, FaultList>::Run();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	AtpgBase<FaultModel, FaultList>::GenerateFaultList();
	VLOG(6) << to_debug(this->faultList, this->circuit->GetMappedCircuit());

	LOG(INFO) << "Generating random test patterns for the given circuit";
	for (size_t lfsrId { 0u }; lfsrId < _maxNumLfsrs; ++lfsrId)
	{
		GeneratePattern(lfsrId);
	}

	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryInputsOnly);
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template <typename FaultModel, typename FaultList>
bool LfsrAtpg<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

Logic IntToLogic(int value)
{
	switch (value)
	{
	case 0:
		return Logic::LOGIC_ZERO;
	case 1:
		return Logic::LOGIC_ONE;
	}

	Logging::Panic();
}

std::string PolynomToString(const std::vector<size_t>& polynom)
{
	std::string result;
	for (auto const& value : polynom)
	{
		result += std::to_string(value);
	}
	return result;
}

template <typename FaultModel, typename FaultList>
void LfsrAtpg<FaultModel, FaultList>::GeneratePattern(size_t lfsrId)
{
	LOG(INFO) << "Generating test pattern in LFSR: " << lfsrId;

	const auto& mappedCircuit { this->circuit->GetMappedCircuit() };
	const auto primaryInputCount { mappedCircuit.GetNumberOfPrimaryInputs() };
	const auto secondaryInputCount { mappedCircuit.GetNumberOfSecondaryInputs() };
	mt19937 mt_rand { static_cast<mt19937::result_type>(chrono::high_resolution_clock::now().time_since_epoch().count()) };

	TestPattern pattern { _maxTimeFrames, primaryInputCount, secondaryInputCount, Logic::LOGIC_DONT_CARE };
	for (size_t index { 0u }; index < secondaryInputCount; ++index)
	{
		pattern.SetSecondaryInput(0u, index, Logic::LOGIC_ZERO);
	}

	// TODO: Implement fixed-value inputs
	const auto polynomSize { primaryInputCount };
	std::vector<size_t> polynom(polynomSize, 0u);
	std::vector<size_t> lfsr(polynomSize, 0u);
	for (size_t index { 0u }; index < polynomSize; index++)
	{
		polynom[index] = (index == polynomSize - 1u) ? 1u : mt_rand() % 2;
		lfsr[index] = mt_rand() % 2;
	}

	LOG(INFO) << "LFSR " << lfsrId << " has plynom " << PolynomToString(polynom) << " seed " << PolynomToString(lfsr);
	for (size_t timeframeId = 0; timeframeId < _maxTimeFrames; ++timeframeId)
	{
		if (timeframeId > 0u)
		{
			// lfsr computes itself unless timeframe = 0, then seed
			std::vector<size_t> tmp_lfsr(lfsr.begin(), lfsr.end());
			for (size_t index { 0u }; index < polynomSize; index++)
			{
				lfsr[index] = (index == 0u)
					? tmp_lfsr[polynomSize - 1u]
					: (tmp_lfsr[polynomSize - 1u] & polynom[index]) ^ tmp_lfsr[index - 1u];
			}
		}

		for (size_t index { 0u }; index < polynomSize; index++)
		{
			// TODO: Implement fixed-value inputs and mapping
			pattern.SetPrimaryInput(timeframeId, index, IntToLogic(lfsr[index]));
		}
	}

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

	size_t testPatternIndex = this->testPatterns.emplace_back(pattern);
	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, std::numeric_limits<size_t>::max(), testPatternIndex, Pattern::OutputCapture::PrimaryOutputsOnly, simConfig);

	size_t faultDetectCount { 0u };
	for(auto [fault, metaData] : this->faultList)
	{
		if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			faultDetectCount++;
		}
	}

	LOG(INFO) << "LFSR " << lfsrId << " detects " << faultDetectCount << " faults";
	LOG(INFO) << "LFSR " << lfsrId << " fault coverage is " << (100.0f * faultDetectCount / static_cast<float>(this->faultList.size())) << "%";

	FileHandle faultListFileHandle("[DataExportDirectory]/lfsr_" + std::to_string(lfsrId) + ".faults.jsonc", false);
	Io::FaultListExchangeFormat faultListExport { *this->circuit, this->faultList };
	Io::ExportFaults(faultListFileHandle.GetOutStream(), faultListExport);

	FileHandle patternFileHandle("[DataExportDirectory]/lfsr_" + std::to_string(lfsrId) + ".patterns.jsonc", false);
	Io::TestPatternExchangeFormat patternListExport { *this->circuit, this->testPatterns, Pattern::InputCapture::PrimaryInputsOnly };
	Io::ExportPatterns(patternFileHandle.GetOutStream(), patternListExport);
	AtpgBase<FaultModel, FaultList>::PrintStatistics();

	for(size_t index { 0u }; index < this->faultList.size(); index++)
	{
		AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, index, Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED, Fault::TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED);
	}
	this->testPatterns.clear();
}

template class LfsrAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class LfsrAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class LfsrAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
