#include "Applications/Scale4Edge/TestPatternGeneration/BmcSequentialAtpg.hpp"

#include <boost/format.hpp>

#include <atomic>
#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Bmc/CipExportProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/PatternExclusionConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace FreiTest::Application::Mixin;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

using LogicContainer = Tpg::LogicContainer01X;
using PinData = Tpg::PinDataGBD<LogicContainer>;
using VcmPinData = Tpg::PinDataG<LogicContainer>;

template<typename FaultModel, typename PinData> struct AtpgConfig { };
template<typename PinData> struct AtpgConfig<Fault::SingleStuckAtFaultModel, PinData> {
	using FaultGenerator = Tpg::StuckAtFaultEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitization = Tpg::StuckAtSensitizationConstraintEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitizationTimeframe = Tpg::StuckAtSensitizationTimeframe;
	using FaultTagger = Tpg::StuckAtFaultCircuitTagger<PinData, Fault::SingleStuckAtFaultModel>;
};
template<typename PinData> struct AtpgConfig<Fault::SingleTransitionDelayFaultModel, PinData> {
	using FaultGenerator = Tpg::TransitionDelayFaultEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitization = Tpg::TransitionSensitizationConstraintEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitizationTimeframe = Tpg::TransitionSensitizationTimeframe;
	using FaultTagger = Tpg::TransitionDelayFaultCircuitTagger<PinData, Fault::SingleTransitionDelayFaultModel>;
};
template<typename PinData> struct AtpgConfig<Fault::CellAwareFaultModel, PinData> {
	using FaultGenerator = Tpg::CellAwareFaultEncoder<PinData>;
	using FaultSensitization = Tpg::CellAwareSensitizationConstraintEncoder<PinData>;
	using FaultSensitizationTimeframe = Tpg::CellAwareSensitizationTimeframe;
	using FaultTagger = Tpg::CellAwareFaultCircuitTagger<PinData>;
};

template <typename FaultModel, typename FaultList>
BmcSequentialAtpg<FaultModel, FaultList>::BmcSequentialAtpg():
	StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SequentialAtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG)
{
}

template <typename FaultModel, typename FaultList>
BmcSequentialAtpg<FaultModel, FaultList>::~BmcSequentialAtpg(void) = default;

template <typename FaultModel, typename FaultList>
void BmcSequentialAtpg<FaultModel, FaultList>::Init(void)
{
	SequentialAtpgBase<FaultModel, FaultList>::Init();
}

template <typename FaultModel, typename FaultList>
void BmcSequentialAtpg<FaultModel, FaultList>::Run(void)
{
	SequentialAtpgBase<FaultModel, FaultList>::Run();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	AtpgBase<FaultModel, FaultList>::GenerateFaultList();
	VLOG(6) << to_debug(this->faultList, this->circuit->GetMappedCircuit());

	LOG(INFO) << "Generating test patterns for " << (this->faultListEnd - this->faultListBegin) << " faults";
	if (this->settingsCombinationalTestabilityCheck == SequentialAtpgBase<FaultModel, FaultList>::CombinationalTestabilityCheck::Enabled)
	{
		// Check for untestable faults as the first step
		std::vector<bool> faultMask(this->faultList.size(), false);
		auto faultCoverage = AtpgBase<FaultModel, FaultList>::CheckCombinationalUntestability(this->faultList, faultMask);
		for (size_t faultIndex { this->faultListBegin }; faultIndex != this->faultListEnd; ++faultIndex)
		{
			if (faultCoverage[faultIndex] != Fault::FaultStatus::FAULT_STATUS_UNDETECTED)
			{
				continue;
			}

			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex,
				Fault::FaultStatus::FAULT_STATUS_UNDETECTED,
				Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL);
		}
	}

	// Generate test patterns in the first iteration where each pattern is generated in one step
	Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
		GeneratePatternForFault(index);
	});
	Logging::ClearCurrentFault();

	this->statistics.Add("Encoding.PatternGeneration.LogicContainer", std::string("LogicContainer") + get_logic_container_name<LogicContainer>, "Type", "LogicContainer used");
	this->statistics.Add("Encoding.PatternGeneration.PinData", std::string("PinData") + get_pin_data_name_v<PinData>, "Type", "PinData used");
	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryAndInitialSecondaryInputs);
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template <typename FaultModel, typename FaultList>
bool BmcSequentialAtpg<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	return SequentialAtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
void BmcSequentialAtpg<FaultModel, FaultList>::GeneratePatternForFault(size_t faultIndex)
{
	using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
	using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
	using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;

	Logging::SetCurrentFault(faultIndex);

	// Check for already detected faults
	auto [fault, metadata] = this->faultList[faultIndex];
	if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
	{
		return;
	}

	LOG(INFO) << "Generating test pattern for fault " << to_string(*fault);

	std::shared_ptr<Bmc::BmcSolverProxy> bmcSolver = Bmc::BmcSolverProxy::CreateBmcSolver(Settings::GetInstance()->BmcSolver);
	ASSERT(bmcSolver) << "Could not initialize BMC-Solver!";

	if (auto exporter = std::dynamic_pointer_cast<Bmc::CipExportProxy>(bmcSolver); exporter)
	{
		auto filename = Settings::GetInstance()->MapFileName("[DataExportDirectory]/" + this->circuit->GetName() + "_fault_" + std::to_string(faultIndex) + ".cip.xz");
		if (std::filesystem::exists(filename))
		{
			std::scoped_lock lock { this->parallelMutex };
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED, Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT);
			return; // Do not export twice
		}

		exporter->SetCompression(Bmc::CipExportProxy::Compression::Lzma);
		exporter->SetFilename(filename);
		exporter->SetComments({
			"Circuit: " + this->circuit->GetName(),
			"Fault: " + to_string(*fault),
			"Fault Index: " + std::to_string(faultIndex + 1u) + " / " + std::to_string(this->faultList.size()),
		});
	}

	bmcSolver->SetSolverTimeout(this->solverTimeout);
	bmcSolver->SetSolverDebug(false);
	bmcSolver->SetSolverSilent(true);
	bmcSolver->SetMaximumDepth(this->settingsMaximumTimeframes + 1u);

	auto faultModel = std::make_shared<FaultModel>(fault);
	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(bmcSolver, this->circuit);
	auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>>(bmcSolver, this->vcmCircuit);

	logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);

	// The BMC-solver requires the circuit to be encoded with good and bad encoding.
	logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, BadTag>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainCircuitTagger<PinData>>();

	// The BMC based approach requires an unary counter to be encoded to detect the current active timeframe.
	logicGenerator->template EmplaceModule<Tpg::UnaryCounterEncoder<PinData>>();

	// Common circuit logic encoding with sequential support.
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, BadTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, BadTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainBaseEncoder<PinData>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::BackwardDChainEncoder<PinData>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::BackwardDChainConnectionEncoder<PinData>>();

	// Pattern generation constraints consist of the initial state (could be replaced with end state of synchronization sequence at some point)
	// and the fault propagation and fault sensitization.
	logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
	logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
	logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, BadTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
	logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
	logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryOutputsOnly, Tpg::FaultPropagationTimeframe::Last);

	Tpg::Vcm::VcmContext vcmContext { "pattern_generation", "Pattern Generation" };
	vcmContext.SetTargetStartState(std::vector<Basic::Logic>(this->circuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_DONT_CARE));
	vcmContext.AddTags(this->vcmTags);
	vcmContext.AddVcmParameters(this->vcmParameters);
	if (auto it = this->vcmConfigurations.find(this->vcmConfiguration); it != this->vcmConfigurations.end())
	{
		vcmContext.AddTags(it->second.GetTags());
		vcmContext.AddVcmParameters(it->second.GetParameters());
	}

	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled)
	{
		vcmContext.SetVcmStartState(std::vector<Basic::Logic>(this->vcmCircuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_ZERO));

		// Encode the whole circuit with good encoding only as there is no fault model for the VCM circuit
		vcmLogicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::UnaryCounterEncoder<VcmPinData>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(logicGenerator, this->vcmInputs, vcmContext.GetVcmParameters());
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(this->vcmOutputs);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<VcmPinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(vcmContext.GetVcmStartState()), Tpg::ConstrainedSequentials::Inputs);
	}

	CpuClock cnfGenerationTimer;
	CpuClock bmcSolverTimer;

	LOG(INFO) << "Generating test pattern with maximum of " << this->settingsMaximumTimeframes << " timeframes";
	cnfGenerationTimer.SetTimeReference();
	logicGenerator->GetContext().SetNumberOfTimeframes(this->settingsMaximumTimeframes + 1u);
	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled) vcmLogicGenerator->GetContext().SetNumberOfTimeframes(this->settingsMaximumTimeframes + 1u);
	ASSERT(logicGenerator->GenerateCircuitLogic()) << "Invalid circuit tagging";
	ASSERT((this->vcmEnable == VcmMixin::VcmEnable::Disabled) || vcmLogicGenerator->GenerateCircuitLogic()) << "Invalid circuit tagging";
	cnfGenerationTimer.Stop();

	VLOG(3) << "Invoking BMC-Solver to find solution";
	bmcSolverTimer.SetTimeReference();
	bmcSolver->Solve();
	bmcSolverTimer.Stop();

	switch (bmcSolver->GetLastResult())
	{
		case Bmc::BmcResult::Reachable:
		{
			Pattern::TestPattern pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndInitialSecondaryInputs);
			VLOG(6) << "Generated test pattern: " << to_string(pattern);

			Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
			if (this->checkAtpgResult == AtpgBase<FaultModel, FaultList>::CheckAtpgResult::CheckEqual)
			{
				AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(faultIndex, pattern, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, *logicGenerator, simConfig);
			}

			size_t testPatternIndex = this->testPatterns.emplace_back(pattern);
			AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, faultIndex, testPatternIndex, Pattern::OutputCapture::PrimaryOutputsOnly, simConfig);

			if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_DETECTED)
			{
				LOG(FATAL) << "Invalid test pattern was generated";
			}
			goto nextFault;
		}

		case Bmc::BmcResult::Unreachable:
		case Bmc::BmcResult::MaxIterationsReached:
		{
			LOG(INFO) << "No test pattern can be created (" << bmcSolver->GetLastResult() << ")";

			std::scoped_lock lock { this->parallelMutex };
			if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
			{
				AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex,
					(bmcSolver->GetLastResult() == Bmc::BmcResult::Unreachable)
						? Fault::FaultStatus::FAULT_STATUS_UNDETECTED
						: Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED,
					(bmcSolver->GetLastResult() == Bmc::BmcResult::Unreachable)
						? Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE
						: Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS);
			}
			goto nextFault;
		}

		default:
		case Bmc::BmcResult::Timeout:
		{
			LOG(WARNING) << "No conclusion about testability could be found ("
				<< bmcSolver->GetLastResult() << ")";

			std::scoped_lock lock { this->parallelMutex };
			if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
			{
				AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex,
					Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED,
					Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT);
			}
			goto nextFault;
		}
	}

nextFault:
	std::scoped_lock lock { this->parallelMutex };
	this->timeTseitin.AddValue(cnfGenerationTimer.TotalRunTime());
	this->timeSolver.AddValue(bmcSolverTimer.TotalRunTime());
	this->tseitinClauses.AddValue(bmcSolver->GetNumberOfClauses());
	AtpgBase<FaultModel, FaultList>::SnapshotStatisticsForIteration();
}

template class BmcSequentialAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class BmcSequentialAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class BmcSequentialAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
