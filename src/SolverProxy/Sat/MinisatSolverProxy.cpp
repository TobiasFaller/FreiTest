#include "SolverProxy/Sat/MinisatSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_MINISAT

#include <minisat-2.2.0/simp/SimpSolver.h>

#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Sat
{

MinisatSolverProxy::MinisatSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_MINISAT),
	_solver()
{
	Reset();
}

MinisatSolverProxy::~MinisatSolverProxy(void) = default;

void MinisatSolverProxy::Reset(void)
{
	_solver = std::make_unique<Minisat::SimpSolver>();
	SatSolverProxy::Reset();
}

bool MinisatSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

BaseLiteral MinisatSolverProxy::NewLiteral()
{
	_solver->newVar();
	return SatSolverProxy::NewLiteral();
}

void MinisatSolverProxy::CommitClause()
{
	Minisat::vec<Minisat::Lit, int> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push(Minisat::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_solver->addClause(clause);
	SatSolverProxy::CommitClause();
}

SatResult MinisatSolverProxy::Solve(void)
{
	SatSolverProxy::Solve();

	Minisat::vec<Minisat::Lit, int> assumptions;
	for (auto& literal : _assumptions)
	{
		assumptions.push(Minisat::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}

	CallbackTimeoutHelper timeoutHelper([solver = _solver.get()]() { solver->interrupt(); });
	timeoutHelper.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { _solver->solveLimited(assumptions, false, true) };
	timeoutHelper.StopTimeout();

	_numberOfConclicts += _solver->conflicts;
	_numberOfPropagations += _solver->propagations;
	_numberOfDecisions += _solver->decisions;

	if (result == Minisat::l_True)
	{
		_lastResult = SatResult::SAT;
	}
	else if (result == Minisat::l_False)
	{
		_lastResult = SatResult::UNSAT;
	}
	else
	{
		_lastResult = SatResult::UNKNOWN;
	}
	return _lastResult;
}

Value MinisatSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->modelValue(Minisat::toLit(ConvertProxyToSolver(lit).GetUnsigned())) };
	if (value == Minisat::l_True || value == Minisat::l_False)
	{
		return (value == Minisat::l_True)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
