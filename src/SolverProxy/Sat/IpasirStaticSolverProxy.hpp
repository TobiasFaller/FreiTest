#pragma once

#ifdef HAS_SAT_SOLVER_IPASIR_STATIC

#include <memory>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

extern "C" { void ipasir_release (void * solver); };

struct IpasirDeleter
{
	void operator()(void * solver) const
	{
		ipasir_release(solver);
	}
};

namespace SolverProxy
{
namespace Sat
{

class IpasirStaticSolverProxy:
	public SatSolverProxy
{
public:
	IpasirStaticSolverProxy(void);
	virtual ~IpasirStaticSolverProxy(void);

	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<void, IpasirDeleter> _solver;

};

};
};

#endif
