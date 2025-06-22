#include "Applications/BaseApplication.hpp"

#include <cassert>
#include <iostream>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"

#include "Applications/Utility/GateLogicOptimization.hpp"
#include "Applications/Circuit/ExportCircuitNodes.hpp"
#include "Applications/Circuit/ExportCircuitHierarchy.hpp"
#include "Applications/Circuit/ExportCircuitStructuralDependence.hpp"
#include "Applications/Circuit/ExportCircuitLogicalDependence.hpp"
#include "Applications/Circuit/ExportCircuitPorts.hpp"
#include "Applications/Circuit/ExportCircuitVerilog.hpp"
#include "Applications/Circuit/ExportCircuitCells.hpp"
#include "Applications/Circuit/ExportCircuitCnf.hpp"


using namespace std;

namespace FreiTest
{
namespace Application
{

BaseApplication::BaseApplication() = default;
BaseApplication::~BaseApplication() = default;

void BaseApplication::PreInit(void)
{
}

void BaseApplication::PostInit(void)
{
}

void BaseApplication::PreRun(void)
{
}

void BaseApplication::PostRun(void)
{
}

bool BaseApplication::SetSetting(string key, string value)
{
	return false;
}

Basic::ApplicationStatistics BaseApplication::GetStatistics(void)
{
	return {};
}

unique_ptr<BaseApplication> BaseApplication::Create(std::string application)
{
	if (application.empty())
	{
		LOG(FATAL) << "No application has been specified!";
		return {};
	}

	if (application == "None")
	{
		LOG(WARNING) << "Explicitly creating no application! Probably setting this configuration option was forgotten.";
		return {};
	}

	if (application == "UTILITY_GATE_LOGIC_OPTIMIZATION")
		return std::make_unique<FreiTest::Application::Utility::GateLogicOptimization>();

	if (application == "CIRCUIT_EXPORT_NODES")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitNodes>();
	if (application == "CIRCUIT_EXPORT_HIERARCHY")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitHierarchy>();
	if (application == "CIRCUIT_EXPORT_PORTS")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitPorts>();
	if (application == "CIRCUIT_EXPORT_VERILOG")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitVerilog>();
	if (application == "CIRCUIT_EXPORT_CELLS")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitCells>();
	if (application == "CIRCUIT_EXPORT_STRUCTURAL_DEPENDENCE")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitStructuralDependence>();
	if (application == "CIRCUIT_EXPORT_LOGICAL_DEPENDENCE")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitLogicalDependence>();
	if (application == "CIRCUIT_EXPORT_CNF")
		return std::make_unique<FreiTest::Application::ExportCircuit::ExportCircuitCnf>();


	LOG(FATAL) << "The application " << application << " was not found!";
	return {};
}

};
};
