#include "Basic/Logging.hpp"

#if __has_include(<execinfo.h>)
# include <execinfo.h>
# define BACKTRACE_AVAILABLE
#endif

#include <unistd.h>
#include <cxxabi.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/format.hpp>

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <regex>
#include <string>
#include <vector>

#include "Helper/StringHelper.hpp"

using namespace std;

INITIALIZE_EASYLOGGINGPP

namespace Logging
{

static std::string GetSignalName(int signal);
static void OnCrash(int signal);

#ifdef __GLIBC__
static void ExitHandler(int status, void* arg);
#else
static void ExitHandler();
#endif

static bool extendedDebug = false;
static bool pauseOnError = false;

// Dynamic logging of fault for current thread
static thread_local size_t _currentFault = 0u;
static size_t _startFault = 0u;
static size_t _endFault = 0u;

// Fast method without locking for debug check
bool IsExtendedDebugEnabled(void)
{
	return extendedDebug;
}
void SetExtendedDebugEnabled(bool enabled)
{
	extendedDebug = enabled;
}

bool IsPauseOnErrorEnabled(void)
{
	return pauseOnError;
}
void SetPauseOnErrorEnabled(bool enabled)
{
	pauseOnError = enabled;
}

void ClearCurrentFault(void)
{
	_currentFault = 0u;
}
void SetCurrentFault(size_t fault)
{
	_currentFault = fault + 1u;
}
size_t GetCurrentFault(void)
{
	return _currentFault;
}
void SetFaultLimits(size_t faultStart, size_t faultEnd)
{
	_startFault = faultStart;
	_endFault = faultEnd;
}

vector<string> Initialize(vector<string> arguments)
{
	uint8_t verboseLevel = 0;
	el::Level logLevel = el::Level::Info;
	string logFile = "";
	string logColor = "dark";
	string vmodules = "";

	vector<string> unprocessed_arguments;
	for (string argument : arguments)
	{
		// Short aliases for verbose log level
		if (argument == "-v" || argument == "--verbose")
		{
			argument = "--log-level=verbose";
		}

		if (StringHelper::StartsWith("--log-level=", argument))
		{
			string level = argument.substr(12);
			if (level == "fatal")
			{
				logLevel = el::Level::Fatal;
			}
			else if (level == "error")
			{
				logLevel = el::Level::Error;
			}
			else if (level == "warn")
			{
				logLevel = el::Level::Warning;
			}
			else if (level == "info")
			{
				logLevel = el::Level::Info;
			}
			else if (level == "verbose")
			{
				if (verboseLevel == 0)
				{
					verboseLevel = 9;
				}
				logLevel = el::Level::Verbose;
			}
			else if (level == "debug")
			{
				logLevel = el::Level::Debug;
			}
			else if (level == "trace")
			{
				logLevel = el::Level::Trace;
			}
			else
			{
				std::cerr << "Invalid log level \"" << level << "\"" << std::endl;
				exit(1);
			}

			continue;
		}

		// After checking for "--verbose"
		if (StringHelper::StartsWith("--log-verbose-level=", argument))
		{
			try
			{
				int level = std::stoi(argument.substr(20));
				if (level < 1 || level > 9)
				{
					std::cerr << "Invalid log level" << std::endl;
					exit(1);
				}

				verboseLevel = level;

				// Increase level to verbose
				if (logLevel == el::Level::Fatal
					|| logLevel == el::Level::Error
					|| logLevel == el::Level::Warning
					|| logLevel == el::Level::Info)
				{
					logLevel = el::Level::Verbose;
				}
			}
			catch (const std::invalid_argument& ex)
			{
				std::cerr << "Invalid verbose log level" << std::endl;
				exit(1);
			}
			continue;
		}

		if (StringHelper::StartsWith("--log-verbose-modules=", argument))
		{
			std::string vmodules = argument.substr(22);
			el::Loggers::setVModules(vmodules.c_str());

			// Increase level to verbose
			if (logLevel == el::Level::Fatal
				|| logLevel == el::Level::Error
				|| logLevel == el::Level::Warning
				|| logLevel == el::Level::Info)
			{
				logLevel = el::Level::Verbose;
			}
			continue;
		}

		if (StringHelper::StartsWith("--log-file=", argument))
		{
			logFile = argument.substr(11);
			continue;
		}

		if (StringHelper::StartsWith("--log-color=", argument))
		{
			logColor = argument.substr(12);
			continue;
		}

		if (StringHelper::StartsWith("--log-debug=", argument))
		{
			extendedDebug = (argument.substr(12) == "extended");
			continue;
		}

		unprocessed_arguments.push_back(argument);
	}

	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cReset", [](auto log){ return "\u001b[0m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cReset2", [](auto log){ return "\u001b[0m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBlack", [](auto log){ return "\u001b[30m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cRed", [](auto log){ return "\u001b[31m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cGreen", [](auto log){ return "\u001b[32m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cYellow", [](auto log){ return "\u001b[33m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBlue", [](auto log){ return "\u001b[34m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cMagenta", [](auto log){ return "\u001b[35m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cCyan", [](auto log){ return "\u001b[36m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cWhite", [](auto log){ return "\u001b[37m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightBlack", [](auto log){ return "\u001b[90m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightRed", [](auto log){ return "\u001b[91m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightGreen", [](auto log){ return "\u001b[92m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightYellow", [](auto log){ return "\u001b[93m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightBlue", [](auto log){ return "\u001b[94m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightMagenta", [](auto log){ return "\u001b[95m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightCyan", [](auto log){ return "\u001b[96m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBrightWhite", [](auto log){ return "\u001b[97m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%cBold", [](auto log){ return "\u001b[1m"; }));
	el::Helpers::installCustomFormatSpecifier(el::CustomFormatSpecifier("%Fault", [logColor](auto log){
		// Index 0 is treated as "inactive" which disables the fault display.
		if (_currentFault == 0u)
		{
			return std::string("");
		}

		// Print fault index like "[Fault  1 / 15] " where the fault index starts at one.
		// The zero in the format string below is not a mistake and fixes the string alignment.
		// The _currentFault is in the range from 1 to _endFault, so print as is.
		std::string faultCount = boost::str(boost::format("%d") % (_endFault - _startFault));
		std::string faultIndex = boost::str(boost::format("%0d") % boost::io::group(std::setw(faultCount.size()), _currentFault - _startFault));
		return std::string((logColor == "dark") ? "\u001b[94m" : ((logColor == "light") ? "\u001b[34m" : ""))
			+ "[Fault " + faultIndex + " / " + faultCount + "]\u001b[0m ";
	}));

	// Apply new configuration
	el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::ToStandardOutput, "true");
	if (logFile == "")
	{
		el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::ToFile, "false");
	}
	else
	{
		el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::ToFile, "true");
		el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Filename, logFile);
	}

	el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Enabled, "false");
	switch (logLevel)
	{
		case el::Level::Trace:
			el::Loggers::reconfigureAllLoggers(el::Level::Trace, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Debug:
			el::Loggers::reconfigureAllLoggers(el::Level::Debug, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Verbose:
			el::Loggers::reconfigureAllLoggers(el::Level::Verbose, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Info:
			el::Loggers::reconfigureAllLoggers(el::Level::Info, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Warning:
			el::Loggers::reconfigureAllLoggers(el::Level::Warning, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Error:
			el::Loggers::reconfigureAllLoggers(el::Level::Error, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		case el::Level::Fatal:
			el::Loggers::reconfigureAllLoggers(el::Level::Fatal, el::ConfigurationType::Enabled, "true");
			// Fall through to next level
		default:
			break;
	}

	if (logLevel == el::Level::Debug || logLevel == el::Level::Trace)
	{
		// Register exit handler function to log exit code
#ifdef __GLIBC__
		::on_exit(&ExitHandler, 0);
#else
		std::atexit(&ExitHandler);
#endif

		// Use color for debug and tracing. Also add the file name to the output.
		if (logColor == "dark")
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cReset2%msg");
			el::Loggers::reconfigureAllLoggers(el::Level::Debug, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cBrightBlue%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Warning, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cBrightYellow%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Error, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cBrightRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Fatal, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cBrightRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Verbose, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cBrightGreen[%fbase %line] %cBrightBlue%msg%cReset2");
		}
		else if (logColor == "light")
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cReset2%msg");
			el::Loggers::reconfigureAllLoggers(el::Level::Debug, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cBlue%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Warning, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cYellow%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Error, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Fatal, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Verbose, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level %thread]%cReset %Fault%cGreen[%fbase %line] %cBlue%msg%cReset2");
		}
		else
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "[%datetime %level %thread] %Fault[%fbase %line] %msg");
		}
	}
	else
	{
		if (logColor == "dark")
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cReset2%msg");
			el::Loggers::reconfigureAllLoggers(el::Level::Debug, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBrightBlue%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Warning, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBrightYellow%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Error, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBrightRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Fatal, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBrightRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Verbose, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBrightBlue%msg%cReset2");
		}
		else if (logColor == "light")
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cReset2%msg");
			el::Loggers::reconfigureAllLoggers(el::Level::Debug, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBlue%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Warning, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cYellow%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Error, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Fatal, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cRed%msg%cReset2");
			el::Loggers::reconfigureAllLoggers(el::Level::Verbose, el::ConfigurationType::Format, "%cBrightBlack[%datetime %level]%cReset %Fault%cBlue%msg%cReset2");
		}
		else
		{
			el::Loggers::reconfigureAllLoggers(el::Level::Global, el::ConfigurationType::Format, "[%datetime %level] %Fault%msg");
		}
	}

	el::Helpers::setCrashHandler(OnCrash);
	el::Loggers::setVerboseLevel(verboseLevel);
	el::Loggers::removeFlag(el::LoggingFlag::AllowVerboseIfModuleNotSpecified);
	el::Loggers::addFlag(el::LoggingFlag::DisableApplicationAbortOnFatalLog);

	// LOG(DEBUG) << "Logging verbose level is " << (int) verboseLevel << "";
	// LOG(DEBUG) << "Logging log level is " << el::LevelHelper::convertToString(logLevel);

	// LOG(DEBUG) << "--------------------------------------------------------------------------------";
	// LOG(TRACE) << "Logging with Trace Level is active!";
	// LOG(DEBUG) << "Logging with Debug Level is active!";
	// VLOG(9) << "Logging with Verbose 9 Level is active!";
	// VLOG(8) << "Logging with Verbose 8 Level is active!";
	// VLOG(7) << "Logging with Verbose 7 Level is active!";
	// VLOG(6) << "Logging with Verbose 6 Level is active!";
	// VLOG(5) << "Logging with Verbose 5 Level is active!";
	// VLOG(4) << "Logging with Verbose 4 Level is active!";
	// VLOG(3) << "Logging with Verbose 3 Level is active!";
	// VLOG(2) << "Logging with Verbose 2 Level is active!";
	// VLOG(1) << "Logging with Verbose 1 Level is active!";
	// LOG(INFO) << "Logging with Info Level is active!";
	// LOG(WARNING) << "Logging with Warning Level is active!";
	// LOG(ERROR) << "Logging with Error Level is active!";
	// LOG(DEBUG) << "Logging with Fatal Level is always active and exits the application!";
	// LOG(DEBUG) << "--------------------------------------------------------------------------------";

	return unprocessed_arguments;
}

/**
 * Quick and dirty method to print a stack trace with demangled method names.
 * All framework stack traces are colored in yellow and the function name is colored in red (if supported).
 * All other stack traces are displayed in the default color white.
 */
bool PrintStacktrace(size_t startIndex)
{
#ifdef BACKTRACE_AVAILABLE
	void *stack_array[1024];
	size_t stack_size;

	stack_size = backtrace(stack_array, 1024);
	if (stack_size > 0)
	{
		regex regex_function_name("^(.*?)\\(([a-zA-Z0-9_]+)((?:\\+0x[0-9a-fA-F]+)?)\\)(.*?)");

		char ** stack_text = backtrace_symbols(stack_array, stack_size);
		for (size_t stack_index = startIndex; stack_index < stack_size; stack_index++)
		{
			string stack_entry = stack_text[stack_index];
			string stack_color = "\033[0m";

			if (size_t position = stack_entry.rfind("/"); position != string::npos)
			{
				stack_entry = stack_entry.substr(position + 1);
				stack_color = "\033[93m";
			}

			// Try to unmangle C++ method names
			smatch match;
			if (regex_match(stack_entry, match, regex_function_name))
			{
				string mangledName = match[2];

				int status = -1;
				char *demangledName = abi::__cxa_demangle(mangledName.c_str(), NULL, NULL, &status);
				if (status == 0)
				{
					// Print C++ method name
					LOG(TRACE) << stack_color << match[1] << "(\033[91m" << demangledName << stack_color << match[3] << ")" << match[4] << "\033[0m";
					free(static_cast<void*>(demangledName));
				}
				else
				{
					// Print C method name
					LOG(TRACE) << stack_color <<  match[1] << "(\033[91m" << mangledName << stack_color << match[3] << ")" << match[4] << "\033[0m";
				}
			}
			else
			{
				// Print unparsable stack entry
				LOG(TRACE) << stack_color << stack_entry << "\033[0m";
			}
		}

		free(static_cast<void *>(stack_text));
	}
#else
	LOG(TRACE) << "No backtrace available on this platform!";
#endif

	return true;
}

static std::string GetSignalName(int signal)
{
	switch (signal)
	{
		case SIGTERM:
			return "SIGTERM, termination request";
		case SIGSEGV:
			return "SIGSEGV, invalid memory access";
		case SIGINT:
			return "SIGINT, external interrupt";
		case SIGKILL:
			return "SIGKILL, invalid instruction";
		case SIGABRT:
			return "SIGABRT, abnormal termination condition";
		case SIGFPE:
			return "SIGFPE, erroneous arithmetic operation";
		default:
			return "Unknown";
	}
}

static void OnCrash(int signal)
{
	DIRECT_CLOG(FATAL, ELPP_CURR_FILE_LOGGER_ID)
		<< "The application has received signal " << signal
		<< " (" << GetSignalName(signal) << "). It will now exit!";
	PrintStacktrace();
	el::Loggers::flushAll();

	Breakpoint();
	el::Helpers::crashAbort(signal);
}

#ifdef __GLIBC__
static void ExitHandler(int status, void* arg)
{
	LOG(INFO) << "Exiting with code " << status << " in:";
	Logging::PrintStacktrace();
	el::Loggers::flushAll();
	if (status != EXIT_SUCCESS) Breakpoint();
}
#else
static void ExitHandler()
{
	LOG(INFO) << "Exiting with unknown code in:";
	Logging::PrintStacktrace();
	el::Loggers::flushAll();
}
#endif

bool IsDebuggerAttached(void)
{
    char buf[4096];

    const int status_fd = ::open("/proc/self/status", O_RDONLY);
    if (status_fd == -1)
        return false;

    const ssize_t num_read = ::read(status_fd, buf, sizeof(buf) - 1);
	::close(status_fd);

    if (num_read <= 0)
        return false;

    buf[num_read] = '\0';
    constexpr char tracerPidString[] = "TracerPid:";
    const auto tracer_pid_ptr = ::strstr(buf, tracerPidString);
    if (!tracer_pid_ptr)
        return false;

    for (const char* characterPtr = tracer_pid_ptr + sizeof(tracerPidString) - 1; characterPtr <= buf + num_read; ++characterPtr)
    {
        if (::isspace(*characterPtr))
            continue;
        else
            return ::isdigit(*characterPtr) != 0 && *characterPtr != '0';
    }

    return true;
}

};
