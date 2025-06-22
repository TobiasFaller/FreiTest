#include "Applications/Mixins/Statistics/FaultStatisticsMixin.hpp"

#include <cstdint>
#include <string>
#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

template<typename FaultList>
FaultStatisticsMixin<FaultList>::FaultStatisticsMixin(std::string configPrefix):
	StatisticsMixin(configPrefix),
	statFaultsUnclassified(),
	statFaultsDetected(),
	statFaultsTimeout(),
	statFaultsUndetectable(),
	statFaultsDeferred(),
	statFaultsAborted(),
	statFaultEfficiency(),
	statFaultCoverage(),
	faultsUnclassified(0u),
	faultsDetected(0u),
	faultsTimeout(0u),
	faultsUndetectable(0u),
	faultsDeferred(0u),
	faultsAborted(0u),
	printStatisticResults(PrintFaultStatisticReport::PrintNothing)
{
	statFaultsUnclassified.SetCollectValues(true);
	statFaultsDetected.SetCollectValues(true);
	statFaultsTimeout.SetCollectValues(true);
	statFaultsUndetectable.SetCollectValues(true);
	statFaultsDeferred.SetCollectValues(true);
	statFaultsAborted.SetCollectValues(true);
	statFaultEfficiency.SetCollectValues(true);
	statFaultCoverage.SetCollectValues(true);
}

template<typename FaultList>
FaultStatisticsMixin<FaultList>::~FaultStatisticsMixin(void) = default;

template<typename FaultList>
bool FaultStatisticsMixin<FaultList>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "PrintFaultStatisticReport", statisticsConfigPrefix))
	{
		return Settings::ParseEnum(value, printStatisticResults, {
			{ "PrintDetail", PrintFaultStatisticReport::PrintDetail },
			{ "PrintSummary", PrintFaultStatisticReport::PrintSummary },
			{ "PrintNothing", PrintFaultStatisticReport::PrintNothing },
		});
	}

	return StatisticsMixin::SetSetting(key, value);
}

template<typename FaultList>
void FaultStatisticsMixin<FaultList>::ResetStatistics(size_t faultsTotal)
{
	faultsUnclassified = faultsTotal;
	faultsDetected = 0u;
	faultsTimeout = 0u;
	faultsUndetectable = 0u;
	faultsDeferred = 0u;
	faultsAborted = 0u;

	SnapshotStatisticsForIteration();
}

template<typename FaultList>
void FaultStatisticsMixin<FaultList>::SnapshotStatisticsForIteration(void)
{
	const size_t faultsTotal = faultsDetected + faultsUndetectable + faultsUnclassified;

	statFaultsUnclassified.AddValue(faultsUnclassified);
	statFaultsDetected.AddValue(faultsDetected);
	statFaultsTimeout.AddValue(faultsTimeout);
	statFaultsUndetectable.AddValue(faultsUndetectable);
	statFaultsDeferred.AddValue(faultsDeferred);
	statFaultsAborted.AddValue(faultsAborted);
	statFaultEfficiency.AddValue(100.0 * (faultsDetected + faultsUndetectable) / faultsTotal);
	statFaultCoverage.AddValue(100.0 * faultsDetected / faultsTotal);
}

template<typename FaultList>
void FaultStatisticsMixin<FaultList>::PrintStatistics(std::string additionalInfo)
{
	LOG(INFO) << "Faults Detected: " << faultsDetected << ", Faults Undetectable: " << faultsUndetectable
		<< ", Faults Unclassified: " << faultsUnclassified << " (" << faultsTimeout << " Timeouts, " << faultsAborted << " Aborted, " << faultsDeferred << " Deferred) ";

	std::cout << "\x1b]2;Detected: " << faultsDetected << ", Undetected: " << faultsUndetectable
		<< ", Unclassified: " << faultsUnclassified << " (" << faultsTimeout << " Timeouts, " << faultsAborted << " Aborted, " << faultsDeferred << " Deferred)"
		<< ((additionalInfo != "") ? (" - " + additionalInfo) : "")
		<< "\x07";
}

template<typename FaultList>
void FaultStatisticsMixin<FaultList>::SetFaultStatus(FaultList& faultList, size_t faultIndex, Fault::FaultStatus status, Fault::TargetedFaultStatus targetedStatus)
{
	auto [fault, metaData] = faultList[faultIndex];

	// Undo old status
	switch (metaData->faultStatus)
	{
		case Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED:
			faultsUnclassified--;
			break;
		case Fault::FaultStatus::FAULT_STATUS_DETECTED:
			faultsDetected--;
			break;
		case Fault::FaultStatus::FAULT_STATUS_UNDETECTED:
			faultsUndetectable--;
			break;
		case Fault::FaultStatus::FAULT_STATUS_EXTENDED:
			break;
		default:
			Logging::Panic();
	}
	switch (metaData->targetedFaultStatus)
	{
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED:
		case Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_FUNCTIONAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_STRUCTURAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_EQUIVALENT:
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED:
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_UNREACHABLE:
			faultsDeferred--;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_MAX_ITERATIONS:
			faultsDeferred--;
			faultsAborted--;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_TIMEOUT:
			faultsDeferred--;
			faultsTimeout--;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE:
			faultsTimeout--;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE:
			faultsAborted--;
			break;
	}

	// Do new status
	switch (status)
	{
		case Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED:
			faultsUnclassified++;
			break;
		case Fault::FaultStatus::FAULT_STATUS_DETECTED:
			faultsDetected++;
			break;
		case Fault::FaultStatus::FAULT_STATUS_UNDETECTED:
			faultsUndetectable++;
			break;
		case Fault::FaultStatus::FAULT_STATUS_EXTENDED:
			break;
		default:
			Logging::Panic();
	}
	switch (targetedStatus)
	{
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED:
		case Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_FUNCTIONAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_STRUCTURAL:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_EQUIVALENT:
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED:
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_UNREACHABLE:
			faultsDeferred++;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_MAX_ITERATIONS:
			faultsDeferred++;
			faultsAborted++;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_TIMEOUT:
			faultsDeferred++;
			faultsTimeout++;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE:
			faultsTimeout++;
			break;
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE:
		case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE:
			faultsAborted++;
			break;
		default:
			Logging::Panic();
	}

	// Apply new status
	metaData->faultStatus = status;
	metaData->targetedFaultStatus = targetedStatus;
}

template<typename FaultList>
void FaultStatisticsMixin<FaultList>::ExportStatistics(const FaultList& faultList)
{
	if (printStatisticResults == PrintFaultStatisticReport::PrintDetail
		|| printStatisticResults == PrintFaultStatisticReport::PrintSummary)
	{
		LOG(INFO) << "Total faults: " << faultList.size();
		LOG(INFO) << "    Detected faults: " << faultsDetected;
		LOG(INFO) << "    Undetectable faults: " << faultsUndetectable;
		LOG(INFO) << "    Unclassified faults: " << faultsUnclassified;
		LOG(INFO) << "        Timeout faults: " << faultsTimeout;
		LOG(INFO) << "        Aborted faults: " << faultsAborted;
	}

	size_t faultsUnclassified = 0u;
	size_t faultsDetected = 0u;
	size_t faultsUndetectable = 0u;
	size_t faultIndex = 0u;
	for (const auto [fault, metaData] : faultList)
	{
		switch (metaData->faultStatus)
		{
			case Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED:
			case Fault::FaultStatus::FAULT_STATUS_EXTENDED:
				faultsUnclassified++;
				if (printStatisticResults == PrintFaultStatisticReport::PrintDetail)
				{
					LOG(INFO) << "Fault " << faultIndex << " (" << to_string(*fault) << ") was not generated a test pattern for!";
				}
				break;

			case Fault::FaultStatus::FAULT_STATUS_DETECTED:
				faultsDetected++;
				if (printStatisticResults == PrintFaultStatisticReport::PrintDetail)
				{
					LOG(INFO) << "Fault " << faultIndex << " (" << to_string(*fault) << ") is detected by test pattern " << metaData->detectingPatternId
						<< " at port " << metaData->detectingNode.node->GetName() << " in timeframe " << metaData->detectingTimeframe
						<< " with good / bad difference of " << metaData->detectingOutputGood << "/" << metaData->detectingOutputBad << "!";
				}
				break;

			case Fault::FaultStatus::FAULT_STATUS_UNDETECTED:
				faultsUndetectable++;
				if (printStatisticResults == PrintFaultStatisticReport::PrintDetail)
				{
					LOG(INFO) << "Fault " << faultIndex << " (" << to_string(*fault) << ") is not testable!";
				}
				break;
		}
		faultIndex++;
	}

	statistics.Add("Atpg.Faults.Unclassified", faultsUnclassified, "Fault(s)", "The number of faults that where not correctly targeted and have now an unclassified status");
	statistics.Add("Atpg.Faults.Undetectable", faultsUndetectable, "Fault(s)", "The number of faults that can not be detected because no output pin can show a difference");
	statistics.Add("Atpg.Faults.Detected", faultsDetected, "Fault(s)", "The number of faults that can be detected (validated via simulation)");
	statistics.Add("Atpg.Faults.Timeout", faultsTimeout.load(), "Fault(s)", "The number of faults that could not be tested for due to a timeout");
	statistics.Add("Atpg.Faults.Aborted", faultsAborted.load(), "Fault(s)", "The number of faults that could not be tested for due to reaching the maximum number of timeframes");
	statistics.Add("Atpg.Faults.Efficiency", 100.0 * (faultsDetected + faultsUndetectable) / faultList.size(), "Percent", "The percentage of faults that are proven detectable or undetectable");
	statistics.Add("Atpg.Faults.Coverage", 100.0 * faultsDetected / faultList.size(), "Percent", "The percentage of faults that are detected by a test pattern");

	StatisticsMixin::ExportStatistics("plots/FaultsUnclassified", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Faults",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Faults with Unclassified Status",
			.yAxisMin = 0.0,
			.yAxisMax = static_cast<double>(faultList.size())
		}, statFaultsUnclassified);
	StatisticsMixin::ExportStatistics("plots/FaultsDetected", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Faults",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Faults with Detected Status",
			.yAxisMin = 0.0,
			.yAxisMax = static_cast<double>(faultList.size())
		}, statFaultsDetected);
	StatisticsMixin::ExportStatistics("plots/FaultsTimeout", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Faults",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Faults with Timeout Status",
			.yAxisMin = 0.0,
			.yAxisMax = static_cast<double>(faultList.size())
		}, statFaultsTimeout);
	StatisticsMixin::ExportStatistics("plots/FaultsUndetectable", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Faults",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::Cumulative,
			},
			.title = "Faults with Undetectable Status",
			.yAxisMin = 0.0,
			.yAxisMax = static_cast<double>(faultList.size())
		}, statFaultsUndetectable);
	StatisticsMixin::ExportStatistics("plots/FaultEfficiency", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Fault Efficiency in %",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Fault Efficiency",
			.yAxisMin = 0.0,
			.yAxisMax = 100.0
		}, statFaultEfficiency);
	StatisticsMixin::ExportStatistics("plots/FaultCoverage", {
			{
				.xAxisLabel = "Iteration",
				.yAxisLabel = "Fault Coverage in %",
				.dataIsCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
				.plotCumulative = Io::StatisticsExporter::CumulativeStatistics::NotCumulative,
			},
			.title = "Fault Coverage",
			.yAxisMin = 0.0,
			.yAxisMax = 100.0
		}, statFaultCoverage);
}

template class FaultStatisticsMixin<Fault::SingleStuckAtFaultList>;
template class FaultStatisticsMixin<Fault::SingleTransitionDelayFaultList>;
template class FaultStatisticsMixin<Fault::CellAwareFaultList>;

};
};
};
