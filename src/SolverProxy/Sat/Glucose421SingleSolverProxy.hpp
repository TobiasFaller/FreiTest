#pragma once

#include <memory>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_GLUCOSE_421_SINGLE

namespace Glucose421
{
	class SimpSolver;
};

namespace SolverProxy
{
namespace Sat
{

class Glucose421SingleSolverProxy:
	public SatSolverProxy
{
public:
	Glucose421SingleSolverProxy(void);
	virtual ~Glucose421SingleSolverProxy(void);

	BaseLiteral NewLiteral(void) override;
	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<Glucose421::SimpSolver> _solver;

};

};
};

#endif
