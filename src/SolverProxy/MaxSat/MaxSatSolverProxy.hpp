#pragma once

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

class MaxSatSolverProxy:
	public virtual SatSolverProxy
{
public:
	MaxSatSolverProxy(SatSolver solver);
	virtual ~MaxSatSolverProxy(void);

	static std::unique_ptr<MaxSatSolverProxy> CreateMaxSatSolver(SatSolver solver, DebugProxy debug = DebugProxy::DISABLED);

	// SAT API
	SatResult Solve(void) override; // Not implemented

	// Max-SAT API
	virtual SatResult MaxSolve(void) = 0;
	virtual void CommitSoftClause(size_t weight = 1u) = 0;
	virtual size_t GetLastCost(void) const;

	// Shorthand methods for convenience
	void CommitSoftClause(BaseLiteral l1, size_t weight = 1u);
	void CommitSoftClause(BaseLiteral l1, BaseLiteral l2, size_t weight = 1u);
	void CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, size_t weight = 1u);
	void CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, size_t weight = 1u);
	void CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, size_t weight = 1u);
	void CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, BaseLiteral l6, size_t weight = 1u);
	void CommitSoftClause(const std::vector< BaseLiteral>& literals, size_t weight = 1u);

protected:
	size_t _lastCost;

};

};
};
