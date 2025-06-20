#include "SolverProxy/SolverStatistics.hpp"

namespace SolverProxy
{

SolverStatistics::SolverStatistics(void):
	instances(0),
	solves(0),
	sat(0),
	unsat(0),
	unknown(0)
{
}

SolverStatistics::~SolverStatistics(void) = default;

};
