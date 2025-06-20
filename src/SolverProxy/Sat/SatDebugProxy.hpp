#pragma once

#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE

#include <memory>
#include <vector>

#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"
#include "SolverProxy/ModernClauseDB.hpp"

namespace SolverProxy
{
namespace Sat
{

class SatDebugProxy:
	public virtual SatSolverProxy,
	public virtual MaxSatSolverProxy,
	public virtual CountSatSolverProxy
{
public:
	SatDebugProxy(SatSolver solver);
	virtual ~SatDebugProxy(void);

	void SetSolver(std::shared_ptr<SatSolverProxy> solver);
	const std::shared_ptr<SatSolverProxy>& GetSolver(void) const;

	void SetSolverTimeout(double timeout) override;
	double GetSolverTimeout(void) const override;

	void SetSolverDebug(bool enable) override;
	bool GetSolverDebug(void) const override;

	void SetSolverSilent(bool silent) override;
	bool GetSolverSilent(void) const override;

	// Clause API
	void NewClause(void) override;
	void CommitClause(void) override;
	BaseLiteral NewLiteral(void) override;
	void AddLiteral(const BaseLiteral lit) override;
	Value GetLiteralValue(const BaseLiteral lit) const override;

	// SAT API
	void Reset(void) override;
	SatResult Solve(void) override;
	SatResult GetLastResult(void) const override;

	bool IsIncrementalSupported(void) const override;
	void AddAssumption(BaseLiteral literal) override;
	void ClearAssumptions(void) override;
	void SetAutomaticLiteral(BaseLiteral literal) override;
	void ClearAutomaticLiteral(void) override;

	// #SAT API
	SatResult CountSolve(void) override;
	size_t GetLastModelCount(void) const override;

	// MAX-SAT API
	SatResult MaxSolve(void) override;
	void CommitSoftClause(size_t weight = 1) override;
	size_t GetLastCost(void) const override;

private:
	std::shared_ptr<SatSolverProxy> solver;

};

};
};

#endif
