#include "SolverProxy/Sat/IpasirStaticSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_IPASIR_STATIC

#include <ipasir/ipasir.h>

#include <atomic>

#include "SolverProxy/TimeoutHelper.hpp"

using namespace std;
using namespace FreiTest::Basic;

namespace SolverProxy
{
namespace Sat
{

IpasirStaticSolverProxy::IpasirStaticSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_IPASIR_STATIC),
	_solver()
{
	Reset();
}

IpasirStaticSolverProxy::~IpasirStaticSolverProxy(void) = default;

void IpasirStaticSolverProxy::Reset(void)
{
	_solver = std::unique_ptr<void, IpasirDeleter>(ipasir_init());
	SatSolverProxy::Reset();
}

bool IpasirStaticSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

void IpasirStaticSolverProxy::CommitClause()
{
	for (auto const& literal : _currentClause)
	{
		ipasir_add(_solver.get(), ConvertProxyToSolver(literal).GetSigned());
	}
	ipasir_add(_solver.get(), 0);
	SatSolverProxy::CommitClause();
}

SatResult IpasirStaticSolverProxy::Solve(void)
{
	class IpasirTerminator: public TimeoutHelper
	{
	public:
		IpasirTerminator(): terminated(false) { }

		bool ShouldTerminate()
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

	IpasirTerminator terminator;
	auto ipasirTerminateFunction = +[](void * data) -> int {
		return reinterpret_cast<IpasirTerminator*>(data)->ShouldTerminate() ? 1 : 0;
	};
	ipasir_set_terminate(_solver.get(), &terminator, ipasirTerminateFunction);

	for (auto const& assumption : _assumptions)
	{
		ipasir_assume(_solver.get(), ConvertProxyToSolver(assumption).GetSigned());
	}

	terminator.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { ipasir_solve(_solver.get()) };
	terminator.StopTimeout();
	ipasir_set_terminate(_solver.get(), nullptr, nullptr);

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

Value IpasirStaticSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { ipasir_val(_solver.get(), ConvertProxyToSolver(lit).GetSigned()) };
	if (value == 0)
	{
		return Value::DontCare;
	}

	return (value > 0)
		? Value::Positive : Value::Negative;
}

};
};

#endif
