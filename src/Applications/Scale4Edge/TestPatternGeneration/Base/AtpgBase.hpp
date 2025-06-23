#pragma once

#include <cstdint>
#include <vector>
#include <mutex>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Statistics/FaultStatisticsMixin.hpp"
#include "Applications/Mixins/Statistics/SimulationStatisticsMixin.hpp"
#include "Applications/Mixins/Statistics/SolverStatisticsMixin.hpp"
#include "Applications/Mixins/Vcd/VcdExportMixin.hpp"
#include "Applications/Mixins/Vcm/VcmMixin.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Pattern/TestPatternList.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Basic/Statistic/AverageStatistic.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

const std::string SCALE4EDGE_ATPG_CONFIG = "Scale4Edge/TestPatternGeneration";

template <typename FaultModel>
class AtpgData
{
};

template<>
class AtpgData<Fault::SingleStuckAtFaultModel> {
public:
	AtpgData(void);
	virtual ~AtpgData(void);

	bool SetSetting(std::string key, std::string value);

protected:
	enum class FaultListReduction { Original, RemoveEquivalent };

	FaultListReduction faultListReduction;
};

template<>
class AtpgData<Fault::SingleTransitionDelayFaultModel> {
public:
	AtpgData(void);
	virtual ~AtpgData(void);

	bool SetSetting(std::string key, std::string value);

protected:
	enum class FaultListReduction { Original, RemoveEquivalent };

	FaultListReduction faultListReduction;
};

template<>
class AtpgData<Fault::CellAwareFaultModel> {
public:
	AtpgData(void);
	virtual ~AtpgData(void);

	bool SetSetting(std::string key, std::string value);

protected:
	std::shared_ptr<Io::Udfm::UdfmModel> udfm;
	std::string udfmFile;
};

/**
 * @brief Provides the basic class for most of the stuck-at and cell-aware based workflows.
 *
 * This class is used to store basic properties like the fault list, the generated test patterns,
 * as well as statistics regarding the faults and patterns.
 * Options that can be applied to most of the workflows are handled by this class too.
 */
template <typename FaultModel, typename FaultList>
class AtpgBase:
	public AtpgData<FaultModel>,
	public virtual BaseApplication,
	public virtual Mixin::FaultStatisticsMixin<FaultList>,
	public virtual Mixin::SimulationStatisticsMixin,
	public virtual Mixin::SolverStatisticsMixin,
	public virtual Mixin::VcdExportMixin<FaultList>,
	public virtual Mixin::VcmMixin
{
public:
	AtpgBase(void);
	virtual ~AtpgBase(void);

	void Init(void) override;
	void Run(void) override;
	void PreRun(void) override;
	void PostRun(void) override;

	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

protected:
	using FaultType = typename FaultList::fault_type;
	using FaultMetaData = typename FaultList::metadata_type;

	enum class FaultListSource { FreiTest, File };
	enum class PrintFaultListReport { PrintDetail, PrintSummary, PrintNothing };
	enum class FaultSimulation { Disabled, Enabled };
	enum class TestPatternExport { Disabled, Enabled };
	enum class PrintTestPatternReport { PrintDetail, PrintSummary, PrintNothing };
	enum class SimulateAllFaults { Disabled, Enabled };
	enum class CheckSimulation { Disabled, Enabled };
	enum class CheckSimulationInitialState { Disabled, CheckEqual };
	enum class CheckSimulationInputs { Disabled, CheckEqual };
	enum class CheckSimulationFlipFlops { Disabled, CheckEqual };
	enum class CheckAtpgResult { Disabled, CheckEqual, CheckInitial };
	enum class CheckMaxIterationCovered { Disabled, Enabled };
	enum class IncrementalSimulation { Disabled, Enabled };

	void GenerateFaultList(void);
	template<typename PinData>
	void ValidateAtpgResult(size_t faultIndex, Pattern::TestPattern& pattern, Pattern::OutputCapture capture, Tpg::LogicGenerator<PinData>& logicGenerator, const Simulation::SimulationConfig& simConfig) const;

	void RunFaultSimulation(const Tpg::Vcm::VcmContext& context, size_t faultIndex, size_t patternIndex, Pattern::OutputCapture capture, const Simulation::SimulationConfig& simConfig);
	bool CheckSensitization(const FaultModel& faultModel, const Simulation::SimulationResult& goodResult) const;
	void ExportTestPatterns(Pattern::InputCapture capture) const;
	void ExportFaultList(void) const;

	std::vector<Fault::FaultStatus> CheckCombinationalUntestability(const FaultList& faultList, const std::vector<bool>& faultMask);

	void ResetStatistics(void);
	void SnapshotStatisticsForIteration(void);
	void PrintStatistics(void);
	void ExportStatistics(void);
	std::string DebugFaultLocation(const Simulation::SimulationResult& atpgGoodResult, const Simulation::SimulationResult& atpgBadResult, const Simulation::SimulationResult& simGoodResult, const Simulation::SimulationResult& simBadResult, size_t faultIndex) const;

	FaultListSource faultListSource;
	std::string faultListFile;
	FaultList faultList;
	FaultList fullFaultList;
	std::vector<std::size_t> faultMapping;
	std::string faultListFilter;
	std::string faultListExclude;

	Pattern::TestPatternList testPatterns;

	Statistic::AverageStatistic statPatternsGenerated;

	PrintFaultListReport printFaultListReport;
	FaultSimulation faultSimulation;
	TestPatternExport testPatternExport;
	PrintTestPatternReport printPatternReport;
	SimulateAllFaults simulateAllFaults;
	CheckSimulation checkSimulation;
	CheckSimulationInitialState checkSimulationInitialState;
	CheckSimulationInputs checkSimulationInputs;
	CheckSimulationFlipFlops checkSimulationFlipFlops;
	CheckAtpgResult checkAtpgResult;
	CheckMaxIterationCovered checkMaxIterationCovered;
	IncrementalSimulation incrementalSimulation;

	size_t patternGenerationThreadLimit;
	size_t solverThreadLimit;
	size_t solverTimeout;
	size_t solverUntestabilityTimeout;
	size_t simulationThreadLimit;

	size_t faultListBegin;
	size_t faultListEnd;

	mutable std::mutex parallelMutex;
	mutable std::atomic<size_t> vcdDebugExportId;

};

};
};
};
