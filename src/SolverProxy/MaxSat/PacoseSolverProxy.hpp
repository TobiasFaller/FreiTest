#pragma once

#ifdef HAS_MAX_SAT_SOLVER_PACOSE

#include <memory>
#include <iostream>

#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"

namespace Pacose
{

class Pacose;
struct ClauseDB;

};

namespace SolverProxy
{
namespace Sat
{

enum class PacoseEncoding
{
	// Adder, Warners 1996, [Warners 1998?]
	Warners,
	// Totalizer [Bailleux & Boufkhad 2003]
	Bailleux,
	// [Asin et. al 2011] Robert Asin, Robert Nieuwenhuis, Albert Oliveras,
	// Enric Rodriguez-Carbonell
	// "Cardinality Networks: a theoretical and empirical study"
	Asin,
	// Modulo Totalizer [Ogawa et. al 2013]
	Ogawa,
	// BailleuxW2
	BailleuxW2,
	// Weighted MaxSAT Totalizer
	WeightedTotalizer,
	// Mixed Radix Weighted Totalizer
	MixedRadixWeightedTotalizer,
	// Mixed Radix Weighted Totalizer
	MixedRadixWeightedTotalizer2,
	// Mixed Radix Weighted Totalizer 19 competition version
	MixedRadixWeightedTotalizer19,
	// Mixed Radix Weighted Totalizer 19 competition version
	MixedRadixWeightedTotalizer19_2,
	// Dynamic Global Polynomial Watchdog [Paxian & Reimer 2018]
	DynamicGlobalPolynomialWatchdog,
	// Heuristic 17 Competition [Koshi, 2014]
	// Selects between "warners, bailleux, ogawa"
	HeuristicQMaxSat17,
	// Heuristic 20 Competition, choosing between warners and dgpw
	Heuristic20,
	// Heuristic 18 Competition, choosing between warners and dgpw
	Heuristic1819,
	// Heuristic 19 Competition
	QMaxSat19
};

std::string to_string(const PacoseEncoding& encoding);
std::ostream& operator<<(std::ostream& out, const PacoseEncoding& encoding);

class PacoseSolverProxy:
	public MaxSatSolverProxy
{
public:
	PacoseSolverProxy(void);
	virtual ~PacoseSolverProxy(void);

	void CommitClause(void) override;
	void CommitSoftClause(size_t weight = 1) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult MaxSolve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	PacoseEncoding GetEncoding(void) const;
	void SetEncoding(PacoseEncoding encoding);

	using MaxSatSolverProxy::CommitClause;
	using MaxSatSolverProxy::CommitSoftClause;

private:
	std::unique_ptr<Pacose::Pacose> _solver;
	std::unique_ptr<Pacose::ClauseDB> _clauses;
};

}
}

#endif
