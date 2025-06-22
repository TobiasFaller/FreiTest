#pragma once

#include "Applications/BaseApplication.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Io/Statistics/StatisticsExporter.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class StatisticsMixin:
	public virtual BaseApplication
{
public:
	StatisticsMixin(std::string configPrefix);
	virtual ~StatisticsMixin(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

protected:
	enum class StatisticDataExport { Disabled, Enabled };
	enum class StatisticDataPlot { Disabled, Enabled };

	void ExportStatistics(std::string name, Io::StatisticsExporter::PlotExportOptions exportOptions, Statistic::AverageStatistic& statistics);

	std::string statisticsConfigPrefix;
	StatisticDataExport statisticDataExport;
	StatisticDataPlot statisticDataPlot;
	Basic::ApplicationStatistics statistics;

};

};
};
};

