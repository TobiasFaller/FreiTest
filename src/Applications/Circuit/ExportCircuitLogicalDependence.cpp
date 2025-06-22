#include "Applications/Circuit/ExportCircuitLogicalDependence.hpp"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <atomic>
#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>

#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Circuit/MappedNode.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"
#include "Tpg/Vcm/VcmContext.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
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

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

LogicalDependency operator|(const LogicalDependency& a, const LogicalDependency& b) {
	return static_cast<LogicalDependency>(static_cast<size_t>(a) | static_cast<size_t>(b));
}
LogicalDependency operator|=(LogicalDependency& a, const LogicalDependency& b) {
	a = static_cast<LogicalDependency>(static_cast<size_t>(a) | static_cast<size_t>(b));
	return a;
}

LogicalDependency operator&(const LogicalDependency& a, const LogicalDependency& b) {
	return static_cast<LogicalDependency>(static_cast<size_t>(a) & static_cast<size_t>(b));
}
LogicalDependency operator&(LogicalDependency& a, const LogicalDependency& b) {
	a = static_cast<LogicalDependency>(static_cast<size_t>(a) & static_cast<size_t>(b));
	return a;
}

bool operator !(const LogicalDependency& a) {
	return static_cast<size_t>(a) == 0;
}

std::string to_string(const LogicalDependency& a) {
	const auto contains = [&](auto dep) { return (a & dep) == dep; };

	if (contains(LogicalDependency::ConstantPositve)) { return "1      "; }
	if (contains(LogicalDependency::ConstantNegative)) { return "0      "; }

	if (contains(LogicalDependency::FunctionalDependent)) {
		if (contains(LogicalDependency::FunctionalEquivalent)) { return "+      "; }
		if (contains(LogicalDependency::FunctionalInverted)) { return "-      "; }

		std::string extension;
		if (contains(LogicalDependency::FunctionalPositiveForcesPositive)) { extension = ", 1->1"; }
		if (contains(LogicalDependency::FunctionalPositiveForcesNegative)) { extension = ", 1->0"; }
		if (contains(LogicalDependency::FunctionalNegativeForcesPositive)) { extension = ", 0->1"; }
		if (contains(LogicalDependency::FunctionalNegativeForcesNegative)) { extension = ", 0->0"; }

		if (contains(LogicalDependency::FunctionalBoth)) { return "D" + extension; }
		if (contains(LogicalDependency::FunctionalPositive)) { return "P" + extension; }
		if (contains(LogicalDependency::FunctionalNegative)) { return "N" + extension; }
	}

	if (contains(LogicalDependency::StructuralDependent)) { return "C      "; }
	if (contains(LogicalDependency::StructuralIndependent)) { return "_      "; }
	__builtin_unreachable();
}

std::string to_short_string(const LogicalDependency& a) {
	const auto contains = [&](auto dep) { return (a & dep) == dep; };

	if (contains(LogicalDependency::ConstantPositve)) { return "1"; }
	if (contains(LogicalDependency::ConstantNegative)) { return "0"; }

	if (contains(LogicalDependency::FunctionalDependent)) {
		if (contains(LogicalDependency::FunctionalEquivalent)) { return "+"; }
		if (contains(LogicalDependency::FunctionalInverted)) { return "-"; }

		if (contains(LogicalDependency::FunctionalBoth)) { return "D"; }
		if (contains(LogicalDependency::FunctionalPositive)) { return "P"; }
		if (contains(LogicalDependency::FunctionalNegative)) { return "N"; }
	}

	if (contains(LogicalDependency::StructuralDependent)) { return "C"; }
	if (contains(LogicalDependency::StructuralIndependent)) { return "_"; }
	__builtin_unreachable();
}

ExportCircuitLogicalDependence::ExportCircuitLogicalDependence(void):
	Mixin::VcmMixin("Circuit/LogicalDependence"),
	portInclude(""),
	portExclude(""),
	enableInputsToInputs(Analysis::Enabled),
	enableInputsToOutputs(Analysis::Enabled),
	enableOutputsToInputs(Analysis::Enabled),
	enableOutputsToOutputs(Analysis::Enabled),
	inputsToInputs(),
	inputsToOutputs(),
	outputsToInputs(),
	outputsToOutputs()
{
}

ExportCircuitLogicalDependence::~ExportCircuitLogicalDependence(void) = default;

void ExportCircuitLogicalDependence::Init(void)
{
	Mixin::VcmMixin::Init();
}

bool ExportCircuitLogicalDependence::SetSetting(std::string key, std::string value)
{
	if (key == "Circuit/LogicalDependence/InputsToInputs") {
		return Settings::ParseEnum(value, enableInputsToInputs, {
			{ "Enabled", Analysis::Enabled },
			{ "Disabled", Analysis::Disabled },
		});
	}
	if (key == "Circuit/LogicalDependence/InputsToOutputs") {
		return Settings::ParseEnum(value, enableInputsToOutputs, {
			{ "Enabled", Analysis::Enabled },
			{ "Disabled", Analysis::Disabled },
		});
	}
	if (key == "Circuit/LogicalDependence/OutputsToInputs") {
		return Settings::ParseEnum(value, enableOutputsToInputs, {
			{ "Enabled", Analysis::Enabled },
			{ "Disabled", Analysis::Disabled },
		});
	}
	if (key == "Circuit/LogicalDependence/OutputsToOutputs") {
		return Settings::ParseEnum(value, enableOutputsToOutputs, {
			{ "Enabled", Analysis::Enabled },
			{ "Disabled", Analysis::Disabled },
		});
	}
	if (key == "Circuit/LogicalDependence/PortInclude") {
		portInclude = value;
		return true;
	}
	if (key == "Circuit/LogicalDependence/PortExclude") {
		portExclude = value;
		return true;
	}

	return Mixin::VcmMixin::SetSetting(key, value);
}

void ExportCircuitLogicalDependence::Run(void)
{
	Mixin::VcmMixin::Run();

	auto const& mappedCircuit { circuit->GetMappedCircuit() };
	auto const inputCount { mappedCircuit.GetNumberOfInputs() };
	auto const outputCount { mappedCircuit.GetNumberOfOutputs() };
	inputsToInputs = decltype(inputsToInputs) (inputCount, std::vector<LogicalDependency>(inputCount, LogicalDependency::StructuralIndependent));
	inputsToOutputs = decltype(inputsToOutputs) (inputCount, std::vector<LogicalDependency>(outputCount, LogicalDependency::StructuralIndependent));
	outputsToInputs = decltype(outputsToInputs) (outputCount, std::vector<LogicalDependency>(inputCount, LogicalDependency::StructuralIndependent));
	outputsToOutputs = decltype(outputsToOutputs) (outputCount, std::vector<LogicalDependency>(outputCount, LogicalDependency::StructuralIndependent));

	inputsIncluded = std::vector<bool>(inputCount, true);
	outputsIncluded = std::vector<bool>(outputCount, true);
	inputsConstant = std::vector<LogicalConstant>(inputCount, LogicalConstant::NotConstant);
	outputsConstant = std::vector<LogicalConstant>(outputCount, LogicalConstant::NotConstant);

	auto includeRegex { std::regex { portInclude } };
	auto excludeRegex { std::regex { portExclude } };
	for (auto [index, input] : mappedCircuit.EnumerateInputs()) {
		auto name { GetName(input) };
		if (portInclude != "") { inputsIncluded[index] = std::regex_match(name, includeRegex); }
		if (portExclude != "") { inputsIncluded[index] = !std::regex_match(name, excludeRegex); }
	}
	for (auto [index, output] : mappedCircuit.EnumerateOutputs()) {
		auto name { GetName(output) };
		if (portInclude != "") { outputsIncluded[index] = std::regex_match(name, includeRegex); }
		if (portExclude != "") { outputsIncluded[index] = !std::regex_match(name, excludeRegex); }
	}

	size_t inputsCount { 0u };
	size_t outputsCount { 0u };
	for (auto const& inputIncluded : inputsIncluded) {
		if (inputIncluded) { inputsCount++; }
	}
	for (auto const& outputIncluded : outputsIncluded) {
		if (outputIncluded) { outputsCount++; }
	}
	LOG(INFO) << "Inputs included " << inputsCount << " of " << inputsIncluded.size();
	for (auto [index, input] : mappedCircuit.EnumerateInputs()) {
		if (!inputsIncluded[index]) { continue; }
		LOG(INFO) << "+ " << GetName(input);
	}
	LOG(INFO) << "Outputs included " << outputsCount << " of " << outputsIncluded.size();
	for (auto [index, output] : mappedCircuit.EnumerateOutputs()) {
		if (!outputsIncluded[index]) { continue; }
		LOG(INFO) << "+ " << GetName(output);
	}

	Parallel::SetThreads(Parallel::Arena::General, 0);
	ComputeStructuralDependence();
	ComputeFunctionalDependence();
	ExportData();
}

void ExportCircuitLogicalDependence::ComputeStructuralDependence() {
	LOG(INFO) << "Computing structural dependence";

	auto const& mappedCircuit { circuit->GetMappedCircuit() };

	std::shared_ptr<SolverProxy::ISolverProxy> nullProxy;
	Tpg::GeneratorContext<Tpg::PinDataG<Tpg::LogicContainer01>> context { nullProxy, circuit };
	context.SetNumberOfTimeframes(1u);
	auto& timeframe { context.GetTimeframe(0u) };

	if (enableInputsToOutputs == Analysis::Enabled || enableInputsToInputs == Analysis::Enabled) {
		// Forward propagate from inputs to outputs
		LOG(INFO) << "+ Computing inputs -> outputs connectivity";
		for (const auto [inputIndex, primaryInput] : mappedCircuit.EnumerateInputs()) {
			timeframe.Reset();

			std::vector<size_t> forwardNodes { primaryInput->GetNodeId() };
			while (!forwardNodes.empty()) {
				timeframe.TagOutputCone<Tpg::GoodTag>(context, forwardNodes);
				forwardNodes.clear();

				// ---------------------------------------------------------------------
				// connection input -> output exist?
				// ---------------------------------------------------------------------
				std::vector<size_t> backwardNodes { };
				for (const auto [outputIndex, output] : mappedCircuit.EnumerateOutputs()) {
					if (timeframe.template HasTag<Tpg::GoodTag>(output->GetNodeId())) {
						inputsToOutputs[inputIndex][outputIndex] |= LogicalDependency::StructuralDependent;
						backwardNodes.push_back(output->GetNodeId());
					}
				}
				timeframe.TagInputCone<Tpg::BadTag>(context, backwardNodes);
				backwardNodes.clear();

				// ---------------------------------------------------------------------
				// connection input -> output -> input exist?
				// ---------------------------------------------------------------------
				for (auto [otherIndex, input] : mappedCircuit.EnumerateInputs()) {
					if (timeframe.template HasTag<Tpg::GoodTag>(input->GetNodeId())) {
						inputsToInputs[inputIndex][otherIndex] |= LogicalDependency::StructuralDependent;
					} else if (timeframe.template HasTag<Tpg::BadTag>(input->GetNodeId())) {
						inputsToInputs[inputIndex][otherIndex] |= LogicalDependency::StructuralDependent;
						forwardNodes.push_back(input->GetNodeId());
					}
				}
			}
		}
	}

	if (enableOutputsToInputs == Analysis::Enabled || enableOutputsToOutputs == Analysis::Enabled) {
		// Backward propagate from outputs to inputs
		LOG(INFO) << "+ Computing outputs -> inputs connectivity";
		for (const auto [outputIndex, primaryOutput] : mappedCircuit.EnumerateOutputs()) {
			timeframe.Reset();

			std::vector<size_t> backwardNodes { primaryOutput->GetNodeId() };
			while (!backwardNodes.empty()) {
				timeframe.TagInputCone<Tpg::GoodTag>(context, backwardNodes);
				backwardNodes.clear();

				// ---------------------------------------------------------------------
				// connection output -> input exist?
				// ---------------------------------------------------------------------
				std::vector<size_t> forwardNodes { };
				for (const auto [inputIndex, input] : mappedCircuit.EnumerateInputs()) {
					if (timeframe.template HasTag<Tpg::GoodTag>(input->GetNodeId())) {
						forwardNodes.push_back(input->GetNodeId());
						outputsToInputs[outputIndex][inputIndex] |= LogicalDependency::StructuralDependent;
					}
				}

				// ---------------------------------------------------------------------
				// connection output -> input -> output exist?
				// ---------------------------------------------------------------------
				timeframe.TagOutputCone<Tpg::BadTag>(context, forwardNodes);
				forwardNodes.clear();

				for (auto [otherIndex, output] : mappedCircuit.EnumerateOutputs()) {
					if (timeframe.template HasTag<Tpg::GoodTag>(output->GetNodeId())) {
						outputsToOutputs[outputIndex][otherIndex] |= LogicalDependency::StructuralDependent;
					} else if (timeframe.template HasTag<Tpg::BadTag>(output->GetNodeId())) {
						outputsToOutputs[outputIndex][otherIndex] |= LogicalDependency::StructuralDependent;
						backwardNodes.push_back(output->GetNodeId());
					}
				}
			}
		}
	}
}

void ExportCircuitLogicalDependence::ComputeFunctionalDependence() {
	auto const& mappedCircuit { circuit->GetMappedCircuit() };
	auto const inputCount { mappedCircuit.GetNumberOfInputs() };
	auto const outputCount { mappedCircuit.GetNumberOfOutputs() };

	const auto create_generator = [&]() -> auto {
		// The logic generator combines all tagging, encoding and constraint modules into one generator.
		std::shared_ptr<SolverProxy::Sat::SatSolverProxy> satSolver = SolverProxy::Sat::SatSolverProxy::CreateSatSolver(Settings::GetInstance()->SatSolver);
		satSolver->SetSolverTimeout(60);

		using PinData = Tpg::PinDataGBD<Tpg::LogicContainer01>;
		using VcmPinData = Tpg::PinDataG<Tpg::LogicContainer01>;
		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(satSolver, this->circuit);
		logicGenerator->GetContext().SetIncremental(false);
		logicGenerator->GetContext().SetNumberOfTimeframes(1u);
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, Tpg::GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, Tpg::BadTag>>();
		logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, Tpg::DiffTag>>();
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, Tpg::BadTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, Tpg::BadTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, Tpg::BadTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, Tpg::BadTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, Tpg::GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, Tpg::BadTag>>();
		logicGenerator->template EmplaceModule<Tpg::DChainBaseEncoder<PinData>>();
		logicGenerator->template EmplaceModule<Tpg::BackwardDChainEncoder<PinData>>();
		assert (logicGenerator->GenerateCircuitLogic());

		if (vcmEnable == Mixin::VcmMixin::VcmEnable::Enabled) {
			auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>>(satSolver, this->vcmCircuit);
			vcmLogicGenerator->GetContext().SetIncremental(false);
			vcmLogicGenerator->GetContext().SetNumberOfTimeframes(1u);
			vcmLogicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, Tpg::GoodTag>>();
			vcmLogicGenerator->template EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(logicGenerator, this->vcmInputs);
			vcmLogicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
			vcmLogicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
			vcmLogicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, Tpg::GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
			vcmLogicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, Tpg::GoodTag>>();
			vcmLogicGenerator->template EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(this->vcmOutputs);
			assert (vcmLogicGenerator->GenerateCircuitLogic());
		}

		return std::make_tuple(satSolver, logicGenerator);
	};

	// -----------------------------------------------------------------------------------------------------------------
	// Constant ports
	// -----------------------------------------------------------------------------------------------------------------
	LOG(INFO) << "Computing constants ports";

	if (enableInputsToInputs == Analysis::Enabled || enableOutputsToInputs == Analysis::Enabled || enableInputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing constant inputs";
		Parallel::ExecuteParallel(0u, inputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto inputIndex) {
			if (IsExcluded(mappedCircuit.GetInput(inputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			const auto inputNodeId { mappedCircuit.GetInput(inputIndex)->GetNodeId() };
			const auto inputContainer { timeframe.GetContainer<Tpg::GoodTag>(context, inputNodeId, Circuit::PortType::Output, 0u) };

			// ---------------------------------------------------------------------
			// positive(input) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(inputContainer, Basic::LogicConstraint::NOT_LOGIC_ONE).l0);

			const auto positiveResult = satSolver->Solve();
			assert (positiveResult != SolverProxy::Sat::SatResult::UNKNOWN);
			if (positiveResult == SolverProxy::Sat::SatResult::UNSAT) {
				inputsConstant[inputIndex] = LogicalConstant::ConstantPositive;
				return;
			}

			// ---------------------------------------------------------------------
			// negative(input) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(inputContainer, Basic::LogicConstraint::NOT_LOGIC_ZERO).l0);

			const auto negativeResult = satSolver->Solve();
			assert (negativeResult != SolverProxy::Sat::SatResult::UNKNOWN);
			if (negativeResult == SolverProxy::Sat::SatResult::UNSAT) {
				inputsConstant[inputIndex] = LogicalConstant::ConstantNegative;
				return;
			}
		});
	}

	if (enableInputsToOutputs == Analysis::Enabled || enableOutputsToOutputs == Analysis::Enabled || enableOutputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing constant outputs";
		Parallel::ExecuteParallel(0u, outputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto outputIndex) {
			if (IsExcluded(mappedCircuit.GetOutput(outputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			const auto outputNodeId { mappedCircuit.GetOutput(outputIndex)->GetNodeId() };
			const auto outputContainer { timeframe.GetContainer<Tpg::GoodTag>(context, outputNodeId, Circuit::PortType::Input, 0u) };

			// ---------------------------------------------------------------------
			// positive(input) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(outputContainer, Basic::LogicConstraint::NOT_LOGIC_ONE).l0);

			const auto positiveResult = satSolver->Solve();
			assert (positiveResult != SolverProxy::Sat::SatResult::UNKNOWN);
			if (positiveResult == SolverProxy::Sat::SatResult::UNSAT) {
				outputsConstant[outputIndex] = LogicalConstant::ConstantPositive;
				return;
			}

			// ---------------------------------------------------------------------
			// negative(input) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(outputContainer, Basic::LogicConstraint::NOT_LOGIC_ZERO).l0);

			const auto negativeResult = satSolver->Solve();
			assert (negativeResult != SolverProxy::Sat::SatResult::UNKNOWN);
			if (negativeResult == SolverProxy::Sat::SatResult::UNSAT) {
				outputsConstant[outputIndex] = LogicalConstant::ConstantNegative;
				return;
			}
		});
	}

	// -----------------------------------------------------------------------------------------------------------------
	// Functional Dependence
	// -----------------------------------------------------------------------------------------------------------------
	// Unateness:
	// - https://si2.epfl.ch/~demichel/publications/archive/2016/2016_hvc.pdf
	// - https://en.wikipedia.org/wiki/Unate_function

	auto const compute_source_dest_dependence = [&](auto& satSolver, auto& encoder, auto& sourceG, auto& sourceD, auto& destG, auto& destD, auto& constant) {
		const auto solve_no_timeout = [&]() {
			const auto result = satSolver->Solve();
			assert (result != SolverProxy::Sat::SatResult::UNKNOWN);
			return result;
		};

		auto result = static_cast<LogicalDependency>(0);

		if (constant != LogicalConstant::ConstantNegative) {
			// ---------------------------------------------------------------------
			// positive(input) -> positive(output) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceG, Basic::Logic::LOGIC_ONE).l0);
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(destG, Basic::LogicConstraint::NOT_LOGIC_ONE).l0);
			if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
				result |= LogicalDependency::FunctionalPositiveForcesPositive;
			} else {
				// ---------------------------------------------------------------------
				// positive(input) -> negative(output) always?
				// ---------------------------------------------------------------------
				satSolver->ClearAssumptions();
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceG, Basic::Logic::LOGIC_ONE).l0);
				satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(destG, Basic::LogicConstraint::NOT_LOGIC_ZERO).l0);
				if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
					result |= LogicalDependency::FunctionalPositiveForcesNegative;
				}
			}
		}

		if (constant != LogicalConstant::ConstantPositive) {
			// ---------------------------------------------------------------------
			// negative(input) -> positive(output) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceG, Basic::Logic::LOGIC_ZERO).l0);
			satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(destG, Basic::LogicConstraint::NOT_LOGIC_ONE).l0);
			if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
				result |= LogicalDependency::FunctionalNegativeForcesPositive;
			} else {
				// ---------------------------------------------------------------------
				// negative(input) -> negative(output) always?
				// ---------------------------------------------------------------------
				satSolver->ClearAssumptions();
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceG, Basic::Logic::LOGIC_ZERO).l0);
				satSolver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(destG, Basic::LogicConstraint::NOT_LOGIC_ZERO).l0);
				if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
					result |= LogicalDependency::FunctionalNegativeForcesNegative;
				}
			}
		}

		if (constant == LogicalConstant::NotConstant) {
			// ---------------------------------------------------------------------
			// difference(input) -> difference(output) always?
			// ---------------------------------------------------------------------
			satSolver->ClearAssumptions();
			satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceD, Basic::Logic::LOGIC_ONE).l0);
			satSolver->AddAssumption(encoder.EncodeLogicValueDetector(destD, Basic::Logic::LOGIC_ZERO).l0);
			if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
				result |= LogicalDependency::FunctionalBoth;
			} else {
				// ---------------------------------------------------------------------
				// difference(input) -> difference(output) and
				// positive(input) <=> positive(output) always?
				// ---------------------------------------------------------------------
				satSolver->ClearAssumptions();
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceD, Basic::Logic::LOGIC_ONE).l0);
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(destD, Basic::Logic::LOGIC_ONE).l0);
				satSolver->AddAssumption(encoder.EncodeLogicEquivalenceDetector(sourceG, destG).l0);
				if (const auto positiveResult = solve_no_timeout(); positiveResult == SolverProxy::Sat::SatResult::UNSAT) {
					result |= LogicalDependency::FunctionalNegative;
				}

				// ---------------------------------------------------------------------
				// difference(input) -> difference(output) and
				// positive(input) <=> negative(output) always?
				// ---------------------------------------------------------------------
				satSolver->ClearAssumptions();
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(sourceD, Basic::Logic::LOGIC_ONE).l0);
				satSolver->AddAssumption(encoder.EncodeLogicValueDetector(destD, Basic::Logic::LOGIC_ONE).l0);
				satSolver->AddAssumption(encoder.EncodeLogic01DifferenceDetector(sourceG, destG).l0);
				if (const auto satResult = solve_no_timeout(); satResult == SolverProxy::Sat::SatResult::UNSAT) {
					result |= LogicalDependency::FunctionalPositive;
				}
			}
		}

		if (result != LogicalDependency::FunctionalIndependent) {
			result |= LogicalDependency::FunctionalDependent;
		}
		return result;
	};

	LOG(INFO) << "Computing functional dependence";

	// -------------------------------------------------------------------------
	// Input to outputs
	// -------------------------------------------------------------------------

	if (enableInputsToOutputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing inputs -> outputs functional dependence";
		std::atomic<int> counter { 0 };
		Parallel::ExecuteParallel(0u, inputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto inputIndex) {
			auto count = counter.fetch_add(1, std::memory_order_acq_rel) + 1;
			if (count % 100 == 99) {
				LOG(INFO) << "  " << std::setprecision(2) << (count * 100.0f / inputCount) << " %";
			}

			if (IsExcluded(mappedCircuit.GetInput(inputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			for (size_t outputIndex { 0u }; outputIndex < outputCount; outputIndex++) {
				if (IsExcluded(mappedCircuit.GetOutput(outputIndex))) { continue; }
				if (inputsToOutputs[inputIndex][outputIndex] != LogicalDependency::StructuralDependent) {
					inputsToOutputs[inputIndex][outputIndex] |= LogicalDependency::FunctionalIndependent;
					continue;
				}
				if (outputsConstant[outputIndex] == LogicalConstant::ConstantPositive) {
					inputsToOutputs[inputIndex][outputIndex] |= LogicalDependency::ConstantPositve;
					continue;
				}
				if (outputsConstant[outputIndex] == LogicalConstant::ConstantNegative) {
					inputsToOutputs[inputIndex][outputIndex] |= LogicalDependency::ConstantNegative;
					continue;
				}

				const auto sourceId { mappedCircuit.GetInput(inputIndex)->GetNodeId() };
				const auto sourceG { timeframe.GetContainer<Tpg::GoodTag>(context, sourceId, Circuit::PortType::Output, 0u) };
				const auto sourceD { timeframe.GetContainer<Tpg::DiffTag>(context, sourceId, Circuit::PortType::Output, 0u) };

				const auto destId { mappedCircuit.GetOutput(outputIndex)->GetNodeId() };
				const auto destG { timeframe.GetContainer<Tpg::GoodTag>(context, destId, Circuit::PortType::Input, 0u) };
				const auto destD { timeframe.GetContainer<Tpg::DiffTag>(context, destId, Circuit::PortType::Input, 0u) };

				const auto result { compute_source_dest_dependence(satSolver, encoder, sourceG, sourceD, destG, destD, inputsConstant[inputIndex]) };
				inputsToOutputs[inputIndex][outputIndex] |= result;
			}
		});
	}

	// -------------------------------------------------------------------------
	// Input to inputs
	// -------------------------------------------------------------------------

	if (enableInputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing inputs -> inputs functional dependence";
		std::atomic<int> counter { 0 };
		Parallel::ExecuteParallel(0u, inputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto inputIndex) {
			auto count = counter.fetch_add(1, std::memory_order_acq_rel) + 1;
			if (count % 100 == 99) {
				LOG(INFO) << "  " << std::setprecision(2) << (count * 100.0f / inputCount) << " %";
			}

			if (IsExcluded(mappedCircuit.GetInput(inputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			for (size_t otherIndex { 0u }; otherIndex < inputCount; otherIndex++) {
				if (IsExcluded(mappedCircuit.GetInput(otherIndex))) { continue; }
				if (inputsToInputs[inputIndex][otherIndex] != LogicalDependency::StructuralDependent) {
					inputsToInputs[inputIndex][otherIndex] |= LogicalDependency::FunctionalIndependent;
					continue;
				}
				if (inputsConstant[otherIndex] == LogicalConstant::ConstantPositive) {
					inputsToInputs[inputIndex][otherIndex] |= LogicalDependency::ConstantPositve;
					continue;
				}
				if (inputsConstant[otherIndex] == LogicalConstant::ConstantNegative) {
					inputsToInputs[inputIndex][otherIndex] |= LogicalDependency::ConstantNegative;
					continue;
				}

				const auto sourceId { mappedCircuit.GetInput(inputIndex)->GetNodeId() };
				const auto sourceG { timeframe.GetContainer<Tpg::GoodTag>(context, sourceId, Circuit::PortType::Output, 0u) };
				const auto sourceD { timeframe.GetContainer<Tpg::DiffTag>(context, sourceId, Circuit::PortType::Output, 0u) };

				const auto destId { mappedCircuit.GetInput(otherIndex)->GetNodeId() };
				const auto destG { timeframe.GetContainer<Tpg::GoodTag>(context, destId, Circuit::PortType::Output, 0u) };
				const auto destD { timeframe.GetContainer<Tpg::DiffTag>(context, destId, Circuit::PortType::Output, 0u) };

				const auto result { compute_source_dest_dependence(satSolver, encoder, sourceG, sourceD, destG, destD, inputsConstant[inputIndex]) };
				inputsToInputs[inputIndex][otherIndex] |= result;
			}
		});
	}

	// -------------------------------------------------------------------------
	// Outputs to inputs
	// -------------------------------------------------------------------------

	if (enableOutputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing outputs -> inputs functional dependence";
		std::atomic<int> counter { 0 };
		Parallel::ExecuteParallel(0u, outputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto outputIndex) {
			auto count = counter.fetch_add(1, std::memory_order_acq_rel) + 1;
			if (count % 100 == 99) {
				LOG(INFO) << "  " << std::setprecision(2) << (count * 100.0f / outputCount) << " %";
			}
			if (IsExcluded(mappedCircuit.GetOutput(outputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			for (size_t inputIndex { 0u }; inputIndex < inputCount; inputIndex++) {
				if (IsExcluded(mappedCircuit.GetInput(inputIndex))) { continue; }
				if (outputsToInputs[outputIndex][inputIndex] != LogicalDependency::StructuralDependent) {
					outputsToInputs[outputIndex][inputIndex] |= LogicalDependency::FunctionalIndependent;
					continue;
				}
				if (inputsConstant[inputIndex] == LogicalConstant::ConstantPositive) {
					outputsToInputs[outputIndex][inputIndex] |= LogicalDependency::ConstantPositve;
					continue;
				}
				if (inputsConstant[inputIndex] == LogicalConstant::ConstantNegative) {
					outputsToInputs[outputIndex][inputIndex] |= LogicalDependency::ConstantNegative;
					continue;
				}

				const auto sourceId { mappedCircuit.GetOutput(outputIndex)->GetNodeId() };
				const auto sourceG { timeframe.GetContainer<Tpg::GoodTag>(context, sourceId, Circuit::PortType::Input, 0u) };
				const auto sourceD { timeframe.GetContainer<Tpg::DiffTag>(context, sourceId, Circuit::PortType::Input, 0u) };

				const auto destId { mappedCircuit.GetInput(inputIndex)->GetNodeId() };
				const auto destG { timeframe.GetContainer<Tpg::GoodTag>(context, destId, Circuit::PortType::Output, 0u) };
				const auto destD { timeframe.GetContainer<Tpg::DiffTag>(context, destId, Circuit::PortType::Output, 0u) };

				const auto result { compute_source_dest_dependence(satSolver, encoder, sourceG, sourceD, destG, destD, outputsConstant[outputIndex]) };
				outputsToInputs[outputIndex][inputIndex] |= result;
			}
		});
	}

	// -------------------------------------------------------------------------
	// Outputs to outputs
	// -------------------------------------------------------------------------

	if (enableOutputsToOutputs == Analysis::Enabled) {
		LOG(INFO) << "+ Computing outputs -> outputs functional dependence";
		std::atomic<int> counter { 0 };
		Parallel::ExecuteParallel(0u, outputCount, Parallel::Arena::General, Parallel::Order::Parallel, [&](auto outputIndex) {
			auto count = counter.fetch_add(1, std::memory_order_acq_rel) + 1;
			if (count % 100 == 99) {
				LOG(INFO) << "  " << std::setprecision(2) << (count * 100.0f / outputCount) << " %";
			}
			if (IsExcluded(mappedCircuit.GetOutput(outputIndex))) { return; }

			auto [satSolver, logicGenerator] = create_generator();
			auto& context { logicGenerator->GetContext() };
			auto& encoder { context.GetEncoder() };
			auto& timeframe { context.GetTimeframe(0u) };

			for (size_t otherIndex { 0u }; otherIndex < outputCount; otherIndex++) {
				if (IsExcluded(mappedCircuit.GetOutput(otherIndex))) { continue; }
				if (outputsToOutputs[outputIndex][otherIndex] != LogicalDependency::StructuralDependent) {
					outputsToOutputs[outputIndex][otherIndex] |= LogicalDependency::FunctionalIndependent;
					continue;
				}
				if (outputsConstant[otherIndex] == LogicalConstant::ConstantPositive) {
					outputsToOutputs[outputIndex][otherIndex] |= LogicalDependency::ConstantPositve;
					continue;
				}
				if (outputsConstant[otherIndex] == LogicalConstant::ConstantNegative) {
					outputsToOutputs[outputIndex][otherIndex] |= LogicalDependency::ConstantNegative;
					continue;
				}

				const auto sourceId { mappedCircuit.GetOutput(outputIndex)->GetNodeId() };
				const auto sourceG { timeframe.GetContainer<Tpg::GoodTag>(context, sourceId, Circuit::PortType::Input, 0u) };
				const auto sourceD { timeframe.GetContainer<Tpg::DiffTag>(context, sourceId, Circuit::PortType::Input, 0u) };

				const auto destId { mappedCircuit.GetOutput(otherIndex)->GetNodeId() };
				const auto destG { timeframe.GetContainer<Tpg::GoodTag>(context, destId, Circuit::PortType::Input, 0u) };
				const auto destD { timeframe.GetContainer<Tpg::DiffTag>(context, destId, Circuit::PortType::Input, 0u) };

				const auto result { compute_source_dest_dependence(satSolver, encoder, sourceG, sourceD, destG, destD, outputsConstant[outputIndex]) };
				outputsToOutputs[outputIndex][otherIndex] |= result;
			}
		});
	}
}

void ExportCircuitLogicalDependence::ExportData() const {
	auto const& mappedCircuit { circuit->GetMappedCircuit() };

	const auto export_text_data = [&](auto& stream, auto const& table, auto const& dimX, auto const& labelX, auto const& dimY, auto const& labelY) {
		stream << "Matrix: (X: " << labelX << " -> Y: " << labelY << ")" << std::endl;
		for (auto [yIndex, y] : dimY) {
			if (IsExcluded(y)) { continue; }
			stream << "\t";
			for (auto [xIndex, x] : dimX) {
				if (IsExcluded(x)) { continue; }
				stream << to_short_string(table[xIndex][yIndex]);
			}
			stream << std::endl;
		}
		stream << std::endl;

		stream << "Detail:" << std::endl;
		for (auto [xIndex, x] : dimX) {
			if (IsExcluded(x)) { continue; }
			stream << GetName(x) << std::endl;
			for (auto [yIndex, y] : dimY) {
				if (IsExcluded(y)) { continue; }
				stream << "\t" << to_string(table[xIndex][yIndex]) << " " << GetName(x) << " -> " << GetName(y) << std::endl;
			}
		}
	};
	const auto export_json_data = [&](auto& stream, auto const& table, auto const& dimX, auto const& labelX, auto const& dimY, auto const& labelY) {
		boost::property_tree::ptree constants;
		for (auto [yIndex, y] : dimY) {
			if (IsExcluded(y)) { continue; }
			if (table.size() == 0) {
				continue;
			}

			const auto relation { table[0][yIndex] };
			const auto contains = [&](auto dep) { return (relation & dep) == dep; };

			if (contains(LogicalDependency::ConstantPositve) || contains(LogicalDependency::ConstantNegative)) {
				boost::property_tree::ptree element;
				element.add("destination", GetName(y));
				element.add("relation", "constant");
				element.add("constant", contains(LogicalDependency::ConstantPositve) ? 1 : 0);
				constants.push_back(std::make_pair("", element));
			}
		}

		boost::property_tree::ptree relations;
		for (auto [xIndex, x] : dimX) {
			if (IsExcluded(x)) { continue; }
			for (auto [yIndex, y] : dimY) {
				if (IsExcluded(y)) { continue; }
				boost::property_tree::ptree element;
				element.add("source", GetName(x));
				element.add("destination", GetName(y));

				const auto relation { table[xIndex][yIndex] };
				const auto contains = [&](auto dep) { return (relation & dep) == dep; };

				if (contains(LogicalDependency::ConstantPositve) || contains(LogicalDependency::ConstantNegative)) {
					continue;
				} else if (contains(LogicalDependency::FunctionalDependent)) {
					element.add("relation", "dependent");
					element.add("does_source_force_dest_to_same", contains(LogicalDependency::FunctionalEquivalent));
					element.add("does_source_force_dest_to_inverse", contains(LogicalDependency::FunctionalInverted));
					element.add("does_source_1_force_dest_1", contains(LogicalDependency::FunctionalPositiveForcesPositive));
					element.add("does_source_0_force_dest_1", contains(LogicalDependency::FunctionalNegativeForcesPositive));
					element.add("does_source_1_force_dest_0", contains(LogicalDependency::FunctionalPositiveForcesNegative));
					element.add("does_source_0_force_dest_0", contains(LogicalDependency::FunctionalNegativeForcesNegative));
					element.add("can_source_toggle_output_in_same_direction", contains(LogicalDependency::FunctionalPositive));
					element.add("can_source_toggle_output_in_inverse_direction", contains(LogicalDependency::FunctionalNegative));
				} else if (contains(LogicalDependency::StructuralDependent) | contains(LogicalDependency::StructuralIndependent)) {
					element.add("relation", "independent");
					element.add("connected", contains(LogicalDependency::StructuralDependent));
				}

				relations.push_back(std::make_pair("", element));
			}
		}

		boost::property_tree::ptree root;
		root.add("sources", labelX);
		root.add("destinations", labelY);
		root.add_child("constants", constants);
		root.add_child("relations", relations);
		boost::property_tree::write_jsonc(stream, root, true);
	};

	LOG(INFO) << "Exporting dependency data";
	if (enableInputsToOutputs == Analysis::Enabled) {
		LOG(INFO) << "+ Exporting inputs -> outputs data";
		for (auto& [name, source, dest] : std::vector<std::tuple<std::string, Circuit::NodeEnumeration, Circuit::NodeEnumeration>> {
			{ "all_inputs_to_all_outputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumerateOutputs() },
			{ "all_inputs_to_primary_outputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "all_inputs_to_secondary_outputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumerateSecondaryOutputs() },
			{ "primary_inputs_to_all_outputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumerateOutputs() },
			{ "primary_inputs_to_primary_outputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "primary_inputs_to_secondary_outputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumerateSecondaryOutputs() },
			{ "secondary_inputs_to_all_outputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumerateOutputs() },
			{ "secondary_inputs_to_primary_outputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "secondary_inputs_to_secondary_outputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumerateSecondaryOutputs() },
		}) {
			FileHandle textHandle("[DataExportDirectory]/" + name + ".txt", false);
			FileHandle jsonHandle("[DataExportDirectory]/" + name + ".json", false);
			export_text_data(textHandle.GetOutStream(), inputsToOutputs, source, "Inputs", dest, "Outputs");
			export_json_data(jsonHandle.GetOutStream(), inputsToOutputs, source, "inputs", dest, "outputs");
		}
	}

	if (enableInputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Exporting inputs -> inputs data";
		for (auto& [name, source, dest] : std::vector<std::tuple<std::string, Circuit::NodeEnumeration, Circuit::NodeEnumeration>> {
			{ "all_inputs_to_all_inputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumerateInputs() },
			{ "all_inputs_to_primary_inputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "all_inputs_to_secondary_inputs", mappedCircuit.EnumerateInputs(), mappedCircuit.EnumerateSecondaryInputs() },
			{ "primary_inputs_to_all_inputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumerateInputs() },
			{ "primary_inputs_to_primary_inputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "primary_inputs_to_secondary_inputs", mappedCircuit.EnumeratePrimaryInputs(), mappedCircuit.EnumerateSecondaryInputs() },
			{ "secondary_inputs_to_all_inputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumerateInputs() },
			{ "secondary_inputs_to_primary_inputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "secondary_inputs_to_secondary_inputs", mappedCircuit.EnumerateSecondaryInputs(), mappedCircuit.EnumerateSecondaryInputs() },
		}) {
			FileHandle textHandle("[DataExportDirectory]/" + name + ".txt", false);
			FileHandle jsonHandle("[DataExportDirectory]/" + name + ".json", false);
			export_text_data(textHandle.GetOutStream(), inputsToInputs, source, "Inputs", dest, "Inputs");
			export_json_data(jsonHandle.GetOutStream(), inputsToInputs, source, "inputs", dest, "inputs");
		}
	}

	if (enableOutputsToInputs == Analysis::Enabled) {
		LOG(INFO) << "+ Exporting outputs -> inputs data";
		for (auto& [name, source, dest] : std::vector<std::tuple<std::string, Circuit::NodeEnumeration, Circuit::NodeEnumeration>> {
			{ "all_outputs_to_all_inputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumerateInputs() },
			{ "all_outputs_to_primary_inputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "all_outputs_to_secondary_inputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumerateSecondaryInputs() },
			{ "primary_outputs_to_all_inputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumerateInputs() },
			{ "primary_outputs_to_primary_inputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "primary_outputs_to_secondary_inputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumerateSecondaryInputs() },
			{ "secondary_outputs_to_all_inputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumerateInputs() },
			{ "secondary_outputs_to_primary_inputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumeratePrimaryInputs() },
			{ "secondary_outputs_to_secondary_inputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumerateSecondaryInputs() },
		}) {
			FileHandle textHandle("[DataExportDirectory]/" + name + ".txt", false);
			FileHandle jsonHandle("[DataExportDirectory]/" + name + ".json", false);
			export_text_data(textHandle.GetOutStream(), outputsToInputs, source, "Outputs", dest, "Inputs");
			export_json_data(jsonHandle.GetOutStream(), outputsToInputs, source, "outputs", dest, "inputs");
		}
	}

	if (enableOutputsToOutputs == Analysis::Enabled) {
		LOG(INFO) << "+ Exporting outputs -> outputs data";
		for (auto& [name, source, dest] : std::vector<std::tuple<std::string, Circuit::NodeEnumeration, Circuit::NodeEnumeration>> {
			{ "all_outputs_to_all_outputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumerateOutputs() },
			{ "all_outputs_to_primary_outputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "all_outputs_to_secondary_outputs", mappedCircuit.EnumerateOutputs(), mappedCircuit.EnumerateSecondaryOutputs() },
			{ "primary_outputs_to_all_outputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumerateOutputs() },
			{ "primary_outputs_to_primary_outputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "primary_outputs_to_secondary_outputs", mappedCircuit.EnumeratePrimaryOutputs(), mappedCircuit.EnumerateSecondaryOutputs() },
			{ "secondary_outputs_to_all_outputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumerateOutputs() },
			{ "secondary_outputs_to_primary_outputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumeratePrimaryOutputs() },
			{ "secondary_outputs_to_secondary_outputs", mappedCircuit.EnumerateSecondaryOutputs(), mappedCircuit.EnumerateSecondaryOutputs() },
		}) {
			FileHandle textHandle("[DataExportDirectory]/" + name + ".txt", false);
			FileHandle jsonHandle("[DataExportDirectory]/" + name + ".json", false);
			export_text_data(textHandle.GetOutStream(), outputsToOutputs, source, "Outputs", dest, "Outputs");
			export_json_data(jsonHandle.GetOutStream(), outputsToOutputs, source, "outputs", dest, "outputs");
		}
	}
}

std::string ExportCircuitLogicalDependence::GetName(const Circuit::MappedNode* node) const {
	auto const& mappedCircuit { circuit->GetMappedCircuit() };
	auto const& metaData { circuit->GetMetaData() };

	std::string name;
	if (mappedCircuit.IsPrimaryInput(node)) {
		name = metaData.GetFriendlyName({ node, { Circuit::PortType::Output, 0u } });
	} else if (mappedCircuit.IsPrimaryOutput(node)) {
		name = metaData.GetFriendlyName({ node, { Circuit::PortType::Input, 0u } });
	} else if (mappedCircuit.IsSecondaryInput(node)) {
		name = metaData.GetFriendlyName({ node->GetSuccessor(0u), { Circuit::PortType::Output, 0u } });
	} else if (mappedCircuit.IsSecondaryOutput(node)) {
		name = metaData.GetFriendlyName({ node, { Circuit::PortType::Input, 0u } });
	} else {
		name = metaData.GetFriendlyName({ node, { Circuit::PortType::Output, 0u } });
	}

	ASSERT(name.find("/") != std::string::npos) << "Name is missing circuit name";
	return name.substr(name.find("/") + 1u);
}

bool ExportCircuitLogicalDependence::IsExcluded(const Circuit::MappedNode* node) const {
	auto const& mappedCircuit { circuit->GetMappedCircuit() };
	if (mappedCircuit.IsInput(node)) {
		return !inputsIncluded[mappedCircuit.GetInputNumber(node)];
	}
	if (mappedCircuit.IsOutput(node)) {
		return !outputsIncluded[mappedCircuit.GetOutputNumber(node)];
	}
	return true;
}

};
};
};
