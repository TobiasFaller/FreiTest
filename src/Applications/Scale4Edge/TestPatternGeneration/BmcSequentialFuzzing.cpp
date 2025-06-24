#include "Applications/Scale4Edge/TestPatternGeneration/BmcSequentialFuzzing.hpp"

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
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
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
BmcSequentialFuzzing<FaultModel, FaultList>::BmcSequentialFuzzing():
	StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	FuzzingBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SequentialAtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG)
{
}

template <typename FaultModel, typename FaultList>
BmcSequentialFuzzing<FaultModel, FaultList>::~BmcSequentialFuzzing(void) = default;

template <typename FaultModel, typename FaultList>
void BmcSequentialFuzzing<FaultModel, FaultList>::Init(void)
{
}

template <typename FaultModel, typename FaultList>
void BmcSequentialFuzzing<FaultModel, FaultList>::Run(void)
{
	Parallel::SetThreads(Parallel::Arena::General, 0u);
	Parallel::SetThreads(Parallel::Arena::PatternGeneration, this->patternGenerationThreadLimit);
	Parallel::SetThreads(Parallel::Arena::FaultSimulation, this->simulationThreadLimit);

	std::mt19937_64 random { this->configSeed };
	for (size_t iteration { 0u }; iteration < this->configNumCircuits; iteration++)
	{
		auto const seed { random() };
		FuzzingBase<FaultModel, FaultList>::GenerateCircuit(seed);
		FuzzingBase<FaultModel, FaultList>::GenerateFaultModel(seed, FuzzingBase<FaultModel, FaultList>::UdfmType::Functional);
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
void BmcSequentialFuzzing<FaultModel, FaultList>::GeneratePatternForFault(size_t seed, size_t faultIndex)
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

	std::shared_ptr<Bmc::BmcSolverProxy> bmcSolver = Bmc::BmcSolverProxy::CreateBmcSolver(Settings::GetInstance()->BmcSolver);
	ASSERT(bmcSolver) << "Could not initialize BMC-Solver!";
	bmcSolver->SetSolverTimeout(this->solverTimeout);
	bmcSolver->SetSolverDebug(false);
	bmcSolver->SetSolverSilent(true);
	bmcSolver->SetMaximumDepth(this->settingsMaximumTimeframes + 1u);

	auto faultModel = std::make_shared<FaultModel>(fault);
	if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
	{
		DVLOG(6) << to_string(faultModel->GetFault()->GetStateMachine());
	}

	// The logic generator combines all tagging, encoding and constraint modules into one generator.
	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(bmcSolver, this->circuit);
	auto faultGenerator = logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);
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
	auto sensitizationEncoder = logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
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

	logicGenerator->GetContext().SetNumberOfTimeframes(this->settingsMaximumTimeframes + 1u);
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

	switch (bmcSolver->Solve())
	{
	case Bmc::BmcResult::Reachable:
	{
		Pattern::TestPattern pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndInitialSecondaryInputs);
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
				bmcSolver->SetTargetTimeframe(timeframeId);
				for (size_t state { 0u }; state < states[0u].size(); state++) {
					auto stateValue { bmcSolver->GetLiteralValue(states[0u][state].l0) };
					auto nextValue { bmcSolver->GetLiteralValue(nextStates[0u][state].l0) };
					std::string trans;
					for (auto& lit : transitions[0u][state]) {
						auto value { bmcSolver->GetLiteralValue(lit.l0) };
						trans += to_string(value);
					}
					DVLOG(6) << "  S" << state << " " << to_string(stateValue)
						<< " -> " << to_string(nextValue) << " " << trans;
				}
				auto none { bmcSolver->GetLiteralValue(noTransitions[0u].l0) };
				DVLOG(6) << "  N " << to_string(none);
				auto sensitized { bmcSolver->GetLiteralValue(sensitizations[0u].l0) };
				DVLOG(6) << "  S " << to_string(sensitized);
			}
		}

		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		simConfig.sequentialMode = Simulation::SequentialMode::Functional;
		AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(faultIndex, pattern, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, *logicGenerator, simConfig);

		size_t testPatternIndex = this->testPatterns.emplace_back(pattern);
		AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, faultIndex, testPatternIndex, Pattern::OutputCapture::PrimaryOutputsOnly, simConfig);
		if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			LOG(FATAL) << "Invalid test pattern was generated";
		}
		return;
	}

	case Bmc::BmcResult::Unreachable:
	case Bmc::BmcResult::MaxIterationsReached:
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
	case Bmc::BmcResult::Timeout:
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
			Pattern::TestPattern testPattern { this->settingsMaximumTimeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs() };
			for (size_t timeframe { 0u }; timeframe < this->settingsMaximumTimeframes; timeframe++) {
				for (size_t input { 0u }; input < mappedCircuit.GetNumberOfPrimaryInputs(); input++) {
					auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
					testPattern.SetPrimaryInput(timeframe, input, logicValue);
				}
			}
			for (size_t input { 0u }; input < mappedCircuit.GetNumberOfSecondaryInputs(); input++) {
				auto const logicValue { (randomLogic(random) == 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO };
				testPattern.SetSecondaryInput(0u, input, logicValue);
			}

			Simulation::SimulationResult goodResult(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
			Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, testPattern, {}, goodResult, simConfig);

			// Do a pre-check if the fault is sensitized in at least one timeframe.
			// If this is not the case then we can skip the simulation as no fault propagation is possible.
			if (!AtpgBase<FaultModel, FaultList>::CheckSensitization(FaultModel(fault), goodResult))
			{
				continue;
			}

			Simulation::SimulationResult badResult(testPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes(), Logic::LOGIC_DONT_CARE);
			badResult.ReplaceWith(goodResult);
			Simulation::SimulateTestPatternEventDrivenIncremental<FaultModel>(mappedCircuit, testPattern, { fault }, std::as_const(goodResult), badResult, simConfig);
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
							<< " was found by test pattern: " << to_string(badResult);
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
							<< " was found by test pattern: " << to_string(badResult);
					}
				}
			}
		}
		return;
	}
}

template class BmcSequentialFuzzing<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class BmcSequentialFuzzing<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class BmcSequentialFuzzing<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
