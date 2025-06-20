#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "SolverProxy/SolverStatistics.hpp"
#include "SolverProxy/ISolverProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

enum class SatSolver
{
	PROD_SAT_SINGLE_GLUCOSE_421,
	PROD_SAT_PARALLEL_GLUCOSE_421,
	PROD_SAT_CADICAL,
	PROD_SAT_MINISAT,
	PROD_SAT_CRYPTO_MINISAT,
	PROD_SAT_IPASIR_DYNAMIC,
	PROD_SAT_IPASIR_STATIC,
	PROD_MAX_SAT_PACOSE,
	PROD_MAX_SAT_INC_BMO_COMPLETE,
	DEBUG_SAT,
	DEBUG_COUNT_SAT,
	DEBUG_MAX_SAT,
	EXPORT_SAT_DIMACS,
	EXPORT_MAX_SAT_DIMACS,
};

enum class DebugProxy
{
	ENABLED,
	DISABLED
};

enum class SatResult
{
	UNKNOWN,
	SAT,
	UNSAT
};

class SatSolverProxy:
	public ISolverProxy
{
public:
	SatSolverProxy(SatSolver solver);
	virtual ~SatSolverProxy(void);

	static std::unique_ptr<SatSolverProxy> CreateSatSolver(SatSolver solver, DebugProxy debug = DebugProxy::DISABLED);
	virtual SatSolver GetSolverType(void) const;

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
	virtual size_t GetNumberOfConflicts(void) const;
	virtual size_t GetNumberOfPropagations(void) const;

	// Clause API
	void NewClause(void) override;
	void CommitClause(void) override;
	BaseLiteral NewLiteral(void) override;
	void AddLiteral(const BaseLiteral lit) override;
	Value GetLiteralValue(const BaseLiteral lit) const override = 0;

	// SAT API
	virtual void Reset(void);
	virtual SatResult Solve(void);
	virtual SatResult GetLastResult(void) const;

	virtual bool IsIncrementalSupported(void) const = 0;
	virtual void AddAssumption(BaseLiteral literal);
	virtual void ClearAssumptions(void);
	virtual void SetAutomaticLiteral(BaseLiteral literal);
	virtual void ClearAutomaticLiteral(void);

	using ISolverProxy::CommitClause;

protected:
	BaseLiteral ConvertProxyToSolver(BaseLiteral lit) const;
	BaseLiteral ConvertSolverToProxy(BaseLiteral lit) const;

	SatSolver _solverType;
	double _solverTimeout;
	bool _solverDebug;
	bool _solverSilent;

	size_t _numberOfInstances;
	size_t _numberOfSolves;
	size_t _numberOfVariables;
	size_t _numberOfClauses;
	size_t _numberOfConclicts;
	size_t _numberOfPropagations;
	size_t _numberOfDecisions;

	BaseLiteral _automaticLiteral;
	std::vector<BaseLiteral> _currentClause;
	std::vector<BaseLiteral> _assumptions;

	SatResult _lastResult;
	size_t _solvesSinceReset;

};

std::string to_string(const SatSolver& solver);
std::ostream& operator<<(std::ostream& out, const SatSolver& result);

std::string to_string(const SatResult& result);
std::ostream& operator<<(std::ostream& out, const SatResult& result);

};
};
