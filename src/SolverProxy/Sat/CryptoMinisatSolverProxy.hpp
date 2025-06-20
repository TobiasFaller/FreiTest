#pragma once

#ifdef HAS_SAT_SOLVER_CRYPTO_MINISAT

#include <memory>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace CMSat
{
	class SATSolver;
};

namespace SolverProxy
{
namespace Sat
{

class CryptoMinisatSolverProxy:
	public SatSolverProxy
{
public:
	CryptoMinisatSolverProxy(void);
	virtual ~CryptoMinisatSolverProxy(void);

	BaseLiteral NewLiteral(void) override;
	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<CMSat::SATSolver> _solver;
};

};
};

#endif

