#include "Applications/Scale4Edge/TestPatternGeneration/SatFullScanFuzzing.hpp"

#include <boost/format.hpp>

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
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Io/CircuitVerilogExporter/CircuitVerilogExporter.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/Sat/Glucose421ParallelSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
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
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/Vcm/VcmContext.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
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
	static size_t GetRequiredTimeframeCount(const Fault::CellAwareFaultModel& faultModel) { return faultModel.GetFault()->GetTimeframeSpread(); }
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
	FuzzingBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG)
{
}

template <typename FaultModel, typename FaultList>
SatFullScanFuzzing<FaultModel, FaultList>::~SatFullScanFuzzing(void) = default;

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
		FuzzingBase<FaultModel, FaultList>::GenerateFaultModel(seed, FuzzingBase<FaultModel, FaultList>::UdfmType::FullScan);
		FuzzingBase<FaultModel, FaultList>::GenerateFaultList(seed);
		VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
		VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

		this->testPatterns.clear();
		Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
			GeneratePatternForFault(seed, index);
		});
		Logging::ClearCurrentFault();
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
	if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		DVLOG(6) << "\n" << to_string(faultModel->GetFault()->GetStateMachine());
	}

	// The logic generator combines all tagging, encoding and constraint modules into one generator.
	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);
	logicGenerator->GetContext().SetIncremental(false);

	auto faultGenerator = logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
	auto faultTagger = logicGenerator->template EmplaceModule<FaultTagger>(faultModel);
	faultTagger->SetTaggingMode(FaultTaggerMode::FullScan);
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
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainBaseEncoder<PinData>>();
	if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::BackwardDChainEncoder<PinData>>();

	// Test constraint encoding which requires the fault to be activated (a difference at the fault location)
	// and the fault effect propagation (a difference at a circuit output port).
	logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
	logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
	auto sensitizationEncoder = logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
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
		Pattern::TestPattern pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndSecondaryInputs);
		DVLOG(3) << "Generated test pattern: " << to_string(pattern);

		if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			auto& states = faultGenerator->GetStates();
			auto& nextStates = faultGenerator->GetNextStates();
			auto& transitions = faultGenerator->GetTransitions();
			auto& noTransitions = faultGenerator->GetNoTransitions();
			auto& sensitizations = sensitizationEncoder->GetSensitizations();
			for (size_t timeframeId { 0u }; timeframeId < states.size(); timeframeId++) {
				DVLOG(6) << "TF" << timeframeId << ":";
				for (size_t state { 0u }; state < states[timeframeId].size(); state++) {
					auto stateValue { satSolver->GetLiteralValue(states[timeframeId][state].l0) };
					auto nextValue { satSolver->GetLiteralValue(nextStates[timeframeId][state].l0) };
					std::string trans;
					for (auto& lit : transitions[timeframeId][state]) {
						auto value { satSolver->GetLiteralValue(lit.l0) };
						trans += to_string(value);
					}
					DVLOG(6) << "  S" << state << " " << to_string(stateValue)
						<< " -> " << to_string(nextValue) << " " << trans;
				}
				auto none { satSolver->GetLiteralValue(noTransitions[timeframeId].l0) };
				auto sensitized { satSolver->GetLiteralValue(sensitizations[timeframeId].l0) };
				DVLOG(6) << "  N " << to_string(none);
				DVLOG(6) << "  S " << to_string(sensitized);
			}
		}

		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		simConfig.sequentialMode = Simulation::SequentialMode::FullScan;
		AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(faultIndex, pattern, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, *logicGenerator, simConfig);

		size_t testPatternIndex = this->testPatterns.emplace_back(pattern);
		AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, faultIndex, testPatternIndex, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, simConfig);
		if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			LOG(FATAL) << "Invalid test pattern was generated";
		}
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
		std::mt19937_64 random { seed };
		std::uniform_int_distribution<size_t> randomLogic { 0u, 1u };

		const auto& mappedCircuit { this->circuit->GetMappedCircuit() };
		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		simConfig.sequentialMode = Simulation::SequentialMode::FullScan;
		const auto [testPrimaryOutputs, testSecondaryOutputs] = Pattern::GetCaptureOutputs(Pattern::OutputCapture::PrimaryAndSecondaryOutputs);
		for (size_t index { 0u }; index < this->configNumSimulations; index++) {
			// Generate random test pattern
			Pattern::TestPattern testPattern { timeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs() };
			for (size_t timeframe { 0u }; timeframe < timeframes; timeframe++) {
				for (size_t input { 0u }; input < mappedCircuit.GetNumberOfPrimaryInputs(); input++) {
					auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
					testPattern.SetPrimaryInput(timeframe, input, logicValue);
				}
			}
			for (size_t timeframe { 0u }; timeframe < timeframes; timeframe++) {
				for (size_t input { 0u }; input < mappedCircuit.GetNumberOfSecondaryInputs(); input++) {
					auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
					testPattern.SetSecondaryInput(timeframe, input, logicValue);
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
			//LOG(INFO) << to_string(testPattern) << " => " << to_string(goodResult) << " vs " << to_string(badResult);

			for (size_t timeframe = 0u; timeframe < testPattern.GetNumberOfTimeframes(); ++timeframe)
			{
				for (size_t index = 0u; testPrimaryOutputs && index < mappedCircuit.GetNumberOfPrimaryOutputs(); ++index)
				{
					const auto* primaryOutput = mappedCircuit.GetPrimaryOutput(index);
					const Basic::Logic good = goodResult.GetOutputLogic(primaryOutput, timeframe);
					const Basic::Logic bad = badResult.GetOutputLogic(primaryOutput, timeframe);
					const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);
					if (__builtin_expect(difference, false))
					{
						LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
							<< " was found by test pattern: " << to_string(testPattern) << "\n"
							<< "Good sim result: " << to_string(goodResult) << "\n"
							<< "Bad sim result:  " << to_string(badResult);
					}
				}

				for (size_t index = 0; testSecondaryOutputs && index < mappedCircuit.GetNumberOfSecondaryOutputs(); ++index)
				{
					auto *const secondaryOutput = mappedCircuit.GetSecondaryOutput(index);
					auto const good = goodResult.GetOutputLogic(secondaryOutput, timeframe);
					auto const bad = badResult.GetOutputLogic(secondaryOutput, timeframe);
					auto const difference = IsValidLogic01(good) && IsValidLogic01(bad) && bad != good;
					if (__builtin_expect(difference, false))
					{
						LOG(FATAL) << "Undetectable fault " << faultIndex << " " << to_string(*fault)
							<< " was found by test pattern: " << to_string(testPattern) << "\n"
							<< "Good sim result: " << to_string(goodResult) << "\n"
							<< "Bad sim result:  " << to_string(badResult);
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
