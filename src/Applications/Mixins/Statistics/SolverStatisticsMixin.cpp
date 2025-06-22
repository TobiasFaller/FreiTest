#include "Applications/Mixins/Statistics/SolverStatisticsMixin.hpp"

#include "Io/Statistics/StatisticsExporter.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

SolverStatisticsMixin::SolverStatisticsMixin(std::string configPrefix):
	StatisticsMixin(configPrefix),
	timeTseitin(),
	timeSolver(),
	tseitinClauses()
{
	timeTseitin.SetCollectValues(true);
	timeSolver.SetCollectValues(true);
	tseitinClauses.SetCollectValues(true);
}

SolverStatisticsMixin::~SolverStatisticsMixin(void) = default;

void SolverStatisticsMixin::ExportStatistics(void)
{
	statistics.Add("Atpg.Tseitin.Runs", timeTseitin.GetCount(), "Time(s)", "The number of calls to GenerateCircuitLogic() on the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.Sum", timeTseitin.GetSum(), "Second(s)", "The total runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.Average", timeTseitin.GetAverageValue(), "Second(s)", "The average runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.Minimal", timeTseitin.GetMinValue(), "Second(s)", "The minimal runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.Maximal", timeTseitin.GetMaxValue(), "Second(s)", "The maximal runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.UpperQuartile", timeTseitin.GetMedian(0.75), "Second(s)", "The upper quartile runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.Median", timeTseitin.GetMedian(0.5), "Second(s)", "The median runtime of the Logic Generator");
	statistics.Add("Atpg.Tseitin.Time.LowerQuartile", timeTseitin.GetMedian(0.25), "Second(s)", "The lower quartile runtime of the Logic Generator");

	StatisticsMixin::ExportStatistics("plots/GraphTseitinTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "CNF Generation Time",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, timeTseitin);
	StatisticsMixin::ExportStatistics("plots/CumulativeGraphTseitinTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative CNF Generation Time",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, timeTseitin);

	statistics.Add("Atpg.Solver.Runs", timeSolver.GetCount(), "Time(s)", "The number of calls to Solve() on the BMC-Solver");
	statistics.Add("Atpg.Solver.Time.Sum", timeSolver.GetSum(), "Second(s)", "The total runtime of the BMC-Solver");
	statistics.Add("Atpg.Solver.Time.Average", timeSolver.GetAverageValue(), "Second(s)", "The average runtime of one BMC-Solver call");
	statistics.Add("Atpg.Solver.Time.Minimal", timeSolver.GetMinValue(), "Second(s)", "The minimal runtime of one BMC-Solver call");
	statistics.Add("Atpg.Solver.Time.Maximal", timeSolver.GetMaxValue(), "Second(s)", "The maximal runtime of one BMC-Solver call");
	statistics.Add("Atpg.Solver.Time.UpperQuartile", timeSolver.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one BMC-Solver call");
	statistics.Add("Atpg.Solver.Time.Median", timeSolver.GetMedian(0.5), "Second(s)", "The median runtime of one BMC-Solver call");
	statistics.Add("Atpg.Solver.Time.LowerQuartile", timeSolver.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one BMC-Solver call");

	StatisticsMixin::ExportStatistics("plots/GraphBmcSolverTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "BMC Solver Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, timeSolver);
	StatisticsMixin::ExportStatistics("plots/CumulativeBmcSolverTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative BMC Solver Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, timeSolver);

	statistics.Add("Atpg.Tseitin.Clauses.Sum", tseitinClauses.GetSum(), "Clause(s)", "The total amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.Average", tseitinClauses.GetAverageValue(), "Clause(s)", "The average amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.Minimal", tseitinClauses.GetMinValue(), "Clause(s)", "The minimal amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.Maximal", tseitinClauses.GetMaxValue(), "Clause(s)", "The maximal amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.UpperQuartile", tseitinClauses.GetMedian(0.75), "Clause(s)", "The upper quartile amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.Median", tseitinClauses.GetMedian(0.5), "Clause(s)", "The median amount of CNF Clauses");
	statistics.Add("Atpg.Tseitin.Clauses.LowerQuartile", tseitinClauses.GetMedian(0.25), "Clause(s)", "The lower quartile amount of CNF Clauses");

	StatisticsMixin::ExportStatistics("plots/GraphTseitinClauses", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "CNF Clauses",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "CNF Clauses",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, tseitinClauses);
	StatisticsMixin::ExportStatistics("plots/CumulativeTseitinClauses", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "CNF Clauses",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative CNF Clauses",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, tseitinClauses);
}

};
};
};
