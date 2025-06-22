#pragma once

#include "Applications/BaseApplication.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VcdExporter/VcdExporter.hpp"
#include "Io/VcdExporter/VcdModelBuilder.hpp"

#include <atomic>
#include <string>
#include <utility>

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

template<typename FaultList>
class VcdExportMixin: public virtual BaseApplication
{
public:
	VcdExportMixin(std::string configPrefix);
	virtual ~VcdExportMixin(void);

	void Init(void) override;
	void Run(void) override;
	void PreRun(void) override;
	void PostRunVcd(const FaultList& faultList);
	bool SetSetting(std::string key, std::string value) override;

protected:
	enum class VcdExport { Disabled, Enabled };

	struct PatternInfo
	{
		size_t patternId;
		const Pattern::TestPattern& pattern;
	};
	struct FaultInfo
	{
		size_t faultId;
		const typename FaultList::fault_type& fault;
	};
	struct VcdInfo
	{
		size_t vcdExportId;
		std::string fileName;
		std::string header;
	};

	void ExportCircuitForVisualisation(void) const;

	void ExportVcdForGoodSimulation(const PatternInfo& pattern, const Simulation::SimulationResult &goodResult) const;
	void ExportVcdForBadSimulation(const FaultList& faultList, const PatternInfo& pattern, const FaultInfo& faultInfo, const Simulation::SimulationResult &badResult) const;

	void ExportVcdForFaultCoverage(const FaultList& faultList, const std::vector<bool>& coverage) const;
	void ExportVcdForPatternFaultCoverage(const FaultList& faultList, const std::vector<bool>& coverage, const PatternInfo& pattern) const;

	VcdInfo GetFaultCoverageVcdInfo(const Circuit::CircuitEnvironment& circuitEnvironment) const;
	VcdInfo GetPatternFaultCoverageVcdInfo(const Circuit::CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern) const;

	VcdInfo GetGoodSimulationVcdInfo(const Circuit::CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern) const;
	VcdInfo GetBadSimulationVcdInfo(const Circuit::CircuitEnvironment& circuitEnvironment, const PatternInfo& pattern, const FaultInfo& faultInfo) const;

	mutable std::atomic<size_t> exportId;
	Io::Vcd::VcdModelBuilder vcdModelBuilder;

	VcdExport vcdExport;
	std::string vcdExportDirectory;
	size_t vcdExportSamples;
	std::string vcdExportCircuitPath;

private:
	std::string vcdConfigPrefix;

};

};
};
};
