#include "Main.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Statistic/MemoryStatistic.hpp"

#include <cstdlib>
#include <vector>
#include <string>

static std::vector<std::string> GetArguments(int argc, char **argv);

int main(int argc, char **argv)
{
	// Enable breakpoint on error logging for debugging purposes
	Logging::SetPauseOnErrorEnabled(true);

	// Initialize returns all unprocessed arguments
	std::vector<std::string> arguments = Logging::Initialize(GetArguments(argc, argv));
	arguments.insert(arguments.begin(), std::string(argv[0])); // Prepend the program name

	bool success = false;
	{
		// Brackets for limiting the scope of the Main object
		FreiTest::Main application(arguments);

 		application.BeforeRun();
   		success = application.Run();
		application.AfterRun();
	}

	FreiTest::Statistic::PrintDetailedMemoryUsage();

	return success ? 0 : -1;
}

static std::vector<std::string> GetArguments(int argc, char **argv)
{
	std::vector<std::string> arguments;

	for (int arg = 1; arg < argc; ++arg)
	{
		arguments.push_back(argv[arg]);
	}

	return arguments;
}
