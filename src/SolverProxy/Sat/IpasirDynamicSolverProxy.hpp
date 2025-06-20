#pragma once

#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC

#include <memory>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"

namespace SolverProxy
{
namespace Sat
{

class DynamicIpasirLoader;

class IpasirDynamicSolverProxy:
	public SatSolverProxy
{
public:
	IpasirDynamicSolverProxy(void);
	virtual ~IpasirDynamicSolverProxy(void);

	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	using SatSolverProxy::CommitClause;

private:
	std::unique_ptr<DynamicIpasirLoader> _ipasirLoader;

};

};
};

#endif
