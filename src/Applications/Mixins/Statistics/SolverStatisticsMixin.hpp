#pragma once

#include "Applications/Mixins/Statistics/StatisticsMixin.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class SolverStatisticsMixin:
	public virtual StatisticsMixin
{
public:
	SolverStatisticsMixin(std::string configPrefix);
	virtual ~SolverStatisticsMixin(void);

protected:
	void ExportStatistics(void);

	Statistic::AverageStatistic timeTseitin;
	Statistic::AverageStatistic timeSolver;
	Statistic::AverageStatistic tseitinClauses;

};

};
};
};
