#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace FreiTest
{
namespace Statistic
{

class AverageStatistic
{
public:
	AverageStatistic();
	virtual ~AverageStatistic();

	void Reset();
	void AddValue(double value);
	void SetCollectValues(bool newValue);
	bool IsCollectValues();
	double GetAverageValue();
	double GetMinValue();
	double GetMaxValue();
	size_t GetCount();
	double GetSum();
	double GetMedian(double percentage=0.5);
	const std::vector<double>& GetCollectedValues(void);
	double GetLastValue(void) const;

private:
	bool collectValues;
	size_t count;
	double valueSum;
	double valueMin;
	double valueMax;
	std::vector<double> collectedValues;
};

};
};
