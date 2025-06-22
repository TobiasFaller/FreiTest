#define BOOST_TEST_MODULE UnaryCounter
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
#include "Tpg/LogicGenerator/UnaryCounter.hpp"
#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"
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

BOOST_AUTO_TEST_SUITE( UnaryCounterTest )

BOOST_AUTO_TEST_CASE( TestOneTimeframe )
{
	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	Builder::CircuitBuilder builder;
	Builder::BuildConfiguration config;
	std::shared_ptr<CircuitEnvironment> circuit = builder.BuildCircuitEnvironment(config);
	std::shared_ptr<Bmc::BmcSolverProxy> proxy = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
	proxy->SetSolverTimeout(5.0);

	Tpg::LogicGenerator<PinData> logicGenerator { proxy, circuit };
	auto& context = logicGenerator.GetContext();
	auto& encoder = context.GetEncoder();
	auto& unaryCounter = context.GetUnaryCounter();

	context.SetNumberOfTimeframes(1u);
	logicGenerator.EmplaceModule<Tpg::UnaryCounterEncoder<PinData>>();
	BOOST_VERIFY(logicGenerator.GenerateCircuitLogic());

	BOOST_CHECK_EQUAL(unaryCounter.GetBits(), 1u);

	// Encode requirement to generate at least one timeframe.
	proxy->SetTargetVariableType(Bmc::VariableType::Auxiliary);
	proxy->SetTargetClauseType(Bmc::ClauseType::Target);
	encoder.EncodeLogicValue(unaryCounter.GetStateForIndex(1u), Logic::LOGIC_ONE);

	BOOST_CHECK_EQUAL(proxy->Solve(), Bmc::BmcResult::Reachable);
	BOOST_CHECK_EQUAL(proxy->GetLastDepth(), 1u);

	// Finally: The test for the unary counter.
	proxy->SetTargetTimeframe(0u);
	BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetStateForIndex(0u)), Logic::LOGIC_ONE);
	BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetStateForIndex(1u)), Logic::LOGIC_ZERO);
	BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetIndicatorForIndex(0u)), Logic::LOGIC_ONE);
}

BOOST_AUTO_TEST_CASE( TestMultipleTimeframe )
{
	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	Builder::CircuitBuilder builder;
	Builder::BuildConfiguration config;
	std::shared_ptr<CircuitEnvironment> circuit = builder.BuildCircuitEnvironment(config);
	std::shared_ptr<Bmc::BmcSolverProxy> proxy = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
	proxy->SetSolverTimeout(5.0);

	Tpg::LogicGenerator<PinData> logicGenerator { proxy, circuit };
	auto& context = logicGenerator.GetContext();
	auto& encoder = context.GetEncoder();
	auto& unaryCounter = context.GetUnaryCounter();

	context.SetNumberOfTimeframes(10u);
	logicGenerator.EmplaceModule<Tpg::UnaryCounterEncoder<PinData>>();
	BOOST_VERIFY(logicGenerator.GenerateCircuitLogic());

	BOOST_CHECK_EQUAL(unaryCounter.GetBits(), 10u);

	// Encode requirement to generate at least one timeframe.
	proxy->SetTargetVariableType(Bmc::VariableType::Auxiliary);
	proxy->SetTargetClauseType(Bmc::ClauseType::Target);
	encoder.EncodeLogicValue(unaryCounter.GetStateForIndex(10u), Logic::LOGIC_ONE);

	BOOST_CHECK_EQUAL(proxy->Solve(), Bmc::BmcResult::Reachable);
	BOOST_CHECK_EQUAL(proxy->GetLastDepth(), 10u);

	// Finally: The test for the unary counter.
	for (size_t timeframe = 0u; timeframe < 10u; ++timeframe)
	{
		proxy->SetTargetTimeframe(timeframe);
		BOOST_CHECK_EQUAL(proxy->GetTargetTimeframe(), timeframe);

		for (size_t i = 0u; i < timeframe + 1u; ++i)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetStateForIndex(i)), Logic::LOGIC_ONE);
		}
		for (size_t i = timeframe + 1u; i < 11u; ++i)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetStateForIndex(i)), Logic::LOGIC_ZERO);
		}

		for (size_t i = 0u; i < timeframe; ++i)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetIndicatorForIndex(i)), Logic::LOGIC_ZERO);
		}
		BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetIndicatorForIndex(timeframe)), Logic::LOGIC_ONE);
		for (size_t i = timeframe + 1u; i < 10u; ++i)
		{
			BOOST_CHECK_EQUAL(encoder.GetSolvedLogicValue(unaryCounter.GetIndicatorForIndex(i)), Logic::LOGIC_ZERO);
		}
	}
}

BOOST_AUTO_TEST_SUITE_END()
