#pragma once

#ifdef HAS_BMC_SOLVER_EXPORT_CIP

#include <string>

#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

namespace SolverProxy
{
namespace Bmc
{

class CipExportProxy: public BmcSolverProxy
{
public:
	enum class Compression { None, Gzip, Bzip2, Lzma };

	CipExportProxy(void);
	virtual ~CipExportProxy(void);

	Value GetLiteralValue(BaseLiteral lit) const override;

	BmcResult Solve(void) override;
	void Reset(void) override;

	void SetFilename(std::string name);
	std::string GetFilename(void) const;

	void SetComments(std::vector<std::string> comments);
	std::vector<std::string> GetComments(void) const;

	void SetCompression(Compression compression);
	Compression GetCompression(void) const;

private:
	std::string filename;
	std::vector<std::string> comments;
	Compression compression;

};

};
};

#endif
