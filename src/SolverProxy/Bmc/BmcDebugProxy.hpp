#pragma once

#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "Basic/Logic.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/ModernClauseDB.hpp"

namespace SolverProxy
{
namespace Bmc
{

class BmcDebugProxy: public BmcSolverProxy
{
public:
	BmcDebugProxy(void);
	virtual ~BmcDebugProxy(void);

	void SetSolver(std::shared_ptr<BmcSolverProxy> solver);
	const std::shared_ptr<BmcSolverProxy>& GetSolver(void) const;

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

	// BMC API
	void Reset(void) override;
	BmcResult Solve(void) override;
	BmcResult GetLastResult(void) const override;
	size_t GetLastDepth(void) const override;

	void AddTimeframeLiteral(BaseLiteral lit, size_t timeframe) override;

	void SetTargetClauseType(ClauseType clauseType) override;
	ClauseType GetTargetClauseType(void) const override;

	void SetTargetVariableType(VariableType variableType) override;
	VariableType GetTargetVariableType(void) const override;

	void SetTargetTimeframe(size_t timeframe) override;
	size_t GetTargetTimeframe(void) const override;

	void SetMaximumDepth(size_t depth) override;
	size_t GetMaximumDepth(void) const override;

private:
	std::shared_ptr<BmcSolverProxy> solver;

};

};
};

#endif
