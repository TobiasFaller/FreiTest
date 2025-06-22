#pragma once

#include <map>
#include <string>

#include "Applications/BaseApplication.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Simulation/CircuitSimulationResult.hpp"
#include "Tpg/Vcm/VcmContext.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class VcmConfiguration
{
public:
	VcmConfiguration(std::string name);
	VcmConfiguration(std::string name, std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter> parameters, std::vector<std::string> tags);
	virtual ~VcmConfiguration(void);

	const std::string& GetName(void) const;
	const std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter>& GetParameters(void) const;
	const std::vector<std::string>& GetTags(void) const;

private:
	std::string name;
	std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter> parameters;
	std::vector<std::string> tags;

};

class VcmMixin:
	public virtual BaseApplication
{
public:
	VcmMixin(std::string configPrefix);
	virtual ~VcmMixin(void);

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;
	void Run(void) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

	enum class VcmEnable { Enabled, Disabled };

protected:
	void LoadVcmCircuit(void);
	bool SimulateVcmPattern(const Tpg::Vcm::VcmContext& context, Pattern::TestPattern& vcmPattern) const;
	Pattern::TestPattern GetVcmPatternForSimulationResult(const Tpg::Vcm::VcmContext& context, const Simulation::SimulationResult& goodResult, const Simulation::SimulationResult& badResult) const;

	std::shared_ptr<Circuit::CircuitEnvironment> vcmCircuit;
	std::vector<Tpg::Vcm::VcmInput> vcmInputs;
	std::vector<Tpg::Vcm::VcmOutput> vcmOutputs;

	// Vcm configuration options
	VcmEnable vcmEnable;
	std::string vcmBaseDirectory;
	std::string vcmImportFilename;
	std::vector<std::string> vcmLibraryFilenames;
	std::string vcmExportPreprocessedFilename;
	std::string vcmExportProcessedFilename;
	std::string vcmTopLevelModuleName;
	std::vector<std::string> vcmTags;
	std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter> vcmParameters;

	std::string vcmConfiguration;
	std::map<std::string, VcmConfiguration> vcmConfigurations;

private:
	std::string vcmConfigPrefix;
	Basic::ApplicationStatistics vcmStatistics;

};

};
};
};
