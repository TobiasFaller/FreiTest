#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "SolverProxy/ModernClauseDB.hpp"
#include "SolverProxy/ISolverProxy.hpp"

namespace SolverProxy
{
namespace Bmc
{

enum class BmcSolver
{
	PROD_NCIP,
	DEBUG_BMC,
	EXPORT_CIP
};

enum class DebugProxy
{
	ENABLED,
	DISABLED
};

enum class BmcResult
{
	Unreachable,
	Reachable,
	Timeout,
	MaxIterationsReached
};

enum class ClauseType
{
	Initial,
	Transition,
	Target
};

enum class VariableType
{
	Input,
	Output,
	Latch,
	Auxiliary
};

class BmcSolverProxy:
	public ISolverProxy
{
public:
	BmcSolverProxy(BmcSolver solverType);
	virtual ~BmcSolverProxy(void);

	static std::unique_ptr<BmcSolverProxy> CreateBmcSolver(BmcSolver type, DebugProxy debug = DebugProxy::DISABLED);
	virtual BmcSolver GetSolverType(void) const;

	virtual void SetSolverTimeout(double timeout);
	virtual double GetSolverTimeout(void) const;

	virtual void SetSolverDebug(bool enable);
	virtual bool GetSolverDebug(void) const;

	virtual void SetSolverSilent(bool silent);
	virtual bool GetSolverSilent(void) const;

	virtual size_t GetNumberOfInstances(void) const;
	virtual size_t GetNumberOfSolves(void) const;
	virtual size_t GetNumberOfClauses(void) const;
	virtual size_t GetNumberOfVariables(void) const;

	// Clause API
	void NewClause(void) override;
	void CommitClause(void) override;
	BaseLiteral NewLiteral(void) override;
	void AddLiteral(const BaseLiteral lit) override;
	Value GetLiteralValue(const BaseLiteral lit) const override = 0;

	// BMC API
	virtual void Reset(void);
	virtual BmcResult Solve(void);
	virtual BmcResult GetLastResult(void) const;
	virtual size_t GetLastDepth(void) const;

	virtual void AddTimeframeLiteral(BaseLiteral lit, size_t timeframe);

	virtual void SetTargetClauseType(ClauseType clauseType);
	virtual ClauseType GetTargetClauseType(void) const;

	virtual void SetTargetVariableType(VariableType variableType);
	virtual VariableType GetTargetVariableType(void) const;

	virtual void SetTargetTimeframe(size_t timeframe);
	virtual size_t GetTargetTimeframe(void) const;

	virtual void SetMaximumDepth(size_t depth);
	virtual size_t GetMaximumDepth(void) const;

	// Shorthand methods for convenience
	void CommitTimeframeClause(BaseLiteral lit1, size_t tf1);
	void CommitTimeframeClause(BaseLiteral lit1, size_t tf1, BaseLiteral lit2, size_t tf2);
	void CommitTimeframeClause(BaseLiteral lit1, size_t tf1, BaseLiteral lit2, size_t tf2, BaseLiteral lit3, size_t tf3);

	using ISolverProxy::CommitClause;

protected:
	BaseLiteral ConvertProxyToSolver(BaseLiteral lit) const;
	BaseLiteral ConvertSolverToProxy(BaseLiteral lit) const;

	BmcSolver _solverType;
	double _solverTimeout;
	bool _solverDebug;
	bool _solverSilent;

	size_t _numberOfInstances;
	size_t _numberOfSolves;
	size_t _numberOfVariables;
	size_t _numberOfClauses;

	size_t _maximumDepth;
	size_t _activeTimeframe;
	ClauseType _activeClauseType;
	VariableType _activeVariableType;
	std::vector<VariableType> _variableTypes;
	ModernClauseDB<std::pair<BaseLiteral, size_t>> _initialClauseDB;
	ModernClauseDB<std::pair<BaseLiteral, size_t>> _transitionClauseDB;
	ModernClauseDB<std::pair<BaseLiteral, size_t>> _targetClauseDB;
	ModernClauseDB<std::pair<BaseLiteral, size_t>>* _activeClauseDB;
	std::vector<std::pair<BaseLiteral, size_t>> _currentClause;

	BmcResult _lastResult;
	size_t _lastDepth;
	size_t _solvesSinceReset;

};

std::string to_string(const BmcSolver& solver);
std::ostream& operator<<(std::ostream& out, const BmcSolver& result);

std::string to_string(const BmcResult& result);
std::ostream& operator<<(std::ostream& out, const BmcResult& result);

};
};
