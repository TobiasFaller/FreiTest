#pragma once

#include <memory>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/CountSat/CountSatSolverProxy.hpp"
#include "SolverProxy/Sat/DimacsExportProxy.hpp"
#include "SolverProxy/ModernClauseDB.hpp"

namespace SolverProxy
{
namespace Sat
{

/**
 * @brief Proxy that interfaces with an external SAT / MaxSAT / #SAT solver
 *        via file export, shell execution, and result import.
 *
 * The proxy serializes the internal clause database into DIMACS / WDIMACS,
 * invokes an external solver process, and parses and stores the produced
 * model or count.
 */
class ExternalSolverProxy:
	public virtual SatSolverProxy,
	public virtual MaxSatSolverProxy,
	public virtual CountSatSolverProxy
{
public:
    /**
     * @brief Compression method used for exported instances.
     */
	enum class Compression { None, Gzip, Bzip2, Lzma };

    /**
     * @brief Output format used when exporting the problem.
     */
	enum class Format { Dimacs, WDimacsClassic, WDimacs2021 };

	/**
     * @brief Constructs an empty solver proxy with default configuration.
     */
	ExternalSolverProxy(void);

	/**
     * @brief Virtual destructor.
     */
	virtual ~ExternalSolverProxy(void);

	/**
	 * @brief Commits the currently constructed hard clause.
	 */
	void CommitClause(void) override;

    /**
     * @brief Commits the currently constructed soft clause.
     *
     * @param weight Weight assigned to the soft clause.
     */
	void CommitSoftClause(size_t weight = 1) override;

	/**
	 * @brief Gets the value of a literal in the last model produced by the solver.
	 *
	 * @param lit The literal whose assignment is queried.
	 * @return The value assigned to the literal (Positive, Negative, or Invalid).
	 */
	Value GetLiteralValue(BaseLiteral) const override;

	/**
	 * @brief Solves the instance as a SAT problem.
	 *
	 * Exports the current hard-clause database, invokes the external solver,
	 * and imports the resulting model.
	 *
	 * @return The satisfiability result.
	 */
	SatResult Solve(void) override;

	/**
	 * @brief Solves the instance as a MaxSAT problem.
	 *
	 * Exports both hard and soft clauses, invokes the external solver,
	 * and imports the resulting optimum model and cost.
	 *
	 * @return The MaxSAT result.
	 */
	SatResult MaxSolve(void) override;

	/**
	 * @brief Solves the instance as a model counting (#SAT) problem.
	 *
	 * Exports the current hard-clause database, invokes the external solver,
	 * and imports the resulting model information. The total model count
	 * is currently not parsed from the solver output.
	 *
	 * @return The counting result.
	 */
	SatResult CountSolve(void) override;

	/**
	 * @brief Resets the proxy and MaxSAT solver state.
	 *
	 * Clears all stored hard and soft clauses, and any previously
	 * imported model or cost information, restoring the proxy to its initial
	 * empty state.
	 */
	void Reset(void) override;

	/**
	 * @brief Indicates whether incremental solving is supported.
	 *
	 * This method is required by the solver proxy interface. For the external
	 * solver backend, it has no semantic effect and therefore always returns true.
	 *
	 * @return Always true.
	 */
	bool IsIncrementalSupported(void) const override;

	/**
	 * @brief Returns the filename used for exporting the problem instance.
	 *
	 * @return Path of the file to which the instance will be written.
	 */
	std::string GetExportFilename(void) const;

	/**
	 * @brief Sets the filename used for exporting the problem instance.
	 *
	 * @param filename Path of the file to which the instance will be written.
	 */
	void SetExportFilename(const std::string);

	/**
	 * @brief Returns the filename used for importing the solver output.
	 *
	 * @return Path of the file from which the solver result will be read.
	 */
	std::string GetImportFilename(void) const;

	/**
	 * @brief Sets the filename used for importing the solver output.
	 *
	 * @param filename Path of the file from which the solver result will be read.
	 */
	void SetImportFilename(const std::string);

	/**
	 * @brief Returns the list of comment lines written to the exported instance.
	 *
	 * @return Vector of comment strings.
	 */
	std::vector<std::string> GetComments(void) const;

	/**
	 * @brief Sets the list of comment lines written to the exported instance.
	 *
	 * @param comments Vector of comment strings to be inserted in the DIMACS header.
	 */
	void SetComments(std::vector<std::string>);

	/**
	 * @brief Returns the command used to invoke the external solver.
	 *
	 * @return Shell command string.
	 */
	std::string GetCmd(void) const;

	/**
	 * @brief Sets the command used to invoke the external solver.
	 *
	 * @param cmd Shell command string.
	 */
	void SetCmd(const std::string&);

	/**
	 * @brief Returns the shell executable used to run the solver command.
	 *
	 * @return Path to the shell (e.g., /bin/bash).
	 */
	std::string GetShell(void) const;

	/**
	 * @brief Sets the shell executable used to run the solver command.
	 *
	 * @param shell Path to the shell executable.
	 */
	void SetShell(const std::string&);

	/**
	 * @brief Returns the compression method used for exporting the instance.
	 *
	 * @return Selected compression mode.
	 */
	Compression GetCompression(void) const;

	/**
	 * @brief Sets the compression method used for exporting the instance.
	 *
	 * @param compression Compression mode to apply.
	 */
	void SetCompression(Compression);

	/**
	 * @brief Returns the format used for exporting the instance.
	 *
	 * @return Selected export format.
	 */
	Format GetExportFormat(void) const;

	/**
	 * @brief Sets the format used for exporting the instance.
	 *
	 * @param format Export format to use (DIMACS / WDIMACS).
	 */
	void SetExportFormat(Format);


private:

    /**
     * @brief Exports the current clause database to a DIMACS / WDIMACS file.
     *
     * The export format, compression, comments, and target filename are
     * determined by the current proxy configuration.
     */
	void Export(void) const;

    /**
     * @brief Executes the external solver command.
     *
     * The command is invoked either directly (if it is an executable script)
     * or through the specified shell.
     *
     * @param cmd   Command or script to execute.
     * @param shell Shell used to execute the command (default: /bin/bash).
     */
	void Execute(const std::string&, const std::string& shell = "/bin/bash") const;

    /**
     * @brief Imports and parses the solver output.
     *
     * Reads the solver result file and updates the last result status,
     * model, and objective value accordingly.
     */
	void Import(void);

	std::vector<std::vector<SolverProxy::BaseLiteral>> _hardClauses;
	std::vector<std::vector<SolverProxy::BaseLiteral>> _assumptions;
	std::vector<std::tuple<size_t, std::vector<SolverProxy::BaseLiteral>>> _softClauses;

	std::string _export_filename;
	std::string _import_filename;
	std::vector<std::string> _comments;

	std::string _cmd, _shell;

	std::vector<uint8_t> _model;

	Compression _compression;
	Format _export_format;
};

};
};
