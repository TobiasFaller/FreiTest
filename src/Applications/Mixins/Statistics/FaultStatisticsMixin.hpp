#pragma once

#include <atomic>
#include <cstdint>
#include <string>

#include "Applications/Mixins/Statistics/StatisticsMixin.hpp"
#include "Basic/Fault/FaultMetaData.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

template<typename FaultList>
class FaultStatisticsMixin:
	public virtual StatisticsMixin
{
public:
	enum class PrintFaultStatisticReport {
		PrintDetail,
		PrintSummary,
		PrintNothing
	};

	FaultStatisticsMixin(std::string configPrefix);
	virtual ~FaultStatisticsMixin(void);

	bool SetSetting(std::string key, std::string value) override;

	void ResetStatistics(size_t faultsTotal);
	void SnapshotStatisticsForIteration(void);
	void PrintStatistics(std::string additionalInfo);
	void SetFaultStatus(FaultList& faultList, size_t faultIndex, Fault::FaultStatus status, Fault::TargetedFaultStatus targetedStatus);
	void ExportStatistics(const FaultList& faultList);

private:
	Statistic::AverageStatistic statFaultsUnclassified;
	Statistic::AverageStatistic statFaultsDetected;
	Statistic::AverageStatistic statFaultsTimeout;
	Statistic::AverageStatistic statFaultsUndetectable;
	Statistic::AverageStatistic statFaultsDeferred;
	Statistic::AverageStatistic statFaultsAborted;
	Statistic::AverageStatistic statFaultEfficiency;
	Statistic::AverageStatistic statFaultCoverage;

	std::atomic<uint64_t> faultsUnclassified;
	std::atomic<uint64_t> faultsDetected;
	std::atomic<uint64_t> faultsTimeout;
	std::atomic<uint64_t> faultsUndetectable;
	std::atomic<uint64_t> faultsDeferred;
	std::atomic<uint64_t> faultsAborted;

	PrintFaultStatisticReport printStatisticResults;

};

};
};
};
