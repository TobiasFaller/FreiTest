#include "Applications/BaseApplication.hpp"

#include <cassert>
#include <iostream>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"

#include "Applications/Utility/GateLogicOptimization.hpp"


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


	LOG(FATAL) << "The application " << application << " was not found!";
	return {};
}

};
};
