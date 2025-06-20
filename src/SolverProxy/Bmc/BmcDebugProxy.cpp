#include "SolverProxy/Bmc/BmcDebugProxy.hpp"

#ifdef HAS_SAT_SOLVER_DEBUG

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"

using namespace std;
using namespace FreiTest::Basic;

namespace SolverProxy
{
namespace Bmc
{

static std::string ConvertClauseToString(const std::vector<std::pair<BaseLiteral, size_t>>& clause) {
	std::string result;
	for (auto const& [lit, timeframe] : clause)
	{
		if (!result.empty()) result += ", ";
		result += to_string(lit);
		result += ":";
		result += std::to_string(timeframe);
	}
	return "(" + result + ")";
}

BmcDebugProxy::BmcDebugProxy():
	BmcSolverProxy(BmcSolver::DEBUG_BMC),
	solver()
{
}

BmcDebugProxy::~BmcDebugProxy(void) = default;

void BmcDebugProxy::SetSolver(std::shared_ptr<BmcSolverProxy> solver)
{
	this->solver.swap(solver);
}

const std::shared_ptr<BmcSolverProxy>& BmcDebugProxy::GetSolver(void) const
{
	return solver;
}

void BmcDebugProxy::SetSolverTimeout(double timeout)
{
	DVLOG(6) << "SetSolverTimeout";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverTimeout(timeout);
}

double BmcDebugProxy::GetSolverTimeout(void) const
{
	DVLOG(6) << "GetSolverTimeout";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverTimeout();
}

void BmcDebugProxy::SetSolverDebug(bool enable)
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverDebug(enable);
}

bool BmcDebugProxy::GetSolverDebug(void) const
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverDebug();
}

void BmcDebugProxy::SetSolverSilent(bool silent)
{
	DVLOG(6) << "SetSolverDebug";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetSolverSilent(silent);
}

bool BmcDebugProxy::GetSolverSilent(void) const
{
	DVLOG(6) << "GetSolverSilent";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetSolverSilent();
}

// ----------------------------------------------------------------------------
// Clause API
// ----------------------------------------------------------------------------

void BmcDebugProxy::NewClause(void)
{
	DVLOG(6) << "NewClause";
	BmcSolverProxy::NewClause();

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->NewClause();
}

void BmcDebugProxy::CommitClause(void)
{
	DVLOG(3) << "CommitClause " << ConvertClauseToString(_currentClause);
	BmcSolverProxy::NewClause();

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->CommitClause();
}

BaseLiteral BmcDebugProxy::NewLiteral(void)
{
	DVLOG(6) << "NewLiteral";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->NewLiteral();
}

void BmcDebugProxy::AddLiteral(BaseLiteral lit)
{
	DVLOG(6) << "AddLiteral";
	BmcSolverProxy::AddLiteral(lit);

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->AddLiteral(lit);
}

Value BmcDebugProxy::GetLiteralValue(BaseLiteral lit) const
{
	DVLOG(6) << "GetLiteralValue";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetLiteralValue(lit);
}

// ----------------------------------------------------------------------------
// SAT API
// ----------------------------------------------------------------------------

void BmcDebugProxy::Reset(void)
{
	DVLOG(3) << "Reset";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->Reset();
}

BmcResult BmcDebugProxy::Solve(void)
{
	DVLOG(3) << "Solve";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->Solve();
}

BmcResult BmcDebugProxy::GetLastResult(void) const
{
	DVLOG(6) << "GetLastResult";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetLastResult();
}

size_t BmcDebugProxy::GetLastDepth(void) const
{
	DVLOG(6) << "GetLastDepth";
	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetLastDepth();
}

// ----------------------------------------------------------------------------
// BMC API
// ----------------------------------------------------------------------------

void BmcDebugProxy::AddTimeframeLiteral(BaseLiteral lit, size_t timeframe)
{
	DVLOG(6) << "AddTimeframeLiteral";
	BmcSolverProxy::AddTimeframeLiteral(lit, timeframe);

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->AddTimeframeLiteral(lit, timeframe);
}

void BmcDebugProxy::SetTargetClauseType(ClauseType clauseType)
{
	DVLOG(6) << "SetTargetClauseType";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetTargetClauseType(clauseType);
}

ClauseType BmcDebugProxy::GetTargetClauseType(void) const
{
	DVLOG(6) << "GetTargetClauseType";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetTargetClauseType();
}

void BmcDebugProxy::SetTargetVariableType(VariableType variableType)
{
	DVLOG(6) << "SetTargetVariableType";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetTargetVariableType(variableType);
}

VariableType BmcDebugProxy::GetTargetVariableType(void) const
{
	DVLOG(6) << "GetTargetVariableType";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetTargetVariableType();
}

void BmcDebugProxy::SetTargetTimeframe(size_t timeframe)
{
	DVLOG(6) << "SetTargetTimeframe";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetTargetTimeframe(timeframe);
}

size_t BmcDebugProxy::GetTargetTimeframe(void) const
{
	DVLOG(6) << "GetTargetTimeframe";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetTargetTimeframe();
}

void BmcDebugProxy::SetMaximumDepth(size_t depth)
{
	DVLOG(6) << "SetMaximumDepth";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->SetMaximumDepth(depth);
}

size_t BmcDebugProxy::GetMaximumDepth(void) const
{
	DVLOG(6) << "GetMaximumDepth";

	ASSERT(solver) << "No solver has been provided to call!";
	return solver->GetMaximumDepth();
}

};
};

#endif
