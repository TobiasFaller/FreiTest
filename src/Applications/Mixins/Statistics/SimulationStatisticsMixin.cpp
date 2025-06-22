#include "Applications/Mixins/Statistics/SimulationStatisticsMixin.hpp"

#include <limits>

#include "Io/Statistics/StatisticsExporter.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

SimulationStatisticsMixin::SimulationStatisticsMixin(std::string configPrefix):
	StatisticsMixin(configPrefix),
	totalSimulationTime()
#ifndef NDEBUG
	,
	simulateForFaultTime(),
	initialFaultFreeSimulationTime(),
	eventDrivenSimulationTime(),
	eventDrivenSimulationIncrementalTime(),
	faultsCoveredBySimulationStat()
#endif
{
	totalSimulationTime.SetCollectValues(true);

#ifndef NDEBUG
	simulateForFaultTime.SetCollectValues(true);
	initialFaultFreeSimulationTime.SetCollectValues(true);
	eventDrivenSimulationTime.SetCollectValues(true);
	eventDrivenSimulationIncrementalTime.SetCollectValues(true);
	faultsCoveredBySimulationStat.SetCollectValues(true);
#endif
}

SimulationStatisticsMixin::~SimulationStatisticsMixin(void) = default;

void SimulationStatisticsMixin::ExportStatistics(void)
{
#ifndef NDEBUG
	statistics.Add("Atpg.Simulation.Total.Runs", totalSimulationTime.GetCount(), "Time(s)", "The number of calls to RunFaultSimulation() for test pattern");
	statistics.Add("Atpg.Simulation.Total.Time.Sum", totalSimulationTime.GetSum(), "Second(s)", "The total runtime of the RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.Average", totalSimulationTime.GetAverageValue(), "Second(s)", "The average runtime of one RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.Minimal", totalSimulationTime.GetMinValue(), "Second(s)", "The minimal runtime of one RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.Maximal", totalSimulationTime.GetMaxValue(), "Second(s)", "The maximal runtime of one RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.UpperQuartile", totalSimulationTime.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.Median", totalSimulationTime.GetMedian(0.5), "Second(s)", "The median runtime of one RunFaultSimulation() call");
	statistics.Add("Atpg.Simulation.Total.Time.LowerQuartile", totalSimulationTime.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one RunFaultSimulation() call");

	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/GraphTotalSimulationTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Total Simulation Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, totalSimulationTime);
	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeTotalSimulationTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative Total Simulation Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, totalSimulationTime);

	statistics.Add("Atpg.Simulation.Fault.Runs", simulateForFaultTime.GetCount(), "Time(s)", "The number of calls to simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.Sum", simulateForFaultTime.GetSum(), "Second(s)", "The total runtime of the simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.Average", simulateForFaultTime.GetAverageValue(), "Second(s)", "The average runtime of one simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.Minimal", simulateForFaultTime.GetMinValue(), "Second(s)", "The minimal runtime of one simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.Maximal", simulateForFaultTime.GetMaxValue(), "Second(s)", "The maximal runtime of one simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.UpperQuartile", simulateForFaultTime.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.Median", simulateForFaultTime.GetMedian(0.5), "Second(s)", "The median runtime of one simulate_for_fault_range");
	statistics.Add("Atpg.Simulation.Fault.Time.LowerQuartile", simulateForFaultTime.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one simulate_for_fault_range");

	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/GraphSimulateForFaultTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Simulate for Fault Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, simulateForFaultTime);
	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeGraphSimulateForFaultTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative Total Simulate for Fault Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, simulateForFaultTime);

	statistics.Add("Atpg.Simulation.InitialFaultFree.Runs", initialFaultFreeSimulationTime.GetCount(), "Time(s)", "The number of calls to initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.Sum", initialFaultFreeSimulationTime.GetSum(), "Second(s)", "The total runtime of the initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.Average", initialFaultFreeSimulationTime.GetAverageValue(), "Second(s)", "The average runtime of one initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.Minimal", initialFaultFreeSimulationTime.GetMinValue(), "Second(s)", "The minimal runtime of one initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.Maximal", initialFaultFreeSimulationTime.GetMaxValue(), "Second(s)", "The maximal runtime of one initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.UpperQuartile", initialFaultFreeSimulationTime.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.Median", initialFaultFreeSimulationTime.GetMedian(0.5), "Second(s)", "The median runtime of one initial fault-free simulation");
	statistics.Add("Atpg.Simulation.InitialFaultFree.Time.LowerQuartile", initialFaultFreeSimulationTime.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one initial fault-free simulation");

	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/GraphInitialFaultFreeSimulationTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Fault-free Simulation Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, initialFaultFreeSimulationTime);
	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeInitialFaultFreeSimulationTime", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative Fault-free Simulation Runtime",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, initialFaultFreeSimulationTime);

	if (eventDrivenSimulationTime.GetCount() > 0)
	{
		statistics.Add("Atpg.Simulation.EventDriven.Runs", eventDrivenSimulationTime.GetCount(), "Time(s)", "The number of calls to event-driven simulation on the Simulator");
		statistics.Add("Atpg.Simulation.EventDriven.Time.Sum", eventDrivenSimulationTime.GetSum(), "Second(s)", "The total runtime of the event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.Average", eventDrivenSimulationTime.GetAverageValue(), "Second(s)", "The average runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.Minimal", eventDrivenSimulationTime.GetMinValue(), "Second(s)", "The minimal runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.Maximal", eventDrivenSimulationTime.GetMaxValue(), "Second(s)", "The maximal runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.UpperQuartile", eventDrivenSimulationTime.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.Median", eventDrivenSimulationTime.GetMedian(0.5), "Second(s)", "The median runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDriven.Time.LowerQuartile", eventDrivenSimulationTime.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one event-driven simulation");

		StatisticsMixin::StatisticsMixin::ExportStatistics("plots/GraphEventDrivenSimulationTime", {
				{
					.xAxisLabel = "Iteration",
					.yAxisLabel = "Time in seconds",
					.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
					.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				},
				.title = "Event-Driven Simulation Runtime",
				.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
				.yAxisMax = std::numeric_limits<double>::quiet_NaN()
			}, eventDrivenSimulationTime);
		StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeEventDrivenSimulationTime", {
				{
					.xAxisLabel = "Iteration",
					.yAxisLabel = "Time in seconds",
					.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
					.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				},
				.title = "Cumulative Event-Driven Simulator Runtime",
				.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
				.yAxisMax = std::numeric_limits<double>::quiet_NaN()
			}, eventDrivenSimulationTime);
	}

	if (eventDrivenSimulationIncrementalTime.GetCount() > 0)
	{
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Runs", eventDrivenSimulationIncrementalTime.GetCount(), "Time(s)", "The number of calls to event-driven simulation on the Simulator");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.Sum", eventDrivenSimulationIncrementalTime.GetSum(), "Second(s)", "The total runtime of the event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.Average", eventDrivenSimulationIncrementalTime.GetAverageValue(), "Second(s)", "The average runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.Minimal", eventDrivenSimulationIncrementalTime.GetMinValue(), "Second(s)", "The minimal runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.Maximal", eventDrivenSimulationIncrementalTime.GetMaxValue(), "Second(s)", "The maximal runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.UpperQuartile", eventDrivenSimulationIncrementalTime.GetMedian(0.75), "Second(s)", "The upper quartile runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.Median", eventDrivenSimulationIncrementalTime.GetMedian(0.5), "Second(s)", "The median runtime of one event-driven simulation");
		statistics.Add("Atpg.Simulation.EventDrivenIncremental.Time.LowerQuartile", eventDrivenSimulationIncrementalTime.GetMedian(0.25), "Second(s)", "The lower quartile runtime of one event-driven simulation");

		StatisticsMixin::StatisticsMixin::ExportStatistics("plots/GraphEventDrivenSimulationIncrementalTime", {
				{
					.xAxisLabel = "Iteration",
					.yAxisLabel = "Time in seconds",
					.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
					.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				},
				.title = "Simulator Runtime",
				.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
				.yAxisMax = std::numeric_limits<double>::quiet_NaN()
			}, eventDrivenSimulationIncrementalTime);
		StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeEventDrivenSimulationIncrementalTime", {
				{
					.xAxisLabel = "Iteration",
					.yAxisLabel = "Time in seconds",
					.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
					.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				},
				.title = "Cumulative Simulator Runtime",
				.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
				.yAxisMax = std::numeric_limits<double>::quiet_NaN()
			}, eventDrivenSimulationIncrementalTime);
	}

	statistics.Add("Atpg.Simulation.CoveredPatterns.Runs", faultsCoveredBySimulationStat.GetCount(), "Time(s)", "The number patterns covered by simulations");

	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/FaultsCoveredBySimulation", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Faults covered by Simulation",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, faultsCoveredBySimulationStat);
	StatisticsMixin::StatisticsMixin::ExportStatistics("plots/CumulativeFaultsCoveredBySimulationStat", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Time in seconds",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Cumulative Faults covered by Simulation",
			.yAxisMin = std::numeric_limits<double>::quiet_NaN(),
			.yAxisMax = std::numeric_limits<double>::quiet_NaN()
		}, faultsCoveredBySimulationStat);
#endif
}

};
};
};
