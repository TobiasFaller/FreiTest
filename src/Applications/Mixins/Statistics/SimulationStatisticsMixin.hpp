#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include "Applications/Mixins/Statistics/StatisticsMixin.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class SimulationStatisticsMixin:
	public virtual StatisticsMixin
{
public:
	SimulationStatisticsMixin(std::string configPrefix);
	virtual ~SimulationStatisticsMixin(void);

	void ExportStatistics(void);

protected:
	Statistic::AverageStatistic totalSimulationTime;

#ifndef NDEBUG
	Statistic::AverageStatistic simulateForFaultTime;
	Statistic::AverageStatistic initialFaultFreeSimulationTime;
	Statistic::AverageStatistic eventDrivenSimulationTime;
	Statistic::AverageStatistic eventDrivenSimulationIncrementalTime;

	Statistic::AverageStatistic faultsCoveredBySimulationStat;
#endif
};

};
};
};
