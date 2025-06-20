#pragma once

#include <string>

#include "Basic/CpuClock.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"

namespace SolverProxy
{

class SolverStatistics
{
public:
	SolverStatistics(void);
	virtual ~SolverStatistics(void);

	// Usage
	FreiTest::Statistic::AverageStatistic clauses;
	FreiTest::Statistic::AverageStatistic variables;
	FreiTest::Statistic::AverageStatistic decisions;
	FreiTest::Statistic::AverageStatistic implications;
	int instances;
	int solves;

	// Runtimes
	CpuClock instancePreparation;
	CpuClock instanceReset;
	CpuClock instanceSolve;

	// Results
	int sat;
	int unsat;
	int unknown;
};

};
