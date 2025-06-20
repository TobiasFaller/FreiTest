#pragma once

#include <easylogging++.h>
#include <unistd.h>

#include <cstdlib>
#include <csignal>
#include <string>
#include <vector>

namespace Logging
{

std::vector<std::string> Initialize(std::vector<std::string> arguments);
bool IsExtendedDebugEnabled(void);
void SetExtendedDebugEnabled(bool enabled);
bool IsPauseOnErrorEnabled(void);
void SetPauseOnErrorEnabled(bool enabled);

void ClearCurrentFault(void);
void SetCurrentFault(size_t fault);
size_t GetCurrentFault(void);
void SetFaultLimits(size_t faultStart, size_t faultEnd);

bool IsDebuggerAttached(void);
bool PrintStacktrace(size_t startIndex = 1u);

// Inline so that the debugger doesn't stop in this method
inline __attribute__((always_inline)) void Breakpoint(void);
inline __attribute__((always_inline)) void Panic [[noreturn]] (const std::string& message = "");

enum class Verbose { Yes, No };
class ExtendedWriter;

};

// --------------------------------------------------------------------------------------------------------------------
// easylogging++ customizations
// --------------------------------------------------------------------------------------------------------------------

#define ASSERT(expression) !(expression) && LOG(FATAL) \
	<< "Assertion \"" << #expression << "\" failed in " \
	<< __FILE__ << " line " << __LINE__ << ": "

#define VAR(expression) LOG(DEBUG) \
	<< #expression << " = " << expression << " in "\
	<< __FILE__ << " in Line " << __LINE__

#define VLOG_VERBOSE(expression) (VLOG_IS_ON(expression) ? Logging::Verbose::Yes : Logging::Verbose::No)
#define DVLOG_VERBOSE(expression) (DVLOG_IS_ON(expression) ? Logging::Verbose::Yes : Logging::Verbose::No)

// ----------------------------------------------------------------------------
// custom debug macros
// ----------------------------------------------------------------------------

#ifndef NDEBUG
# define DASSERT(expression) ASSERT(expression)
# define DVAR(expression) __builtin_expect(Logging::IsExtendedDebugEnabled(), false) && VAR(expression)
# define DVLOG_IS_ON(expression) __builtin_expect(Logging::IsExtendedDebugEnabled(), false) && VLOG_IS_ON(expression)
#else // NDEBUG
# define DASSERT(expression) el::base::NullWriter()
# define DVAR(expression) el::base::NullWriter()
# define DVLOG_IS_ON(expression) false
#endif // NDEBUG

#undef DCLOG
#undef DCVLOG
#undef DCLOG_IF
#undef DCVLOG_IF
#undef DCLOG_EVERY_N
#undef DCVLOG_EVERY_N
#undef DCLOG_AFTER_N
#undef DCVLOG_AFTER_N
#undef DCLOG_N_TIMES
#undef DCVLOG_N_TIMES

#ifndef NDEBUG
#define DCLOG(LEVEL, ...) Logging::IsExtendedDebugEnabled() && CLOG(LEVEL, __VA_ARGS__)
#define DCVLOG(vlevel, ...) Logging::IsExtendedDebugEnabled() && CVLOG(vlevel, __VA_ARGS__)
#define DCLOG_IF(condition, LEVEL, ...) Logging::IsExtendedDebugEnabled() && CLOG_IF(condition, LEVEL, __VA_ARGS__)
#define DCVLOG_IF(condition, vlevel, ...) Logging::IsExtendedDebugEnabled() && CVLOG_IF(condition, vlevel, __VA_ARGS__)
#define DCLOG_EVERY_N(n, LEVEL, ...) Logging::IsExtendedDebugEnabled() && CLOG_EVERY_N(n, LEVEL, __VA_ARGS__)
#define DCVLOG_EVERY_N(n, vlevel, ...) Logging::IsExtendedDebugEnabled() && CVLOG_EVERY_N(n, vlevel, __VA_ARGS__)
#define DCLOG_AFTER_N(n, LEVEL, ...) Logging::IsExtendedDebugEnabled() && CLOG_AFTER_N(n, LEVEL, __VA_ARGS__)
#define DCVLOG_AFTER_N(n, vlevel, ...) Logging::IsExtendedDebugEnabled() && CVLOG_AFTER_N(n, vlevel, __VA_ARGS__)
#define DCLOG_N_TIMES(n, LEVEL, ...) Logging::IsExtendedDebugEnabled() && CLOG_N_TIMES(n, LEVEL, __VA_ARGS__)
#define DCVLOG_N_TIMES(n, vlevel, ...) Logging::IsExtendedDebugEnabled() && CVLOG_N_TIMES(n, vlevel, __VA_ARGS__)
#else // NDEBUG
#define DCLOG(LEVEL, ...) el::base::NullWriter()
#define DCVLOG(vlevel, ...) el::base::NullWriter()
#define DCLOG_IF(condition, LEVEL, ...) el::base::NullWriter()
#define DCVLOG_IF(condition, vlevel, ...) el::base::NullWriter()
#define DCLOG_EVERY_N(n, LEVEL, ...) el::base::NullWriter()
#define DCVLOG_EVERY_N(n, vlevel, ...) el::base::NullWriter()
#define DCLOG_AFTER_N(n, LEVEL, ...) el::base::NullWriter()
#define DCVLOG_AFTER_N(n, vlevel, ...) el::base::NullWriter()
#define DCLOG_N_TIMES(n, LEVEL, ...) el::base::NullWriter()
#define DCVLOG_N_TIMES(n, vlevel, ...) el::base::NullWriter()
#endif // NDEBUG

// ----------------------------------------------------------------------------
// Fixes for LOG_IF and VLOG_IF
// ----------------------------------------------------------------------------

// Fix condition chaining for LOG_IF commands
#undef ELPP_WRITE_LOG_IF
#define ELPP_WRITE_LOG_IF(writer, condition_, level, dispatchAction, ...) \
	(condition_) && ELPP_WRITE_LOG(writer, level, dispatchAction, __VA_ARGS__)

// Fix condition chaining in VLOG and VLOG_IF
#undef CVERBOSE
#undef CVERBOSE_IF

#if ELPP_VERBOSE_LOG
#define CVERBOSE(writer, vlevel, dispatchAction, ...) \
	(VLOG_IS_ON(vlevel)) && writer(el::Level::Verbose, __FILE__, __LINE__, ELPP_FUNC, dispatchAction, vlevel) \
  		.construct(el_getVALength(__VA_ARGS__), __VA_ARGS__)
#define CVERBOSE_IF(writer, condition_, vlevel, dispatchAction, ...) \
	(VLOG_IS_ON(vlevel) && (condition_)) && writer(el::Level::Verbose, __FILE__, __LINE__, ELPP_FUNC, dispatchAction, vlevel) \
		.construct(el_getVALength(__VA_ARGS__), __VA_ARGS__)
#else
#define CVERBOSE(writer, vlevel, dispatchAction, ...) el::base::NullWriter()
#define CVERBOSE_IF(writer, condition_, vlevel, dispatchAction, ...) el::base::NullWriter()
#endif  // ELPP_VERBOSE_LOG

// ----------------------------------------------------------------------------
// ERROR and FATAL level breakpoint and stacktrace
// ----------------------------------------------------------------------------

#undef CLOG
#undef CVLOG
#undef CLOG_IF
#undef CVLOG_IF
#undef CLOG_EVERY_N
#undef CVLOG_EVERY_N
#undef CLOG_AFTER_N
#undef CVLOG_AFTER_N
#undef CLOG_N_TIMES
#undef CVLOG_N_TIMES

#define CLOG(LEVEL, ...) C##LEVEL(Logging::ExtendedWriter, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CLOG_IF(condition, LEVEL, ...) C##LEVEL##_IF(Logging::ExtendedWriter, condition, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CLOG_EVERY_N(n, LEVEL, ...) C##LEVEL##_EVERY_N(Logging::ExtendedWriter, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CLOG_AFTER_N(n, LEVEL, ...) C##LEVEL##_AFTER_N(Logging::ExtendedWriter, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CLOG_N_TIMES(n, LEVEL, ...) C##LEVEL##_N_TIMES(Logging::ExtendedWriter, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CVLOG(vlevel, ...) CVERBOSE(Logging::ExtendedWriter, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CVLOG_IF(condition, vlevel, ...) CVERBOSE_IF(Logging::ExtendedWriter, condition, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CVLOG_EVERY_N(n, vlevel, ...) CVERBOSE_EVERY_N(Logging::ExtendedWriter, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CVLOG_AFTER_N(n, vlevel, ...) CVERBOSE_AFTER_N(Logging::ExtendedWriter, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define CVLOG_N_TIMES(n, vlevel, ...) CVERBOSE_N_TIMES(Logging::ExtendedWriter, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)

#define DIRECT_CLOG(LEVEL, ...) C##LEVEL(el::base::Writer, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CLOG_IF(condition, LEVEL, ...) C##LEVEL##_IF(el::base::Writer, condition, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CLOG_EVERY_N(n, LEVEL, ...) C##LEVEL##_EVERY_N(el::base::Writer, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CLOG_AFTER_N(n, LEVEL, ...) C##LEVEL##_AFTER_N(el::base::Writer, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CLOG_N_TIMES(n, LEVEL, ...) C##LEVEL##_N_TIMES(el::base::Writer, n, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CVLOG(vlevel, ...) CVERBOSE(el::base::Writer, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CVLOG_IF(condition, vlevel, ...) CVERBOSE_IF(el::base::Writer, condition, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CVLOG_EVERY_N(n, vlevel, ...) CVERBOSE_EVERY_N(el::base::Writer, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CVLOG_AFTER_N(n, vlevel, ...) CVERBOSE_AFTER_N(el::base::Writer, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)
#define DIRECT_CVLOG_N_TIMES(n, vlevel, ...) CVERBOSE_N_TIMES(el::base::Writer, n, vlevel, el::base::DispatchAction::NormalLog, __VA_ARGS__)

namespace Logging
{

class ExtendedWriter
{
public:
	ExtendedWriter(el::Level level, const char* file, el::base::type::LineNumber line,
			const char* func, el::base::DispatchAction dispatchAction = el::base::DispatchAction::NormalLog,
			el::base::type::VerboseLevel verboseLevel = 0):
		writer(std::make_unique<el::base::Writer>(level, file, line, func, dispatchAction, verboseLevel)),
		level(level),
		file(file),
		line(line)
	{
	}

	ExtendedWriter(el::LogMessage* msg, el::base::DispatchAction dispatchAction = el::base::DispatchAction::NormalLog):
		writer(std::make_unique<el::base::Writer>(msg, dispatchAction)),
		level(msg->level()),
		file(msg->file()),
		line(msg->line())
	{
	}

	virtual ~ExtendedWriter(void)
	{
		// Let the base writer do it's job.
		writer.reset(nullptr);

		if (level == el::Level::Error)
		{
			el::Loggers::flushAll();
			if (Logging::IsPauseOnErrorEnabled())
			{
				Logging::Breakpoint();
			}
		}
		else if (level == el::Level::Fatal)
		{
			DIRECT_CLOG(FATAL, ELPP_CURR_FILE_LOGGER_ID)
				<< "Aborting application. Reason: Fatal log at [" << file << ":" << line << "]";
			Logging::PrintStacktrace(2);

			el::Loggers::flushAll();
			Logging::Breakpoint();

			_exit(EXIT_FAILURE);
		}
	}

	template <typename T>
	inline ExtendedWriter& operator<<(const T& log) {
		*writer << log;
		return *this;
	}

	inline ExtendedWriter& operator<<(std::ostream& (*log)(std::ostream&)) {
		*writer << log;
		return *this;
	}

	inline operator bool() {
		return true;
	}

	inline ExtendedWriter& construct(el::Logger* logger, bool needLock = true)
	{
		writer->construct(logger, needLock);
		return *this;
	}

	inline ExtendedWriter& construct(int count, const char* loggerIds, ...)
	{
		va_list args;
		va_start(args, loggerIds);
		writer->construct(count, loggerIds, args);
		va_end(args);
		return *this;
	}

private:
	std::unique_ptr<el::base::Writer> writer;
	el::Level level;
	std::string file;
	el::base::type::LineNumber line;

};

inline __attribute__((always_inline)) void Breakpoint(void)
{
	if (IsDebuggerAttached())
	{
		__asm__ volatile("int $0x03");
	}
}

inline __attribute__((always_inline)) void Panic(const std::string& message)
{
	DIRECT_CLOG_IF(message != "", FATAL, ELPP_CURR_FILE_LOGGER_ID) << message;
	DIRECT_CLOG(FATAL, ELPP_CURR_FILE_LOGGER_ID)
		<< "Panic function was invoked. The application will now exit!";
	Logging::PrintStacktrace();

	el::Loggers::flushAll();
	Logging::Breakpoint();

	_exit(EXIT_FAILURE);
}

}
