#include "SolverProxy/Sat/ExternalSolverProxy.hpp"

#ifdef HAS_SAT_SOLVER_DIMACS

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <chrono>
#include <iostream>
#include <filesystem> // requires std++17
#include <sstream>
#include <regex>

#include <sys/wait.h>
#include <unistd.h>

#include "Basic/Logging.hpp"

namespace SolverProxy
{
namespace Sat
{

ExternalSolverProxy::ExternalSolverProxy(void):
	SatSolverProxy(SatSolver::EXTERNAL_ARBITRARY_SOLVER),
	MaxSatSolverProxy(SatSolver::EXTERNAL_ARBITRARY_SOLVER),
	CountSatSolverProxy(SatSolver::EXTERNAL_ARBITRARY_SOLVER),
	_hardClauses(),
	_softClauses(),
	_export_filename(),
	_import_filename(),
	_comments(),
	_cmd(),
	_shell("/bin/bash"),
	_compression(Compression::None),
	_export_format(Format::Dimacs)
{
	Reset();
}

ExternalSolverProxy::~ExternalSolverProxy(void) = default;

bool ExternalSolverProxy::IsIncrementalSupported(void) const
{
	return true;
}

void ExternalSolverProxy::Reset(void)
{
	_hardClauses.clear();
	_softClauses.clear();
	MaxSatSolverProxy::Reset();
}

SatResult ExternalSolverProxy::Solve(void)
{
	Export();
	Execute(GetCmd(), GetShell());
	Import();
	return _lastResult;
}

SatResult ExternalSolverProxy::MaxSolve(void)
{
	Export();
	Execute(GetCmd(), GetShell());
	Import();
	return _lastResult;
}

SatResult ExternalSolverProxy::CountSolve(void)
{
	Export();
	Execute(GetCmd(), GetShell());
	Import();
	_lastModelCount = 0u; // FIXME: Haven't found the format in which CountSAT indicates the total model count yet.
	return _lastResult;
}

void ExternalSolverProxy::CommitSoftClause(size_t weight)
{
	DASSERT(weight > 0) << "Weight can not be smaller or equals to 0";
	_softClauses.emplace_back(weight, std::move(_currentClause));
	SatSolverProxy::CommitClause();
}

void ExternalSolverProxy::CommitClause(void)
{
	_hardClauses.push_back(_currentClause);
	SatSolverProxy::CommitClause();
}

std::string ExternalSolverProxy::GetExportFilename(void) const
{
	return _export_filename;
}

void ExternalSolverProxy::SetExportFilename(const std::string filename)
{
	this->_export_filename = filename;
}

std::string ExternalSolverProxy::GetImportFilename(void) const
{
	return _import_filename;
}

void ExternalSolverProxy::SetImportFilename(const std::string filename)
{
	this->_import_filename = filename;
}

std::string ExternalSolverProxy::GetCmd(void) const
{
    return this->_cmd;
}

void ExternalSolverProxy::SetCmd(const std::string& cmd)
{
    this->_cmd = cmd;
}

std::string ExternalSolverProxy::GetShell(void) const
{
    return this->_shell;
}

void ExternalSolverProxy::SetShell(const std::string& shell)
{
    this->_shell = shell;
}

std::vector<std::string> ExternalSolverProxy::GetComments(void) const
{
	return _comments;
}

void ExternalSolverProxy::SetComments(std::vector<std::string> comments)
{
	this->_comments = comments;
}

void ExternalSolverProxy::SetCompression(ExternalSolverProxy::Compression compression)
{
	this->_compression = compression;
}

ExternalSolverProxy::Compression ExternalSolverProxy::GetCompression(void) const
{
	return _compression;
}

void ExternalSolverProxy::SetExportFormat(ExternalSolverProxy::Format format)
{
	this->_export_format = format;
}

ExternalSolverProxy::Format ExternalSolverProxy::GetExportFormat(void) const
{
	return _export_format;
}

void ExternalSolverProxy::Export(void) const
{
	auto now_tickcount = std::chrono::system_clock::now();
	auto now_timet = std::chrono::system_clock::to_time_t(now_tickcount);
	auto time_tm = std::localtime(&now_timet);

	char timestamp[128u];
	LOG_IF(std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d_%H-%M-%S", time_tm) == 0u, FATAL)
		<< "The timestamp was not converted into a textural presentation";

	std::string filename = GetExportFilename();
	if (filename == "")
	{
		filename = "./max-sat-problem-" + std::string(timestamp);
		switch (_export_format)
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
	out << "c   Generated by: FreiTest External Solver Proxy" << std::endl;
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
	const bool exportSoftClauses = _export_format != Format::Dimacs;

	// Documentation for multiple formats:
	// - https://www.maxhs.org/docs/wdimacs.html
	switch (_export_format)
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

		switch (_export_format)
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

		switch (_export_format)
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

void ExternalSolverProxy::Execute(const std::string& cmd, const std::string& shell) const
{
	auto is_shell_script = [](const std::string& cmd) -> bool
	{
		std::error_code ec;
		auto file_status = std::filesystem::status(cmd, ec);

		if(ec)
			LOG(WARNING) << "Filesystem error code " << ec.value() << " = " << ec.message();

		if (!std::filesystem::is_regular_file(file_status))
			return false;

		return access(cmd.c_str(), X_OK) == 0;
	};

	if (cmd.empty())
		return;

	pid_t pid = fork();

	if (pid == 0)
	{
		if (is_shell_script(cmd))
			execl(cmd.c_str(), cmd.c_str(), nullptr);
		else
			execl(shell.c_str(), shell.c_str(), "-c", cmd.c_str(), nullptr);
	}

	else if (pid > 0)
	{
		int status;

		if (waitpid(pid, &status, 0) < 0)
			LOG(FATAL) << "Wait has failed!";

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            LOG(WARNING) << "External solver terminated abnormally! Exit code: " << WEXITSTATUS(status);
        }
	}
	else
	{
		LOG(FATAL) << "fork() failed!";
	}
}

void ExternalSolverProxy::Import()
{

	std::string filename = GetImportFilename();
	DASSERT(!filename.empty()) << "Model's filename is unspecified!";

	std::ifstream in {filename};

	std::string line;
	while(std::getline(in, line))
	{
		std::istringstream ss(line);

		char tag;
		ss >> tag;

		switch(tag)
		{
			case 'c':
			{
				std::string comment;

				std::getline(ss, comment);
				boost::algorithm::trim(comment);
				LOG(INFO) << "[External Solver's Comment]: " << comment;

				break;
			}
			case 's':
			{
				std::string verdict;

				std::getline(ss, verdict);
				boost::algorithm::trim(verdict);

				static const std::regex sat(R"(^sat(isfiable)?$)", std::regex_constants::icase);
				static const std::regex unsat(R"(^unsat(isfiable)?$)", std::regex_constants::icase);
				static const std::regex unknown(R"(^unknown$)", std::regex_constants::icase);
				static const std::regex optimum(R"(^optimum\s+found$)", std::regex_constants::icase);

				if (std::regex_match(verdict, sat) || std::regex_match(verdict, optimum))
					_lastResult = SatResult::SAT;
				else if (std::regex_match(verdict, unsat))
					_lastResult = SatResult::UNSAT;
				else if (std::regex_match(verdict, unknown))
					_lastResult = SatResult::UNKNOWN;
				else
				{
					_lastResult = SatResult::UNKNOWN;
					LOG(WARNING) << "Solution line malformed in model file (" << GetImportFilename() << ")";
				}

				break;

			}

			case 'o': // Only for MaxSAT. Update directly the cost.
			{
				ss >> _lastCost;
				LOG(INFO) << "[External Solver's Optimum]: Cost is " << _lastCost;
				break;
			}

			case 'v':
			{

				std::string vline;
				std::getline(ss, vline);
				boost::algorithm::trim(vline);

				bool is_bitstring = !vline.empty() && (vline.find_first_not_of("01") == std::string::npos);

				// The model is given in a bit-string format.
				// This can possibly span to multiple  lines.
				// This format doesn't have a 0 terminator at
				// the end of the assignment.
				if (is_bitstring)
				{
					for (const char& c: vline)
					{
						_model.push_back(c == '1');
					}
				}

				// The model is given  in  a  signed  format.
				// This again can possibly span  to  multiple
				// lines. The variables are 1-based  indexed.
				// Their value corresponds to the  index   of
				// the _model (0-based) and their sign to the
				// assigned value on that index.
				else
				{

					std::istringstream literals(vline);
					int literal;

					while(literals >> literal)
					{
						if (!literal) // 0, when used, is a line termination token
							break;

						unsigned var = std::abs(literal) - 1;
						bool val = (literal > 0);

						if (_model.size() <= var)
							_model.resize(var + 1);

						_model[var] = val;
					}
				}
				break;
			}
			default:
			{
				LOG(WARNING) << "Unknown tag found in model file: " << tag;
				break;
			}
		}
	}
}

Value ExternalSolverProxy::GetLiteralValue(BaseLiteral lit) const
{
	if (!lit.IsSet())
	{
		return Value::Invalid;
	}

	auto var_value = _model[ConvertProxyToSolver(lit).GetVariable()];
	auto lit_value = lit.IsNegated() ? !var_value : var_value;

	return lit_value? Value::Positive : Value::Negative;

}

};
};

#endif
