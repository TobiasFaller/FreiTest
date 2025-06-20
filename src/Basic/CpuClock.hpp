#pragma once

#include <sys/resource.h>

#include <chrono>


class CpuClock
{
public:
	enum class TimeUnit
	{
		SECONDS,
		MILLISECONDS,
		MICROSECONDS
	};

	static const TimeUnit SECONDS = TimeUnit::SECONDS;
	static const TimeUnit MILLISECONDS = TimeUnit::MILLISECONDS;
	static const TimeUnit MICROSECONDS = TimeUnit::MICROSECONDS;

	CpuClock(void);
	virtual ~CpuClock(void);

	void SetTimeReference(void);
	void Reset(void);
	void Restart(void);
	void Stop(void);

	double TotalProcessTime(TimeUnit tunit = TimeUnit::SECONDS);
	double AverageProcessTime(TimeUnit tunit = TimeUnit::SECONDS);
	double ProcessTimeSinceReference(TimeUnit tunit = TimeUnit::SECONDS);
	double ProcessTimeSinceStartOfApp(TimeUnit tunit = TimeUnit::SECONDS);

	double TotalThreadTime(TimeUnit tunit = TimeUnit::SECONDS);
	double AverageThreadTime(TimeUnit tunit = TimeUnit::SECONDS);
	double ThreadTimeSinceReference(TimeUnit tunit = TimeUnit::SECONDS);
	double ThreadTimeSinceStartOfApp(TimeUnit tunit = TimeUnit::SECONDS);

	double TotalRunTime(TimeUnit tunit = TimeUnit::SECONDS);
	double AverageRunTime(TimeUnit tunit = TimeUnit::SECONDS);
	double RunTimeSinceReference(TimeUnit tunit = TimeUnit::SECONDS);

private:
	double ConvertRuntime(double second, TimeUnit target = TimeUnit::SECONDS);

	bool _running;
	size_t _numberOfMeasurements;

	struct rusage _startProcessTime;
	struct rusage _startThreadTime;
	std::chrono::high_resolution_clock::time_point _startRunTime;

	double _totalProcessTime;
	double _totalThreadTime;
	double _totalRunTime;

};
