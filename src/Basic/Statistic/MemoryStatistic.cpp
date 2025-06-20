#include "Basic/Statistic/MemoryStatistic.hpp"

#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Statistic
{

// According to https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
double GetMemoryUsageWithDifferentNumber(void)
{
	int   value = 0;
	double result = 0.0;

	FILE* file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != nullptr){
		if (strncmp(line, "VmRSS:", 6) == 0){

			value = strlen(line);
			const char* p = line;
			while (*p <'0' || *p > '9') p++;
			line[value-3] = '\0';
			value = atoi(p);
			break;
		}
	}
	fclose(file);
	result =  (double)value * (double)getpagesize() / (1024*1024);
	return result;
}

double GetVirtualMemoryUsage(void)
{
	double virtualMemory = 0.0;
	double residentMemory = 0.0;

	GetMemoryUsage(virtualMemory, residentMemory);

	return virtualMemory;
}

double GetResidentMemoryUsage(void)
{
	double virtualMemory = 0.0;
	double residentMemory = 0.0;

	GetMemoryUsage(virtualMemory, residentMemory);

	return residentMemory;
}

void PrintDetailedMemoryUsage(void)
{
	double virtualMemory = 0.0;
	double residentMemory = 0.0;

	GetMemoryUsage(virtualMemory, residentMemory);

	LOG(INFO) << "Current Memory Usage: virtual " << virtualMemory << " MiB; resident: " << residentMemory << " MiB";
}

void GetMemoryUsage(double& virtualMemory, double& residentMemory)
{
	char  name[256];
	pid_t pd = getpid();
	int   value = 0;

	sprintf(name, "/proc/%d/statm", pd);
	FILE* in = fopen(name, "rb");
	if (in == nullptr) return;

	ASSERT(fscanf(in, "%d", &value) == 1) << "Failed to parse memory statistics from \"/proc\".";
	virtualMemory =  (double)value * (double)getpagesize() / (1024*1024);

	ASSERT(fscanf(in, "%d", &value) == 1) << "Failed to parse memory statistics from \"/proc\".";
	residentMemory =  (double)value * (double)getpagesize() / (1024*1024);

	fclose(in);
}

};
};
