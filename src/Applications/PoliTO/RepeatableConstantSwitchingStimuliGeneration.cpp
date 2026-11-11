#include "Applications/PoliTO/RepeatableConstantSwitchingStimuliGeneration.hpp"

#include "Basic/Logic.hpp"
#include "Basic/Settings.hpp"
#include "Helper/StringHelper.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/Disassembler/RiscvPulpDisassembler.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"


using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

RepeatableConstantSwitchingStimuliGeneration::RepeatableConstantSwitchingStimuliGeneration():
	Mixin::VcmMixin("PoliTO/RepeatableConstantSwitchingStimuliGeneration"),
	statistics(),
	timeframes(0u),
    stressTarget("UNINITIALIZED_STRESS_TARGET"),
	stressTargetCells(StressTargetCells::IncludeCellNets),
	stressTargetSubmodules(StressTargetSubmodules::IncludeSubmodules),
	states{0u,0u,0u},
	dChainOptimization(LiteralDChainOptimization::No),
	disassemble(RiscvCompliant::No),
	solverSetting(),
	moduleWatcherPath(),
	modulePrinterPath(),
	vcmWatcherPath(),
	vcmPrinterPath(),
	moduleWatcherImporter(std::make_unique<Io::ValueWatcherImporter>()),
	modulePrinterImporter(std::make_unique<Io::ValuePrinterImporter>()),
	vcmWatcherImporter(std::make_unique<Io::ValueWatcherImporter>()),
	vcmPrinterImporter(std::make_unique<Io::ValuePrinterImporter>())
{
	vcmEnable = VcmEnable::Enabled;
}

RepeatableConstantSwitchingStimuliGeneration::~RepeatableConstantSwitchingStimuliGeneration() = default;

bool RepeatableConstantSwitchingStimuliGeneration::SetSetting(std::string key, std::string value)
{

    if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetCells")
    {
		return Settings::ParseEnum(value, stressTargetCells, {
				{"IncludeCellNets", StressTargetCells::IncludeCellNets},
				{"ExcludeCellNets", StressTargetCells::ExcludeCellNets}
			});
	}

    if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetSubmodules")
    {
		return Settings::ParseEnum(value, stressTargetSubmodules, {
				{"IncludeSubmodules", StressTargetSubmodules::IncludeSubmodules},
				{"ExcludeSubmodules", StressTargetSubmodules::ExcludeSubmodules}
			});
	}

    if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/MaxSATSolverSetting")
    {

		return Settings::ParseEnum(value, solverSetting, {
				{"WARNERS",SolverProxy::Sat::PacoseEncoding::Warners},
				{"BAILLEUX",SolverProxy::Sat::PacoseEncoding::Bailleux},
				{"ASIN",SolverProxy::Sat::PacoseEncoding::Asin},
				{"OGAWA",SolverProxy::Sat::PacoseEncoding::Ogawa},
				{"BAILLEUXW2",SolverProxy::Sat::PacoseEncoding::BailleuxW2},
				{"WEIGHTED_TOTALIZER",SolverProxy::Sat::PacoseEncoding::WeightedTotalizer},
				{"MIXED_RADIX_WEIGHTED_TOTALIZER",SolverProxy::Sat::PacoseEncoding::MixedRadixWeightedTotalizer},
				{"MIXED_RADIX_WEIGHTED_TOTALIZER2",SolverProxy::Sat::PacoseEncoding::MixedRadixWeightedTotalizer2},
				{"MIXED_RADIX_WEIGHTED_TOTALZIER19",SolverProxy::Sat::PacoseEncoding::MixedRadixWeightedTotalizer19},
				{"MIXED_RADIX_WEIGHTED_TOTALIZER19_2",SolverProxy::Sat::PacoseEncoding::MixedRadixWeightedTotalizer19_2},
				{"DYNAMIC_GLOBAL_POLYNOMIAL_WATCHDOG",SolverProxy::Sat::PacoseEncoding::DynamicGlobalPolynomialWatchdog},
				{"HEURISTIC_Q_MAXSAT_17",SolverProxy::Sat::PacoseEncoding::HeuristicQMaxSat17},
				{"HEURISTIC20",SolverProxy::Sat::PacoseEncoding::Heuristic20},
				{"HEURISTIC1819",SolverProxy::Sat::PacoseEncoding::Heuristic1819},
				{"Q_MAXSAT_19",SolverProxy::Sat::PacoseEncoding::QMaxSat19},
				// -- INC-BMO SPECIFIC --
				{"WBO",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_WBO},
				{"LINEAR_SU",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_LINEAR_SU},
				{"PART_MSU3",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_PART_MSU3},
				{"LSU_INCBMO",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_LSU_INCBMO},
				{"LSU_MRSBEAVER",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_LSU_MRSBEAVER},
				{"LSU_MCS",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_LSU_MCS},
				{"OLL",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_OLL},
				{"BEST",SolverProxy::Sat::IncBmoSolverProxy::Algorithm::ALGORITHM_BEST}
			});

    }

	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/LiteralDChainOptimization")
	{
		return Settings::ParseEnum(value, dChainOptimization, {
				{"Yes", LiteralDChainOptimization::Yes},
				{"No",  LiteralDChainOptimization::No}
			});
	}

	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/Timeframes")
	{
        timeframes = std::stoi(value);

		return true;
    }

	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/StressTargetHierarchyName")
    {
    	stressTarget = value;

		return true;
	}

    //TODO: Multiplier Sampling for OR1200
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/RiscvCore")
	{
		return Settings::ParseEnum(value, disassemble, {
				{"Yes", RiscvCompliant::Yes},
				{"No",  RiscvCompliant::No}
			});
	}

	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateA")
	{
		Settings::ParseSizet(value, states.firstTimeframeOfStateA);

		return true;
	}
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateB")
	{
		Settings::ParseSizet(value, states.firstTimeframeOfStateB);

		return true;
	}
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/FirstTimeframeOfStateC")
	{
		Settings::ParseSizet(value, states.firstTimeframeOfStateC);

		return true;
	}

	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/ModuleWatcher")
	{
		moduleWatcherPath = value;
		return true;
	}
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/ModulePrinter")
	{
		modulePrinterPath = value;
		return true;
	}
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/VcmWatcher")
	{
		vcmWatcherPath = value;
		return true;
	}
	if (key == "PoliTO/RepeatableConstantSwitchingStimuliGeneration/VcmPrinter")
	{
		vcmPrinterPath = value;
		return true;
	}

	return Mixin::VcmMixin::SetSetting(key, value);

}

void  RepeatableConstantSwitchingStimuliGeneration::Init()
{
	Mixin::VcmMixin::Init();

	LOG_IF(moduleWatcherPath.empty(), WARNING) << "Not loading any watchers for module";
	if (!moduleWatcherPath.empty())
	{
		FileHandle moduleWatcherHandle { moduleWatcherPath , true };
		if (!moduleWatcherImporter->LoadWatchers(moduleWatcherHandle.GetStream(), "module/", this->circuit->GetName() + "/"))
		{
			LOG(FATAL) << "Could not load module value watchers from file " << moduleWatcherPath;
		}
	}

	LOG_IF(modulePrinterPath.empty(), WARNING) << "Not loading any printers for module";
	if (!modulePrinterPath.empty())
	{
		FileHandle modulePrinterHandle { modulePrinterPath , true };
		if (!modulePrinterImporter->LoadPrinters(modulePrinterHandle.GetStream(), "module/"))
		{
			LOG(FATAL) << "Could not load module value printers from file " << modulePrinterPath;
		}
	}

	LOG_IF(vcmWatcherPath.empty(), WARNING) << "Not loading any watchers for vcm";
	if (!vcmWatcherPath.empty())
	{
		FileHandle vcmWatcherHandle { vcmWatcherPath , true };
		if (!vcmWatcherImporter->LoadWatchers(vcmWatcherHandle.GetStream(), "vcm/", vcmTopLevelModuleName + "/"))
		{
			LOG(FATAL) << "Could not load module value watchers from file " << vcmWatcherPath;
		}
	}

	LOG_IF(vcmPrinterPath.empty(), WARNING) << "Not loading any printers for vcm";
	if (!vcmPrinterPath.empty())
	{
		FileHandle vcmPrinterHandle { vcmPrinterPath , true };
		if (!vcmPrinterImporter->LoadPrinters(vcmPrinterHandle.GetStream(), "vcm/"))
		{
			LOG(FATAL) << "Could not load module value printers from file " << vcmPrinterPath;
		}
	}
}

void RepeatableConstantSwitchingStimuliGeneration::Run()
{
	using LogicContainer = Tpg::LogicContainer01;
	using PinData        = Tpg::PinDataGSwitch<LogicContainer>;
	using VcmPinData     = Tpg::PinDataG<LogicContainer>;

	Mixin::VcmMixin::Run();

	auto processorCircuit = this->circuit;

	std::shared_ptr<SolverProxy::Sat::MaxSatSolverProxy> maxSatSolver = SolverProxy::Sat::MaxSatSolverProxy::CreateMaxSatSolver(Settings::GetInstance()->SatSolver);

	if (auto incBmo = std::dynamic_pointer_cast<SolverProxy::Sat::IncBmoSolverProxy>(maxSatSolver); incBmo)
	{
		incBmo->SetAlgorithm(std::get<SolverProxy::Sat::IncBmoSolverProxy::Algorithm>(solverSetting));
		incBmo->SetBmoSearch(SolverProxy::Sat::IncBmoSolverProxy::BmoSearch::ENABLED);
		incBmo->SetCompleteMode(SolverProxy::Sat::IncBmoSolverProxy::CompleteMode::COMPLETE);
	}
	else if (auto pacose = std::dynamic_pointer_cast<SolverProxy::Sat::PacoseSolverProxy>(maxSatSolver); pacose)
	{
		pacose->SetEncoding(std::get<SolverProxy::Sat::PacoseEncoding>(solverSetting));
	}
	else
	{
		Logging::Panic("Downcasting to a MaxSAT solver failed!");
	}

	auto processorLogicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>> (maxSatSolver, processorCircuit);
	processorLogicGenerator->EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
	processorLogicGenerator->EmplaceModule<Tpg::InputLogicEncoder<PinData,GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01);
	processorLogicGenerator->EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01);
	processorLogicGenerator->EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData,GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01);
	processorLogicGenerator->EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData,GoodTag>>(Basic::MakeUnclockedSimpleFlipFlopModel());
	processorLogicGenerator->EmplaceModule<Tpg::OutputLogicEncoder<PinData,GoodTag>>();
	processorLogicGenerator->EmplaceModule<Tpg::SequentialConnectionEncoder<PinData,GoodTag>>();

	Tpg::Vcm::VcmContext vcmContext { "stress_generation", "Constant & Repeatable Stress Inducing Stimuli Generation" };
	vcmContext.AddTags(vcmTags);
	vcmContext.AddVcmParameters(vcmParameters);
	if (auto it = vcmConfigurations.find(vcmConfiguration); it != vcmConfigurations.end())
	{
		vcmContext.AddTags(it->second.GetTags());
		vcmContext.AddVcmParameters(it->second.GetParameters());
	}

	auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>> (maxSatSolver, vcmCircuit);
	vcmLogicGenerator->EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, GoodTag>>();
	vcmLogicGenerator->EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(processorLogicGenerator, vcmInputs, vcmContext.GetVcmParameters());
	vcmLogicGenerator->EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01);
	vcmLogicGenerator->EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01);
	vcmLogicGenerator->EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	vcmLogicGenerator->EmplaceModule<Tpg::SequentialConnectionEncoder<VcmPinData, GoodTag>>();
	vcmLogicGenerator->EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, GoodTag>>();
	vcmLogicGenerator->EmplaceModule<Tpg::SequentialConstraintEncoder<VcmPinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(Basic::Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
	vcmLogicGenerator->EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(vcmOutputs);

	// No incremental constraint building since old assumptions must be eliminated by the MaxSAT solver during the generation of new assumptions.
	processorLogicGenerator->GetContext().SetIncremental(false);
	vcmLogicGenerator->GetContext().SetIncremental(false);

	CpuClock cnfGenerationTimer;
	CpuClock maxSatSolverTimer;

	cnfGenerationTimer.SetTimeReference();
	processorLogicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
	vcmLogicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
	ASSERT(processorLogicGenerator->GenerateCircuitLogic() && vcmLogicGenerator->GenerateCircuitLogic()) << "Invalid circuit tagging";
	cnfGenerationTimer.Stop();

	const auto stressTargets = GetStressTargets();
	std::vector<LogicContainer> maximizationLiterals;
	std::vector<size_t> maximizationLiteralWeights;
	maximizationLiterals.reserve(stressTargets.size());

	auto& context = processorLogicGenerator->GetContext();
	auto& encoder = context.GetEncoder();

	// TODO: Configuration options for multiple timeframes
	///////////////////////////////////////////////////////////////////
	// Generate and add switching constraints between timeframes     //
	// for the net of the stress target we want to toggle. These     //
	// constraints are the soft clauses of the MAX Sat instance.     //
	// The solve must satisfy as many as possible. Interpet it as a  //
	// FSM:              ______                                      //
	//   ______    _____| state|                                     //
	//  | state|  /     |  B   |                                     //
	//  |  A   | /      |______|                                     //
	//  |______|                                                     //
	//                                                               //
	///////////////////////////////////////////////////////////////////
	std::vector<size_t> switchingTimeframes {states.firstTimeframeOfStateA};

	for (auto& timeframe: switchingTimeframes)
	{
		for (const auto& stressTarget : stressTargets)
		{
			if (stressTarget.clauseWeight == 0)
				continue;

			auto nodeId = stressTarget.target.node->GetNodeId();
			auto const literalState1 = context.GetTimeframe(timeframe).GetContainer<GoodTag>(context, nodeId, Circuit::PortType::Output, 0u);
			auto const literalState2 = context.GetTimeframe(timeframe+1).GetContainer<GoodTag>(context, nodeId, Circuit::PortType::Output, 0u);
			auto maximizationLiteral = encoder.EncodeLogic01DifferenceDetector(literalState1, literalState2);
			maxSatSolver->CommitSoftClause(maximizationLiteral.l0, stressTarget.clauseWeight);
			maximizationLiterals.push_back(maximizationLiteral);

			auto switching = context.GetTimeframe(timeframe+1).GetInternalContainer<SwitchTag>(context, nodeId);
			switching = maximizationLiteral;

			LOG(INFO) << "Added Maximization Literal for " << stressTarget.target.node->GetOutputSignalName()
			<< " Between TF " << timeframe << " and TF " << timeframe+1;

		}
	}

	///////////////////////////////////////////////////////////////////
	// In order to have a repeatable and a constant stress generation//
	// after switching once we MUST return to the initial state. This//
	// implies that all the switching must be undone with a second   //
	// instruction. Hence, we enforce an equivalence between the     //
	// initial state and the state that follows the first switching. //
	// Complementing the FSM above:                                  //
	//                   ______                                      //
	//   ______    _____| state|                                     //
	//  | state|  /     |  B   |_                                    //
	//  |  A   | /      |______| \                                   //
	//  |______|                  \                                  //
	//      \_____________________/                                  //
	//                                                               //
	///////////////////////////////////////////////////////////////////
	std::vector<std::tuple<size_t, size_t>> statesEquivalences {
		std::make_tuple(states.firstTimeframeOfStateA, states.firstTimeframeOfStateC)
	};

	for (auto& [timeframe1, timeframe2] : statesEquivalences)
	{
		for (const auto& stressTarget : stressTargets)
		{
			if (stressTarget.clauseWeight == 0)
				continue;

			auto nodeId = stressTarget.target.node->GetNodeId();
			auto const literalState1 = context.GetTimeframe(timeframe1).GetContainer<GoodTag>(context, nodeId, Circuit::PortType::Output, 0u);
			auto const literalState2 = context.GetTimeframe(timeframe2).GetContainer<GoodTag>(context, nodeId, Circuit::PortType::Output, 0u);

			encoder.EncodeEquality(literalState1, literalState2);

			LOG(INFO) << "Added States Equivalence for node " << stressTarget.target.node->GetName()
			<< " for TF " << timeframe1 << " and TF " << timeframe2;

		}
	}

	///////////////////////////////////////////////////////////////////
	// Daisy chain of output and input literals for every target.    //
	// In specific, the switching literals of each gate's I/O for    //
	// all timeframes are daisy chained in order to improve solving  //
	// times. Id est:                                                //
	//                                                               //
	// Output_lit |-> (Input_lit_1, ..., Input_lit_n) <=>            //
	// (~Output_lit, Input_lit_1, ..., Input_lit_n)                  //
	//                                                               //
	// If the output switches, then at least one input must switch   //
	// If the output does not switch, clause becomes transparent     //
	///////////////////////////////////////////////////////////////////
	if (dChainOptimization == LiteralDChainOptimization::Yes)
	{
		auto switchingTimeframe = states.firstTimeframeOfStateB;

		for (auto& stressTarget : stressTargets)
		{
			std::vector<Tpg::LogicContainer01> nodeInputDifferences;

			for (size_t input = 0u; stressTarget.target.node->GetNumberOfInputs(); input++)
			{
				auto const* inputNode = stressTarget.target.node->GetInput(input);
				auto const& inputSwitching = context.GetTimeframe(switchingTimeframe).GetInternalContainer<SwitchTag>(context, inputNode->GetNodeId());
				if (!inputSwitching.IsSet())
				{
					goto next_node;
				}

				nodeInputDifferences.push_back(inputSwitching);
			}

			if (nodeInputDifferences.size() > 0)
			{
				auto const& outputSwitching = context.GetTimeframe(switchingTimeframe).GetInternalContainer<SwitchTag>(context, stressTarget.target.node->GetNodeId());
				ASSERT(outputSwitching.IsSet()) << "Switching literal not set for output of node : " << stressTarget.target.node->GetOutputSignalName();
				encoder.EncodeImplication(outputSwitching, encoder.EncodeOr(nodeInputDifferences));
			}

			next_node: ;
		}
	}

	maxSatSolverTimer.SetTimeReference();
	auto result = maxSatSolver->MaxSolve();
	maxSatSolverTimer.Stop();

	LOG(INFO) << "The MaxSAT result is " << to_string(result);
	LOG(INFO) << "CNF Generated in: " << cnfGenerationTimer.RunTimeSinceReference() << " seconds";
	LOG(INFO) << "MaxSAT Instance was solved in: " << maxSatSolverTimer.RunTimeSinceReference() << " seconds";

	if (result != SolverProxy::Sat::SatResult::SAT)
	{
		return;
	}

	size_t satisfiedSoftClauses = 0;
	for (auto const& lit : maximizationLiterals)
	{
		satisfiedSoftClauses += ((maxSatSolver->GetLiteralValue(lit.l0) == SolverProxy::Value::Positive) ? 1 : 0);
	}

	statistics.Add("Total Soft Clauses", maximizationLiterals.size(), "clauses", "Total number of requested transitions to be induced between state B and state A");
	statistics.Add("Not Satisfied Soft Clauses", maximizationLiterals.size() - satisfiedSoftClauses, "clauses", "Number of switches NOT induced between state B and state A.");
	statistics.Add("Satisfied Soft Clauses", satisfiedSoftClauses, "clauses", "Number of switches between state B and state A.");
	statistics.Add("MAX Sat Solving Time", maxSatSolverTimer.RunTimeSinceReference(), "seconds", "The seconds it took for the instance to be solved." );

	LOG(INFO) << "Soft Clauses Satisfied: " << satisfiedSoftClauses << "/" << maximizationLiterals.size();

	Helper::ValueWatcher processorWatcher { };
	Helper::ValueWatcher vcmWatcher { };

	moduleWatcherImporter->ApplyWatchers(processorWatcher, *this->circuit, vcmContext.GetTags(), vcmContext.GetTarget());
	vcmWatcherImporter->ApplyWatchers(vcmWatcher, *this->vcmCircuit, vcmContext.GetTags(), vcmContext.GetTarget());

	auto processorWatches { processorWatcher.GetWatcherValues<PinData, GoodTag>(processorLogicGenerator->GetContext()) };
	auto vcmWatches { vcmWatcher.GetWatcherValues<VcmPinData, GoodTag>(vcmLogicGenerator->GetContext()) };

	std::vector<Helper::ValueWatcher::WatcherValues> watchedValues;
	watchedValues.reserve(processorWatches.size() + vcmWatches.size());
	watchedValues.insert(watchedValues.end(), processorWatches.begin(), processorWatches.end());
	watchedValues.insert(watchedValues.end(), vcmWatches.begin(), vcmWatches.end());

	Helper::ValuePrinter printer { };
	printer.SetTitle(vcmContext.GetDisplayName());
	printer.SetTimeframeTitles(+[](size_t timeframe) { return "Timeframe #" + std::to_string(timeframe); });
	printer.SetEqualColumnWidths(true);
	modulePrinterImporter->ApplyPrinters(printer, vcmContext.GetTags(), vcmContext.GetTarget());
	printer.AddDivider(Helper::ValuePrinter::DivideType::BoldSolid);
	vcmPrinterImporter->ApplyPrinters(printer, vcmContext.GetTags(), vcmContext.GetTarget());

	FileHandle output {"[DataExportDirectory]/RepeatableConstantSwitchingSnapshot.txt", false};
	output.GetOutStream() << printer.ToPrintable(watchedValues);

	LOG(INFO) << std::endl << printer.ToPrintable(watchedValues) << std::endl;

	// REMARK: The instruction register MUST be named "decode_instr" in every risc-v core used in this application
	if (disassemble == RiscvCompliant::Yes)
	{
		const Helper::ValueWatcher::WatcherValues * instructionRegister = nullptr;

		for ( auto& watcher : watchedValues)
		{
			if (watcher.name == "module/decode_instr")
			{
				instructionRegister = &watcher;
			}
		}

		ASSERT(instructionRegister != nullptr) << "Instruction Register watcher not found.";

		for (size_t timeframe = 0u; timeframe < instructionRegister->timeframes.size(); timeframe++)
		{
			auto bitValue = static_cast<size_t>(instructionRegister->timeframes[timeframe]);

			auto xpulpInstruction = Riscv::DecodePulp({
				static_cast<uint8_t>((bitValue >> 0u) & 0xFF),
				static_cast<uint8_t>((bitValue >> 8u) & 0xFF),
				static_cast<uint8_t>((bitValue >> 16u) & 0xFF),
				static_cast<uint8_t>((bitValue >> 24u) & 0xFF)
			});

			std::string outputString = "";
			for ( auto& instruction : xpulpInstruction)
			{
				outputString += " " + instruction;
			}

			LOG(INFO) << "Instruction at Timeframe #" << timeframe << " : " << outputString;

		}
	}

	return;
}

std::vector<RepeatableConstantSwitchingStimuliGeneration::StressTarget> RepeatableConstantSwitchingStimuliGeneration::GetStressTargets() const
{
	const auto& metaData = this->circuit->GetMetaData();
	const auto dutMetaData = (stressTargetSubmodules == StressTargetSubmodules::IncludeSubmodules)
		? metaData.GetGroupsWithPrefix(stressTarget)
		: std::vector<const Circuit::GroupMetaData*> { metaData.GetGroup(stressTarget) };

	std::vector<StressTarget> stressTargets;

	for (const auto& group : dutMetaData)
	{
		if (stressTargetCells == StressTargetCells::ExcludeCellNets
			&& group->GetSourceInfo().GetProperty<bool>("module-is-cell").value_or(false))
		{
			continue;
		}

		// Primitives are also groups
		if (group->GetSourceInfo().GetProperty<bool>("module-is-primitive").value_or(false))
		{
			continue;
		}

		for (const auto& net: group->GetWires())
		{
			for (const auto& connection : net.GetConnections())
			{
				if (!connection)
				{
					continue;
				}

				for (const auto& driver : connection->GetMappedSources())
				{
					stressTargets.push_back({
						.target = driver,
						.clauseWeight = 1u
					});
				}
			}
		}
	}

	LOG(INFO) << "Stress Target consists of " << stressTargets.size() << " nets.";

	return stressTargets;
}

Basic::ApplicationStatistics RepeatableConstantSwitchingStimuliGeneration::GetStatistics()
{
	return std::move(statistics);
}

};
};
};
