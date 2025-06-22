#include "Applications/Mixins/Statistics/StatisticsMixin.hpp"

#include "Basic/Settings.hpp"


namespace FreiTest
{
namespace Application
{
namespace Mixin
{

StatisticsMixin::StatisticsMixin(std::string configPrefix):
	statisticsConfigPrefix(configPrefix),
	statisticDataExport(StatisticDataExport::Disabled),
	statisticDataPlot(StatisticDataPlot::Disabled),
	statistics()
{
}

StatisticsMixin::~StatisticsMixin(void) = default;

void StatisticsMixin::Init(void)
{
}

void StatisticsMixin::Run(void)
{
}

Basic::ApplicationStatistics StatisticsMixin::GetStatistics(void)
{
	return std::move(statistics);
}

void StatisticsMixin::ExportStatistics(std::string name, Io::StatisticsExporter::PlotExportOptions exportOptions, Statistic::AverageStatistic& statistics)
{
	if (statisticDataExport != StatisticDataExport::Enabled)
	{
		return;
	}

	Io::StatisticsExporter::ExportAsCsv("[DataExportDirectory]/" + name, exportOptions, statistics);
	if (statisticDataPlot == StatisticDataPlot::Enabled)
	{
		Io::StatisticsExporter::PlotCsv("[DataExportDirectory]/" + name, exportOptions);
	}
}

bool StatisticsMixin::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "StatisticsDataExport", statisticsConfigPrefix))
	{
		return Settings::ParseEnum(value, statisticDataExport, {
			{ "Disabled", StatisticDataExport::Disabled },
			{ "Enabled", StatisticDataExport::Enabled },
		});
	}
	if (Settings::IsOption(key, "StatisticsDataPlot", statisticsConfigPrefix))
	{
		return Settings::ParseEnum(value, statisticDataPlot, {
			{ "Disabled", StatisticDataPlot::Disabled },
			{ "Enabled", StatisticDataPlot::Enabled },
		});
	}

	return false;
}

};
};
};
