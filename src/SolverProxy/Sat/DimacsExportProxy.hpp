#pragma once

#ifdef HAS_SAT_SOLVER_DIMACS

#include <memory>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"
#include "SolverProxy/ModernClauseDB.hpp"

namespace SolverProxy
{
namespace Sat
{

class DimacsExportProxy:
	public virtual SatSolverProxy,
	public virtual MaxSatSolverProxy,
	public virtual CountSatSolverProxy
{
public:
	enum class Compression { None, Gzip, Bzip2, Lzma };
	enum class Format { Dimacs, WDimacsClassic, WDimacs2021 };

	DimacsExportProxy(void);
	virtual ~DimacsExportProxy(void);

	void CommitClause(void) override;
	void CommitSoftClause(size_t weight = 1) override;
	Value GetLiteralValue(BaseLiteral lit) const override;

	SatResult Solve(void) override;
	SatResult MaxSolve(void) override;
	SatResult CountSolve(void) override;
	void Reset(void) override;
	bool IsIncrementalSupported(void) const override;

	std::string GetFilename(void) const;
	void SetFilename(std::string filename);

	std::vector<std::string> GetComments(void) const;
	void SetComments(std::vector<std::string> comments);

	Compression GetCompression(void) const;
	void SetCompression(Compression compression);

	Format GetFormat(void) const;
	void SetFormat(Format format);

	using MaxSatSolverProxy::CommitClause;
	using MaxSatSolverProxy::CommitSoftClause;

private:
	void Export(void) const;

	std::vector<SolverProxy::BaseLiteral> _currentClause;
	std::vector<std::vector<SolverProxy::BaseLiteral>> _hardClauses;
	std::vector<std::vector<SolverProxy::BaseLiteral>> _assumptions;
	std::vector<std::tuple<size_t, std::vector<SolverProxy::BaseLiteral>>> _softClauses;

	std::string _filename;
	std::vector<std::string> _comments;
	Compression _compression;
	Format _format;
};

};
};

#endif
