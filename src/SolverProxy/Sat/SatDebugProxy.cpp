#include "SolverProxy/Sat/SatDebugProxy.hpp"

#ifdef HAS_SAT_SOLVER_DEBUG

#include "Basic/Logging.hpp"

namespace SolverProxy
{
namespace Sat
{

std::string ConvertClauseToString(const std::vector<BaseLiteral>& clause) {
	std::string result;
	for (auto const& lit : clause)
	{
		if (!result.empty()) result += ", ";
		result += to_string(lit);
	}
	return "(" + result + ")";
}

SatDebugProxy::SatDebugProxy(SatSolver type):
	SatSolverProxy(type),
	MaxSatSolverProxy(type),
	CountSatSolverProxy(type),
	solver(nullptr)
{
}

SatDebugProxy::~SatDebugProxy(void) = default;

void SatDebugProxy::SetSolver(std::shared_ptr<SatSolverProxy> solver)
{
	this->solver.swap(solver);
}

const std::shared_ptr<SatSolverProxy>& SatDebugProxy::GetSolver(void) const
{
	return solver;
}

void SatDebugProxy::SetSolverTimeout(double timeout)
{
	DVLOG(6) << "SetSolverTimeout";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverTimeout(timeout);
}

double SatDebugProxy::GetSolverTimeout(void) const
{
	DVLOG(6) << "GetSolverTimeout";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverTimeout();
}

void SatDebugProxy::SetSolverDebug(bool enable)
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverDebug(enable);
}

bool SatDebugProxy::GetSolverDebug(void) const
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverDebug();
}

void SatDebugProxy::SetSolverSilent(bool silent)
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverSilent(silent);
}

bool SatDebugProxy::GetSolverSilent(void) const
{
	DVLOG(6) << "GetSolverSilent";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverSilent();
}

// ----------------------------------------------------------------------------
// Clause API
// ----------------------------------------------------------------------------

void SatDebugProxy::NewClause(void)
{
	DVLOG(6) << "NewClause";
	SatSolverProxy::NewClause();

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->NewClause();
}

void SatDebugProxy::CommitClause(void)
{
	DVLOG(3) << "CommitClause " << ConvertClauseToString(_currentClause);
	SatSolverProxy::CommitClause();

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->CommitClause();
}

BaseLiteral SatDebugProxy::NewLiteral(void)
{
	DVLOG(6) << "NewLiteral";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->NewLiteral();
}

void SatDebugProxy::AddLiteral(BaseLiteral lit)
{
	DVLOG(6) << "AddLiteral";
	SatSolverProxy::AddLiteral(lit);

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->AddLiteral(lit);
}

Value SatDebugProxy::GetLiteralValue(const BaseLiteral lit) const
{
	DVLOG(6) << "GetLiteralValue";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetLiteralValue(lit);
}

// ----------------------------------------------------------------------------
// SAT API
// ----------------------------------------------------------------------------

void SatDebugProxy::Reset(void)
{
	DVLOG(3) << "Reset";
	SatSolverProxy::Reset();

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->Reset();
}

SatResult SatDebugProxy::Solve(void)
{
	DVLOG(3) << "Solve";

	ASSERT(GetSolverType() == SatSolver::DEBUG_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return solver->Solve();
}

SatResult SatDebugProxy::GetLastResult(void) const
{
	DVLOG(3) << "GetLastResult";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetLastResult();
}

bool SatDebugProxy::IsIncrementalSupported(void) const
{
	DVLOG(6) << "IsIncrementalSupported";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->IsIncrementalSupported();
}

void SatDebugProxy::AddAssumption(BaseLiteral literal)
{
	DVLOG(3) << "AddAssumption " << to_string(literal);

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->AddAssumption(literal);
}

void SatDebugProxy::ClearAssumptions(void)
{
	DVLOG(3) << "ClearAssumptions";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->ClearAssumptions();
}

void SatDebugProxy::SetAutomaticLiteral(BaseLiteral literal)
{
	DVLOG(3) << "SetAutomaticLiteral " << to_string(literal);

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetAutomaticLiteral(literal);
}

void SatDebugProxy::ClearAutomaticLiteral(void)
{
	DVLOG(3) << "ClearAutomaticLiteral";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->ClearAutomaticLiteral();
}

// ----------------------------------------------------------------------------
// #SAT API
// ----------------------------------------------------------------------------

SatResult SatDebugProxy::CountSolve(void)
{
	DVLOG(3) << "CountSolve";

	ASSERT(GetSolverType() == SatSolver::DEBUG_COUNT_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return std::dynamic_pointer_cast<CountSatSolverProxy>(solver)->CountSolve();
}

size_t SatDebugProxy::GetLastModelCount(void) const
{
	DVLOG(6) << "GetLastModelCount";

	ASSERT(GetSolverType() == SatSolver::DEBUG_COUNT_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return std::dynamic_pointer_cast<CountSatSolverProxy>(solver)->GetLastModelCount();
}

// ----------------------------------------------------------------------------
// Max-SAT API
// ----------------------------------------------------------------------------

SatResult SatDebugProxy::MaxSolve(void)
{
	DVLOG(3) << "MaxSolve";

	ASSERT(GetSolverType() == SatSolver::DEBUG_MAX_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return std::dynamic_pointer_cast<MaxSatSolverProxy>(solver)->MaxSolve();
}

void SatDebugProxy::CommitSoftClause(size_t weight)
{
	DVLOG(3) << "CommitSoftClause " << ConvertClauseToString(_currentClause) << ", weight=" << std::to_string(weight);

	ASSERT(GetSolverType() == SatSolver::DEBUG_MAX_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return std::dynamic_pointer_cast<MaxSatSolverProxy>(solver)->CommitSoftClause(weight);
}

size_t SatDebugProxy::GetLastCost(void) const
{
	DVLOG(6) << "GetLastCost";

	ASSERT(GetSolverType() == SatSolver::DEBUG_MAX_SAT);
	ASSERT(solver) << "No solver has been provided to call!";
	return std::dynamic_pointer_cast<MaxSatSolverProxy>(solver)->GetLastCost();
}

};
};

#endif
