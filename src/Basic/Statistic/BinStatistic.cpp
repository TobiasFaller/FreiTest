#include "Basic/Statistic/BinStatistic.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Statistic
{

BinStatistic::BinStatistic():
	bins(),
	minVal(0),
	maxVal(0),
	step(0)
{}
BinStatistic::~BinStatistic() = default;

void BinStatistic::setSize(unsigned int noBins, double minVal, double maxVal) {
	bins.resize(noBins+2, 0);
	this->minVal = minVal;
	this->maxVal = maxVal;
	this->step = ((maxVal - minVal) / noBins);
}

// add a value (and performs some basic sanity checks)
void BinStatistic::AddValue(double value)
{
	ASSERT(value >= minVal) << "BinStatistic value " << value << " is smaller than minimum value " << minVal;
	ASSERT(value <= maxVal) << "BinStatistic value " << value << " is larger than maximum value " << maxVal;

	// find the bin
	int bin = (value - minVal) / step;

	// modify the bin to handle the extra bin for minval
	if (value == minVal) {
		bin = 0;
	}
	else {
		bin++;
	}

	bins[bin]++;
}

// returns a string with the number of items in each bin
std::string BinStatistic::ReportValues() {
	std::string retVal;

	for (auto val: bins) {
		retVal += std::to_string(val) + ", ";
	}

	return retVal;
}

};
};
