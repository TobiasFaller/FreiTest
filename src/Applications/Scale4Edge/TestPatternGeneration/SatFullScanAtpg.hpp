#pragma once

#include <cstdint>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

/*!
 * \brief This application implements a combinatorial Atpg test flow.
 *
 * The assumptions of this test flow is that the circuit is either of
 * combinatorial nature (no flip-flops or latches) or that all flip-flops
 * and latches are accessible through a scan chain (full-scan).
 * All inputs are assumed to be accessible.
 *
 * Steps:
 * 1. Fault list generation by using the mapped circuit
 * 2. (Fault list compaction for equivalent faults)
 * 3. For each fault in the fault list:
 *   - Transform the circuit into a boolean CNF (conjunctive normal form)
 *     - Tag all relevant circuit parts to be encoded with
 *       GoodTag (good, no fault) and BadTag (bad, fault)
 *     - Encode the tagged circuit parts by using the logic generator
 *   - Solve the CNF formula by using a SAT-solver
 *   - When SAT-result is "SAT" (satisfiable):
 *     - Add the fault to testable fault list
 *     - Extract test pattern from used literals
 *     - Use simulation to check if the test pattern detects other faults
 *       - Simulate test pattern on circuit without fault
 *       - For each remaining fault:
 *         - Simulate test pattern on circuit with fault
 *         - When result is different than the fault-free result
 *           - Add the fault to testable fault list
 *   - When SAT-result is "UNSAT" (unsatisfiable):
 *     - Add the fault to non-testable fault list
 * 4. Export statistics
 */

template<typename FaultModel, typename FaultList>
class SatFullScanAtpg:
	public AtpgBase<FaultModel, FaultList>
{
public:
	SatFullScanAtpg(void);
	virtual ~SatFullScanAtpg(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

protected:
	enum class MaximizeDontCareValues {
		Disabled,
		Enabled
	};
	enum class MaximizeDontCarePorts {
		None,
		Inputs,
		Outputs,
		InputAndOutputs
	};
	enum class MaximizeDontCareFlipFlops {
		None,
		Inputs,
		Outputs,
		InputAndOutputs
	};

	void GeneratePatternForFault(size_t faultIndex);

	MaximizeDontCareValues maximizeDontCareValues;
	MaximizeDontCarePorts maximizeDontCarePorts;
	MaximizeDontCareFlipFlops maximizeDontCareFlipFlops;
	size_t maximizeDontCarePortWeight;
	size_t maximizeDontCareFlipFlopWeight;

};

};
};
};
