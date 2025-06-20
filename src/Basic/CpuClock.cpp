#include "Basic/CpuClock.hpp"

#include <sys/time.h>

#include <cmath>

#include "Basic/Logging.hpp"

#define usr_sec(a) ((double)((a).ru_utime.tv_sec))
#define sys_sec(a) ((double)((a).ru_stime.tv_sec))
#define usr_mic(a) ((double)((a).ru_utime.tv_usec))
#define sys_mic(a) ((double)((a).ru_stime.tv_usec))

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;

CpuClock::CpuClock():
	_running(false),
	_numberOfMeasurements(0),
	_totalProcessTime(0.0),
	_totalThreadTime(0.0),
	_totalRunTime(0.0)
{
}

CpuClock::~CpuClock() = default;

void CpuClock::SetTimeReference()
{
	_running = true;
	getrusage(RUSAGE_SELF, &_startProcessTime);
	getrusage(RUSAGE_THREAD, &_startThreadTime);
	_startRunTime = high_resolution_clock::now();
}

void CpuClock::Reset()
{
	_totalProcessTime = 0;
	_totalThreadTime = 0;
	_totalRunTime = 0;
}

void CpuClock::Restart()
{
	SetTimeReference();
}

void CpuClock::Stop()
{
	if (_running)
	{
		_totalProcessTime += ProcessTimeSinceReference(TimeUnit::SECONDS);
		_totalThreadTime += ThreadTimeSinceReference(TimeUnit::SECONDS);
		_totalRunTime += RunTimeSinceReference(TimeUnit::SECONDS);
		_numberOfMeasurements++;
	}

	_running = false;
}

double CpuClock::RunTimeSinceReference(TimeUnit unit)
{
	return ConvertRuntime(duration_cast<duration<double>>(high_resolution_clock::now() - _startRunTime).count(), unit);
}

double CpuClock::TotalRunTime(TimeUnit unit)
{
	return ConvertRuntime(_totalRunTime + (_running ? RunTimeSinceReference(TimeUnit::SECONDS) : 0.0), unit);
}

double CpuClock::AverageRunTime(TimeUnit unit)
{
	if (_numberOfMeasurements == 0u)
	{
		return 0.0;
	}
	else
	{
		return ConvertRuntime(_totalRunTime / static_cast<double>(_numberOfMeasurements), unit);
	}
}

double CpuClock::ProcessTimeSinceReference(TimeUnit unit)
{
	struct rusage now;
	getrusage(RUSAGE_SELF, &now);

	return ConvertRuntime(
		(usr_sec(now) - usr_sec(_startProcessTime))
		+ (sys_sec(now) - sys_sec(_startProcessTime))
		+ ((usr_mic(now) - usr_mic(_startProcessTime)) / 1e6)
		+ ((sys_mic(now) - sys_mic(_startProcessTime)) / 1e6),
		unit);
}

double CpuClock::TotalProcessTime(TimeUnit unit)
{
	return ConvertRuntime(_totalProcessTime + (_running ? ProcessTimeSinceReference(TimeUnit::SECONDS) : 0.0), unit);
}

double CpuClock::AverageProcessTime(TimeUnit unit)
{
	return ConvertRuntime(_totalProcessTime / static_cast<double>(std::max(_numberOfMeasurements, static_cast<size_t>(1u))), unit);
}

double CpuClock::ProcessTimeSinceStartOfApp(TimeUnit unit)
{
	struct rusage now;
	getrusage(RUSAGE_SELF, &now);
	return ConvertRuntime(usr_sec(now) + sys_sec(now) + (usr_mic(now) / 1e6) + (sys_mic(now) / 1e6), unit);
}

double CpuClock::ThreadTimeSinceReference(TimeUnit unit)
{
	struct rusage now;
	getrusage(RUSAGE_THREAD, &now);

	return ConvertRuntime(
		(usr_sec(now) - usr_sec(_startThreadTime))
		+ (sys_sec(now) - sys_sec(_startThreadTime))
		+ ((usr_mic(now) - usr_mic(_startThreadTime)) / 1e6)
		+ ((sys_mic(now) - sys_mic(_startThreadTime)) / 1e6),
		unit);
}

double CpuClock::TotalThreadTime(TimeUnit unit)
{
	return ConvertRuntime(_totalThreadTime + (_running ? ThreadTimeSinceReference(TimeUnit::SECONDS) : 0.0), unit);
}

double CpuClock::AverageThreadTime(TimeUnit unit)
{
	return ConvertRuntime(_totalThreadTime / static_cast<double>(std::max(_numberOfMeasurements, static_cast<size_t>(1u))), unit);
}

double CpuClock::ThreadTimeSinceStartOfApp(TimeUnit unit)
{
	struct rusage now;
	getrusage(RUSAGE_THREAD, &now);
	return ConvertRuntime(usr_sec(now) + sys_sec(now) + (usr_mic(now) / 1e6) + (sys_mic(now) / 1e6), unit);
}

double CpuClock::ConvertRuntime(double seconds, TimeUnit targetUnit)
{
	switch(targetUnit)
	{
	case SECONDS: return seconds;
	case MILLISECONDS: return seconds * 1e3;
	case MICROSECONDS: return seconds * 1e6;
	}

	Logging::Panic("Unsupported time unit provided");
}
