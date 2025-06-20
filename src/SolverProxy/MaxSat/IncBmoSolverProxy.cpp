#include "SolverProxy/MaxSat/IncBmoSolverProxy.hpp"

#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wmismatched-tags"
#include <inc-bmo-complete/MaxSAT.h>
#include <inc-bmo-complete/MaxTypes.h>
#include <inc-bmo-complete/algorithms/Alg_LinearSU.h>
#include <inc-bmo-complete/algorithms/Alg_LinearSU_IncBMO.h>
#include <inc-bmo-complete/algorithms/Alg_LinearSU_IncCluster.h>
#include <inc-bmo-complete/algorithms/Alg_MSU3.h>
#include <inc-bmo-complete/algorithms/Alg_OLL.h>
#include <inc-bmo-complete/algorithms/Alg_OLL_IncCluster.h>
#include <inc-bmo-complete/algorithms/Alg_PartMSU3.h>
#include <inc-bmo-complete/algorithms/Alg_WBO.h>
#include <inc-bmo-complete/algorithms/Alg_OBV.h>
#include <inc-bmo-complete/algorithms/Alg_BLS.h>
#pragma GCC diagnostic pop

#include "Basic/Logging.hpp"

namespace SolverProxy
{
namespace Sat
{

IncBmoSolverProxy::IncBmoSolverProxy(void):
	SatSolverProxy(SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE),
	MaxSatSolverProxy(SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE),
	_algorithm(Algorithm::ALGORITHM_WBO),
	_incrementalMode(IncrementalMode::INCREMENTAL_ITERATIVE),
	_verbosity(Verbosity::VERBOSITY_MINIMAL),
	_clusterAlgorithm(ClusterAlgorithm::DIVISIVE_MAX_SEPARATE),
	_clusterStrategy(ClusterStrategy::DIVISIVE),
	_numClusters(100000u),
	_bmoSearch(BmoSearch::ENABLED),
	_weightMode(WeightMode::WEIGHTED),
	_weightStrategy(WeightStrategy::WEIGHT_NORMAL),
	_symmetryBreaking(SymmetryBreaking::SYMMETRIC_BREAKING),
	_symmetryLimit(500000u),
	_cardinalityEncoding(CardinalityEncoding::TOTALIZER),
	_pbEncoding(PbEncoding::GTE),
	_roundingStatistics(RoundingStatistics::MEAN),
	_numIterations(100000u),
	_numConflicts(10000u),
	_completeMode(CompleteMode::COMPLETE),
	_localConflictLimit(false),
	_partitionStrategy(PartitionStrategy::BINARY),
	_graphType(GraphType::RES),
	_maxFormula()
{
	Reset();
}

IncBmoSolverProxy::~IncBmoSolverProxy(void) = default;

void IncBmoSolverProxy::Reset(void)
{
	_solver = nullptr;
	_maxFormula = std::make_unique<openwbo::MaxSATFormula>();
	_maxFormula->setFormat(_FORMAT_MAXSAT_);
	_maxFormula->setProblemType(_WEIGHTED_);
	MaxSatSolverProxy::Reset();
}

bool IncBmoSolverProxy::IsIncrementalSupported(void) const
{
	return false;
}

void IncBmoSolverProxy::SetAlgorithm(Algorithm algorithm)
{
	_algorithm = algorithm;
}

IncBmoSolverProxy::Algorithm IncBmoSolverProxy::GetAlgorithm(void) const
{
	return _algorithm;
}

void IncBmoSolverProxy::SetIncrementalMode(IncrementalMode incrementalMode)
{
	_incrementalMode = incrementalMode;
}

IncBmoSolverProxy::IncrementalMode IncBmoSolverProxy::GetIncrementalMode(void) const
{
	return _incrementalMode;
}

void IncBmoSolverProxy::SetVerbosity(Verbosity verbosity)
{
	_verbosity = verbosity;
}

IncBmoSolverProxy::Verbosity IncBmoSolverProxy::GetVerbosity(void) const
{
	return _verbosity;
}

void IncBmoSolverProxy::SetClusterAlgorithm(ClusterAlgorithm clusterAlgorithm)
{
	_clusterAlgorithm = clusterAlgorithm;
}

IncBmoSolverProxy::ClusterAlgorithm IncBmoSolverProxy::GetClusterAlgorithm(void) const
{
	return _clusterAlgorithm;
}

void IncBmoSolverProxy::SetClusterStrategy(ClusterStrategy clusterStrategy)
{
	_clusterStrategy = clusterStrategy;
}

IncBmoSolverProxy::ClusterStrategy IncBmoSolverProxy::GetClusterStrategy(void) const
{
	return _clusterStrategy;
}

void IncBmoSolverProxy::SetNumClusters(size_t numClusters)
{
	_numClusters = numClusters;
}

size_t IncBmoSolverProxy::GetNumClusters(void) const
{
	return _numClusters;
}

void IncBmoSolverProxy::SetBmoSearch(BmoSearch bmoSearch)
{
	_bmoSearch = bmoSearch;
}

IncBmoSolverProxy::BmoSearch IncBmoSolverProxy::GetBmoSearch(void) const
{
	return _bmoSearch;
}

void IncBmoSolverProxy::SetWeightMode(WeightMode weightMode)
{
	_weightMode = weightMode;
}

IncBmoSolverProxy::WeightMode IncBmoSolverProxy::GetWeightMode(void) const
{
	return _weightMode;
}

void IncBmoSolverProxy::SetWeightStrategy(WeightStrategy weightStrategy)
{
	_weightStrategy = weightStrategy;
}

IncBmoSolverProxy::WeightStrategy IncBmoSolverProxy::GetWeightStrategy(void) const
{
	return _weightStrategy;
}

void IncBmoSolverProxy::SetSymmetryBreaking(SymmetryBreaking symmetryBreaking)
{
	_symmetryBreaking = symmetryBreaking;
}

IncBmoSolverProxy::SymmetryBreaking IncBmoSolverProxy::GetSymmetryBreaking(void) const
{
	return _symmetryBreaking;
}

void IncBmoSolverProxy::SetSymmetryLimit(size_t symmetryLimit)
{
	_symmetryLimit = symmetryLimit;
}

size_t IncBmoSolverProxy::GetSymmetryLimit(void) const
{
	return _symmetryLimit;
}

void IncBmoSolverProxy::SetCardinalityEncoding(CardinalityEncoding cardinalityEncoding)
{
	_cardinalityEncoding = cardinalityEncoding;
}

IncBmoSolverProxy::CardinalityEncoding IncBmoSolverProxy::GetCardinalityEncoding(void) const
{
	return _cardinalityEncoding;
}

void IncBmoSolverProxy::SetPbEncoding(PbEncoding pbEncoding)
{
	_pbEncoding = pbEncoding;
}

IncBmoSolverProxy::PbEncoding IncBmoSolverProxy::GetPbEncoding(void) const
{
	return _pbEncoding;
}

void IncBmoSolverProxy::SetRoundingStatistics(RoundingStatistics roundingStatistics)
{
	_roundingStatistics = roundingStatistics;
}

IncBmoSolverProxy::RoundingStatistics IncBmoSolverProxy::GetRoundingStatistics(void) const
{
	return _roundingStatistics;
}

void IncBmoSolverProxy::SetNumIterations(size_t numIterations)
{
	_numIterations = numIterations;
}

size_t IncBmoSolverProxy::GetNumIterations(void) const
{
	return _numIterations;
}

void IncBmoSolverProxy::SetNumConflicts(size_t numConflicts)
{
	_numConflicts = numConflicts;
}

size_t IncBmoSolverProxy::GetNumConflicts(void) const
{
	return _numConflicts;
}

void IncBmoSolverProxy::SetCompleteMode(CompleteMode completeMode)
{
	_completeMode = completeMode;
}

IncBmoSolverProxy::CompleteMode IncBmoSolverProxy::GetCompleteMode(void) const
{
	return _completeMode;
}

void IncBmoSolverProxy::SetConflictLimitLocal(bool localConflictLimit)
{
	_localConflictLimit = localConflictLimit;
}

bool IncBmoSolverProxy::IsConflictLimitLocal(void) const
{
	return _localConflictLimit;
}

void IncBmoSolverProxy::SetPartitionStrategy(PartitionStrategy partitionStrategy)
{
	_partitionStrategy = partitionStrategy;
}

IncBmoSolverProxy::PartitionStrategy IncBmoSolverProxy::GetPartitionStrategy(void) const
{
	return _partitionStrategy;
}

void IncBmoSolverProxy::SetGraphType(GraphType graphType)
{
	_graphType = graphType;
}

IncBmoSolverProxy::GraphType IncBmoSolverProxy::GetGraphType(void) const
{
	return _graphType;
}

BaseLiteral IncBmoSolverProxy::NewLiteral(void)
{
	_maxFormula->newVar();
	return SatSolverProxy::NewLiteral();
}

void IncBmoSolverProxy::CommitClause(void)
{
	BMO_SOLVER_NSPACE::vec<BMO_SOLVER_NSPACE::Lit> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push(BMO_SOLVER_NSPACE::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_maxFormula->addHardClause(clause);
	MaxSatSolverProxy::CommitClause();
}

void IncBmoSolverProxy::CommitSoftClause(size_t weight)
{
	DASSERT(weight > 0) << "Weight can not be smaller or equals to 0";
	BMO_SOLVER_NSPACE::vec<BMO_SOLVER_NSPACE::Lit> clause;
	for (auto const& literal : _currentClause)
	{
		clause.push(BMO_SOLVER_NSPACE::toLit(ConvertProxyToSolver(literal).GetUnsigned()));
	}
	_maxFormula->addSoftClause(weight, clause);
	_maxFormula->setMaximumWeight(weight);
	_maxFormula->updateSumWeights(weight);
	MaxSatSolverProxy::CommitClause();
}

SatResult IncBmoSolverProxy::MaxSolve(void)
{
	const auto get_verbosity = [&]() -> int {
		switch (_verbosity)
		{
		case Verbosity::VERBOSITY_MINIMAL:
			return _VERBOSITY_MINIMAL_;
		case Verbosity::VERBOSITY_SOME:
			return _VERBOSITY_SOME_;
		}
		Logging::Panic("Invalid verbosity option");
	};
	const auto get_weight_mode = [&]() -> int {
		switch (_weightMode)
		{
		case WeightMode::UNWEIGHTED:
			return _UNWEIGHTED_;
		case WeightMode::WEIGHTED:
			return _WEIGHTED_;
		}
		Logging::Panic("Invalid weight mode option");
	};
	const auto get_weight_strategy = [&]() -> int {
		switch (_weightStrategy)
		{
		case WeightStrategy::WEIGHT_NONE:
			return _WEIGHT_NONE_;
		case WeightStrategy::WEIGHT_NORMAL:
			return _WEIGHT_NORMAL_;
		case WeightStrategy::WEIGHT_DIVERSIFY:
			return _WEIGHT_DIVERSIFY_;
		}
		Logging::Panic("Invalid weight strategy option");
	};
	const auto get_algorithm = [&]() -> int {
		switch (_algorithm)
		{
		case Algorithm::ALGORITHM_WBO:
			return _ALGORITHM_WBO_;
  		case Algorithm::ALGORITHM_LINEAR_SU:
		  	return _ALGORITHM_LINEAR_SU_;
  		case Algorithm::ALGORITHM_MSU3:
		  	return _ALGORITHM_MSU3_;
  		case Algorithm::ALGORITHM_PART_MSU3:
		  	return _ALGORITHM_PART_MSU3_;
  		case Algorithm::ALGORITHM_OLL:
		  	return _ALGORITHM_OLL_;
  		case Algorithm::ALGORITHM_BEST:
		  	return _ALGORITHM_BEST_;
  		case Algorithm::ALGORITHM_LSU_INCBMO:
		  	return _ALGORITHM_LSU_INCBMO_;
  		case Algorithm::ALGORITHM_LSU_MRSBEAVER:
		  	return _ALGORITHM_LSU_MRSBEAVER_;
  		case Algorithm::ALGORITHM_LSU_MCS:
		  	return _ALGORITHM_LSU_MCS_;
		}
		Logging::Panic("Invalid algorithm option");
	};
	const auto get_incremental_mode = [&]() -> int {
		switch (_incrementalMode)
		{
		case IncrementalMode::INCREMENTAL_NONE:
			return _INCREMENTAL_NONE_;
		case IncrementalMode::INCREMENTAL_BLOCKING:
			return _INCREMENTAL_BLOCKING_;
		case IncrementalMode::INCREMENTAL_WEAKENING:
			return _INCREMENTAL_WEAKENING_;
		case IncrementalMode::INCREMENTAL_ITERATIVE:
			return _INCREMENTAL_ITERATIVE_;
		}
		Logging::Panic("Invalid incremental mode option");
	};
	const auto get_cluster_strategy = [&]() -> ClusterAlg {
		switch (_clusterStrategy)
		{
			case ClusterStrategy::DIVISIVE:
				return ClusterAlg::_DIVISIVE_;
		}
		Logging::Panic("Invalid cluster strategy option");
	};
	const auto get_symmetry_limit = [&]() -> int {
		return _symmetryLimit;
	};
	const auto get_symmetry_breaking = [&]() -> bool {
		switch (_symmetryBreaking)
		{
			case SymmetryBreaking::SYMMETRIC:
				return false;
			case SymmetryBreaking::SYMMETRIC_BREAKING:
				return true;
		}
		Logging::Panic("Invalid symmetry breaking option");
	};
	const auto get_bmo_search = [&]() -> bool {
		switch (_bmoSearch)
		{
		case BmoSearch::ENABLED:
			return true;
		case BmoSearch::DISABLED:
			return false;
		}
		Logging::Panic("Invalid bmo search option");
	};
	const auto get_cardinality = [&]() -> int {
		switch (_cardinalityEncoding)
		{
			case CardinalityEncoding::CARDINALITY_NETWORKS:
				return _CARD_CNETWORKS_;
			case CardinalityEncoding::TOTALIZER:
				return _CARD_TOTALIZER_;
			case CardinalityEncoding::MODULO_TOTALIZER:
				return _CARD_MTOTALIZER_;
		}
		Logging::Panic("Invalid cardinality encoding option");
	};
	const auto get_pb = [&]() -> int {
		switch (_pbEncoding)
		{
			case PbEncoding::SWC:
				return _PB_SWC_;
			case PbEncoding::GTE:
				return _PB_GTE_;
			case PbEncoding::GTE_CLUSTER:
				return _PB_GTECLUSTER_;
			case PbEncoding::ADDER:
				return _PB_ADDER_;
		}
		Logging::Panic("Invalid pn encoding option");
	};
	const auto get_rounding_statistics = [&]() -> Statistics {
		switch (_roundingStatistics)
		{
			case RoundingStatistics::MIN:
				return Statistics::_MIN_;
			case RoundingStatistics::MAX:
				return Statistics::_MAX_;
			case RoundingStatistics::MEDIAN:
				return Statistics::_MEDIAN_;
			case RoundingStatistics::MEAN:
				return Statistics::_MEAN_;
		}
		Logging::Panic("Invalid rounding statistic option");
	};
	const auto get_num_clusters = [&]() -> int {
		return _numClusters;
	};
	const auto get_num_iterations = [&]() -> int {
		return _numIterations;
	};
	const auto get_num_conflicts = [&]() -> int {
		return _numConflicts;
	};
	const auto get_complete_mode = [&]() -> bool {
		switch (_completeMode)
		{
			case CompleteMode::COMPLETE:
				return true;
			case CompleteMode::INCOMPLETE:
				return false;
		}
		Logging::Panic("Invalid complete mode option");
	};
	const auto get_local_conflics = [&]() -> bool {
		return _localConflictLimit;
	};
	const auto get_partition_strategy = [&]() -> int {
		switch (_partitionStrategy)
		{
			case PartitionStrategy::SEQUENTIAL:
				return _PART_SEQUENTIAL_;
			case PartitionStrategy::SEQUENTIAL_SORTED:
				return _PART_SEQUENTIAL_SORTED_;
			case PartitionStrategy::BINARY:
				return _PART_BINARY_;
		}
		Logging::Panic("Invalid partition strategy option");
	};
	const auto get_graph_type = [&]() -> int {
		switch (_graphType)
		{
			case GraphType::VIG:
				return 0;
			case GraphType::CVIG:
				return 1;
			case GraphType::RES:
				return 2;
		}
		Logging::Panic("Invalid graph type option");
	};

	(void) get_weight_mode;
	(void) get_algorithm;
	(void) get_incremental_mode;

	_maxFormula->setInitialVars(_maxFormula->nVars());
	_maxFormula->setHardWeight(_maxFormula->getSumWeights() + 1u);

	switch (_algorithm)
	{
	case Algorithm::ALGORITHM_WBO:
		_solver = std::make_unique<openwbo::WBO>(
			get_verbosity(),
			get_weight_strategy(),
			get_symmetry_breaking(),
			get_symmetry_limit()
		);
		_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		break;

	case Algorithm::ALGORITHM_LINEAR_SU:
		if (_clusterAlgorithm == ClusterAlgorithm::DIVISIVE_MAX_SEPARATE)
		{
			auto solver = std::make_unique<openwbo::LinearSUIncCluster>(
				get_verbosity(),
				get_bmo_search(),
				get_cardinality(),
				get_pb(),
				get_cluster_strategy(),
				get_rounding_statistics(),
				get_num_clusters()
			);
			solver->loadFormula(_maxFormula->copyMaxSATFormula());
			solver->initializeCluster();
			_solver = std::move(solver);
		}
		else if (_clusterAlgorithm == ClusterAlgorithm::NONE)
		{
			_solver = std::make_unique<openwbo::LinearSU>(
				get_verbosity(),
				get_bmo_search(),
				get_cardinality(),
				get_pb()
			);
			_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		}
		break;

	case Algorithm::ALGORITHM_PART_MSU3:
		_solver = std::make_unique<openwbo::PartMSU3>(
			get_verbosity(),
			get_partition_strategy(),
			get_graph_type(),
			get_cardinality()
		);
		_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		break;

	case Algorithm::ALGORITHM_MSU3:
		_solver = std::make_unique<openwbo::MSU3>(
			get_verbosity()
		);
		_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		break;

	case Algorithm::ALGORITHM_LSU_INCBMO:
	{
		auto solver = std::make_unique<openwbo::LinearSUIncBMO>(
			get_verbosity(),
			get_bmo_search(),
			get_cardinality(),
			get_pb(),
			get_cluster_strategy(),
			get_rounding_statistics(),
			get_num_clusters(),
			get_complete_mode()
		);
		solver->loadFormula(_maxFormula->copyMaxSATFormula());
		solver->initializeCluster();
		_solver = std::move(solver);
		break;
	}

	case Algorithm::ALGORITHM_LSU_MRSBEAVER:
		_solver = std::make_unique<openwbo::OBV>(
			get_verbosity(),
			get_cardinality(),
			get_num_conflicts(),
			get_num_iterations(),
			get_local_conflics()
		);
		_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		break;

	case Algorithm::ALGORITHM_LSU_MCS:
		_solver = std::make_unique<openwbo::BLS>(
			get_verbosity(),
			get_cardinality(),
			get_num_conflicts(),
			get_num_iterations(),
			get_local_conflics()
		);
		_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		break;

	case Algorithm::ALGORITHM_OLL:
		if (_clusterAlgorithm == ClusterAlgorithm::DIVISIVE_MAX_SEPARATE)
		{
			auto solver = std::make_unique<openwbo::OLLIncCluster>(
				get_verbosity(),
				get_cardinality(),
				get_cluster_strategy(),
				get_rounding_statistics(),
				get_num_clusters()
			);
			solver->loadFormula(_maxFormula->copyMaxSATFormula());
			solver->initializeCluster();
			_solver = std::move(solver);
		}
		else if (_clusterAlgorithm == ClusterAlgorithm::NONE)
		{
			_solver = std::make_unique<openwbo::OLL>(
				get_verbosity(),
				get_cardinality()
			);
			_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		}
		break;

	case Algorithm::ALGORITHM_BEST:
		if (_weightMode == WeightMode::UNWEIGHTED)
		{
			auto solver = std::make_unique<openwbo::PartMSU3>(
				get_verbosity(),
				_PART_BINARY_,
				2,
				get_cardinality()
			);
			solver->loadFormula(_maxFormula->copyMaxSATFormula());
			if (solver->chooseAlgorithm() == _ALGORITHM_MSU3_)
			{
				_solver = std::make_unique<openwbo::MSU3>(
					get_verbosity()
				);
				_solver->loadFormula(_maxFormula->copyMaxSATFormula());
			}
			else
			{
				_solver = std::move(solver);
			}
		}
		else if (_weightMode == WeightMode::WEIGHTED)
		{
			_solver = std::make_unique<openwbo::OLL>(
				get_verbosity(),
				get_cardinality()
			);
			_solver->loadFormula(_maxFormula->copyMaxSATFormula());
		}
		break;
	}

	try
	{
		_solver->search();
	}
	catch (int result)
	{
		if (result == _SATISFIABLE_ || result == _OPTIMUM_)
		{
			_lastResult = SatResult::SAT;
			_lastCost = _solver->getUB();
		}
		else if (result == _UNSATISFIABLE_)
		{
			_lastResult = SatResult::UNSAT;
			_lastCost = 0u;
		}
		else
		{
			_lastResult = SatResult::UNKNOWN;
			_lastCost = 0u;
		}
		return _lastResult;
	}

	_lastResult = SatResult::UNKNOWN;
	return _lastResult;
}

Value IncBmoSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto const& solution { _solver->getSolution() };
	auto result { solution[ConvertProxyToSolver(lit).GetVariable()] };
	if (result == BMO_SOLVER_NSPACE::l_True || result == BMO_SOLVER_NSPACE::l_False)
	{
		return (result == BMO_SOLVER_NSPACE::l_True)
			? Value::Positive : Value::Negative;
	}
	return Value::DontCare;
}

};
};

#endif
