#include "SolverProxy/Sat/IpasirDynamicSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC

#include <atomic>
#include <memory>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "SolverProxy/Sat/Ipasir/DynamicIpasirLoader.hpp"
#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Sat
{

IpasirDynamicSolverProxy::IpasirDynamicSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_SAT_IPASIR_DYNAMIC),
	_ipasirLoader()
{
	Reset();
}

IpasirDynamicSolverProxy::~IpasirDynamicSolverProxy(void) = default;

void IpasirDynamicSolverProxy::Reset(void)
{
	_ipasirLoader = DynamicIpasirLoader::CreateInstance(Settings::GetInstance()->DynamicIpasirLibraryLocation);
	SatSolverProxy::Reset();
}

bool IpasirDynamicSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

void IpasirDynamicSolverProxy::CommitClause()
{
	for (auto const& literal : _currentClause)
	{
		_ipasirLoader->IpasirAdd(literal.GetSigned());
	}
	_ipasirLoader->IpasirAdd(0);
	SatSolverProxy::CommitClause();
}

SatResult IpasirDynamicSolverProxy::Solve(void)
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

	if (!_ipasirLoader) {
  		LOG(FATAL) << "Ipasir SAT-solver could not be created";
  		return SatResult::UNKNOWN;
	}

	IpasirTerminator terminator;
	auto ipasirTerminateFunction = +[](void * data) -> int {
		return reinterpret_cast<IpasirTerminator*>(data)->ShouldTerminate() ? 1 : 0;
	};
	_ipasirLoader->IpasirSetTerminate(&terminator, ipasirTerminateFunction);

	for (auto const& assumption : _assumptions)
	{
		_ipasirLoader->IpasirAssume(assumption.GetSigned());
	}

	terminator.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	auto result { _ipasirLoader->IpasirSolve() };
	terminator.StopTimeout();
	_ipasirLoader->IpasirSetTerminate(nullptr, nullptr);

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

Value IpasirDynamicSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _ipasirLoader->IpasirVal(ConvertProxyToSolver(lit).GetSigned()) };
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
