#define BOOST_TEST_MODULE SolverProxy
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>
#include <memory>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest;
using namespace SolverProxy;

const std::vector<Sat::SatSolver> SOLVER_SAT {
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_SINGLE
	Sat::SatSolver::PROD_SAT_SINGLE_GLUCOSE_421,
#endif
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL
	Sat::SatSolver::PROD_SAT_PARALLEL_GLUCOSE_421,
#endif
#ifdef HAS_SAT_SOLVER_CADICAL
	Sat::SatSolver::PROD_SAT_CADICAL,
#endif
#ifdef HAS_SAT_SOLVER_MINISAT
	Sat::SatSolver::PROD_SAT_MINISAT,
#endif
#ifdef HAS_SAT_SOLVER_CRYPTO_MINISAT
	Sat::SatSolver::PROD_SAT_CRYPTO_MINISAT,
#endif
#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC
	// Requires built dynamic library (*.so file)
	// Sat::SatSolver::PROD_SAT_IPASIR_DYNAMIC,
#endif
#ifdef HAS_SAT_SOLVER_IPASIR_STATIC
	Sat::SatSolver::PROD_SAT_IPASIR_STATIC,
#endif
};

const std::vector<Sat::SatSolver> SOLVER_MAXSAT {
#ifdef HAS_MAX_SAT_SOLVER_PACOSE
	Sat::SatSolver::PROD_MAX_SAT_PACOSE,
#endif
#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE
	Sat::SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE,
#endif
};

const std::vector<Sat::SatSolver> SOLVER_COUNTSAT {
};

const std::vector<Bmc::BmcSolver> SOLVER_BMC {
#ifdef HAS_BMC_SOLVER_NCIP
	Bmc::BmcSolver::PROD_NCIP,
#endif
};

int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( SolverProxyTest )

BOOST_AUTO_TEST_CASE( TestBaseLiterals )
{
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(0), BaseLiteral::MakeFromSigned(1));
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(1), BaseLiteral::MakeFromSigned(-1));
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(2), BaseLiteral::MakeFromSigned(2));
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(3), BaseLiteral::MakeFromSigned(-2));
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(4), BaseLiteral::MakeFromSigned(3));
	BOOST_CHECK_EQUAL(BaseLiteral::MakeFromUnsigned(5), BaseLiteral::MakeFromSigned(-3));
}

BOOST_AUTO_TEST_CASE( TestLiteralIds )
{
	auto const test = [](auto& solver) {
		auto const type { solver->GetSolverType() };
		LOG(INFO) << "Solver " << to_string(type) << " initialized";
		BOOST_CHECK_EQUAL(solver->GetNumberOfVariables(), 1u); // Constant literal
		BOOST_CHECK_EQUAL(solver->NewLiteral().GetVariable(), 2);
		BOOST_CHECK_EQUAL(solver->GetNumberOfVariables(), 2u);
		BOOST_CHECK_EQUAL(solver->NewLiteral().GetVariable(), 3);
		BOOST_CHECK_EQUAL(solver->GetNumberOfVariables(), 3u);
		solver.reset();
		LOG(INFO) << "Solver " << to_string(type) << " destructed";
	};

	for (auto const& solver : SOLVER_SAT)
	{
		auto instance { Sat::SatSolverProxy::CreateSatSolver(solver) };
		test(instance);
	}
	for (auto const& solver : SOLVER_MAXSAT)
	{
		auto instance { Sat::MaxSatSolverProxy::CreateSatSolver(solver) };
		test(instance);
	}
	for (auto const& solver : SOLVER_COUNTSAT)
	{
		auto instance { Sat::CountSatSolverProxy::CreateSatSolver(solver) };
		test(instance);
	}
	for (auto const& solver : SOLVER_BMC)
	{
		auto instance { Bmc::BmcSolverProxy::CreateBmcSolver(solver) };
		test(instance);
	}
}

BOOST_AUTO_TEST_CASE( TestSatApi )
{
	auto const test = [](auto& solver) {
		auto const type { solver->GetSolverType() };
		LOG(INFO) << "Solver " << to_string(type) << " initialized";

		auto lit1 { solver->NewLiteral() };
		auto lit2 { solver->NewLiteral() };
		auto lit3 { solver->NewLiteral() };

		solver->CommitClause(lit1);
		BOOST_CHECK_EQUAL(solver->Solve(), Sat::SatResult::SAT);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit1), Value::Positive);

		solver->CommitClause(-lit1, -lit2);
		BOOST_CHECK_EQUAL(solver->Solve(), Sat::SatResult::SAT);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit1), Value::Positive);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit2), Value::Negative);

		if (solver->IsIncrementalSupported())
		{
			solver->AddAssumption(-lit3);
			BOOST_CHECK_EQUAL(solver->Solve(), Sat::SatResult::SAT);
			BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit1), Value::Positive);
			BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit2), Value::Negative);
			BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit3), Value::Negative);
			solver->AddAssumption(lit2);
			BOOST_CHECK_EQUAL(solver->Solve(), Sat::SatResult::UNSAT);
			solver->ClearAssumptions();
			BOOST_CHECK_EQUAL(solver->Solve(), Sat::SatResult::SAT);
			BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit1), Value::Positive);
			BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit2), Value::Negative);
		}
	};

	for (auto const& solver : SOLVER_SAT)
	{
		auto instance { Sat::SatSolverProxy::CreateSatSolver(solver) };
		instance->SetSolverTimeout(5.0);
		test(instance);
	}
}

BOOST_AUTO_TEST_CASE( TestMaxSatApi )
{
	auto const test = [](auto& solver) {
		auto const type { solver->GetSolverType() };
		LOG(INFO) << "Solver " << to_string(type) << " initialized";

		auto lit1 { solver->NewLiteral() };
		auto lit2 { solver->NewLiteral() };
		auto lit3 { solver->NewLiteral() };

		solver->CommitClause(-lit1);
		solver->CommitSoftClause(lit1, 3u);
		solver->CommitSoftClause(-lit2, 2u);
		solver->CommitSoftClause(-lit3, 1u);
		BOOST_CHECK_EQUAL(solver->MaxSolve(), Sat::SatResult::SAT);
		BOOST_CHECK_EQUAL(solver->GetLastCost(), 3u);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit1), Value::Negative);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit2), Value::Negative);
		BOOST_CHECK_EQUAL(solver->GetLiteralValue(lit3), Value::Negative);
	};

	for (auto const& solver : SOLVER_MAXSAT)
	{
		auto instance { Sat::MaxSatSolverProxy::CreateMaxSatSolver(solver) };
		instance->SetSolverTimeout(5.0);
		test(instance);
	}
}

BOOST_AUTO_TEST_CASE( TestCountSatApi )
{
	auto const test = [](auto& solver) {
		auto const type { solver->GetSolverType() };
		LOG(INFO) << "Solver " << to_string(type) << " initialized";

		auto lit1 { solver->NewLiteral() };
		auto lit2 { solver->NewLiteral() };
		auto lit3 { solver->NewLiteral() };

		solver->CommitClause(-lit2);
		solver->CommitClause(lit1, lit3);
		BOOST_CHECK_EQUAL(solver->CountSolve(), Sat::SatResult::SAT);
		BOOST_CHECK_EQUAL(solver->GetLastModelCount(), 3u);
	};

	for (auto const& solver : SOLVER_COUNTSAT)
	{
		auto instance { Sat::CountSatSolverProxy::CreateCountSatSolver(solver) };
		instance->SetSolverTimeout(5.0);
		test(instance);
	}
}

BOOST_AUTO_TEST_CASE( TestBmcApi )
{
	auto const test = [](auto& solver) {
		auto const type { solver->GetSolverType() };
		LOG(INFO) << "Solver " << to_string(type) << " initialized";

        solver->SetTargetVariableType(Bmc::VariableType::Latch);
		auto bit1 { solver->NewLiteral() };
		auto bit2 { solver->NewLiteral() };

        solver->SetTargetClauseType(Bmc::ClauseType::Initial);
        solver->CommitClause(-bit1);
        solver->CommitClause(-bit2);

        solver->SetTargetClauseType(Bmc::ClauseType::Transition);
		solver->CommitTimeframeClause(-bit1, 0u, -bit1, 1u);
		solver->CommitTimeframeClause( bit1, 0u,  bit1, 1u);
		solver->CommitTimeframeClause(-bit1, 0u, -bit2, 0u, -bit2, 1u);
		solver->CommitTimeframeClause( bit1, 0u, -bit2, 0u,  bit2, 1u);
		solver->CommitTimeframeClause(-bit1, 0u,  bit2, 0u,  bit2, 1u);
		solver->CommitTimeframeClause( bit1, 0u,  bit2, 0u, -bit2, 1u);

        solver->SetTargetClauseType(Bmc::ClauseType::Target);
        solver->CommitClause(bit1);
        solver->CommitClause(bit2);
		BOOST_CHECK_EQUAL(solver->Solve(), Bmc::BmcResult::Reachable);

        solver->SetTargetTimeframe(0u);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit1), Value::Negative);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit2), Value::Negative);

        solver->SetTargetTimeframe(1u);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit1), Value::Positive);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit2), Value::Negative);

        solver->SetTargetTimeframe(2u);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit1), Value::Negative);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit2), Value::Positive);

        solver->SetTargetTimeframe(3u);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit1), Value::Positive);
        BOOST_CHECK_EQUAL(solver->GetLiteralValue(bit2), Value::Positive);
        
        solver->SetTargetTimeframe(0u);
        solver->SetTargetClauseType(Bmc::ClauseType::Transition);
        solver->CommitClause(-bit1);
		BOOST_CHECK_EQUAL(solver->Solve(), Bmc::BmcResult::Unreachable);
	};

	for (auto const& solver : SOLVER_BMC)
	{
		auto instance { Bmc::BmcSolverProxy::CreateBmcSolver(solver) };
		instance->SetSolverTimeout(5.0);
		test(instance);
	}
}

BOOST_AUTO_TEST_SUITE_END()
