#pragma once

#ifdef HAS_SAT_SOLVER_CADICAL

#include <memory>
#include <vector>

#include "Basic/Logic.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace CaDiCaL
{
	class Solver;
};

namespace SolverProxy
{
namespace Sat
{

class CadicalSolverProxy : public SatSolverProxy
{
public:
	CadicalSolverProxy(void);
	virtual ~CadicalSolverProxy(void);

	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<CaDiCaL::Solver> _solver;

};

};
};

#endif
