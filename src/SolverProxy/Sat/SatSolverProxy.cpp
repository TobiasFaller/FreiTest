#include "SolverProxy/Sat/SatSolverProxy.hpp"

#include <thread>

#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/MaxSat/IncBmoSolverProxy.hpp"
#include "SolverProxy/MaxSat/PacoseSolverProxy.hpp"
#include "SolverProxy/Sat/SatDebugProxy.hpp"
#include "SolverProxy/Sat/DimacsExportProxy.hpp"
#include "SolverProxy/Sat/CadicalSolverProxy.hpp"
#include "SolverProxy/Sat/Glucose421SingleSolverProxy.hpp"
#include "SolverProxy/Sat/Glucose421ParallelSolverProxy.hpp"
#include "SolverProxy/Sat/MinisatSolverProxy.hpp"
#include "SolverProxy/Sat/CryptoMinisatSolverProxy.hpp"
#include "SolverProxy/Sat/IpasirDynamicSolverProxy.hpp"
#include "SolverProxy/Sat/IpasirStaticSolverProxy.hpp"

#include "Basic/Logging.hpp"

using namespace std;

namespace SolverProxy
{
namespace Sat
{

std::unique_ptr<SatSolverProxy> SatSolverProxy::CreateSatSolver(SatSolver solver, DebugProxy debug)
{
	switch (solver)
	{
		case SatSolver::PROD_MAX_SAT_PACOSE:
		case SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE:
			return MaxSatSolverProxy::CreateMaxSatSolver(solver, debug);

		default:
			break;
	}

	if (debug == DebugProxy::ENABLED)
	{
#ifndef HAS_SAT_SOLVER_DEBUG
		Logging::Panic("SAT debug solver is currently not supported!");
#else
		std::shared_ptr<SatSolverProxy> wrappedSolver = CreateSatSolver(solver);
		auto debugSolver = std::make_unique<SatDebugProxy>(SatSolver::DEBUG_SAT);
		debugSolver->SetSolver(wrappedSolver);
		return debugSolver;
#endif
	}

	switch(solver)
	{

		case SatSolver::PROD_SAT_SINGLE_GLUCOSE_421:
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_SINGLE
			return std::make_unique<Glucose421SingleSolverProxy>();
#else
			Logging::Panic("Glucose 4.2.1 (Single-Core) is currently not supported!");
#endif

		case SatSolver::PROD_SAT_PARALLEL_GLUCOSE_421:
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL
			return std::make_unique<Glucose421ParallelSolverProxy>();
#else
			Logging::Panic("Glucose 4.2.1 (Parallel) is currently not supported!");
#endif

		case SatSolver::PROD_SAT_CADICAL:
#ifdef HAS_SAT_SOLVER_CADICAL
			return std::make_unique<CadicalSolverProxy>();
#else
			Logging::Panic("CaDiCaL is currently not supported!");
#endif

		case SatSolver::PROD_SAT_MINISAT:
#ifdef HAS_SAT_SOLVER_MINISAT
			return std::make_unique<MinisatSolverProxy>();
#else
			Logging::Panic("MiniSAT is currently not supported!");
#endif

		case SatSolver::PROD_SAT_CRYPTO_MINISAT:
#ifdef HAS_SAT_SOLVER_CRYPTO_MINISAT
			return std::make_unique<CryptoMinisatSolverProxy>();
#else
			Logging::Panic("Crypto MiniSAT is currently not supported!");
#endif

		case SatSolver::PROD_SAT_IPASIR_DYNAMIC:
#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC
			return std::make_unique<IpasirDynamicSolverProxy>();
#else
			Logging::Panic("Ipasir (Dynamic) is currently not supported!");
#endif

		case SatSolver::PROD_SAT_IPASIR_STATIC:
#ifdef HAS_SAT_SOLVER_IPASIR_STATIC
			return std::make_unique<IpasirStaticSolverProxy>();
#else
			Logging::Panic("Ipasir (Static) is currently not supported!");
#endif

		case SatSolver::DEBUG_SAT:
#ifdef HAS_SAT_SOLVER_DEBUG
			return std::make_unique<SatDebugProxy>(SatSolver::DEBUG_SAT);
#else
			Logging::Panic("SAT debug solver is currently not supported!");
#endif

		case SatSolver::EXPORT_SAT_DIMACS:
#ifdef HAS_SAT_SOLVER_DIMACS
		{
			auto proxy = std::make_unique<DimacsExportProxy>();
			proxy->SetFormat(DimacsExportProxy::Format::Dimacs);
			return proxy;
		}
#else
			Logging::Panic("DIMACS (CNF) export format is currently not supported!");
#endif

		default:
			LOG(FATAL) << "Unknown Solver type: " << solver;
			return std::unique_ptr<SatSolverProxy>();
	}
}

std::string to_string(const SatSolver& solver)
{
	switch (solver)
	{
		case SatSolver::PROD_SAT_SINGLE_GLUCOSE_421:
			return "PROD_SAT_SINGLE_GLUCOSE_421";
		case SatSolver::PROD_SAT_PARALLEL_GLUCOSE_421:
			return "PROD_SAT_PARALLEL_GLUCOSE_421";
		case SatSolver::PROD_SAT_CADICAL:
			return "PROD_SAT_CADICAL";
		case SatSolver::PROD_SAT_MINISAT:
			return "PROD_SAT_MINISAT";
		case SatSolver::PROD_SAT_CRYPTO_MINISAT:
			return "PROD_SAT_CRYPTO_MINISAT";
		case SatSolver::PROD_SAT_IPASIR_DYNAMIC:
			return "PROD_SAT_IPASIR_DYNAMIC";
		case SatSolver::PROD_SAT_IPASIR_STATIC:
			return "PROD_SAT_IPASIR_STATIC";
		case SatSolver::PROD_MAX_SAT_PACOSE:
			return "PROD_MAX_SAT_PACOSE";
		case SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE:
			return "PROD_MAX_SAT_INC_BMO_COMPLETE";
		case SatSolver::EXPORT_SAT_DIMACS:
			return "EXPORT_SAT_DIMACS";
		case SatSolver::EXPORT_MAX_SAT_DIMACS:
			return "EXPORT_MAX_SAT_DIMACS";
		default:
			return "INVALID";
	}
}

std::ostream& operator<<(std::ostream& out, const SatSolver& solver)
{
	out << to_string(solver);
	return out;
}

std::string to_string(const SatResult& result)
{
	switch (result)
	{
		case SatResult::UNKNOWN:
			return "UNKNOWN";
		case SatResult::SAT:
			return "SAT";
		case SatResult::UNSAT:
			return "UNSAT";
		default:
			return "INVALID";
	}
}

std::ostream& operator<<(std::ostream& out, const SatResult& result)
{
	out << to_string(result);
	return out;
}

// ----------------------------------------------------------------------------

SatSolverProxy::SatSolverProxy(SatSolver solver):
	_solverType(solver),
	_solverTimeout(0.0),
	_solverDebug(false),
	_solverSilent(true),
	_numberOfInstances(0u),
	_numberOfSolves(0u),
	_numberOfVariables(0u),
	_numberOfClauses(0u),
	_numberOfConclicts(0u),
	_numberOfPropagations(0u),
	_numberOfDecisions(0u),
	_automaticLiteral(UNDEFINED_LIT),
	_currentClause(),
	_assumptions(),
	_lastResult(SatResult::UNKNOWN),
	_solvesSinceReset(0u)
{
}

SatSolverProxy::~SatSolverProxy(void) = default;

SatSolver SatSolverProxy::GetSolverType(void) const
{
	return _solverType;
}

double SatSolverProxy::GetSolverTimeout(void) const
{
	return _solverTimeout;
}

void SatSolverProxy::SetSolverTimeout(double newTimeout)
{
	_solverTimeout = newTimeout;
}

void SatSolverProxy::SetSolverDebug(bool enable)
{
	_solverDebug = enable;
}

bool SatSolverProxy::GetSolverDebug(void) const
{
	return _solverDebug;
}

void SatSolverProxy::SetSolverSilent(bool silent)
{
	_solverSilent = silent;
}

bool SatSolverProxy::GetSolverSilent(void) const
{
	return _solverSilent;
}

size_t SatSolverProxy::GetNumberOfInstances(void) const
{
	return _numberOfInstances;
}

size_t SatSolverProxy::GetNumberOfSolves(void) const
{
	return _numberOfSolves;
}

size_t SatSolverProxy::GetNumberOfVariables(void) const
{
	return _numberOfVariables;
}

size_t SatSolverProxy::GetNumberOfClauses(void) const
{
	return _numberOfClauses;
}

size_t SatSolverProxy::GetNumberOfConflicts(void) const
{
	return _numberOfConclicts;
}

size_t SatSolverProxy::GetNumberOfPropagations(void) const
{
	return _numberOfPropagations;
}

BaseLiteral SatSolverProxy::ConvertProxyToSolver(BaseLiteral lit) const
{
	return BaseLiteral::MakeFromUnsigned(lit.GetUnsigned() - 2u);
}

BaseLiteral SatSolverProxy::ConvertSolverToProxy(BaseLiteral lit) const
{
	return BaseLiteral::MakeFromUnsigned(lit.GetUnsigned() + 2u);
}

// ----------------------------------------------------------------------------
// Clause API
// ----------------------------------------------------------------------------

BaseLiteral SatSolverProxy::NewLiteral(void)
{
	_numberOfVariables++; // Skip invalid literal
	return BaseLiteral::MakeFromVariable(_numberOfVariables);
}

void SatSolverProxy::NewClause(void)
{
	DASSERT(_currentClause.size() == 0) << "Opening new clause although existing one is not closed";
	if (_automaticLiteral != UNDEFINED_LIT)
	{
		AddLiteral(_automaticLiteral);
	}
}

void SatSolverProxy::AddLiteral(const BaseLiteral literal)
{
	DASSERT(literal.IsSet()) << "Adding invalid unset literal";
	_currentClause.push_back(literal);
}

void SatSolverProxy::CommitClause(void)
{
	_currentClause.clear();
	_numberOfClauses++;
}

// ----------------------------------------------------------------------------
// SAT API
// ----------------------------------------------------------------------------

void SatSolverProxy::Reset(void)
{
	//_numberOfInstances = _numberOfInstances;
	_numberOfSolves = 0u;
	_numberOfVariables = 0u;
	_numberOfClauses = 0u;
	_numberOfConclicts = 0u;
	_numberOfPropagations = 0u;
	_numberOfDecisions = 0u;
	_automaticLiteral = UNDEFINED_LIT;

	_solvesSinceReset = 0;
	_currentClause = {};
	_assumptions = {};

	NewLiteral(); // Constant 1
	CommitClause(POS_LIT);
}

SatResult SatSolverProxy::Solve(void)
{
	if (_solvesSinceReset++ == 0u)
	{
		_numberOfInstances++;
	}
	_numberOfSolves++;
	return SatResult::UNKNOWN;
}

SatResult SatSolverProxy::GetLastResult(void) const
{
	return _lastResult;
}

void SatSolverProxy::AddAssumption(BaseLiteral literal)
{
	DASSERT(literal.IsSet()) << "Adding invalid unset literal";
	_assumptions.push_back(literal);
}

void SatSolverProxy::ClearAssumptions(void)
{
	_assumptions.clear();
}

void SatSolverProxy::SetAutomaticLiteral(BaseLiteral literal)
{
	DASSERT(literal.IsSet()) << "Setting invalid unset literal";
	_automaticLiteral = literal;
}

void SatSolverProxy::ClearAutomaticLiteral(void)
{
	_automaticLiteral = UNDEFINED_LIT;
}

};
};

	