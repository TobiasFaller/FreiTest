#pragma once

#include <cmath>
#include <string>
#include <vector>

namespace FreiTest
{
namespace Statistic
{

// a simple bin statistic: values are sorted into noBins+2 (extra bin for min and max value)
class BinStatistic {
public:
	BinStatistic();
	virtual ~BinStatistic();

	void setSize(unsigned int noBins, double minVal, double maxVal);

	// add a value (and performs some basic sanity checks)
	void AddValue(double value);

	// returns a string with the number of items in each bin
	std::string ReportValues();

private:
	std::vector<unsigned long> bins;
	double minVal;
	double maxVal;
	double step;

};

};
};
