#pragma once

namespace FreiTest
{
namespace Statistic
{

double GetMemoryUsageWithDifferentNumber(void);
double GetVirtualMemoryUsage(void);
double GetResidentMemoryUsage(void);
void GetMemoryUsage(double& virtualMemory, double& residentMemory);

void PrintDetailedMemoryUsage(void);

};
};
