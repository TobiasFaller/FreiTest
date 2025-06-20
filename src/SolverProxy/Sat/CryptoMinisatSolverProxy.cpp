#include "SolverProxy/Sat/CryptoMinisatSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_CRYPTO_MINISAT

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wmismatched-tags"
#include <crypto-minisat/cryptominisat.h>
#pragma GCC diagnostic pop

namespace SolverProxy
{
namespace Sat
{

CryptoMinisatSolverProxy::CryptoMinisatSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_CRYPTO_MINISAT),
	_solver()
{
	Reset();
}

CryptoMinisatSolverProxy::~CryptoMinisatSolverProxy(void) = default;

void CryptoMinisatSolverProxy::Reset(void)
{
	_solver = std::make_unique<CMSat::SATSolver>();
	SatSolverProxy::Reset();
}

bool CryptoMinisatSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

SatResult CryptoMinisatSolverProxy::Solve(void)
{
	SatSolverProxy::Solve();

	std::vector<CMSat::Lit> assumptions;
	for (auto const& assumption : _assumptions)
	{
		assumptions.push_back(CMSat::Lit::toLit(ConvertProxyToSolver(assumption).GetUnsigned()));
	}

	_solver->set_timeout_all_calls(_solverTimeout);
	auto result { _solver->solve(&assumptions) };

	if (result == CMSat::l_True)
	{
		_lastResult = SatResult::SAT;
	}
	else if (result == CMSat::l_False)
	{
		_lastResult = SatResult::UNSAT;
	}
	else
	{
		_lastResult = SatResult::UNKNOWN;
	}
	return _lastResult;
}

BaseLiteral CryptoMinisatSolverProxy::NewLiteral()
{
	_solver->new_var();
	return SatSolverProxy::NewLiteral();
}

void CryptoMinisatSolverProxy::CommitClause()
{
	std::vector<CMSat::Lit> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push_back(CMSat::Lit::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_solver->add_clause(clause);
	SatSolverProxy::CommitClause();
}

Value CryptoMinisatSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->get_model()[ConvertProxyToSolver(lit).GetVariable()] };
	if (value == CMSat::l_True || value == CMSat::l_False)
	{
		return ((value ^ lit.IsNegated()) == CMSat::l_True)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
