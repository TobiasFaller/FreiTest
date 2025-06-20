#include "SolverProxy/Sat/CadicalSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_CADICAL

#include <cadical/cadical.hpp>

#include <atomic>

#include "SolverProxy/TimeoutHelper.hpp"

using namespace std;
using namespace FreiTest::Basic;

namespace SolverProxy
{
namespace Sat
{

CadicalSolverProxy::CadicalSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_CADICAL),
	_solver()
{
	Reset();
}

CadicalSolverProxy::~CadicalSolverProxy(void) = default;

void CadicalSolverProxy::Reset(void)
{
	_solver = std::make_unique<CaDiCaL::Solver>();
	SatSolverProxy::Reset();
}

bool CadicalSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

void CadicalSolverProxy::CommitClause()
{
	for (auto const& literal : _currentClause)
	{
		_solver->add(literal.GetSigned());
	}
	_solver->add(0);
	SatSolverProxy::CommitClause();
}

SatResult CadicalSolverProxy::Solve(void)
{
	class CadicalTerminator:
		public CaDiCaL::Terminator,
		public TimeoutHelper
	{
	public:
		CadicalTerminator(): terminated(false) { }

		bool terminate() override
		{
			return terminated.load(std::memory_order_acquire);
		}

		void OnTimeout(void) override
		{
			terminated.store(true, std::memory_order_release);
		}

	private:
		std::atomic<bool> terminated;
	};

	SatSolverProxy::Solve();

	_solver->reset_assumptions();
	for (auto const& assumption : _assumptions)
	{
		_solver->assume(assumption.GetSigned());
	}

	CadicalTerminator terminator;
	_solver->connect_terminator(&terminator);
	terminator.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { _solver->solve() };
	terminator.StopTimeout();
  	_solver->disconnect_terminator();

	if (result == 10)
	{
		_lastResult = SatResult::SAT;
	}
	else if (result == 20)
	{
		_lastResult = SatResult::UNSAT;
	}
	else
	{
		_lastResult = SatResult::UNKNOWN;
	}
	return _lastResult;
}

Value CadicalSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->val(lit.GetSigned()) };
	if (value != 0)
	{
		return (value > 0)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
