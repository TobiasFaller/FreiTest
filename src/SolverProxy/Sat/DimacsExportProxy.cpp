#include "SolverProxy/Sat/DimacsExportProxy.hpp"

#ifdef HAS_SAT_SOLVER_DIMACS

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/lzma.hpp>

#include <chrono>
#include <iostream>

#include "Basic/Logging.hpp"

namespace SolverProxy
{
namespace Sat
{

DimacsExportProxy::DimacsExportProxy(void):
	SatSolverProxy(SatSolver::EXPORT_SAT_DIMACS),
	MaxSatSolverProxy(SatSolver::EXPORT_SAT_DIMACS),
	CountSatSolverProxy(SatSolver::EXPORT_SAT_DIMACS),
	_hardClauses(),
	_softClauses(),
	_filename(),
	_comments(),
	_compression(Compression::None),
	_format(Format::Dimacs)
{
	Reset();
}

DimacsExportProxy::~DimacsExportProxy(void) = default;

bool DimacsExportProxy::IsIncrementalSupported(void) const
{
	return true;
}

void DimacsExportProxy::Reset(void)
{
	_hardClauses.clear();
	_softClauses.clear();
	MaxSatSolverProxy::Reset();
}

SatResult DimacsExportProxy::Solve(void)
{
	Export();
	_lastResult = SatResult::UNKNOWN;
	return _lastResult;
}

SatResult DimacsExportProxy::MaxSolve(void)
{
	Export();
	_lastResult = SatResult::UNKNOWN;
	_lastCost = 0u;
	return _lastResult;
}

SatResult DimacsExportProxy::CountSolve(void)
{
	Export();
	_lastResult = SatResult::UNKNOWN;
	_lastModelCount = 0u;
	return _lastResult;
}

void DimacsExportProxy::CommitSoftClause(size_t weight)
{
	DASSERT(weight > 0) << "Weight can not be smaller or equals to 0";
	_softClauses.emplace_back(weight, std::move(_currentClause));
	SatSolverProxy::CommitClause();
}

void DimacsExportProxy::CommitClause(void)
{
	_hardClauses.push_back(_currentClause);
	SatSolverProxy::CommitClause();
}

std::string DimacsExportProxy::GetFilename(void) const
{
	return _filename;
}

void DimacsExportProxy::SetFilename(std::string filename)
{
	this->_filename = filename;
}

std::vector<std::string> DimacsExportProxy::GetComments(void) const
{
	return _comments;
}

void DimacsExportProxy::SetComments(std::vector<std::string> comments)
{
	this->_comments = comments;
}

void DimacsExportProxy::SetCompression(DimacsExportProxy::Compression compression)
{
	this->_compression = compression;
}

DimacsExportProxy::Compression DimacsExportProxy::GetCompression(void) const
{
	return _compression;
}

void DimacsExportProxy::SetFormat(DimacsExportProxy::Format format)
{
	this->_format = format;
}

DimacsExportProxy::Format DimacsExportProxy::GetFormat(void) const
{
	return _format;
}

void DimacsExportProxy::Export(void) const
{
	auto now_tickcount = std::chrono::system_clock::now();
	auto now_timet = std::chrono::system_clock::to_time_t(now_tickcount);
	auto time_tm = std::localtime(&now_timet);

	char timestamp[128u];
	LOG_IF(std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", time_tm) == 0u, FATAL)
		<< "The timestamp was not converted into a textural presentaiton";

	std::string filename = GetFilename();
	if (filename == "")
	{
		filename = "./max-sat-problem-" + std::string(timestamp);
		switch (_format)
		{
			case Format::Dimacs: filename += ".cnf"; break;
			case Format::WDimacsClassic: filename += ".wcnf"; break;
			case Format::WDimacs2021: filename += ".wcnf"; break;
		}
	}

	std::ofstream outfstream { filename };
	LOG_IF(!outfstream.good(), FATAL) << "Could not open file " << filename << " for exporting Max-SAT problem.";

	boost::iostreams::filtering_ostreambuf outfilter;
	switch(_compression)
	{
		case Compression::Bzip2:
			outfilter.push(boost::iostreams::bzip2_compressor());
			break;
		case Compression::Gzip:
			outfilter.push(boost::iostreams::gzip_compressor());
			break;
		case Compression::Lzma:
			outfilter.push(boost::iostreams::lzma_compressor());
			break;
		case Compression::None:
			break;
		default:
			Logging::Panic();
	}
	outfilter.push(outfstream);
	std::ostream out { &outfilter };

	auto clause_to_string = [&](const std::vector<BaseLiteral>& clause) -> std::string {
		std::string result;

		for (size_t literalIndex = 0u; literalIndex < clause.size(); ++literalIndex)
		{
			if (literalIndex != 0u) result += " ";
			result += std::to_string(ConvertProxyToSolver(clause[literalIndex]).GetSigned());
		}
		result += " 0";

		return result;
	};

	out << "c ------------------------------------------------------------------------------" << std::endl;
	out << "c   Generated by: FreiTest Max-SAT Export Proxy" << std::endl;
	out << "c   Generation time: " << std::left << std::string(timestamp) << std::endl;
	out << "c   Comments:" << std::endl;
	for (auto comment : _comments) out << "c       " << std::left << comment << std::endl;
	out << "c ------------------------------------------------------------------------------" << std::endl;
	out << std::endl;

	const auto get_top_value = [&]() -> size_t {
		size_t sum = 0u;
		for (auto [weight, clause] : _softClauses)
		{
			sum += weight;
		}
		return sum + 1u;
	};

	const size_t numberOfVariables = _numberOfVariables - 1u;
	const size_t numberOfHardClauses = _hardClauses.size() + _assumptions.size();
	const size_t numberOfSoftClauses = _softClauses.size();
	const size_t topValue = get_top_value();
	const bool exportSoftClauses = _format != Format::Dimacs;

	// Documentation for multiple formats:
	// - http://www.maxhs.org/docs/wdimacs.html
	switch (_format)
	{
		case Format::Dimacs:
			// DIMACS CNF:
			// - https://people.sc.fsu.edu/~jburkardt/data/cnf/cnf.html
			out << "p cnf " << std::to_string(numberOfVariables)
				<< " " << std::to_string(numberOfHardClauses)
				<< std::endl;
			break;

		case Format::WDimacsClassic:
			// Weighted DIMACS CNF:
			// - https://maxsat-evaluations.github.io/2021/rules.html#input
			out << "p wcnf " << std::to_string(numberOfVariables)
				<< " " << std::to_string(numberOfHardClauses + numberOfSoftClauses)
				<< " " << std::to_string(topValue)
				<< std::endl;
			break;

		case Format::WDimacs2021:
			// Weighted DIMACS CNF 2021 Format:
			// - https://maxsat-evaluations.github.io/2021/format.html
			// - There is no p line in the new format.
			break;
	}

	out << "c Hard clauses: " << std::to_string(_hardClauses.size()) << std::endl;
	out << std::endl;
	for (size_t clauseIndex = 0u; clauseIndex < _hardClauses.size(); ++clauseIndex)
	{
		if (clauseIndex % 10u == 0u)
		{
			out << std::endl;
			out << "c Hard clauses " << std::to_string(clauseIndex) << " to "
				<< std::to_string(std::min(clauseIndex + 10u, _hardClauses.size() - 1u)) << std::endl;
		}

		switch (_format)
		{
			case Format::Dimacs: break; // No prefix
			case Format::WDimacsClassic: out << std::to_string(topValue) << " "; break;
			case Format::WDimacs2021: out << "h "; break;
		}
		out << clause_to_string(_hardClauses[clauseIndex]) << std::endl;
	}
	out << std::endl;

	out << "c Assumption clauses: " << std::to_string(_assumptions.size()) << std::endl;
	out << std::endl;
	for (size_t clauseIndex = 0u; clauseIndex < _assumptions.size(); ++clauseIndex)
	{
		if (clauseIndex % 10u == 0u)
		{
			out << std::endl;
			out << "c Assumption clauses " << std::to_string(clauseIndex) << " to "
				<< std::to_string(std::min(clauseIndex + 10u, _assumptions.size() - 1u)) << std::endl;
		}

		switch (_format)
		{
			case Format::Dimacs: break; // No prefix
			case Format::WDimacsClassic: out << std::to_string(topValue) << " "; break;
			case Format::WDimacs2021: out << "h "; break;
		}
		out << clause_to_string(_assumptions[clauseIndex]) << std::endl;
	}
	out << std::endl;

	if (exportSoftClauses)
	{
		out << "c Weighted soft clauses: " << std::to_string(_softClauses.size()) << std::endl;
		out << std::endl;
		for (size_t clauseIndex = 0u; clauseIndex < _softClauses.size(); ++clauseIndex)
		{
			if (clauseIndex % 10u == 0u)
			{
				out << std::endl;
				out << "c Weighted soft clauses " << std::to_string(clauseIndex) << " to "
					<< std::to_string(std::min(clauseIndex + 10u, _softClauses.size() - 1u)) << std::endl;
			}

			const auto [weight, clause] = _softClauses[clauseIndex];
			out << std::to_string(weight) << " " << clause_to_string(clause) << std::endl;
		}
		out << std::endl;
	}

	out.flush();
	outfstream.close();
}

Value DimacsExportProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	return Value::DontCare;
}

};
};

#endif
