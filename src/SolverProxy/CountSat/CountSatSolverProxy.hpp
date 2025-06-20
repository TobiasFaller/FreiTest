#pragma once

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

class CountSatSolverProxy:
	public virtual SatSolverProxy
{
public:
	CountSatSolverProxy(SatSolver solver);
	virtual ~CountSatSolverProxy(void);

	static std::unique_ptr<CountSatSolverProxy> CreateCountSatSolver(SatSolver solver, DebugProxy debug = DebugProxy::DISABLED);

	// SAT API
	SatResult Solve(void) override; // Not implemented

	// #SAT API
	virtual SatResult CountSolve(void) = 0;
	virtual size_t GetLastModelCount(void) const;

	using ISolverProxy::CommitClause;

protected:
	size_t _lastModelCount;

};
};
};
