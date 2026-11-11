#pragma once

#include "Applications/BaseApplication.hpp"

#include <memory>
#include <string>
#include <vector>

#include "Applications/Mixins/Vcm/VcmMixin.hpp"
#include "Applications/Mixins/InitialStateMixin.hpp"
#include "Basic/EnumStatistics.hpp"
#include "Basic/Container/ConcurrentList.hpp"
#include "Basic/Logic.hpp"
#include "Circuit/MappedNode.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

class StressTarget
{
public:
	StressTarget(std::string name, std::vector<std::string> netNames, std::vector<const Circuit::MappedNode*> nets);
	virtual ~StressTarget(void);

	const std::string& GetName(void) const;
	const std::vector<std::string>& GetNetNames(void) const;
	const std::vector<const Circuit::MappedNode*>& GetNets(void) const;

private:
	std::string name;
	std::vector<std::string> netNames;
	std::vector<const Circuit::MappedNode*> nets;

};

enum class StressStatus { Unknown, Done, Timeout };


std::string to_string(const StressStatus& status);

class StressMetaData
{
public:
	StressMetaData(void);
	StressMetaData(StressStatus status, std::vector<size_t> netTransitions);
	virtual ~StressMetaData(void);

	const StressStatus& GetStatus(void) const;
	void SetStatus(StressStatus status);

	const std::vector<size_t>& GetNetTransitions(void) const;
	void SetNetTransitions(std::vector<size_t> netTransitions);

private:
	StressStatus status;
	std::vector<size_t> netTransitions;

};

class MultiPointStressStimuliGeneration:
	public Mixin::VcmMixin,
	public Mixin::InitialStateMixin
{
public:
	MultiPointStressStimuliGeneration();
	virtual ~MultiPointStressStimuliGeneration();

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;
	void Run(void) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

private:
	enum class NetSampleSource { None, FreiTest, File };
	enum class ExportRiscvStressPrograms { Enabled, Disabled };

	void InitializeStressTargets(void);
	std::vector<std::vector<size_t>> GenerateSwitchCountProbeOrder(size_t targetIndex);
	void GenerateStressPattern(size_t targetIndex);
	void PrintStatistics(void) const;
	void ExportStressResults(void);
	void ExportStatistics(void);

	NetSampleSource netSampleSource;
	std::string netNamesPath;
	size_t netSampleCount;
	size_t netSampleSize;
	std::string netSamplesPath;
	std::string targetedModule;

	std::string moduleWatcherPath;
	std::string modulePrinterPath;
	std::string vcmWatcherPath;
	std::string vcmPrinterPath;
	std::unique_ptr<Io::ValueWatcherImporter> moduleWatcherImporter;
	std::unique_ptr<Io::ValuePrinterImporter> modulePrinterImporter;
	std::unique_ptr<Io::ValueWatcherImporter> vcmWatcherImporter;
	std::unique_ptr<Io::ValuePrinterImporter> vcmPrinterImporter;

	mutable std::mutex mutex;
	ExportRiscvStressPrograms exportRiscvAssembly;
	std::string disassemblerSourceCode;
	std::vector<std::string> disassemblerSources;
	std::string programPrologue;
	std::string programEpilogue;
	Basic::Container::ConcurrentList<StressTarget, StressMetaData> stressTargets;
	Basic::EnumStatistics<StressStatus> stressStatistics;
	Basic::ApplicationStatistics statistics;

};

}
}
}
