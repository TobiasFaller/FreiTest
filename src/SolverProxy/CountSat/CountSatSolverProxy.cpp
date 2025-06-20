#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"

#include "SolverProxy/Sat/SatDebugProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

std::unique_ptr<CountSatSolverProxy> CountSatSolverProxy::CreateCountSatSolver(SatSolver solver, DebugProxy debug)
{
	if (debug == DebugProxy::ENABLED)
	{
#ifndef HAS_SAT_SOLVER_DEBUG
		Logging::Panic("Count-SAT debug solver is currently not supported!");
#else
		std::shared_ptr<CountSatSolverProxy> wrappedSolver = CreateCountSatSolver(solver);
		auto debugSolver = std::make_unique<SatDebugProxy>(SatSolver::DEBUG_COUNT_SAT);
		debugSolver->SetSolver(wrappedSolver);
		return debugSolver;
#endif
	}

	switch(solver)
	{
		case SatSolver::DEBUG_COUNT_SAT:
#ifdef HAS_SAT_SOLVER_DEBUG
			return std::make_unique<SatDebugProxy>(SatSolver::DEBUG_COUNT_SAT);
#else
			Logging::Panic("Count-SAT debug solver is currently not supported!");
#endif

		default:
			LOG(FATAL) << "Unknown Solver type: " << solver;
			return std::unique_ptr<CountSatSolverProxy>();
	}
}

// ----------------------------------------------------------------------------

CountSatSolverProxy::CountSatSolverProxy(SatSolver solver):
	SatSolverProxy(solver),
	_lastModelCount(0)
{
}

CountSatSolverProxy::~CountSatSolverProxy(void) = default;

SatResult CountSatSolverProxy::Solve(void)
{
	Logging::Panic("Not implemented! You have to use CountSolve() instead!");
}

size_t CountSatSolverProxy::GetLastModelCount(void) const
{
	return _lastModelCount;
}

};
};
