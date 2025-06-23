#pragma once

#include <cstdint>
#include <string>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/SequentialAtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

/*!
 * \brief This application implements a sequential ATPG test flow.
 *
 * The assumptions of this test flow is that the circuit is only accessible
 * from the top-level input / output ports.
 * Therefore, multiple timeframes might be required to find a test pattern
 * for a fault and less faults are testable than when using the full-scan
 * ATPG approach.
 *
 * Steps:
 * 1. Fault list generation by using the mapped circuit
 * 2. (Fault list compaction for equivalent faults)
 * 3. For each fault in the fault list check for testability
 *   - Run the steps from the full-scan ATPG workflow
 * 4. Reset the fault statistics of all testable and aborted faults to unclassified
 * 5. Generate a test pattern in one step for each fault
 *    For each fault in the fault list do:
 *   - For an increasing number n of timeframes
 *     - Transform the circuit with n timeframes into a boolean CNF (conjunctive normal form)
 *       - Unroll the circuit over n timeframes
 *       - Tag all relevant circuit parts to be encoded with
 *         GoodTag (good, no fault) and BadTag (bad, fault)
 *       - Encode the tagged circuit parts by using the logic generator
 *     - Solve the CNF formula by using a SAT-solver
 *     - When SAT-result is "SAT" (satisfiable):
 *       - Add the fault to testable fault list
 *       - Extract test pattern from used literals
 *       - Use simulation to check if the test pattern detects other faults
 *         - Simulate test pattern on circuit without fault
 *         - For each remaining fault:
 *           - Simulate test pattern on circuit with fault
 *           - When result is different than the fault-free result
 *             - Add the fault to testable fault list
 *     - When SAT-result is "UNSAT" (unsatisfiable):
 *       - Extended the timeframe count
 *       - Or set the fault status to aborted (timeout)
 * 6. Extract the propagation patterns from the previously generated test patterns
 * 7. (Generate a test pattern in two steps)
 *    - For each fault in the fault list do the same steps as in step 5
 *      However, the target constraints are set such that the fault is propagated
 *      to any secondary port instead of a primary port.
 *    - If no sensitization pattern could be created set the fault status to untestable
 *    - If a sensitization pattern was generated extract the end state
 *      of the sensitization pattern via simulation
 *    - Search the propagation cache for a valid propagation pattern
 *      - For each propagation pattern in the cache combine the sensitization pattern
 *        and propagation pattern and test the resulting test pattern with a simulation
 *      - If the pattern is a valid test pattern then set the fault to testable
 *    - If no propagation pattern in the cache exists a new one is generated
 *      - Do the same steps as in step 5
 *        However, the initial condition is specified as the end condition of the
 *        sensitization pattern.
 *        The target constraints are set such that the fault effect is propagated to
 *        a primary output port.
 *    - If no propagation pattern could be created then the fault is aborted (timeout).
 *    - If a propagation pattern could be created both patterns (sensitization and propagation)
 *      are combined
 *      - The generated pattern is validated via simulation
 *      - The fault is marked as testable
 * 8. Export statistics
 */
template<typename FaultModel, typename FaultList>
class SatSequentialAtpg:
	public SequentialAtpgBase<FaultModel, FaultList>
{
public:
	SatSequentialAtpg(void);
	virtual ~SatSequentialAtpg(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

protected:
	void GeneratePatternForFault(size_t faultIndex);
	void GeneratePatternWithCacheForFault(size_t faultIndex);

};

};
};
};
