#pragma once

#ifdef HAS_BMC_SOLVER_NCIP

#include <ncip/bmc-format-cip.hpp>

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

namespace Ncip
{
	namespace Backend {
		struct MiniCraigTag;
		struct CadiCraigTag;
	};

	template<typename Impl>
	class BmcSolver;
	class BmcResult;
	class CipProblemBuilder;
};

namespace SolverProxy
{
namespace Bmc
{

class NcipSolverProxy:
	public BmcSolverProxy
{
public:
	NcipSolverProxy(void);
	virtual ~NcipSolverProxy(void);

	BaseLiteral NewLiteral(void) override;
	void CommitClause(void) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	BmcResult Solve(void) override;
	void Reset(void) override;

private:
	std::unique_ptr<Ncip::BmcSolver<Ncip::Backend::MiniCraigTag>> _solver;
	std::unique_ptr<Ncip::CipProblemBuilder> _builder;
	std::unique_ptr<Ncip::BmcResult> _result;

};

};
};

#endif
