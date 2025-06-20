#include "SolverProxy/Sat/Glucose421SingleSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_GLUCOSE_421_SINGLE

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wreorder"
#include <glucose-4.2.1/simp/SimpSolver.h>
#pragma GCC diagnostic pop

#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Sat
{

Glucose421SingleSolverProxy::Glucose421SingleSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_SINGLE_GLUCOSE_421),
	_solver()
{
	Reset();
}

Glucose421SingleSolverProxy::~Glucose421SingleSolverProxy(void) = default;

void Glucose421SingleSolverProxy::Reset(void)
{
	_solver = std::make_unique<Glucose421::SimpSolver>();
	SatSolverProxy::Reset();
}

bool Glucose421SingleSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

BaseLiteral Glucose421SingleSolverProxy::NewLiteral()
{
	_solver->newVar();
	return SatSolverProxy::NewLiteral();
}

void Glucose421SingleSolverProxy::CommitClause()
{
	Glucose421::vec<Glucose421::Lit> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push(Glucose421::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_solver->addClause(clause);
	SatSolverProxy::CommitClause();
}

SatResult Glucose421SingleSolverProxy::Solve(void)
{
	SatSolverProxy::Solve();

	Glucose421::vec<Glucose421::Lit> assumptions;
	for (auto const& assumption : _assumptions)
	{
		assumptions.push(Glucose421::toLit(ConvertProxyToSolver(assumption).GetUnsigned()));
	}

	CallbackTimeoutHelper timeoutHelper([solver = _solver.get()]() { solver->interrupt(); });
	timeoutHelper.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { _solver->solve(assumptions, false, true) };
	timeoutHelper.StopTimeout();

	_numberOfConclicts += _solver->conflicts;
	_numberOfPropagations += _solver->propagations;
	_numberOfDecisions += _solver->decisions;

	if (result == Glucose421::l_True)
	{
		_lastResult = SatResult::SAT;
	}
	else if (result == Glucose421::l_False)
	{
		_lastResult = SatResult::UNSAT;
	}
	else
	{
		_lastResult = SatResult::UNKNOWN;
	}
	return _lastResult;
}

Value Glucose421SingleSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->modelValue(Glucose421::toLit(ConvertProxyToSolver(lit).GetUnsigned())) };
	if (value == Glucose421::l_True || value == Glucose421::l_False)
	{
		return (value == Glucose421::l_True)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
