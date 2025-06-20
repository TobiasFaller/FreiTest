#pragma once

#include "Applications/BaseApplication.hpp"

namespace FreiTest
{
namespace Application
{
namespace Utility
{

/**
 * @brief This workflow automatically generates C++ code together with the code comments that is used to generate clauses.
 *
 * The generated clauses implement the logic gates that are specified via logic tables in the workflow.
 * The logic tables can contain DON'T CARE values which are assigned a value of LOGIC_ONE or LOGIC_ZERO such that the
 * resulting clauses are minimized in size and number.
 * The simplification of the logic terms (clauses) and assignment of the DON'T CARE values is conducted by a Max-SAT solver.
 * The maximization problem is specified as a CNF that is constructed similar to a Karnaugh-Veitch / symmetry diagram.
 *
 * Currently this generator supports three encoding formats:
 * 1. 01 Encoding with LOGIC_HIGH and LOGIC_LOW
 * 2. 01X Encoding with LOGIC_HIGH, LOGIC_LOW and DON'T CARE
 * 3. U01X Encoding with LOGIC_HIGH, LOGIC_LOW, DON'T CARE and UNKNOWN
 */
class GateLogicOptimization: public BaseApplication
{
public:
	GateLogicOptimization(void);
	virtual ~GateLogicOptimization(void);

	void Init(void) override;
	void Run(void) override;

};

};
};
};
