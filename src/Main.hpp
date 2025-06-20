#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Settings.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Basic/CpuClock.hpp"

namespace FreiTest
{

class Main
{
public:
	Main(std::vector<std::string> arguments);

	void BeforeRun(void);
	bool Run(void);
	void AfterRun(void);

private:
	std::vector<std::string> arguments;
	Basic::ApplicationStatistics statistics;
	CpuClock globalTimer;

	std::shared_ptr<Settings> settings;
	std::shared_ptr<Circuit::CircuitEnvironment> circuit;

	void PrintUsage(void);
	bool LoadCircuitFromSettings(void);
	int GenerateRandomSeed(const std::string &circuit_name);
};

};
