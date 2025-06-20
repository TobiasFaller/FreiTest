#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

#include <memory>
#include <type_traits>

#include "Basic/Logging.hpp"
#include "SolverProxy/Bmc/BmcDebugProxy.hpp"
#include "SolverProxy/Bmc/CipExportProxy.hpp"
#include "SolverProxy/Bmc/NcipSolverProxy.hpp"

namespace SolverProxy
{
namespace Bmc
{

std::unique_ptr<BmcSolverProxy> BmcSolverProxy::CreateBmcSolver(BmcSolver type, DebugProxy debug)
{
	if (debug == DebugProxy::ENABLED)
	{
#ifndef HAS_BMC_SOLVER_DEBUG
		Logging::Panic("BMC debug solver is currently not supported!");
#else
		std::shared_ptr<BmcSolverProxy> wrappedSolver = CreateBmcSolver(type);
		auto debugSolver = std::make_unique<BmcDebugProxy>();
		debugSolver->SetSolver(wrappedSolver);
		return debugSolver;
#endif
	}

	switch(type)
	{
		case BmcSolver::PROD_NCIP:
#ifdef HAS_BMC_SOLVER_NCIP
			return std::make_unique<NcipSolverProxy>();
#else
			Logging::Panic("NCIP (Next Craig Interpolant Prover) is currently not supported!");
#endif

		case BmcSolver::DEBUG_BMC:
#ifdef HAS_BMC_SOLVER_DEBUG
			return std::make_unique<BmcDebugProxy>();
#else
			Logging::Panic("BMC debug proxy is currently not supported!");
#endif

		case BmcSolver::EXPORT_CIP:
#ifdef HAS_BMC_SOLVER_EXPORT_CIP
			return std::make_unique<CipExportProxy>();
#else
			Logging::Panic("CIP (Craig Interpolant Prover) export format is currently not supported!");
#endif

		default:
			LOG(WARNING) << "Unsupported BMC-Solver selected";
			return std::unique_ptr<BmcSolverProxy>();
	}
}


std::string to_string(const BmcSolver& solver)
{
	switch (solver)
	{
		case BmcSolver::PROD_NCIP:
			return "PROD_NCIP";
		case BmcSolver::DEBUG_BMC:
			return "DEBUG_BMC";
		case BmcSolver::EXPORT_CIP:
			return "EXPORT_CIP";
		default:
			return "INVALID";
	}
}

std::ostream& operator<<(std::ostream& out, const BmcSolver& solver)
{
	out << to_string(solver);
	return out;
}

std::string to_string(const BmcResult& result)
{
	switch (result)
	{
		case BmcResult::Unreachable:
			return "Unreachable";
		case BmcResult::Reachable:
			return "Reachable";
		case BmcResult::Timeout:
			return "Timeout";
		case BmcResult::MaxIterationsReached:
			return "MaxIterationsReached";
		default:
			return "Invalid";
	}
}

std::ostream& operator<<(std::ostream& out, const BmcResult& result)
{
	out << to_string(result);
	return out;
}

// -----------------------------------------------------------------------------

BmcSolverProxy::BmcSolverProxy(BmcSolver solver):
	_solverType(solver),
	_solverTimeout(0.0),
	_solverDebug(false),
	_solverSilent(true),
	_numberOfInstances(0u),
	_numberOfSolves(0u),
	_numberOfVariables(0u),
	_numberOfClauses(0u),
	_maximumDepth(100u),
	_activeTimeframe(0u),
	_activeClauseType(ClauseType::Initial),
	_activeVariableType(VariableType::Auxiliary),
	_variableTypes(),
	_initialClauseDB(),
	_transitionClauseDB(),
	_targetClauseDB(),
	_activeClauseDB(&_initialClauseDB),
	_currentClause(),
	_lastResult(BmcResult::MaxIterationsReached),
	_lastDepth(0u),
	_solvesSinceReset(0u)
{
}

BmcSolverProxy::~BmcSolverProxy(void) = default;

BmcSolver BmcSolverProxy::GetSolverType(void) const
{
	return _solverType;
}

double BmcSolverProxy::GetSolverTimeout(void) const
{
	return _solverTimeout;
}

void BmcSolverProxy::SetSolverTimeout(double newTimeout)
{
	_solverTimeout = newTimeout;
}

void BmcSolverProxy::SetSolverDebug(bool enable)
{
	_solverDebug = enable;
}

bool BmcSolverProxy::GetSolverDebug(void) const
{
	return _solverDebug;
}

void BmcSolverProxy::SetSolverSilent(bool silent)
{
	_solverSilent = silent;
}

bool BmcSolverProxy::GetSolverSilent(void) const
{
	return _solverSilent;
}

size_t BmcSolverProxy::GetNumberOfInstances(void) const
{
	return _numberOfInstances;
}

size_t BmcSolverProxy::GetNumberOfSolves(void) const
{
	return _numberOfSolves;
}

size_t BmcSolverProxy::GetNumberOfVariables(void) const
{
	return _numberOfVariables;
}

size_t BmcSolverProxy::GetNumberOfClauses(void) const
{
	return _numberOfClauses;
}

BaseLiteral BmcSolverProxy::ConvertProxyToSolver(BaseLiteral lit) const
{
	return BaseLiteral::MakeFromUnsigned(lit.GetUnsigned() - 2u);
}

BaseLiteral BmcSolverProxy::ConvertSolverToProxy(BaseLiteral lit) const
{
	return BaseLiteral::MakeFromUnsigned(lit.GetUnsigned() + 2u);
}

// ----------------------------------------------------------------------------
// Clause API
// ----------------------------------------------------------------------------

BaseLiteral BmcSolverProxy::NewLiteral(void)
{
	_numberOfVariables++;
	_variableTypes.emplace_back(_activeVariableType);
	return BaseLiteral::MakeFromVariable(_numberOfVariables);
}

void BmcSolverProxy::NewClause(void)
{
	DASSERT(_currentClause.size() == 0) << "Opening new clause although existing one is not closed";
}

void BmcSolverProxy::AddLiteral(BaseLiteral literal)
{
	DASSERT(literal.IsSet()) << "Adding invalid unset literal";
	_currentClause.emplace_back(literal, _activeTimeframe);
}

void BmcSolverProxy::CommitClause(void)
{
	_activeClauseDB->push_back(_currentClause);
	_currentClause.clear();
	_numberOfClauses++;
}

// ----------------------------------------------------------------------------
// BMC API
// ----------------------------------------------------------------------------

void BmcSolverProxy::Reset(void)
{
	//_numberOfInstances = _numberOfInstances;
	_numberOfSolves = 0u;
	_numberOfVariables = 0u;
	_numberOfClauses = 0u;
	_activeTimeframe = 0u;
	_activeClauseType = ClauseType::Initial;
	_activeVariableType = VariableType::Auxiliary;
	_variableTypes = {};
	_initialClauseDB = {};
	_transitionClauseDB = {};
	_targetClauseDB = {};
	_activeClauseDB = &_initialClauseDB;

	_solvesSinceReset = 0;
	_currentClause = {};

	SetTargetVariableType(VariableType::Latch);
	NewLiteral(); // Constant 1
	SetTargetClauseType(ClauseType::Initial);
	CommitClause(POS_LIT);
	SetTargetClauseType(ClauseType::Transition);
	CommitClause(POS_LIT);
	SetTargetClauseType(ClauseType::Target);
	CommitClause(POS_LIT);
}

BmcResult BmcSolverProxy::Solve(void)
{
	if (_solvesSinceReset++ == 0u)
	{
		_numberOfInstances++;
	}
	_numberOfSolves++;
	return BmcResult::MaxIterationsReached;
}

BmcResult BmcSolverProxy::GetLastResult(void) const
{
	return _lastResult;
}

size_t BmcSolverProxy::GetLastDepth(void) const
{
	return _lastDepth;
}

void BmcSolverProxy::SetTargetClauseType(ClauseType clauseType)
{
	_activeClauseType = clauseType;
	switch (clauseType)
	{
		case ClauseType::Initial:
			_activeClauseDB = &_initialClauseDB;
			break;

		case ClauseType::Transition:
			_activeClauseDB = &_transitionClauseDB;
			break;

		case ClauseType::Target:
			_activeClauseDB = &_targetClauseDB;
			break;

		default:
			Logging::Panic("Invalid clause type");
			break;
	}
}

ClauseType BmcSolverProxy::GetTargetClauseType(void) const
{
	return _activeClauseType;
}

void BmcSolverProxy::SetTargetVariableType(VariableType variableType)
{
	_activeVariableType = variableType;
}

VariableType BmcSolverProxy::GetTargetVariableType(void) const
{
	return _activeVariableType;
}

void BmcSolverProxy::SetTargetTimeframe(size_t timeframe)
{
	_activeTimeframe = timeframe;
}

size_t BmcSolverProxy::GetTargetTimeframe(void) const
{
	return _activeTimeframe;
}

void BmcSolverProxy::SetMaximumDepth(size_t depth)
{
	_maximumDepth = depth;
}

size_t BmcSolverProxy::GetMaximumDepth(void) const
{
	return _maximumDepth;
}

void BmcSolverProxy::AddTimeframeLiteral(BaseLiteral lit1, size_t tf1)
{
	DASSERT(lit1.IsSet()) << "Invalid literal";
	DASSERT(tf1 == 0u || _variableTypes[lit1.GetVariable() - 1u] == VariableType::Latch) << "Only latches can have a timeframe different than 0";
	_currentClause.emplace_back(lit1, tf1);
}

void BmcSolverProxy::CommitTimeframeClause(BaseLiteral lit1, size_t tf1)
{
	AddTimeframeLiteral(lit1, tf1);
	CommitClause();
}

void BmcSolverProxy::CommitTimeframeClause(BaseLiteral lit1, size_t tf1, BaseLiteral lit2, size_t tf2)
{
	AddTimeframeLiteral(lit1, tf1);
	AddTimeframeLiteral(lit2, tf2);
	CommitClause();
}

void BmcSolverProxy::CommitTimeframeClause(BaseLiteral lit1, size_t tf1, BaseLiteral lit2, size_t tf2, BaseLiteral lit3, size_t tf3)
{
	AddTimeframeLiteral(lit1, tf1);
	AddTimeframeLiteral(lit2, tf2);
	AddTimeframeLiteral(lit3, tf3);
	CommitClause();
}

};
};
