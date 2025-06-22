#define BOOST_TEST_MODULE BinaryCounter
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>
#include <memory>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/BinaryCounter.hpp"
#include "Tpg/LogicGenerator/Utility/BinaryCounterEncoder.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Tpg;
using namespace SolverProxy;

int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( BinaryCounterTest )

BOOST_AUTO_TEST_CASE( TestOneTimeframe )
{
	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	Builder::CircuitBuilder builder;
	Builder::BuildConfiguration config;
	std::shared_ptr<CircuitEnvironment> circuit = builder.BuildCircuitEnvironment(config);
	std::shared_ptr<Bmc::BmcSolverProxy> proxy = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
	proxy->SetSolverTimeout(5.0);
	proxy->SetSolverDebug(true);

	Tpg::LogicGenerator<PinData> logicGenerator { proxy, circuit };
	auto& context = logicGenerator.GetContext();
	auto& encoder = context.GetEncoder();
	auto& binaryCounter = context.GetBinaryCounter();

	context.SetNumberOfTimeframes(1u);
	logicGenerator.EmplaceModule<Tpg::BinaryCounterEncoder<PinData>>();
	BOOST_VERIFY(logicGenerator.GenerateCircuitLogic());

	BOOST_CHECK_EQUAL(binaryCounter.GetBits(), 1u);

	// Encode requirement to generate at least one timeframe.
	proxy->SetTargetVariableType(Bmc::VariableType::Auxiliary);
	proxy->SetTargetClauseType(Bmc::ClauseType::Target);
	encoder.EncodeLogicValue(binaryCounter.GetBit(0u), Logic::LOGIC_ONE);

	BOOST_CHECK_EQUAL(proxy->Solve(), Bmc::BmcResult::Reachable);
	BOOST_CHECK_EQUAL(proxy->GetLastDepth(), 1u);

	// Finally: The test for the binary counter.
	proxy->SetTargetTimeframe(0u);
	BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(binaryCounter.GetBit(0u)), Logic::LOGIC_ZERO);
}

BOOST_AUTO_TEST_CASE( TestMultipleTimeframe4 )
{
	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	Builder::CircuitBuilder builder;
	Builder::BuildConfiguration config;
	std::shared_ptr<CircuitEnvironment> circuit = builder.BuildCircuitEnvironment(config);
	std::shared_ptr<Bmc::BmcSolverProxy> proxy = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
	proxy->SetSolverTimeout(5.0);
	proxy->SetSolverDebug(true);

	Tpg::LogicGenerator<PinData> logicGenerator { proxy, circuit };
	auto& context = logicGenerator.GetContext();
	auto& encoder = context.GetEncoder();
	auto& binaryCounter = context.GetBinaryCounter();

	context.SetNumberOfTimeframes(4u);
	logicGenerator.EmplaceModule<Tpg::BinaryCounterEncoder<PinData>>();
	BOOST_VERIFY(logicGenerator.GenerateCircuitLogic());

	BOOST_CHECK_EQUAL(binaryCounter.GetBits(), 2u);

	// Encode requirement to generate at least one timeframe.
	proxy->SetTargetVariableType(Bmc::VariableType::Auxiliary);
	proxy->SetTargetClauseType(Bmc::ClauseType::Target);
	encoder.EncodeLogicValue(binaryCounter.GetBit(1u), Logic::LOGIC_ONE);
	encoder.EncodeLogicValue(binaryCounter.GetBit(0u), Logic::LOGIC_ONE);

	BOOST_CHECK_EQUAL(proxy->Solve(), Bmc::BmcResult::Reachable);
	BOOST_CHECK_EQUAL(proxy->GetLastDepth(), 3u);

	// Finally: The test for the binary counter.
	for (size_t timeframe { 0u }; timeframe < proxy->GetLastDepth(); ++timeframe)
	{
		proxy->SetTargetTimeframe(timeframe);
		BOOST_CHECK_EQUAL(proxy->GetTargetTimeframe(), timeframe);

		for (size_t bit { 0u }; bit < binaryCounter.GetBits(); ++bit)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(binaryCounter.GetBit(bit)),
				(timeframe & (1 << bit)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);
		}
	}
}

BOOST_AUTO_TEST_CASE( TestMultipleTimeframe10 )
{
	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	Builder::CircuitBuilder builder;
	Builder::BuildConfiguration config;
	std::shared_ptr<CircuitEnvironment> circuit = builder.BuildCircuitEnvironment(config);
	std::shared_ptr<Bmc::BmcSolverProxy> proxy = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
	proxy->SetSolverTimeout(5.0);
	proxy->SetSolverDebug(true);

	Tpg::LogicGenerator<PinData> logicGenerator { proxy, circuit };
	auto& context = logicGenerator.GetContext();
	auto& encoder = context.GetEncoder();
	auto& binaryCounter = context.GetBinaryCounter();

	context.SetNumberOfTimeframes(10u);
	logicGenerator.EmplaceModule<Tpg::BinaryCounterEncoder<PinData>>();
	BOOST_VERIFY(logicGenerator.GenerateCircuitLogic());

	BOOST_CHECK_EQUAL(binaryCounter.GetBits(), 4u);

	// Encode requirement to generate at least one timeframe.
	proxy->SetTargetVariableType(Bmc::VariableType::Auxiliary);
	proxy->SetTargetClauseType(Bmc::ClauseType::Target);
	encoder.EncodeLogicValue(binaryCounter.GetBit(3u), Logic::LOGIC_ONE);
	encoder.EncodeLogicValue(binaryCounter.GetBit(2u), Logic::LOGIC_ZERO);
	encoder.EncodeLogicValue(binaryCounter.GetBit(1u), Logic::LOGIC_ZERO);
	encoder.EncodeLogicValue(binaryCounter.GetBit(0u), Logic::LOGIC_ONE);

	BOOST_CHECK_EQUAL(proxy->Solve(), Bmc::BmcResult::Reachable);
	BOOST_CHECK_EQUAL(proxy->GetLastDepth(), 9u);

	// Finally: The test for the binary counter.
	for (size_t timeframe { 0u }; timeframe < proxy->GetLastDepth(); ++timeframe)
	{
		proxy->SetTargetTimeframe(timeframe);
		BOOST_CHECK_EQUAL(proxy->GetTargetTimeframe(), timeframe);

		for (size_t bit { 0u }; bit < binaryCounter.GetBits(); ++bit)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(binaryCounter.GetBit(bit)),
				(timeframe & (1 << bit)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
