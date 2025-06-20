#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"

#include "SolverProxy/Sat/SatDebugProxy.hpp"
#include "SolverProxy/Sat/DimacsExportProxy.hpp"
#include "SolverProxy/MaxSat/IncBmoSolverProxy.hpp"
#include "SolverProxy/MaxSat/PacoseSolverProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

std::unique_ptr<MaxSatSolverProxy> MaxSatSolverProxy::CreateMaxSatSolver(SatSolver solver, DebugProxy debug)
{
	if (debug == DebugProxy::ENABLED)
	{
#ifndef HAS_SAT_SOLVER_DEBUG
		Logging::Panic("Max-SAT debug solver is currently not supported!");
#else
		std::shared_ptr<MaxSatSolverProxy> wrappedSolver = CreateMaxSatSolver(solver);
		auto debugSolver = std::make_unique<SatDebugProxy>(SatSolver::DEBUG_MAX_SAT);
		debugSolver->SetSolver(wrappedSolver);
		return debugSolver;
#endif
	}

	switch(solver)
	{
		case SatSolver::PROD_MAX_SAT_PACOSE:
#ifdef HAS_MAX_SAT_SOLVER_PACOSE
			return std::make_unique<PacoseSolverProxy>();
#else
			Logging::Panic("Pacose is currently not supported!");
#endif

		case SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE:
#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE
			return std::make_unique<IncBmoSolverProxy>();
#else
			Logging::Panic("Incremental OpenWBO (Complete) is currently not supported!");
#endif

		case SatSolver::EXPORT_MAX_SAT_DIMACS:
#ifdef HAS_SAT_SOLVER_DIMACS
		{
			auto proxy = std::make_unique<DimacsExportProxy>();
			proxy->SetFormat(DimacsExportProxy::Format::WDimacsClassic);
			return proxy;
		}
#else
			Logging::Panic("WDIMACS (WCNF) export format is currently not supported!");
#endif

		case SatSolver::DEBUG_MAX_SAT:
#ifdef HAS_SAT_SOLVER_DEBUG
			return std::make_unique<SatDebugProxy>(SatSolver::DEBUG_MAX_SAT);
#else
			Logging::Panic("Max-SAT debug solver is currently not supported!");
#endif

		default:
			LOG(FATAL) << "Unknown Solver type: " << solver;
			return std::unique_ptr<MaxSatSolverProxy>();
	}
}

// ----------------------------------------------------------------------------

MaxSatSolverProxy::MaxSatSolverProxy(SatSolver solver):
	SatSolverProxy(solver),
	_lastCost(0u)
{
}

MaxSatSolverProxy::~MaxSatSolverProxy() = default;

SatResult MaxSatSolverProxy::Solve(void)
{
	Logging::Panic("Not implemented! You have to use MaxSolve() instead!");
}

size_t MaxSatSolverProxy::GetLastCost(void) const
{
	return _lastCost;
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, BaseLiteral l2, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	AddLiteral(l5);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(BaseLiteral l1, BaseLiteral l2, BaseLiteral l3, BaseLiteral l4, BaseLiteral l5, BaseLiteral l6, size_t weight)
{
	NewClause();
	AddLiteral(l1);
	AddLiteral(l2);
	AddLiteral(l3);
	AddLiteral(l4);
	AddLiteral(l5);
	AddLiteral(l6);
	CommitSoftClause(weight);
}

void MaxSatSolverProxy::CommitSoftClause(const std::vector< BaseLiteral>& literals, size_t weight)
{
	NewClause();
	for ( BaseLiteral lit : literals )
	{
		AddLiteral(lit);
	}
	CommitSoftClause(weight);
}

}
}
