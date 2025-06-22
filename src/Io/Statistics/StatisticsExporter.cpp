#include "Io/Statistics/StatisticsExporter.hpp"

#include <cstdlib>
#include <string>
#include <fstream>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"
#include "Helper/FileHandle.hpp"

using namespace std;
using namespace FreiTest::Statistic;

namespace FreiTest
{
namespace Io
{

StatisticsExporter::StatisticsExporter(void) = default;
StatisticsExporter::~StatisticsExporter(void) = default;

bool StatisticsExporter::ExportAsCsv(string path, CsvExportOptions exportOptions, AverageStatistic& statistics)
{
	if (!statistics.IsCollectValues())
	{
		LOG(ERROR) << "There was no statistic data collected to be exported";
	}
	FileHandle handle(path + ".csv", false);
	ofstream& out = handle.GetOutStream();
	if (!out.good())
	{
		LOG(ERROR) << "Could not write to file " << path << " (" << handle.GetFilename() << ")";
		return false;
	}

	out << exportOptions.xAxisLabel << ";" << exportOptions.yAxisLabel << "\n";
	if (statistics.GetCount() == 0u)
	{
		return true;
	}

	if ((exportOptions.plotCumulative == CumulativeStatistics::NotCumulative
			&& exportOptions.dataIsCumulative == CumulativeStatistics::NotCumulative)
		|| (exportOptions.plotCumulative == CumulativeStatistics::Cumulative
			&& exportOptions.dataIsCumulative == CumulativeStatistics::Cumulative))
	{
		size_t index = 0u;
		for (double value : statistics.GetCollectedValues())
		{
			out << index++ << ";" << value << "\n";
		}
	}
	else if (exportOptions.plotCumulative == CumulativeStatistics::Cumulative
			&& exportOptions.dataIsCumulative == CumulativeStatistics::NotCumulative)
	{
		out << 0u << ";" << 0.0 << "\n";

		double sum = 0.0;
		size_t index = 0u;
		for (double value : statistics.GetCollectedValues())
		{
			sum += value;
			out << index++ << ";" << sum << "\n";
		}
	}
	else if (exportOptions.plotCumulative == CumulativeStatistics::NotCumulative
			&& exportOptions.dataIsCumulative == CumulativeStatistics::Cumulative)
	{
		auto& values = statistics.GetCollectedValues();
		for (size_t sample = 0u; sample < values.size() - 1u; ++sample)
		{
			double diff = values[sample + 1u] - values[sample];
			out << sample++ << ";" << diff << "\n";
		}
	}
	else
	{
		Logging::Panic("Invalid cumulative configuration");
	}

	return true;
}
bool StatisticsExporter::PlotCsv(string path, PlotExportOptions exportOptions)
{
	{ // Limit the scope of the file handle
		FileHandle handle(path + ".gnuplot", false);

		ofstream& out = handle.GetOutStream();
		if (!out.good())
		{
			LOG(ERROR) << "Could not write to file " << path << " (" << handle.GetFilename() << ")";
			return false;
		}

		out << "set datafile separator ';'\n";
		out << "set decimalsign '.'\n";
		out << "set title '" << exportOptions.title << "'\n";
		out << "set xlabel '" << exportOptions.xAxisLabel << "'\n";
		out << "set ylabel '" << exportOptions.yAxisLabel << "'\n";
		if (!std::isnan(exportOptions.yAxisMin) && !std::isnan(exportOptions.yAxisMax))
		{
			out << "set yrange [" << exportOptions.yAxisMin << ":" << exportOptions.yAxisMax << "]\n";
		}

		out << "set term pdf color size 29.7cm,21cm\n";
		out << "set output '" << Settings::GetInstance()->MapFileName(path + ".pdf", false) << "'\n";
		out << "plot '" << Settings::GetInstance()->MapFileName(path, false) << ".csv' using :2 every ::1 with lines title '" << exportOptions.title << "'\n";

		out << "set term png transparent size 1024,768\n";
		out << "set output '" << Settings::GetInstance()->MapFileName(path + ".png", false) << "'\n";
		out << "plot '" << Settings::GetInstance()->MapFileName(path, false) << ".csv' using :2 every ::1 with lines title '" << exportOptions.title << "'\n";
	}

	string command = "gnuplot " + Settings::GetInstance()->MapFileName(path + ".gnuplot", false);
	LOG(INFO) << "Executing command \"" << command << "\"";

	int statusCode = std::system(command.c_str());
	if (statusCode != 0)
	{
		return false;
	}

	return true;
}

bool StatisticsExporter::ExportAsCsvAndPlot(string path, PlotExportOptions exportOptions, AverageStatistic& statistics)
{
	bool result = ExportAsCsv(path, exportOptions, statistics);
	if (!result)
	{
		return false;
	}

	return PlotCsv(path, exportOptions);
}

};
};
