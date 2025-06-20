#include "Main.hpp"

#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <vector>

#include "Applications/BaseApplication.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Statistic/MemoryStatistic.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"
#include "Io/VerilogImporter/VerilogConverter.hpp"

namespace FreiTest
{

Main::Main(std::vector<std::string> arguments):
	arguments(arguments),
	statistics(),
	settings(std::make_shared<Settings>()),
	circuit()
{
	// TODO: Remove these calls as soon as everything is migrated
	Settings::SetInstance(settings);
}

void Main::BeforeRun(void)
{
	time_t initTime = time((time_t *) NULL);
	std::string formattedTime = std::string(ctime(&initTime));

	std::string hostname = "undefined";
	if (getenv("HOST")) {
		hostname = getenv("HOST");
	}

	std::string workingDirectory = std::filesystem::current_path().generic_string();
	std::string commandLine;
	for (size_t argument = 0; argument < arguments.size(); ++argument)
	{
		if (commandLine != "")
		{
			commandLine += " ";
		}

		commandLine.append("\"" + arguments[argument] + "\"");
	}

	commandLine = StringHelper::Trim(commandLine);
	formattedTime = StringHelper::Trim(formattedTime);

	LOG(INFO) << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	LOG(INFO) << "FreiTest by University of Freiburg (Build " << __DATE__  << " " << __TIME__ << ")";
	LOG(INFO) << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	LOG(INFO) << "Host: \"" << hostname << "\"";
	LOG(INFO) << "Path: \"" << arguments[0] << "\"";
	LOG(INFO) << "Command line: \"" << commandLine << "\"";
	LOG(INFO) << "Working directory: \"" << workingDirectory << "\"";
	LOG(INFO) << "Start time: \"" << formattedTime << "\"";

	statistics.Add("FreiTest.Host", hostname, "", "The name of the host computer on which the framework was run");
	statistics.Add("FreiTest.Path", arguments[0], "", "The path of the executable");
	statistics.Add("FreiTest.CommandLine", commandLine, "", "The command which was used to run the framework");
	statistics.Add("FreiTest.WorkingDirectory", workingDirectory, "", "The directory which was used to run the framework");
	statistics.Add("FreiTest.StartTime", formattedTime, "", "The time at which the execution was started");

	// TODO: Find a way to replace this copy & paste nonsense
	// ------------------------------------------------------------------------
	// SAT-Solver
	// ------------------------------------------------------------------------
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_SINGLE
	LOG(INFO) << "SAT / Glucose 4.2.1 (Single-Core) support: yes";
	statistics.Add("FreiTest.Solvers.Sat.Glucose421Single", "Yes", "", "SAT / Glucose 4.2.1 (Single-Core) support");
#else
	LOG(INFO) << "SAT / Glucose 4.2.1 (Single-Core) support: no";
	statistics.Add("FreiTest.Solvers.Sat.Glucose421Single", "No", "", "SAT / Glucose 4.2.1 (Single-Core) support");
#endif
#ifdef HAS_SAT_SOLVER_GLUCOSE_421_PARALLEL
	LOG(INFO) << "SAT / Glucose 4.2.1 (Parallel) support: yes";
	statistics.Add("FreiTest.Solvers.Sat.Glucose421Parallel", "Yes", "", "SAT / Glucose 4.2.1 (Parallel) support");
#else
	LOG(INFO) << "SAT / Glucose 4.2.1 (Parallel) support: no";
	statistics.Add("FreiTest.Solvers.Sat.Glucose421Parallel", "No", "", "SAT / Glucose 4.2.1 (Parallel) support");
#endif
#ifdef HAS_SAT_SOLVER_CADICAL
	LOG(INFO) << "SAT / CaDiCaL support: yes";
	statistics.Add("FreiTest.Solvers.Sat.Cadical", "Yes", "", "SAT / CaDiCaL support");
#else
	LOG(INFO) << "SAT / CaDiCaL support: no";
	statistics.Add("FreiTest.Solvers.Sat.Cadical", "No", "", "SAT / CaDiCaL support");
#endif
#ifdef HAS_SAT_SOLVER_MINISAT
	LOG(INFO) << "SAT / MiniSAT support: yes";
	statistics.Add("FreiTest.Solvers.Sat.MiniSat", "Yes", "", "SAT / MiniSAT 2.2.0 support");
#else
	LOG(INFO) << "SAT / MiniSAT support: no";
	statistics.Add("FreiTest.Solvers.Sat.MiniSat", "No", "", "SAT / MiniSAT 2.2.0 support");
#endif
#ifdef HAS_SAT_SOLVER_CRYPTO_MINISAT
	LOG(INFO) << "SAT / Crypto MiniSAT support: yes";
	statistics.Add("FreiTest.Solvers.Sat.CryptoMiniSat", "Yes", "", "SAT / Crypto MiniSAT 5 support");
#else
	LOG(INFO) << "SAT / Crypto MiniSAT support: no";
	statistics.Add("FreiTest.Solvers.Sat.CryptoMiniSat", "No", "", "SAT / Crypto MiniSAT 5 support");
#endif
#ifdef HAS_SAT_SOLVER_IPASIR_DYNAMIC
	LOG(INFO) << "SAT / Ipasir (Dynamic) support: yes";
	statistics.Add("FreiTest.Solvers.Sat.IpasirDynamic", "Yes", "", "SAT / Ipasir (Dynamic) support");
#else
	LOG(INFO) << "SAT / Ipasir (Dynamic) support: no";
	statistics.Add("FreiTest.Solvers.Sat.IpasirDynamic", "No", "", "SAT / Ipasir (Dynamic) support");
#endif
#ifdef HAS_SAT_SOLVER_IPASIR_STATIC
	LOG(INFO) << "SAT / Ipasir (Static) support: yes";
	statistics.Add("FreiTest.Solvers.Sat.IpasirStatic", "Yes", "", "SAT / Ipasir (Static) support");
#else
	LOG(INFO) << "SAT / Ipasir (Static) support: no";
	statistics.Add("FreiTest.Solvers.Sat.IpasirStatic", "No", "", "SAT / Ipasir (Static) support");
#endif
#ifdef HAS_SAT_SOLVER_DEBUG
	LOG(INFO) << "SAT / Debug support: yes";
	statistics.Add("FreiTest.Solvers.Sat.Debug", "Yes", "", "SAT / Debug support");
#else
	LOG(INFO) << "SAT / Debug support: no";
	statistics.Add("FreiTest.Solvers.Sat.Debug", "No", "", "SAT / Debug support");
#endif
#ifdef HAS_SAT_SOLVER_DIMACS
	LOG(INFO) << "SAT / DIMACS (CNF) export support: yes";
	statistics.Add("FreiTest.Solvers.Sat.DimacsExport", "Yes", "", "SAT / DIMACS (CNF) export support");
#else
	LOG(INFO) << "SAT / DIMACS (CNF) export support: no";
	statistics.Add("FreiTest.Solvers.Sat.DimacsExport", "No", "", "SAT / DIMACS (CNF) export support");
#endif

	// ------------------------------------------------------------------------
	// #SAT-Solver
	// ------------------------------------------------------------------------
#ifdef HAS_SAT_SOLVER_DEBUG
	LOG(INFO) << "Count-SAT / Debug support: yes";
	statistics.Add("FreiTest.Solvers.CountSat.Debug", "Yes", "", "Count-SAT / Debug support");
#else
	LOG(INFO) << "Count-SAT / Debug support: no";
	statistics.Add("FreiTest.Solvers.CountSat.Debug", "No", "", "Count-SAT / Debug support");
#endif

	// ------------------------------------------------------------------------
	// Max-SAT-Solver
	// ------------------------------------------------------------------------
#ifdef HAS_MAX_SAT_SOLVER_PACOSE
	LOG(INFO) << "Max-SAT / Pacose support: yes";
	statistics.Add("FreiTest.Solvers.MaxSat.Pacose", "Yes", "", "Max-SAT / Pacose support");
#else
	LOG(INFO) << "Max-SAT / Pacose support: no";
	statistics.Add("FreiTest.Solvers.MaxSat.Pacose", "No", "", "Max-SAT / Pacose support");
#endif
#ifdef HAS_MAX_SAT_SOLVER_INC_BMO_COMPLETE
	LOG(INFO) << "Max-SAT / Incremental OpenWBO (Complete) support: yes";
	statistics.Add("FreiTest.Solvers.MaxSat.OpenWboComplete", "Yes", "", "Max-SAT / Incremental OpenWBO (Complete) support");
#else
	LOG(INFO) << "Max-SAT / Incremental OpenWBO (Complete) support: no";
	statistics.Add("FreiTest.Solvers.MaxSat.OpenWboComplete", "No", "", "Max-SAT / Incremental OpenWBO (Complete) support");
#endif
#ifdef HAS_SAT_SOLVER_DEBUG
	LOG(INFO) << "Max-SAT / Debug support: yes";
	statistics.Add("FreiTest.Solvers.MaxSat.Debug", "Yes", "", "Max-SAT / Debug support");
#else
	LOG(INFO) << "Max-SAT / Debug support: no";
	statistics.Add("FreiTest.Solvers.MaxSat.Debug", "No", "", "Max-SAT / Debug support");
#endif
#ifdef HAS_SAT_SOLVER_DIMACS
	LOG(INFO) << "Max-SAT / WDIMACS (WCNF) export support: yes";
	statistics.Add("FreiTest.Solvers.MaxSat.WdimacsExport", "Yes", "", "Max-SAT / WDIMACS (WCNF) export support");
#else
	LOG(INFO) << "Max-SAT / WDIMACS (WCNF) export support: no";
	statistics.Add("FreiTest.Solvers.MaxSat.WdimacsExport", "No", "", "Max-SAT / WDIMACS (WCNF) export support");
#endif

	// ------------------------------------------------------------------------
	// BMC-Solver
	// ------------------------------------------------------------------------
#ifdef HAS_BMC_SOLVER_NCIP
	LOG(INFO) << "BMC / NCIP support: yes";
	statistics.Add("FreiTest.Solvers.Bmc.Ncip", "Yes", "", "BMC / NCIP (Next Craig Interpolant Prover) support");
#else
	LOG(INFO) << "BMC / NCIP support: no";
	statistics.Add("FreiTest.Solvers.Bmc.Ncip", "No", "", "BMC / NCIP (Next Craig Interpolant Prover) support");
#endif
#ifdef HAS_BMC_SOLVER_DEBUG
	LOG(INFO) << "BMC / Debug support: yes";
	statistics.Add("FreiTest.Solvers.Bmc.Debug", "Yes", "", "BMC / Debug support");
#else
	LOG(INFO) << "BMC / Debug support: no";
	statistics.Add("FreiTest.Solvers.Bmc.Debug", "No", "", "BMC / Debug support");
#endif
#ifdef HAS_BMC_SOLVER_EXPORT_CIP
	LOG(INFO) << "BMC / CIP (Craig interpolant prover) export support: yes";
	statistics.Add("FreiTest.Solvers.Bmc.CipExport", "Yes", "", "BMC / CIP (Craig interpolant prover) export support");
#else
	LOG(INFO) << "BMC / CIP (Craig interpolant prover) export support: no";
	statistics.Add("FreiTest.Solvers.Bmc.CipExport", "No", "", "BMC / CIP (Craig interpolant prover) export support");
#endif


	globalTimer.SetTimeReference();
}

void Main::AfterRun(void)
{
	time_t endTime = time((time_t *) nullptr);
	std::string formattedTime = std::string(ctime(&endTime));
	formattedTime = StringHelper::Trim(formattedTime);
	statistics.Add("FreiTest.EndTime", formattedTime, "", "The time at which execution finished");
	statistics.Add("FreiTest.Runtime", globalTimer.RunTimeSinceReference(), "Second(s)", "The total runtime of the program");

	LOG(INFO) << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
	LOG(INFO) << "FreiTest by University of Freiburg (Build " << __DATE__  << " " << __TIME__ << ")";
	LOG(INFO) << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

	statistics.PrintHumanReadableToStream(std::cout);

	std::string statisticsFileName = Settings::GetInstance()->StatisticsExportFilename;
	if (statisticsFileName != "")
	{
		FileHandle handle(statisticsFileName, false);
		std::ostream& out = handle.GetOutStream();
		if (out.good())
		{
			statistics.PrintJsonToStream(out);
		}
		else
		{
			LOG(ERROR) << "Could not write statistics file to \"" << statisticsFileName << "\"";
		}
	}
}

bool Main::Run(void)
{
	// The first argument is the program name.
	std::vector<std::pair<std::string, std::string>> commandLineOptions;
	for (size_t i = 1; i < arguments.size(); i++)
	{
		const std::string argument = arguments[i];

		// Normal option not handled by the code above
		if (argument.size() >= 2u && argument[0u] == '-')
		{
			LOG_IF(i + 1u >= arguments.size(), FATAL) << "No value has been given for option " << arguments[i];
			commandLineOptions.emplace_back(arguments[i], arguments[i + 1]);
			i++;
			continue;
		}

		LOG(FATAL) << "Unknown option has been passed on command line: \"" << arguments[i] << "\"";
		__builtin_unreachable();
	}

	if (!settings->ParseCommandLineSetting(commandLineOptions))
	{
		LOG(ERROR) << "Failed to apply configuration settings. Exiting.";
		return false;
	}

	LOG(INFO) << "Circuit name: \"" << settings->CircuitName << "\"";
	statistics.Add("Options.CircuitName", settings->CircuitName, "", "The user specified circuit name");

	if (settings->CircuitName.empty())
	{
		LOG(ERROR) << "No circuit name provided!";
		return false;
	}

	Basic::ApplicationStatistics configuration;
	boost::property_tree::ptree settingsList;
	for (auto& option : settings->GetHistory())
	{
		boost::property_tree::ptree settingElement;
		settingElement.add("setting", option.name);
		settingElement.add("value", option.value);
		settingsList.push_back(std::make_pair("", settingElement));
	}
	statistics.Add("Settings", settingsList);

	// Set deterministic Pseudo-Random start parameters
	srand(GenerateRandomSeed(settings->CircuitName));

	// All setting data is collected -> Call the implemented applications using the BaseApplication interfaces
	auto application = Application::BaseApplication::Create(settings->Application);
	if (application)
	{
		LOG(INFO) << "Created application " << settings->Application;
		for (auto& option : settings->GetApplicationSettings())
		{
			if (!application->SetSetting(option.name, option.value) && !option.optional)
			{
				LOG(FATAL) << "Unexpected key : " << option.name << " with value " << option.value;
			}
		}
		LOG(INFO) << "Configured application";
	}
	else
	{
		LOG(FATAL) << "No application has been created!";
	}

	Statistic::PrintDetailedMemoryUsage();

	if (!LoadCircuitFromSettings())
	{
		return false;
	}

	Statistic::PrintDetailedMemoryUsage();

	/*#######################################################################
	 *
	 * Run application
	 *
	 *#####################################################################*/

	if (application)
	{
		CpuClock applicationInitTimer;
		CpuClock applicationRunTimer;

		application->SetCircuit(circuit);

		applicationInitTimer.SetTimeReference();
		application->PreInit();
		application->Init();
		application->PostInit();
		applicationInitTimer.Stop();
		statistics.Add("Application.Runtime.Init", applicationInitTimer.TotalRunTime(),
			"Second(s)", "The time the application was initializing");

		applicationRunTimer.SetTimeReference();
		application->PreRun();
		application->Run();
		application->PostRun();
		applicationRunTimer.Stop();

		statistics.Add("Application.Runtime.Run", applicationRunTimer.TotalRunTime(),
			"Second(s)", "The time the application was running");
		statistics.Add("Application.Runtime.Total", applicationInitTimer.TotalRunTime() + applicationRunTimer.TotalRunTime(),
			"Second(s)", "The total time the application was executing");

		statistics.Merge("Application", application->GetStatistics());
	}

	return true;
}

int Main::GenerateRandomSeed(const std::string &circuitName)
{
	int random_seed = 1235813237;

	for (unsigned int i = 0; i < circuitName.length(); i++)
		random_seed += circuitName[i];

	return random_seed;
}

bool Main::LoadCircuitFromSettings(void)
{
	if(settings->CircuitSourceType == Settings::CircuitSourceType::None)
	{
		LOG(WARNING) << "Not loading any circuit";
		return true;
	}

	LOG(INFO) << "Loading circuit \"" << settings->CircuitName << "\"";
	if(settings->CircuitSourceType == Settings::CircuitSourceType::Verilog)
	{
		Io::Verilog::VerilogConverter verilogConverter;
		auto circuit = verilogConverter.LoadCircuit(settings);
		if (!circuit)
		{
			LOG(ERROR) << "Failed to load circuits!";
			return false;
		}

		LOG(INFO) << "Loaded Verilog circuit";
		this->circuit = std::move(circuit);
	}
	else
	{
		LOG(ERROR) << "Invalid circuit source type";
		return false;
	}

	statistics.Add("Circuit.Name", circuit->GetName(), "", "The name of the loaded circuit");
	statistics.Add("Circuit.Unmapped.Gates", circuit->GetUnmappedCircuit().GetNumberOfNodes(), "Gate(s)", "The number of gates in the unmapped circuit");
	statistics.Add("Circuit.Unmapped.PrimaryInputs", circuit->GetUnmappedCircuit().GetNumberOfPrimaryInputs(), "Input(s)", "The number of input wires in the unmapped circuit");
	statistics.Add("Circuit.Unmapped.PrimaryOutputs", circuit->GetUnmappedCircuit().GetNumberOfPrimaryOutputs(), "Output(s)", "The number of output wires in the unmapped circuit");
	statistics.Add("Circuit.Mapped.Gates", circuit->GetMappedCircuit().GetNumberOfNodes(), "Gate(s)", "The number of gates in the mapped circuit");
	statistics.Add("Circuit.Mapped.PrimaryInputs", circuit->GetMappedCircuit().GetNumberOfPrimaryInputs(), "Input(s)", "The number of input wires in the mapped circuit");
	statistics.Add("Circuit.Mapped.PrimaryOutputs", circuit->GetMappedCircuit().GetNumberOfPrimaryOutputs(), "Output(s)", "The number of output wires in the mapped circuit");
	statistics.Add("Circuit.Mapped.FlipFlops", circuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), "Flip-Flop(s)", "The number of flip-flops in the mapped circuit");

	return true;
}

};
