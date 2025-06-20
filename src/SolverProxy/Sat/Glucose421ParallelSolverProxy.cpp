#include "SolverProxy/Sat/Glucose421ParallelSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wreorder"
#include <glucose-4.2.1/parallel/MultiSolvers.h>
#pragma GCC diagnostic pop

#include "Basic/Logging.hpp"
#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Sat
{

Glucose421ParallelSolverProxy::Glucose421ParallelSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_PARALLEL_GLUCOSE_421),
	_solver(),
	_threadCount(0u),
	_maxThreadCount(0u)
{
	Reset();
}

Glucose421ParallelSolverProxy::~Glucose421ParallelSolverProxy(void) = default;

void Glucose421ParallelSolverProxy::Reset(void)
{
	_solver = std::make_unique<Glucose421::MultiSolvers>(GetUsedThreadCount());
	SatSolverProxy::Reset();
}

bool Glucose421ParallelSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

size_t Glucose421ParallelSolverProxy::GetThreadCount(void) const
{
	return _threadCount;
}

void Glucose421ParallelSolverProxy::SetThreadCount(size_t threads)
{
	_threadCount = threads;
}

size_t Glucose421ParallelSolverProxy::GetMaximumThreadCount(void) const
{
	return _maxThreadCount;
}

void Glucose421ParallelSolverProxy::SetMaximumThreadCount(size_t threads)
{
	_maxThreadCount = threads;
}

size_t Glucose421ParallelSolverProxy::GetUsedThreadCount(void) const
{
	if (_threadCount != 0u)
	{
		return  (_maxThreadCount != 0u && _threadCount > _maxThreadCount)
			? _maxThreadCount : _threadCount;
	}

	size_t threadCount = std::thread::hardware_concurrency();
	if (threadCount != 0)
	{
		LOG(INFO) << "Processor core count detected to be " << threadCount;
	}
	else
	{
		LOG(WARNING) << "Could not get processor core count. Assuming it to be 4!";
		threadCount = 4u;
	}

	if (_maxThreadCount != 0u && threadCount > _maxThreadCount)
	{
		return _maxThreadCount;
	}

	return threadCount;
}

BaseLiteral Glucose421ParallelSolverProxy::NewLiteral()
{
	_solver->newVar();
	return SatSolverProxy::NewLiteral();
}

void Glucose421ParallelSolverProxy::CommitClause()
{
	Glucose421::vec<Glucose421::Lit> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push(Glucose421::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_solver->addClause(clause);
	SatSolverProxy::CommitClause();
}

SatResult Glucose421ParallelSolverProxy::Solve(void)
{
	SatSolverProxy::Solve();

	auto threadCount { GetUsedThreadCount() };
	if (static_cast<size_t>(_solver->getSolverCount()) != threadCount) {
		_solver->setSolverCount(threadCount);
	}

	Glucose421::vec<Glucose421::Lit> assumptions;
	for (auto const& assumption : _assumptions)
	{
		LOG(INFO) << "assume " << to_string(assumption);
		assumptions.push(Glucose421::toLit(ConvertProxyToSolver(assumption).GetUnsigned()));
	}

	CallbackTimeoutHelper timeoutHelper([solver = _solver.get()]() { solver->interrupt(); });
	timeoutHelper.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { _solver->solve(assumptions, false, true) };
	timeoutHelper.StopTimeout();

	for (size_t thread { 0u }; thread < static_cast<size_t>(_solver->getSolverCount()); thread++)
	{
		_numberOfConclicts += _solver->getSolver(thread)->conflicts;
		_numberOfPropagations += _solver->getSolver(thread)->propagations;
		_numberOfDecisions += _solver->getSolver(thread)->decisions;
	}

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

Value Glucose421ParallelSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->model[ConvertProxyToSolver(lit).GetVariable()] };
	if (value == Glucose421::l_True || value == Glucose421::l_False)
	{
		return ((value ^ lit.IsNegated()) == Glucose421::l_True)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
