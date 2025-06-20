#include "SolverProxy/Bmc/NcipSolverProxy.hpp"

#ifdef HAS_BMC_SOLVER_NCIP

#include <ncip/bmc-format-cip.hpp>
#include <ncip/bmc-ncip.hpp>

#include "Basic/Logging.hpp"
#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Bmc
{

NcipSolverProxy::NcipSolverProxy():
	BmcSolverProxy(BmcSolver::PROD_NCIP),
	_solver(),
	_builder(),
	_result()
{
	Reset();
}

NcipSolverProxy::~NcipSolverProxy(void) = default;

void NcipSolverProxy::Reset(void)
{
	_builder = std::make_unique<Ncip::CipProblemBuilder>();
	_solver = nullptr;
	_result = nullptr;
	BmcSolverProxy::Reset();
}

BmcResult NcipSolverProxy::Solve(void)
{
	Ncip::BmcConfiguration config;
	config.SetLogLevel(Ncip::LogLevel::None);
	config.SetCraigInterpolant(Ncip::CraigInterpolant::Asymmetric);
	config.SetEnableCraigInterpolation(true);
	config.SetEnableFixPointCheck(true);
	config.SetMaximumDepth(_maximumDepth);
	config.SetLogLevel(Ncip::LogLevel::None);
	if (_solverDebug)
	{
		config.SetLogLevel(Ncip::LogLevel::Debug);
	}
	else if (!_solverSilent)
	{
		config.SetLogLevel(Ncip::LogLevel::Info);
	}

	auto [cipProblem, bmcProblem] { _builder->Build() };
	_solver = std::make_unique<decltype(_solver)::element_type>(bmcProblem, config);

	CallbackTimeoutHelper timeoutHelper([solver = _solver.get()]() { solver->Interrupt(); });
	timeoutHelper.StartTimeout(static_cast<uint64_t>(_solverTimeout * 1000.0));
	_result = std::make_unique<Ncip::BmcResult>(_solver->Solve());
	timeoutHelper.StopTimeout();

	switch (_result->GetStatus())
	{
		case Ncip::BmcStatus::Sat:
			_lastResult = BmcResult::Reachable;
			break;
		case Ncip::BmcStatus::Unsat:
			_lastResult = BmcResult::Unreachable;
			break;
		case Ncip::BmcStatus::Interrupted:
			_lastResult = BmcResult::Timeout;
			break;
		case Ncip::BmcStatus::DepthLimitReached:
			_lastResult = BmcResult::MaxIterationsReached;
			break;
		default:
			__builtin_unreachable();
	}
	_lastDepth = _result->GetDepth();
	return _lastResult;
}

BaseLiteral NcipSolverProxy::NewLiteral(void)
{
	auto const get_type = [&](const VariableType& type) {
		switch (type)
		{
			case VariableType::Input: return Ncip::CipVariableType::Input;
			case VariableType::Output: return Ncip::CipVariableType::Output;
			case VariableType::Latch: return Ncip::CipVariableType::Latch;
			case VariableType::Auxiliary: return Ncip::CipVariableType::Tseitin;
			default: __builtin_unreachable();
		}
	};
	_builder->AddVariable(get_type(_activeVariableType));
	return BmcSolverProxy::NewLiteral();
}

void NcipSolverProxy::CommitClause(void)
{
	auto const get_type = [&](const ClauseType& type) {
		switch (type)
		{
			case ClauseType::Initial: return Ncip::CipClauseType::Initial;
			case ClauseType::Transition: return Ncip::CipClauseType::Transition;
			case ClauseType::Target: return Ncip::CipClauseType::Target;
			default: __builtin_unreachable();
		}
	};

	Ncip::BmcClause clause;
	for (auto const& [literal, timeframe] : _currentClause)
	{
		clause.push_back(Ncip::BmcLiteral::FromLiteral(ConvertProxyToSolver(literal).GetUnsigned(), timeframe));
	}
	_builder->AddClause(get_type(_activeClauseType), clause);
	BmcSolverProxy::CommitClause();
}

Value NcipSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto const literal { Ncip::BmcLiteral::FromLiteral(ConvertProxyToSolver(lit).GetUnsigned(), _activeTimeframe) };
	auto const assignment { _result->GetModel().GetAssignment(literal) };
	switch (assignment) {
		case Ncip::BmcAssignment::DontCare: return Value::DontCare;
		case Ncip::BmcAssignment::Positive: return Value::Positive;
		case Ncip::BmcAssignment::Negative: return Value::Negative;
		default: __builtin_unreachable();
	}
}

};
};

#endif
