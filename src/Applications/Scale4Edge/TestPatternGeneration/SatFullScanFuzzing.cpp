#include "Applications/Scale4Edge/TestPatternGeneration/SatFullScanFuzzing.hpp"

#include <boost/format.hpp>

#include <atomic>
#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/AtpgDebug.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/Vcm/VcmContext.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

using LogicContainer = Tpg::LogicContainer01X;
using PinData = Tpg::PinDataGBD<LogicContainer>;

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
	static size_t GetRequiredTimeframeCount(const Fault::CellAwareFaultModel& faultModel) { return faultModel.GetFault()->GetStateMachine().GetTimeframes(); }
};

template <typename FaultModel, typename FaultList>
SatFullScanFuzzing<FaultModel, FaultList>::SatFullScanFuzzing(void):
	Mixin::StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	Mixin::VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	FuzzingBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	scanChainType(ScanChainType::LaunchOnShift)
{
	// Adjust default settings for fuzzing.
	// Disable fault simulation to generate a pattern for each fault.
	this->faultSimulation = AtpgBase<FaultModel, FaultList>::FaultSimulation::Disabled;
	this->solverTimeout = 60u;
}

template <typename FaultModel, typename FaultList>
SatFullScanFuzzing<FaultModel, FaultList>::~SatFullScanFuzzing(void) = default;

template <typename FaultModel, typename FaultList>
bool SatFullScanFuzzing<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "ScanChainType", SCALE4EDGE_ATPG_CONFIG))
	{
		return Settings::ParseEnum(value, scanChainType, {
			{ "LaunchOnShift", ScanChainType::LaunchOnShift },
			{ "EnhancedScan", ScanChainType::EnhancedScan }
		});
	}
	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
void SatFullScanFuzzing<FaultModel, FaultList>::Init(void)
{
}

template <typename FaultModel, typename FaultList>
void SatFullScanFuzzing<FaultModel, FaultList>::Run(void)
{
	Parallel::SetThreads(Parallel::Arena::General, 0u);
	Parallel::SetThreads(Parallel::Arena::PatternGeneration, this->patternGenerationThreadLimit);
	Parallel::SetThreads(Parallel::Arena::FaultSimulation, this->simulationThreadLimit);

	std::mt19937_64 random { this->configSeed };
	for (size_t iteration { 0u }; iteration < this->configNumCircuits; iteration++)
	{
		auto const seed { random() };
		FuzzingBase<FaultModel, FaultList>::GenerateCircuit(seed);
		FuzzingBase<FaultModel, FaultList>::GenerateFaultModel(seed);
		FuzzingBase<FaultModel, FaultList>::GenerateFaultList(seed);
		VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
		VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

		Logging::SetFaultLimits(this->faultListBegin, this->faultListEnd);
		Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
			GeneratePatternForFault(seed, index);
		});
		Logging::ClearCurrentFault();

		auto const faultCount = this->faultListEnd - this->faultListBegin;
		auto const patternCount = this->testPatterns.size();
		auto const simulationCount = patternCount + (faultCount - patternCount) * this->configNumSimulations;
		this->testPatterns.clear();

		LOG(INFO) << boost::format("Checked circuit %s (Inputs: %2d, Ouputs: %2d, Cells: %2d, Faults: %3d, Patterns: %3d, Simulations: %6d) \u2713")
			% this->GetCircuitName() % this->GetNumberOfInputs() % this->GetNumberOfOutputs() % this->GetNumberOfCells()
			% faultCount % patternCount % simulationCount;
	}
}

template <typename FaultModel, typename FaultList>
void SatFullScanFuzzing<FaultModel, FaultList>::GeneratePatternForFault(size_t seed, size_t faultIndex)
{
	using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
	using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
	using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;
	using FaultTagger = typename AtpgConfig<FaultModel, PinData>::FaultTagger;
	using FaultTaggerMode = typename AtpgConfig<FaultModel, PinData>::FaultTaggerMode;

	Logging::SetCurrentFault(faultIndex);

	// Check for already detected faults
	auto [fault, metadata] = this->faultList[faultIndex];
	if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
	{
		return;
	}

	std::shared_ptr<Sat::SatSolverProxy> satSolver = Sat::SatSolverProxy::CreateSatSolver(Settings::GetInstance()->SatSolver);
	ASSERT(satSolver) << "Could not initialize SAT-Solver!";
	satSolver->SetSolverTimeout(this->solverTimeout);

	auto faultModel = std::make_shared<FaultModel>(fault);

	// The logic generator combines all tagging, encoding and constraint modules into one generator.
	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);
	logicGenerator->GetContext().SetIncremental(false);

	auto faultGenerator = logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
	auto faultTagger = logicGenerator->template EmplaceModule<FaultTagger>(faultModel);
	switch (scanChainType) {
		case ScanChainType::LaunchOnShift: faultTagger->SetTaggingMode(FaultTaggerMode::Functional); break;
		case ScanChainType::EnhancedScan: faultTagger->SetTaggingMode(FaultTaggerMode::Independent); break;
	}
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainCircuitTagger<PinData>>();

	// Basic circuit encoding for exactly one timeframe.
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, BadTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	if (scanChainType == ScanChainType::LaunchOnShift)
	{
		logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, BadTag>>();
	}
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainBaseEncoder<PinData>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::BackwardDChainEncoder<PinData>>();

	// Test constraint encoding which requires the fault to be activated (a difference at the fault location)
	// and the fault effect propagation (a difference at a circuit output port).
	logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
	switch (scanChainType) {
		case ScanChainType::LaunchOnShift: logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs); break;
		case ScanChainType::EnhancedScan: logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs); break;
	}
	logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
	// For the cell-aware model the propagation constraint "Any" is required due to different lengths of the patterns.
	logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryAndSecondaryOutputs, Tpg::FaultPropagationTimeframe::Any);

	Tpg::Vcm::VcmContext vcmContext { "pattern_generation", "Pattern Generation" };
	vcmContext.SetTargetStartState(std::vector<Basic::Logic>(this->circuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_DONT_CARE));
	vcmContext.AddTags(this->vcmTags);
	vcmContext.AddVcmParameters(this->vcmParameters);
	if (auto it = this->vcmConfigurations.find(this->vcmConfiguration); it != this->vcmConfigurations.end())
	{
		vcmContext.AddTags(it->second.GetTags());
		vcmContext.AddVcmParameters(it->second.GetParameters());
	}

	auto timeframes = AtpgConfig<FaultModel, PinData>::GetRequiredTimeframeCount(*faultModel);
	logicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
	if (!logicGenerator->GenerateCircuitLogic())
	{
		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNDETECTED,
				Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE);
		}
		goto validate_untestable;
	}

	switch (satSolver->Solve())
	{
	case Sat::SatResult::SAT:
	{
		auto const pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndSecondaryInputs);
		auto const resultGood = Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator->GetContext());
		auto const resultBad = Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator->GetContext());
		DVLOG(3) << "Generated test pattern: " << to_string(pattern) << std::endl << std::endl
			<< Helper::DebugFaultLocation<Helper::DebugSimulationGoodBad, FaultModel>(
				{ fault }, *this->circuit, resultGood, resultBad, resultGood, resultBad);

		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		switch (scanChainType) {
			case ScanChainType::LaunchOnShift: simConfig.sequentialMode = Simulation::SequentialMode::Functional; break;
			case ScanChainType::EnhancedScan: simConfig.sequentialMode = Simulation::SequentialMode::Independent; break;
		}
		AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(faultIndex, pattern, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, *logicGenerator, simConfig);

		size_t testPatternIndex = this->testPatterns.emplace_back(pattern);

		AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, faultIndex, testPatternIndex, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, simConfig);
		ASSERT (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED);
		return;
	}

	case Sat::SatResult::UNSAT:
	{
		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNDETECTED,
				Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE);
		}
		goto validate_untestable;
	}

	default:
	case Sat::SatResult::UNKNOWN:
	{
		LOG(WARNING) << "No conclusion about testability could be found (UNKNOWN)";
		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED,
				Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT);
		}
		return;
	}
	} // end case

validate_untestable:
	DVLOG(3) << "Checking if there is a pattern that finds " << to_string(*fault);
	{
		using PinData = PinDataGB<LogicContainer>;
		using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
		using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
		using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;
		using FaultTagger = typename AtpgConfig<FaultModel, PinData>::FaultTagger;
		using FaultTaggerMode = typename AtpgConfig<FaultModel, PinData>::FaultTaggerMode;

		// Try without D-Chain
		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);
		logicGenerator->GetContext().SetIncremental(false);
		auto faultGenerator = logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
		auto faultTagger = logicGenerator->template EmplaceModule<FaultTagger>(faultModel);
		switch (scanChainType) {
			case ScanChainType::LaunchOnShift: faultTagger->SetTaggingMode(FaultTaggerMode::Functional); break;
			case ScanChainType::EnhancedScan: faultTagger->SetTaggingMode(FaultTaggerMode::Independent); break;
		}
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, BadTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		if (scanChainType == ScanChainType::LaunchOnShift)
		{
			logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
			logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, BadTag>>();
		}
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();

		logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
		switch (scanChainType) {
			case ScanChainType::LaunchOnShift: logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs); break;
			case ScanChainType::EnhancedScan: logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs); break;
		}
		logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
		logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryAndSecondaryOutputs, Tpg::FaultPropagationTimeframe::Any);

		logicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
		if (logicGenerator->GenerateCircuitLogic() && satSolver->Solve() == Sat::SatResult::SAT)
		{
			const auto resultGood = Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator->GetContext());
			const auto resultBad = Tpg::Extractor::ExtractSimulationResult<PinData, BadTag>(logicGenerator->GetContext());
			Helper::DebugExportSimulation("atpg-good", *this->circuit, resultGood);
			Helper::DebugExportSimulation("atpg-bad", *this->circuit, resultBad);

			LOG(FATAL) << "Untestable fault " << faultIndex << " " << to_string(*fault)
				<< " was found by full-scan ATPG without D-chains:" << std::endl
				<< Helper::DebugFaultLocation<Helper::DebugSimulationGoodBad, FaultModel>(
					{ fault }, *this->circuit, resultGood, resultBad, resultGood, resultBad) << std::endl
				<< Helper::DebugSimulation<Helper::DebugSimulationGoodBad>(
					*this->circuit, resultGood, resultBad, resultGood, resultBad) << std::endl
				<< to_debug(this->circuit->GetMappedCircuit());
		}
	}
	{
		using PinData = PinDataGB<LogicContainer>;
		using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
		using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
		using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;

		// Try using a BMC Solver without D-Chain
		std::shared_ptr<Bmc::BmcSolverProxy> bmcSolver = Bmc::BmcSolverProxy::CreateBmcSolver(Settings::GetInstance()->BmcSolver);
		bmcSolver->SetSolverTimeout(this->solverTimeout);

		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(bmcSolver, this->circuit);
		logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, BadTag>>();
		if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainCircuitTagger<PinData>>();
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

		logicGenerator->template EmplaceModule<Tpg::UnaryCounterEncoder<PinData>>();
		logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
		logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
		logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
		logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryAndSecondaryOutputs, Tpg::FaultPropagationTimeframe::Any);

		logicGenerator->GetContext().SetNumberOfTimeframes(timeframes + 1u);
		if (logicGenerator->GenerateCircuitLogic() && bmcSolver->Solve() == Bmc::BmcResult::Reachable)
		{
			const auto resultGood = Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator->GetContext());
			const auto resultBad = Tpg::Extractor::ExtractSimulationResult<PinData, BadTag>(logicGenerator->GetContext());
			Helper::DebugExportSimulation("atpg-good", *this->circuit, resultGood);
			Helper::DebugExportSimulation("atpg-bad", *this->circuit, resultBad);

			LOG(FATAL) << "Untestable fault " << faultIndex << " " << to_string(*fault)
				<< " was found by sequential ATPG:" << std::endl
				<< Helper::DebugFaultLocation<Helper::DebugSimulationGoodBad, FaultModel>(
					{ fault }, *this->circuit, resultGood, resultBad, resultGood, resultBad) << std::endl
				<< Helper::DebugSimulation<Helper::DebugSimulationGoodBad>(
					*this->circuit, resultGood, resultBad, resultGood, resultBad) << std::endl
				<< to_debug(this->circuit->GetMappedCircuit());
		}
	}
	{
		// Try using random simulation
		std::mt19937_64 random { seed };
		std::uniform_int_distribution<size_t> randomLogic { 0u, 1u };

		const auto& mappedCircuit { this->circuit->GetMappedCircuit() };
		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		switch (scanChainType) {
			case ScanChainType::LaunchOnShift: simConfig.sequentialMode = Simulation::SequentialMode::Functional; break;
			case ScanChainType::EnhancedScan: simConfig.sequentialMode = Simulation::SequentialMode::Independent; break;
		}
		for (size_t index { 0u }; index < this->configNumSimulations; index++) {
			// Generate random test pattern
			Pattern::TestPattern testPattern { timeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs() };
			for (size_t timeframe { 0u }; timeframe < timeframes; timeframe++) {
				for (size_t primaryInput { 0u }; primaryInput < mappedCircuit.GetNumberOfPrimaryInputs(); primaryInput++) {
					auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
					testPattern.SetPrimaryInput(timeframe, primaryInput, logicValue);
				}
				for (size_t secondaryInput { 0u }; secondaryInput < mappedCircuit.GetNumberOfSecondaryInputs(); secondaryInput++) {
					auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
					testPattern.SetSecondaryInput(timeframe, secondaryInput, logicValue);
				}
			}

			Simulation::SimulationResult goodResult(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
			Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, testPattern, {}, goodResult, simConfig);

			// Do a pre-check if the fault is sensitized in at least one timeframe.
			// If this is not the case then we can skip the simulation as no fault propagation is possible.
			if (!AtpgBase<FaultModel, FaultList>::CheckSensitization(FaultModel(fault), goodResult))
			{
				continue;
			}

			Simulation::SimulationResult badResult(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			Simulation::SimulateTestPatternNaive<FaultModel>(mappedCircuit, testPattern, { fault }, badResult, simConfig);

			for (size_t timeframe = 0u; timeframe < testPattern.GetNumberOfTimeframes(); ++timeframe)
			{
				for (size_t primaryIndex = 0u; primaryIndex < mappedCircuit.GetNumberOfPrimaryOutputs(); ++primaryIndex)
				{
					auto *const primaryOutput = mappedCircuit.GetPrimaryOutput(primaryIndex);
					auto const good = goodResult.GetOutputLogic(primaryOutput, timeframe);
					auto const bad = badResult.GetOutputLogic(primaryOutput, timeframe);
					auto const difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);
					if (__builtin_expect(difference, false))
					{
						Simulation::SimulationResult verifyGood(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
						Simulation::SimulationResult verifyBad(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
						Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, testPattern, {}, verifyGood, simConfig);
						Simulation::SimulateTestPatternNaive<FaultModel>(mappedCircuit, testPattern, { fault }, verifyBad, simConfig);

						Helper::DebugExportSimulation("sim-good", *this->circuit, goodResult);
						Helper::DebugExportSimulation("sim-bad", *this->circuit, badResult);
						Helper::DebugExportSimulation("check-good", *this->circuit, verifyGood);
						Helper::DebugExportSimulation("check-bad", *this->circuit, verifyBad);

						LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
							<< " was found by test pattern at primary output:" << std::endl
							<< Helper::DebugFaultLocation<Helper::DebugSimulationVsCheck, FaultModel>(
								{ fault }, *this->circuit, goodResult, badResult, verifyGood, verifyBad) << std::endl
							<< Helper::DebugSimulation<Helper::DebugSimulationVsCheck>(
								*this->circuit, goodResult, badResult, verifyGood, verifyBad) << std::endl
							<< to_debug(this->circuit->GetMappedCircuit());
					}
				}

				for (size_t secondaryIndex = 0; secondaryIndex < mappedCircuit.GetNumberOfSecondaryOutputs(); ++secondaryIndex)
				{
					auto *const secondaryOutput = mappedCircuit.GetSecondaryOutput(secondaryIndex);
					auto const good = goodResult.GetOutputLogic(secondaryOutput, timeframe);
					auto const bad = badResult.GetOutputLogic(secondaryOutput, timeframe);
					auto const difference = IsValidLogic01(good) && IsValidLogic01(bad) && bad != good;
					if (__builtin_expect(difference, false))
					{
						Simulation::SimulationResult verifyGood(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
						Simulation::SimulationResult verifyBad(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
						Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, testPattern, {}, verifyGood, simConfig);
						Simulation::SimulateTestPatternNaive<FaultModel>(mappedCircuit, testPattern, { fault }, verifyBad, simConfig);

						Helper::DebugExportSimulation("sim-good", *this->circuit, goodResult);
						Helper::DebugExportSimulation("sim-bad", *this->circuit, badResult);
						Helper::DebugExportSimulation("check-good", *this->circuit, verifyGood);
						Helper::DebugExportSimulation("check-bad", *this->circuit, verifyBad);

						LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
							<< " was found by test pattern at secondary output:" << std::endl
							<< secondaryOutput->GetName() << ": " << to_string(badResult) << std::endl
							<< Helper::DebugFaultLocation<Helper::DebugSimulationVsCheck, FaultModel>(
								{ fault }, *this->circuit, goodResult, badResult, verifyGood, verifyBad) << std::endl
							<< Helper::DebugSimulation<Helper::DebugSimulationVsCheck>(
								*this->circuit, goodResult, badResult, verifyGood, verifyBad) << std::endl
							<< to_debug(this->circuit->GetMappedCircuit());
					}
				}
			}
		}
		return;
	}
}

template class SatFullScanFuzzing<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class SatFullScanFuzzing<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class SatFullScanFuzzing<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
