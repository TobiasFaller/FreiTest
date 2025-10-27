#include "Applications/Fuzzing/StimSatFuzzing.hpp"

#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"
#include "Circuit/DriverFinder.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"

using namespace SolverProxy;

namespace FreiTest
{
namespace Application
{
namespace Fuzzing
{

const std::string optionPrefix = "Fuzzing";

StimSatFuzzing::StimSatFuzzing(void):
	stimuliPath("UNINITIALIZED"),
	stimuli(),
	primaryInputs(),
	primaryOutputs(),
	secondaryInputs(),
	secondaryOutputs()
{
}

StimSatFuzzing::~StimSatFuzzing(void) = default;

bool StimSatFuzzing::SetSetting(std::string key, std::string value)
{
    if (Settings::IsOption(key, "Stimuli", optionPrefix))
    {
        stimuliPath = value;
        return true;
    }

    return false;
}

void StimSatFuzzing::Init(void)
{
	//std::mt19937 random { seed };
	//stimuli = { timeframes, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_DONT_CARE };
	//for (size_t input { 0u }; input < mappedCircuit.GetNumberOfSecondaryInputs(); input++)
	//{
	//	stimuli.SetSecondaryInput(0u, input, (random() % 2u) ? Basic::Logic::LOGIC_ZERO : Basic::Logic::LOGIC_ONE);
	//}
	//for (size_t timeframe { 0u }; timeframe < timeframes; timeframe++)
	//{
	//	for (size_t input { 0u }; input < mappedCircuit.GetNumberOfPrimaryInputs(); input++)
	//	{
	//	    stimuli.SetPrimaryInput(timeframe, input, (random() % 2u) ? Basic::Logic::LOGIC_ZERO : Basic::Logic::LOGIC_ONE);
	//	}
	//}

    boost::property_tree::ptree root {};
	boost::property_tree::read_jsonc(Settings::GetInstance()->MapFileName(stimuliPath), root);

	enum class Type { PrimaryInput, PrimaryOutput, SecondaryInput, SecondaryOutput };
	auto const& mappedCircuit { circuit->GetMappedCircuit() };
	auto find_all = [&](auto const& source, auto& references, auto const& type) {
		for (auto it = source.ordered_begin(); it != source.not_found(); ++it)
		{
			auto reference { circuit->GetName() };
			if (type == Type::PrimaryInput || type == Type::PrimaryOutput)
			{
				auto const port { it->second.template get_value<std::string>() };
				if (StringHelper::EndsWith("]", port))
				{
					reference += "/" + port + "/" + port;
				}
				else
				{
					reference += "/" + port + " [0]/" + port + " [0]";
				}
			}
			else if (type == Type::SecondaryInput || type == Type::SecondaryOutput)
			{
				reference += "/" + it->second.template get_value<std::string>() + "/gate";
			}

			auto group { circuit->GetMetaData().GetGroup(reference) };
			ASSERT(group != nullptr) << "Reference " << reference << " was not found";

			auto const find_node = [&](auto& self, auto const* group, bool (Circuit::MappedCircuit::* check)(const Circuit::MappedNode* node) const) -> bool {
				for (auto const* node : group->GetMappedNodes())
				{
					if ((mappedCircuit.*check)(node))
					{
						references.push_back(node);
						return true;
					}
				}

				for (auto const* subgroup : group->GetSubGroups())
				{
					if (self(self, subgroup, check))
					{
						return true;
					}
				}
				return false;
			};

			if (type == Type::PrimaryInput) ASSERT(find_node(find_node, group, &Circuit::MappedCircuit::IsPrimaryInput)) << "Could not find port";
			if (type == Type::PrimaryOutput) ASSERT(find_node(find_node, group, &Circuit::MappedCircuit::IsPrimaryOutput)) << "Could not find port";
			if (type == Type::SecondaryInput) ASSERT(find_node(find_node, group, &Circuit::MappedCircuit::IsSecondaryInput)) << "Could not find port";
			if (type == Type::SecondaryOutput) ASSERT(find_node(find_node, group, &Circuit::MappedCircuit::IsSecondaryOutput)) << "Could not find port";
		}
	};

	auto const& inputs { root.get_child("circuit.inputs") };
	auto const& outputs { root.get_child("circuit.outputs") };
	auto const& flipflops { root.get_child("circuit.flipflops") };
	find_all(inputs, primaryInputs, Type::PrimaryInput);
	find_all(outputs, primaryOutputs, Type::PrimaryOutput);
	find_all(flipflops, secondaryInputs, Type::SecondaryInput);
	find_all(flipflops, secondaryOutputs, Type::SecondaryOutput);

	for (auto const* input : primaryInputs)
	{
		ASSERT(mappedCircuit.IsPrimaryInput(input));
	}
	for (auto const* output : primaryOutputs)
	{
		ASSERT(mappedCircuit.IsPrimaryOutput(output));
	}
	for (auto const* input : secondaryInputs)
	{
		ASSERT(mappedCircuit.IsSecondaryInput(input));
	}
	for (auto const* output : secondaryOutputs)
	{
		ASSERT(mappedCircuit.IsSecondaryOutput(output));
	}

	auto const& stimInsChild { root.get_child("stimuli.inputs") };
	auto const stimFfsChild { root.get_child("stimuli.flipflops") };
	stimuli = { stimInsChild.size(), mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs(), Basic::Logic::LOGIC_DONT_CARE };

	auto const stimFfs { stimFfsChild.template get_value<std::string>() };
	ASSERT(stimFfs.size() == secondaryInputs.size());
	for (size_t index { 0u }; index < secondaryInputs.size(); index++)
	{
		stimuli.SetSecondaryInput(0u, mappedCircuit.GetSecondaryInputNumber(secondaryInputs[index]), Basic::GetLogicForCharacter(stimFfs[index]));
	}
	size_t timeframe { 0u };
	for (auto it = stimInsChild.ordered_begin(); it != stimInsChild.not_found(); ++it)
	{
		auto const stimIns { it->second.template get_value<std::string>() };
		ASSERT(stimIns.size() == primaryInputs.size());
		for (size_t index { 0u }; index < primaryInputs.size(); index++)
		{
			stimuli.SetPrimaryInput(timeframe, mappedCircuit.GetPrimaryInputNumber(primaryInputs[index]), Basic::GetLogicForCharacter(stimIns[index]));
		}
		timeframe++;
	}
}

void StimSatFuzzing::Run(void)
{
    using PinData = Tpg::PinDataG<Tpg::LogicContainer01X>;

	std::shared_ptr<Sat::SatSolverProxy> solver = Sat::SatSolverProxy::CreateSatSolver(Settings::GetInstance()->SatSolver);
    ASSERT(solver) << "Could not initialize SAT-Solver!";
	solver->SetSolverTimeout(10000);

	auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(solver, this->circuit);
	logicGenerator->GetContext().SetIncremental(false);
	logicGenerator->GetContext().SetNumberOfTimeframes(stimuli.GetNumberOfTimeframes());
	logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, Tpg::GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, Tpg::GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, Tpg::GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, Tpg::GoodTag>>();
    logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, Tpg::GoodTag>>(Tpg::MakePredefinedPortConstraint(stimuli.GetPrimaryInputs()), Tpg::ConstrainedPorts::Inputs);
	logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, Tpg::GoodTag>>(Tpg::MakeConstantSequentialInitializer(stimuli.GetSecondaryInputs(0u)), Tpg::ConstrainedSequentials::Inputs);
	ASSERT(logicGenerator->GenerateCircuitLogic());
    ASSERT(solver->Solve() == Sat::SatResult::SAT);

    auto const simulation { Tpg::Extractor::ExtractSimulationResult<PinData, Tpg::GoodTag>(logicGenerator->GetContext()) };

    FileHandle handle("[DataExportDirectory]/trace.txt", false);
    auto& out { handle.GetOutStream() };

	auto const convert_to_string = [&simulation](auto const& references, size_t timeframe) -> std::string {
		std::string values;
		for (auto const* reference : references)
		{
			values += to_string(simulation[timeframe][reference->GetNodeId()]);
		}
		return values;
	};

	for (size_t timeframe { 0u }; timeframe < stimuli.GetNumberOfTimeframes(); timeframe++)
	{
		out << "FF " << timeframe << " " << convert_to_string(this->secondaryInputs, timeframe) << std::endl;
		out << "IN " << timeframe << " " << convert_to_string(this->primaryInputs, timeframe) << std::endl;
		out << "OUT " << timeframe << " " << convert_to_string(this->primaryOutputs, timeframe) << std::endl;
	}
}

};
};
};
