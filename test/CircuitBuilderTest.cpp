#define BOOST_TEST_MODULE CircuitBuilder
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>
#include <tuple>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"

using namespace FreiTest::Circuit;

int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace", "--log-verbose", "--log-verbose-modules=CircuitBuilder=9" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( CircuitBuilderTest )


BOOST_AUTO_TEST_CASE( TestWithBasicSingleInputGate )
{
	for (auto [name, category, type] : std::vector<std::tuple<std::string, CellCategory, CellType>> {
		{ "inverter", CellCategory::MAIN_INV, CellType::INV },
		{ "buffer", CellCategory::MAIN_BUF, CellType::BUF },
	})
	{
		Builder::CircuitBuilder builder;

		auto inputNodeId = builder.EmplaceMappedNode("input", CellCategory::MAIN_IN, CellType::P_IN, 0);
		auto inverterNodeId = builder.EmplaceMappedNode(name, category, type, 1);
		auto outputNodeId = builder.EmplaceMappedNode("output", CellCategory::MAIN_OUT, CellType::P_OUT, 1);
		auto& inputNode = builder.GetMappedNode(inputNodeId);
		auto& inverterNode = builder.GetMappedNode(inverterNodeId);
		auto& outputNode = builder.GetMappedNode(outputNodeId);

		inputNode.AddSuccessorNode(inverterNodeId);
		inverterNode.SetInputNode(0u, inputNodeId);
		inverterNode.AddSuccessorNode(outputNodeId);
		outputNode.SetInputNode(0u, inverterNodeId);

		builder.AddMappedPrimaryInput(inputNodeId);
		builder.AddMappedPrimaryOutput(outputNodeId);

		Builder::BuildConfiguration config;
		auto circuitEnvironment = std::shared_ptr<CircuitEnvironment>(builder.BuildCircuitEnvironment(config));
		auto& mappedCircuit = circuitEnvironment->GetMappedCircuit();

		// Basic checks for the circuit
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfNodes(), 3u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfInputs(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfOutputs(), 1u);

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetName(), "input");
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfInputs(), 0u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfSuccessors(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetSuccessor(0u), mappedCircuit.GetNode(1u));

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetName(), name);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfInputs(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfSuccessors(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetInput(0u), mappedCircuit.GetNode(0u));
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetSuccessor(0u), mappedCircuit.GetNode(2u));

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetName(), "output");
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfInputs(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfSuccessors(), 0u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetInput(0u), mappedCircuit.GetNode(1u));
	}
}


BOOST_AUTO_TEST_CASE( TestWithBasicTwoInputGate )
{
	for (auto [name, category, type] : std::vector<std::tuple<std::string, CellCategory, CellType>> {
		{ "and", CellCategory::MAIN_AND, CellType::AND },
		{ "nand", CellCategory::MAIN_NAND, CellType::NAND },
		{ "or", CellCategory::MAIN_OR, CellType::OR },
		{ "nor", CellCategory::MAIN_NOR, CellType::NOR },
		{ "xor", CellCategory::MAIN_XOR, CellType::XOR },
		{ "xnor", CellCategory::MAIN_XNOR, CellType::XNOR },
	})
	{
		Builder::CircuitBuilder builder;

		auto inputNodeId = builder.EmplaceMappedNode("input", CellCategory::MAIN_IN, CellType::P_IN, 0u);
		auto inputNodeId2 = builder.EmplaceMappedNode("input2", CellCategory::MAIN_IN, CellType::P_IN, 0u);
		auto andNodeId = builder.EmplaceMappedNode(name, category, type, 2u);
		auto outputNodeId = builder.EmplaceMappedNode("output", CellCategory::MAIN_OUT, CellType::P_OUT, 1u);
		auto& inputNode = builder.GetMappedNode(inputNodeId);
		auto& inputNode2 = builder.GetMappedNode(inputNodeId2);
		auto& andNode = builder.GetMappedNode(andNodeId);
		auto& outputNode = builder.GetMappedNode(outputNodeId);

		inputNode.AddSuccessorNode(andNodeId);
		inputNode2.AddSuccessorNode(andNodeId);
		andNode.SetInputNode(0u, inputNodeId);
		andNode.SetInputNode(1u, inputNodeId2);
		andNode.AddSuccessorNode(outputNodeId);
		outputNode.SetInputNode(0u, andNodeId);

		builder.AddMappedPrimaryInput(inputNodeId);
		builder.AddMappedPrimaryInput(inputNodeId2);
		builder.AddMappedPrimaryOutput(outputNodeId);

		Builder::BuildConfiguration config;
		auto circuitEnvironment = std::shared_ptr<CircuitEnvironment>(builder.BuildCircuitEnvironment(config));
		auto& mappedCircuit = circuitEnvironment->GetMappedCircuit();

		// Basic checks for the circuit
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfNodes(), 4u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfInputs(), 2u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfOutputs(), 1u);

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetName(), "input");
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfInputs(), 0u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfSuccessors(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetSuccessor(0u), mappedCircuit.GetNode(2u));

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetName(), "input2");
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfInputs(), 0u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfSuccessors(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetSuccessor(0u), mappedCircuit.GetNode(2u));

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetName(), name);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfInputs(), 2u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfSuccessors(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetInput(0u), mappedCircuit.GetNode(0u));
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetInput(1u), mappedCircuit.GetNode(1u));
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetSuccessor(0u), mappedCircuit.GetNode(3u));

		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetName(), "output");
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetNumberOfInputs(), 1u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetNumberOfSuccessors(), 0u);
		BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetInput(0u), mappedCircuit.GetNode(2u));
	}
}


BOOST_AUTO_TEST_CASE( TestWithComplexCircuit )
{
	Builder::CircuitBuilder builder;

	auto inputNodeId = builder.EmplaceMappedNode("input", CellCategory::MAIN_IN, CellType::P_IN, 0);
	auto inputNodeId2 = builder.EmplaceMappedNode("input2", CellCategory::MAIN_IN, CellType::P_IN, 0);
	auto inputNodeId3 = builder.EmplaceMappedNode("input3", CellCategory::MAIN_IN, CellType::P_IN, 0);
	auto inputNodeId4 = builder.EmplaceMappedNode("input4", CellCategory::MAIN_IN, CellType::P_IN, 0);
	auto xorNodeId = builder.EmplaceMappedNode("xor", CellCategory::MAIN_XNOR, CellType::XNOR, 3);
	auto andNodeId = builder.EmplaceMappedNode("and", CellCategory::MAIN_AND, CellType::AND, 2);
	auto orNodeId = builder.EmplaceMappedNode("or", CellCategory::MAIN_OR, CellType::OR, 2);
	auto outputNodeId = builder.EmplaceMappedNode("output", CellCategory::MAIN_OUT, CellType::P_OUT, 1);
	auto& inputNode = builder.GetMappedNode(inputNodeId);
	auto& inputNode2 = builder.GetMappedNode(inputNodeId2);
	auto& inputNode3 = builder.GetMappedNode(inputNodeId3);
	auto& inputNode4 = builder.GetMappedNode(inputNodeId4);
	auto& xorNode = builder.GetMappedNode(xorNodeId);
	auto& andNode = builder.GetMappedNode(andNodeId);
	auto& orNode = builder.GetMappedNode(orNodeId);
	auto& outputNode = builder.GetMappedNode(outputNodeId);

	inputNode.AddSuccessorNode(xorNodeId);
	inputNode2.AddSuccessorNode(xorNodeId);
	inputNode3.AddSuccessorNode(xorNodeId);
	inputNode4.AddSuccessorNode(orNodeId);
	xorNode.SetInputNode(0u, inputNodeId);
	xorNode.SetInputNode(1u, inputNodeId2);
	xorNode.SetInputNode(2u, inputNodeId3);
	xorNode.AddSuccessorNode(andNodeId);
	andNode.SetInputNode(0u, xorNodeId);
	andNode.SetInputNode(1u, xorNodeId);
	andNode.AddSuccessorNode(orNodeId);
	orNode.SetInputNode(0u, andNodeId);
	orNode.SetInputNode(1u, inputNodeId4);
	orNode.AddSuccessorNode(outputNodeId);
	outputNode.SetInputNode(0u, orNodeId);

	builder.AddMappedPrimaryInput(inputNodeId);
	builder.AddMappedPrimaryInput(inputNodeId2);
	builder.AddMappedPrimaryInput(inputNodeId3);
	builder.AddMappedPrimaryInput(inputNodeId4);
	builder.AddMappedPrimaryOutput(outputNodeId);

	Builder::BuildConfiguration config;
	auto circuitEnvironment = std::shared_ptr<CircuitEnvironment>(builder.BuildCircuitEnvironment(config));
	auto& mappedCircuit = circuitEnvironment->GetMappedCircuit();

	// Basic checks for the circuit
	BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfNodes(), 8u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfInputs(), 4u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNumberOfOutputs(), 1u);

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetName(), "input");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfInputs(), 0u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(0u)->GetSuccessor(0u), mappedCircuit.GetNode(4u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetName(), "input2");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfInputs(), 0u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(1u)->GetSuccessor(0u), mappedCircuit.GetNode(4u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetName(), "input3");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfInputs(), 0u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(2u)->GetSuccessor(0u), mappedCircuit.GetNode(4u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetName(), "input4");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetNumberOfInputs(), 0u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(3u)->GetSuccessor(0u), mappedCircuit.GetNode(6u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetName(), "xor");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetNumberOfInputs(), 3u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetInput(0u), mappedCircuit.GetNode(0u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetInput(1u), mappedCircuit.GetNode(1u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetInput(2u), mappedCircuit.GetNode(2u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(4u)->GetSuccessor(0u), mappedCircuit.GetNode(5u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetName(), "and");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetNumberOfInputs(), 2u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetInput(0u), mappedCircuit.GetNode(4u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetInput(1u), mappedCircuit.GetNode(4u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(5u)->GetSuccessor(0u), mappedCircuit.GetNode(6u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetName(), "or");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetNumberOfInputs(), 2u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetNumberOfSuccessors(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetInput(0u), mappedCircuit.GetNode(5u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetInput(1u), mappedCircuit.GetNode(3u));
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(6u)->GetSuccessor(0u), mappedCircuit.GetNode(7u));

	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(7u)->GetName(), "output");
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(7u)->GetNumberOfInputs(), 1u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(7u)->GetNumberOfSuccessors(), 0u);
	BOOST_CHECK_EQUAL(mappedCircuit.GetNode(7u)->GetInput(0u), mappedCircuit.GetNode(6u));
}

BOOST_AUTO_TEST_SUITE_END()
