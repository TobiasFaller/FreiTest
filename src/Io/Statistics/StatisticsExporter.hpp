#pragma once

#include <string>

#include "Basic/Statistic/AverageStatistic.hpp"

namespace FreiTest
{
namespace Io
{

class StatisticsExporter
{
public:
	enum class CumulativeStatistics
	{
		Cumulative,
		NotCumulative
	};

	struct CsvExportOptions
	{
		std::string xAxisLabel;
		std::string yAxisLabel;
		CumulativeStatistics dataIsCumulative;
		CumulativeStatistics plotCumulative;
	};

	struct PlotExportOptions: public CsvExportOptions
	{
		std::string title;
		double yAxisMin;
		double yAxisMax;
	};

	StatisticsExporter(void);
	virtual ~StatisticsExporter(void);

	static bool ExportAsCsv(std::string path, CsvExportOptions exportOptions, Statistic::AverageStatistic& statistics);
	static bool PlotCsv(std::string path, PlotExportOptions exportOptions);
	static bool ExportAsCsvAndPlot(std::string path, PlotExportOptions exportOptions, Statistic::AverageStatistic& statistics);

};

};
};
