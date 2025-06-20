#pragma once

#ifdef HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL

#include <memory>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace Glucose421
{
	class MultiSolvers;
};

namespace SolverProxy
{
namespace Sat
{

class Glucose421ParallelSolverProxy:
	public SatSolverProxy
{
public:
	Glucose421ParallelSolverProxy(void);
	virtual ~Glucose421ParallelSolverProxy(void);

	BaseLiteral NewLiteral(void) override;
	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	size_t GetMaximumThreadCount(void) const;
	size_t GetThreadCount(void) const;
	size_t GetUsedThreadCount(void) const;
	void SetMaximumThreadCount(size_t threads);
	void SetThreadCount(size_t threads);

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<Glucose421::MultiSolvers> _solver;
	size_t _threadCount;
	size_t _maxThreadCount;

};

};
};

#endif
