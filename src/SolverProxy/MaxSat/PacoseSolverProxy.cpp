#include "SolverProxy/MaxSat/PacoseSolverProxy.hpp"

#ifdef HAS_MAX_SAT_SOLVER_PACOSE

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <pacose/maxSAT/Pacose.h>
#include <pacose/Helper/ClauseDB.h>
#include <pacose/solver-proxy/SATSolverProxy.h>
#pragma GCC diagnostic pop

#include "Basic/Logging.hpp"
#include "SolverProxy/TimeoutHelper.hpp"

namespace SolverProxy
{
namespace Sat
{

PacoseSolverProxy::PacoseSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_MAX_SAT_PACOSE),
	MaxSatSolverProxy(SatSolver::PROD_MAX_SAT_PACOSE),
	_solver(),
	_clauses()
{
	Reset();
}

PacoseSolverProxy::~PacoseSolverProxy(void) = default;

void PacoseSolverProxy::Reset(void)
{
	_solver = std::make_unique<Pacose::Pacose>();
	_clauses = std::make_unique<Pacose::ClauseDB>();
	_solver->InitSatSolver(SATSolverType::CADICAL);
	MaxSatSolverProxy::Reset();
}

bool PacoseSolverProxy::IsIncrementalSupported(void) const
{
	return false;
}

PacoseEncoding PacoseSolverProxy::GetEncoding(void) const
{
	switch (_solver->_settings.GetEncoding())
	{
		case Pacose::EncodingType::WARNERS:
			return PacoseEncoding::Warners;
		case Pacose::EncodingType::BAILLEUX:
			return PacoseEncoding::Bailleux;
		case Pacose::EncodingType::ASIN:
			return PacoseEncoding::Asin;
		case Pacose::EncodingType::OGAWA:
			return PacoseEncoding::Ogawa;
		case Pacose::EncodingType::BAILLEUXW2:
			return PacoseEncoding::BailleuxW2;
		case Pacose::EncodingType::WMTO:
			return PacoseEncoding::WeightedTotalizer;
		case Pacose::EncodingType::MRWTO:
			return PacoseEncoding::MixedRadixWeightedTotalizer;
		case Pacose::EncodingType::MRWTO2:
			return PacoseEncoding::MixedRadixWeightedTotalizer2;
		case Pacose::EncodingType::MRWTO19:
			return PacoseEncoding::MixedRadixWeightedTotalizer19;
		case Pacose::EncodingType::MRWTO19_2:
			return PacoseEncoding::MixedRadixWeightedTotalizer19_2;
		case Pacose::EncodingType::DGPW18:
			return PacoseEncoding::DynamicGlobalPolynomialWatchdog;
		case Pacose::EncodingType::HEURISTICQMAXSAT17:
			return PacoseEncoding::HeuristicQMaxSat17;
		case Pacose::EncodingType::HEURISTIC20:
			return PacoseEncoding::Heuristic20;
		case Pacose::EncodingType::HEURISTIC1819:
			return PacoseEncoding::Heuristic1819;
		case Pacose::EncodingType::QMAXSAT19:
			return PacoseEncoding::QMaxSat19;
		default:
			Logging::Panic("Pacose has invalid encoding!");
	}
}

void PacoseSolverProxy::SetEncoding(PacoseEncoding encoding)
{
	switch(encoding)
	{
		case PacoseEncoding::Warners:
			_solver->_settings.SetEncoding(Pacose::EncodingType::WARNERS);
			break;
		case PacoseEncoding::Bailleux:
			_solver->_settings.SetEncoding(Pacose::EncodingType::BAILLEUX);
			break;
		case PacoseEncoding::Asin:
			_solver->_settings.SetEncoding(Pacose::EncodingType::ASIN);
			break;
		case PacoseEncoding::Ogawa:
			_solver->_settings.SetEncoding(Pacose::EncodingType::OGAWA);
			break;
		case PacoseEncoding::BailleuxW2:
			_solver->_settings.SetEncoding(Pacose::EncodingType::BAILLEUXW2);
			break;
		case PacoseEncoding::WeightedTotalizer:
			_solver->_settings.SetEncoding(Pacose::EncodingType::WMTO);
			break;
		case PacoseEncoding::MixedRadixWeightedTotalizer:
			_solver->_settings.SetEncoding(Pacose::EncodingType::MRWTO);
			break;
		case PacoseEncoding::MixedRadixWeightedTotalizer2:
			_solver->_settings.SetEncoding(Pacose::EncodingType::MRWTO2);
			break;
		case PacoseEncoding::MixedRadixWeightedTotalizer19:
			_solver->_settings.SetEncoding(Pacose::EncodingType::MRWTO19);
			break;
		case PacoseEncoding::MixedRadixWeightedTotalizer19_2:
			_solver->_settings.SetEncoding(Pacose::EncodingType::MRWTO19_2);
			break;
		case PacoseEncoding::DynamicGlobalPolynomialWatchdog:
			_solver->_settings.SetEncoding(Pacose::EncodingType::DGPW18);
			break;
		case PacoseEncoding::HeuristicQMaxSat17:
			_solver->_settings.SetEncoding(Pacose::EncodingType::HEURISTICQMAXSAT17);
			break;
		case PacoseEncoding::Heuristic20:
			_solver->_settings.SetEncoding(Pacose::EncodingType::HEURISTIC20);
			break;
		case PacoseEncoding::Heuristic1819:
			_solver->_settings.SetEncoding(Pacose::EncodingType::HEURISTIC1819);
			break;
		case PacoseEncoding::QMaxSat19:
			_solver->_settings.SetEncoding(Pacose::EncodingType::QMAXSAT19);
			break;
		default :
			Logging::Panic("This encoding type is not supported");
			break;
	}
}

void PacoseSolverProxy::CommitClause(void)
{
	std::vector<int> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push_back(ConvertProxyToSolver(literal).GetSigned());
	}

	_clauses->nbHardClauses++;
	_clauses->clauses.push_back(clause);
	_clauses->weights.push_back(UINT64_MAX);
	MaxSatSolverProxy::CommitClause();
}

void PacoseSolverProxy::CommitSoftClause(size_t weight)
{
	std::vector<int> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push_back(ConvertProxyToSolver(literal).GetSigned());
	}

	_clauses->maxWeight = std::max(_clauses->maxWeight, weight);
	_clauses->sumOfSoftWeights += weight;
	_clauses->nbSoftClauses++;
	_clauses->nbUnitSoftClauses += (clause.size() == 1) ? 1 : 0;
	_clauses->clauses.push_back(clause);
	_clauses->weights.push_back(weight);
	MaxSatSolverProxy::CommitClause();
}

SatResult PacoseSolverProxy::MaxSolve(void)
{
	SatSolverProxy::Solve();

	_solver->_settings.verbosity = 0;
	if (_solverDebug)
	{
		_solver->_settings.verbosity = 2;
	}
	else if (!_solverSilent)
	{
		_solver->_settings.verbosity = 1;
	}

	_clauses->nbVars = _numberOfVariables;
	auto top { _clauses->sumOfSoftWeights + 1u };
	auto index { 0u };
	for (auto& weight : _clauses->weights)
	{
		if (weight == UINT64_MAX)
		{
			weight = top;
			for (auto& literal : _clauses->clauses[index])
			{
				_clauses->nbVarsInHard = std::max(_clauses->nbVarsInHard, std::abs(literal));
			}
		}
		index++;
	}

	auto result = _solver->SolveProcedure(*_clauses);
	if (result == 10)
	{
		_lastResult = SatResult::SAT;
		_lastCost = _solver->GetOValue();
	}
	else if (result == 20)
	{
		_lastResult = SatResult::UNSAT;
		_lastCost = 0;
	}
	else
	{
		_lastResult = SatResult::UNKNOWN;
		_lastCost = 0;
	}
	return _lastResult;
}

Value PacoseSolverProxy::GetLiteralValue(const BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto value { _solver->GetModel(ConvertProxyToSolver(lit).GetVariable() + 1u) ^ lit.IsNegated() };
	if (value != 0)
	{
		return !(value & 1)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

std::string to_string(const PacoseEncoding& encoding)
{
	switch (encoding)
	{
	case PacoseEncoding::Warners:
		return "Adder, Warners 1996";
	case PacoseEncoding::Bailleux:
		return "Totalizer";
	case PacoseEncoding::Asin:
		return "Cardinality Networks, Asin";
	case PacoseEncoding::Ogawa:
		return "Modulo Totalizer";
	case PacoseEncoding::BailleuxW2:
		return "BailleuxW2";
	case PacoseEncoding::WeightedTotalizer:
		return "Weighted MaxSAT Totalizer";
	case PacoseEncoding::MixedRadixWeightedTotalizer:
		return "Mixed Radix Weighted Totalizer";
	case PacoseEncoding::MixedRadixWeightedTotalizer2:
		return "Mixed Radix Weighted Totalizer 2";
	case PacoseEncoding::MixedRadixWeightedTotalizer19:
		return "Mixed Radix Weighted Totalizer '19";
	case PacoseEncoding::MixedRadixWeightedTotalizer19_2:
		return "Mixed Radix Weighted Totalizer '19 2";
	case PacoseEncoding::DynamicGlobalPolynomialWatchdog:
		return "Dynamic Global Polynomial Watchdog";
	case PacoseEncoding::HeuristicQMaxSat17:
		return "Heuristic '17 Competition";
	case PacoseEncoding::Heuristic20:
		return "Heuristic '20 Competition";
	case PacoseEncoding::Heuristic1819:
		return "Heuristic '18 Competition";
	case PacoseEncoding::QMaxSat19:
		return "Heuristic '19 Competition";
	default:
		Logging::Panic("Invalid encoding");
	}
}

std::ostream& operator<<(std::ostream& out, const PacoseEncoding& encoding)
{
	out << to_string(encoding);
	return out;
}

}
}

#endif

