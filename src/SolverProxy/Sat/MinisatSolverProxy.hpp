#pragma once

#ifdef HAS_SAT_SOLVER_MINISAT

#include <memory>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace Minisat
{
	class SimpSolver;
};

namespace SolverProxy
{
namespace Sat
{

class MinisatSolverProxy:
	public SatSolverProxy
{
public:
	MinisatSolverProxy(void);
	virtual ~MinisatSolverProxy(void);

	BaseLiteral NewLiteral(void) override;
	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<Minisat::SimpSolver> _solver;

};

};
};

#endif
