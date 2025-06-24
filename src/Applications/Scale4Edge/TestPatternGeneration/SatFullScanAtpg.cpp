#include "Applications/Scale4Edge/TestPatternGeneration/SatFullScanAtpg.hpp"

#include <boost/format.hpp>

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
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
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
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/Vcm/VcmContext.hpp"

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
SatFullScanAtpg<FaultModel, FaultList>::SatFullScanAtpg(void):
	StatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_ATPG_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_ATPG_CONFIG),
	VcmMixin(SCALE4EDGE_ATPG_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_ATPG_CONFIG),
	maximizeDontCareValues(MaximizeDontCareValues::Disabled),
	maximizeDontCarePorts(MaximizeDontCarePorts::Inputs),
	maximizeDontCareFlipFlops(MaximizeDontCareFlipFlops::Inputs),
	maximizeDontCarePortWeight(1u),
	maximizeDontCareFlipFlopWeight(1u)
{
}

template <typename FaultModel, typename FaultList>
SatFullScanAtpg<FaultModel, FaultList>::~SatFullScanAtpg(void) = default;

template <typename FaultModel, typename FaultList>
void SatFullScanAtpg<FaultModel, FaultList>::Init(void)
{
	AtpgBase<FaultModel, FaultList>::Init();
}

template <typename FaultModel, typename FaultList>
void SatFullScanAtpg<FaultModel, FaultList>::Run(void)
{
	AtpgBase<FaultModel, FaultList>::Run();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	AtpgBase<FaultModel, FaultList>::GenerateFaultList();
	VLOG(6) << to_debug(this->faultList, this->circuit->GetMappedCircuit());

	LOG(INFO) << "Generating test patterns for " << (this->faultListEnd - this->faultListBegin) << " faults";
	Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
		GeneratePatternForFault(index);
	});
	Logging::ClearCurrentFault();

	this->statistics.Add("Encoding.PatternGeneration.LogicContainer", std::string("LogicContainer") + get_logic_container_name<LogicContainer>, "Type", "LogicContainer used");
	this->statistics.Add("Encoding.PatternGeneration.PinData", std::string("PinData") + get_pin_data_name_v<PinData>, "Type", "PinData used");
	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryAndSecondaryInputs);
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template <typename FaultModel, typename FaultList>
bool SatFullScanAtpg<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/TestPatternGeneration/MaximizeDontCareValues")
	{
		return Settings::ParseEnum(value, maximizeDontCareValues, {
			{ "Disabled", MaximizeDontCareValues::Disabled },
			{ "Enabled", MaximizeDontCareValues::Enabled }
		});
	}
	if (key == "Scale4Edge/TestPatternGeneration/MaximizeDontCarePorts")
	{
		return Settings::ParseEnum(value, maximizeDontCarePorts, {
			{ "None", MaximizeDontCarePorts::None },
			{ "Inputs", MaximizeDontCarePorts::Inputs },
			{ "Outputs", MaximizeDontCarePorts::Outputs },
			{ "InputAndOutputs", MaximizeDontCarePorts::InputAndOutputs }
		});
	}
	if (key == "Scale4Edge/TestPatternGeneration/MaximizeDontCareFlipFlops")
	{
		return Settings::ParseEnum(value, maximizeDontCareFlipFlops, {
			{ "None", MaximizeDontCareFlipFlops::None },
			{ "Inputs", MaximizeDontCareFlipFlops::Inputs },
			{ "Outputs", MaximizeDontCareFlipFlops::Outputs },
			{ "InputAndOutputs", MaximizeDontCareFlipFlops::InputAndOutputs }
		});
	}
	if (key == "Scale4Edge/TestPatternGeneration/MaximizeDontCarePortWeight")
	{
		return Settings::ParseSizet(value, maximizeDontCarePortWeight);
	}
	if (key == "Scale4Edge/TestPatternGeneration/MaximizeDontCareFlipFlopWeight")
	{
		return Settings::ParseSizet(value, maximizeDontCareFlipFlopWeight);
	}

	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
Basic::ApplicationStatistics SatFullScanAtpg<FaultModel, FaultList>::GetStatistics(void)
{
	return AtpgBase<FaultModel, FaultList>::GetStatistics();
}

template <typename FaultModel, typename FaultList>
void SatFullScanAtpg<FaultModel, FaultList>::GeneratePatternForFault(size_t faultIndex)
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

	LOG_IF(this->maximizeDontCareValues == MaximizeDontCareValues::Enabled, WARNING) << "Maximization of DON'T CARE values enabled. Forcing Pacose Max-SAT solver.";
	std::shared_ptr<Sat::SatSolverProxy> satSolver = Sat::SatSolverProxy::CreateSatSolver(
		(maximizeDontCareValues == MaximizeDontCareValues::Enabled) ? Sat::SatSolver::PROD_MAX_SAT_PACOSE : Settings::GetInstance()->SatSolver
	);
	if (!satSolver)
	{
		LOG(FATAL) << "Could not initialize SAT-Solver!";
		return;
	}

	satSolver->SetSolverTimeout(this->solverTimeout);
	if (auto parallelGlucose = std::dynamic_pointer_cast<Sat::Glucose421ParallelSolverProxy>(satSolver); parallelGlucose)
	{
		parallelGlucose->SetMaximumThreadCount(this->solverThreadLimit);
	}

	// A single stuck-at fault model.
	auto faultModel = std::make_shared<FaultModel>(fault);
	auto timeframes = AtpgConfig<FaultModel, PinData>::GetRequiredTimeframeCount(*faultModel);

	// The logic generator combines all tagging, encoding and constraint modules into one generator.
	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);
	auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>>(satSolver, this->vcmCircuit);
	logicGenerator->GetContext().SetIncremental(false);
	vcmLogicGenerator->GetContext().SetIncremental(false);

	logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);

	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled)
	{
		// The whole circuit is marked with good encoding and the fault location and it's output cone is marked with bad encoding.
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
	}
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

	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled)
	{
		vcmContext.SetVcmStartState(std::vector<Basic::Logic>(this->vcmCircuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_ZERO));

		// Encode the whole circuit with good encoding only as there is no fault model for the VCM circuit
		vcmLogicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(logicGenerator, this->vcmInputs, vcmContext.GetVcmParameters());
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		vcmLogicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(this->vcmOutputs);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<VcmPinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(vcmContext.GetVcmStartState()), Tpg::ConstrainedSequentials::Inputs);
	}

	if (maximizeDontCareValues == MaximizeDontCareValues::Enabled) {
		auto maximizePorts = Tpg::ConstrainedPorts::None;
		auto maximizeFlipFlops = Tpg::ConstrainedSequentials::None;
		switch (maximizeDontCarePorts)
		{
			case MaximizeDontCarePorts::None: maximizePorts = Tpg::ConstrainedPorts::None; break;
			case MaximizeDontCarePorts::Inputs: maximizePorts = Tpg::ConstrainedPorts::Inputs; break;
			case MaximizeDontCarePorts::Outputs: maximizePorts = Tpg::ConstrainedPorts::Outputs; break;
			case MaximizeDontCarePorts::InputAndOutputs: maximizePorts = Tpg::ConstrainedPorts::InputAndOutputs; break;
		}
		switch (maximizeDontCareFlipFlops)
		{
			case MaximizeDontCareFlipFlops::None: maximizeFlipFlops = Tpg::ConstrainedSequentials::None; break;
			case MaximizeDontCareFlipFlops::Inputs: maximizeFlipFlops = Tpg::ConstrainedSequentials::Inputs; break;
			case MaximizeDontCareFlipFlops::Outputs: maximizeFlipFlops = Tpg::ConstrainedSequentials::Outputs; break;
			case MaximizeDontCareFlipFlops::InputAndOutputs: maximizeFlipFlops = Tpg::ConstrainedSequentials::InputAndOutputs; break;
		}

		auto portConstraints = logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, GoodTag>>(Tpg::MakeConstantPortConstraint(LogicConstraint::ONLY_LOGIC_DONT_CARE), maximizePorts, Tpg::ConstraintType::MaximizationConstraint);
		auto flipFlopConstraints = logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, GoodTag>>(Tpg::MakeConstantSequentialConstraint(LogicConstraint::ONLY_LOGIC_DONT_CARE), maximizeFlipFlops, Tpg::ConstraintType::MaximizationConstraint);

		portConstraints->SetWeight(std::max(maximizeDontCarePortWeight, static_cast<size_t>(1u)));
		flipFlopConstraints->SetWeight(std::max(maximizeDontCareFlipFlopWeight, static_cast<size_t>(1u)));
	}

	LOG(INFO) << "Generating test pattern for fault " << to_string(*fault);

	Sat::SatResult result;
	CpuClock cnfGenerationTimer;
	CpuClock satSolverTimer;

	cnfGenerationTimer.SetTimeReference();
	logicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
	if (this->vcmEnable == VcmMixin::VcmEnable::Enabled) vcmLogicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
	if (!logicGenerator->GenerateCircuitLogic()
		|| ((this->vcmEnable == VcmMixin::VcmEnable::Enabled) && !vcmLogicGenerator->GenerateCircuitLogic()))
	{
		cnfGenerationTimer.Stop();
		LOG(INFO) << "Fault is untestable as no circuit output can be reached";

		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNDETECTED,
				Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE);
		}
		goto nextFault;
	}
	cnfGenerationTimer.Stop();

	VLOG(3) << "Invoking SAT-Solver to find solution";
	satSolverTimer.SetTimeReference();
	result = (maximizeDontCareValues == MaximizeDontCareValues::Enabled)
		? std::dynamic_pointer_cast<Sat::MaxSatSolverProxy>(satSolver)->MaxSolve()
		: satSolver->Solve();
	satSolverTimer.Stop();

	switch (result)
	{
	case Sat::SatResult::SAT:
	{
		Pattern::TestPattern pattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(logicGenerator->GetContext(), Pattern::InputCapture::PrimaryAndSecondaryInputs);
		VLOG(3) << "Generated test pattern: " << to_string(pattern);

		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		simConfig.sequentialMode = Simulation::SequentialMode::FullScan;

		if (this->checkAtpgResult == AtpgBase<FaultModel, FaultList>::CheckAtpgResult::CheckEqual)
		{
			AtpgBase<FaultModel, FaultList>::ValidateAtpgResult(faultIndex, pattern, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, *logicGenerator, simConfig);
		}

		size_t testPatternIndex = this->testPatterns.emplace_back(pattern);
		AtpgBase<FaultModel, FaultList>::RunFaultSimulation(vcmContext, faultIndex, testPatternIndex, Pattern::OutputCapture::PrimaryAndSecondaryOutputs, simConfig);

		if (metadata->faultStatus != Fault::FaultStatus::FAULT_STATUS_DETECTED)
		{
			LOG(FATAL) << "Invalid test pattern was generated";
		}

		goto nextFault;
	}

	case Sat::SatResult::UNSAT:
	{
		LOG(INFO) << "There exists no test pattern (" << to_string(result) << ")";

		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNDETECTED,
				Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE);
		}
		goto nextFault;
	}

	default:
	case Sat::SatResult::UNKNOWN:
	{
		LOG(WARNING) << "No conclusion about testability could be found (" << to_string(result) << ")";

		std::scoped_lock lock { this->parallelMutex };
		if (metadata->faultStatus == Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED)
		{
			AtpgBase<FaultModel, FaultList>::SetFaultStatus(this->faultList, faultIndex, Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED,
				Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT);
		}
		goto nextFault;
	}

	} // end case

nextFault:
	std::scoped_lock lock { this->parallelMutex };
	this->timeTseitin.AddValue(cnfGenerationTimer.TotalRunTime());
	this->timeSolver.AddValue(satSolverTimer.TotalRunTime());
	this->tseitinClauses.AddValue(satSolver->GetNumberOfClauses());
	AtpgBase<FaultModel, FaultList>::SnapshotStatisticsForIteration();
}

template class SatFullScanAtpg<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class SatFullScanAtpg<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class SatFullScanAtpg<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
