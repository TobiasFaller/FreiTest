#pragma once

#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE

#include <memory>
#include <vector>

#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"

namespace openwbo
{
	class MaxSAT;
	class MaxSATFormula;
};

namespace SolverProxy
{
namespace Sat
{

class IncBmoSolverProxy:
	public MaxSatSolverProxy
{
public:
	enum class Algorithm {
		ALGORITHM_WBO,
		ALGORITHM_LINEAR_SU,
		ALGORITHM_PART_MSU3,
		ALGORITHM_MSU3,
		ALGORITHM_LSU_INCBMO,
		ALGORITHM_LSU_MRSBEAVER,
		ALGORITHM_LSU_MCS,
		ALGORITHM_OLL,
		ALGORITHM_BEST
	};
	enum class ClusterAlgorithm {
		NONE,
		DIVISIVE_MAX_SEPARATE
	};
	enum class ClusterStrategy {
		DIVISIVE
	};
	enum class WeightMode {
		UNWEIGHTED,
		WEIGHTED
	};
	enum class IncrementalMode {
		INCREMENTAL_NONE,
		INCREMENTAL_BLOCKING,
		INCREMENTAL_WEAKENING,
		INCREMENTAL_ITERATIVE
	};
	enum class Verbosity {
		VERBOSITY_MINIMAL,
		VERBOSITY_SOME
	};
	enum class BmoSearch {
		ENABLED,
		DISABLED
	};
	enum class WeightStrategy {
		WEIGHT_NONE,
		WEIGHT_NORMAL,
		WEIGHT_DIVERSIFY
	};
	enum class SymmetryBreaking {
		SYMMETRIC,
		SYMMETRIC_BREAKING
	};
	enum class CardinalityEncoding {
		CARDINALITY_NETWORKS,
		TOTALIZER,
		MODULO_TOTALIZER
	};
	enum class PbEncoding {
		SWC,
		GTE,
		GTE_CLUSTER,
		ADDER
	};
	enum class RoundingStatistics {
		MEAN,
		MEDIAN,
		MIN,
		MAX
	};
	enum class CompleteMode {
		COMPLETE,
		INCOMPLETE
	};
	enum class PartitionStrategy {
		SEQUENTIAL,
		SEQUENTIAL_SORTED,
		BINARY
	};
	enum class GraphType {
		VIG,
		CVIG,
		RES
	};

	IncBmoSolverProxy(void);
	virtual ~IncBmoSolverProxy(void);

	void CommitClause(void) override;
	void CommitSoftClause(size_t weight = 1) override;
	BaseLiteral NewLiteral(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult MaxSolve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	void SetAlgorithm(Algorithm algorithm);
	Algorithm GetAlgorithm(void) const;

	void SetIncrementalMode(IncrementalMode incrementalMode);
	IncrementalMode GetIncrementalMode(void) const;

	void SetVerbosity(Verbosity verbosity);
	Verbosity GetVerbosity(void) const;

	void SetClusterAlgorithm(ClusterAlgorithm clusterAlgorithm);
	ClusterAlgorithm GetClusterAlgorithm(void) const;

	void SetClusterStrategy(ClusterStrategy clusterStrategy);
	ClusterStrategy GetClusterStrategy(void) const;

	void SetNumClusters(size_t numClusters);
	size_t GetNumClusters(void) const;

	void SetBmoSearch(BmoSearch bmoSearch);
	BmoSearch GetBmoSearch(void) const;

	void SetWeightMode(WeightMode weightMode);
	WeightMode GetWeightMode(void) const;

	void SetWeightStrategy(WeightStrategy weightStrategy);
	WeightStrategy GetWeightStrategy(void) const;

	void SetSymmetryBreaking(SymmetryBreaking symmetryBreaking);
	SymmetryBreaking GetSymmetryBreaking(void) const;

	void SetSymmetryLimit(size_t symmetryLimit);
	size_t GetSymmetryLimit(void) const;

	void SetCardinalityEncoding(CardinalityEncoding cardinalityEncoding);
	CardinalityEncoding GetCardinalityEncoding(void) const;

	void SetPbEncoding(PbEncoding pbEncoding);
	PbEncoding GetPbEncoding(void) const;

	void SetRoundingStatistics(RoundingStatistics roundingStatistics);
	RoundingStatistics GetRoundingStatistics(void) const;

	void SetNumIterations(size_t numIterations);
	size_t GetNumIterations(void) const;

	void SetNumConflicts(size_t numConflicts);
	size_t GetNumConflicts(void) const;

	void SetCompleteMode(CompleteMode completeMode);
	CompleteMode GetCompleteMode(void) const;

	void SetConflictLimitLocal(bool localConflictLimit);
	bool IsConflictLimitLocal(void) const;

	void SetPartitionStrategy(PartitionStrategy partitionStrategy);
	PartitionStrategy GetPartitionStrategy(void) const;

	void SetGraphType(GraphType graphType);
	GraphType GetGraphType(void) const;

	using MaxSatSolverProxy::CommitClause;
	using MaxSatSolverProxy::CommitSoftClause;

private:
	Algorithm _algorithm;
	IncrementalMode _incrementalMode;
	Verbosity _verbosity;
	ClusterAlgorithm _clusterAlgorithm;
	ClusterStrategy _clusterStrategy;
	size_t _numClusters;
	BmoSearch _bmoSearch;
	WeightMode _weightMode;
	WeightStrategy _weightStrategy;
	SymmetryBreaking _symmetryBreaking;
	size_t _symmetryLimit;
	CardinalityEncoding _cardinalityEncoding;
	PbEncoding _pbEncoding;
	RoundingStatistics _roundingStatistics;
	size_t _numIterations;
	size_t _numConflicts;
	CompleteMode _completeMode;
	bool _localConflictLimit;
	PartitionStrategy _partitionStrategy;
	GraphType _graphType;

	std::unique_ptr<openwbo::MaxSAT> _solver;
	std::unique_ptr<openwbo::MaxSATFormula> _maxFormula;

};

};
};

#endif
