#include "Basic/Statistic/AverageStatistic.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Statistic
{

AverageStatistic::AverageStatistic()
{
	collectValues = false;
	count = 0u;
	valueMin = 0;
	valueMax = 0;
	valueSum = 0;
}

AverageStatistic::~AverageStatistic() = default;

void AverageStatistic::Reset()
{
	count = 0u;
	valueMin = 0;
	valueMax = 0;
	valueSum = 0;
	collectedValues.clear();
}

void AverageStatistic::AddValue(double value)
{
	if (collectValues)
	{
		collectedValues.push_back(value);
	}
	count++;
	valueSum += value;
	if (count == 1 || value < valueMin)
	{
		valueMin = value;
	}
	if (count == 1 || value > valueMax)
	{
		valueMax = value;
	}
}

void AverageStatistic::SetCollectValues(bool newValue)
{
	collectValues = newValue;
}

bool AverageStatistic::IsCollectValues()
{
	return collectValues;
}

double AverageStatistic::GetAverageValue()
{
	if (count > 0)
	{
		return valueSum / count;
	}

	return 0;
}

double AverageStatistic::GetMinValue()
{
	return valueMin;
}

double AverageStatistic::GetMaxValue()
{
	return valueMax;
}

size_t AverageStatistic::GetCount()
{
	return count;
}

double AverageStatistic::GetSum()
{
	return valueSum;
}

double AverageStatistic::GetMedian(double percentage)
{
	if (collectedValues.size() == 0)
	{
		return 0;
	}

	std::vector<double> copy(std::cbegin(collectedValues), std::cend(collectedValues));
	std::sort(std::begin(copy), std::end(copy));

	size_t index;
	if (percentage < 0.5)
	{
		index = std::floor(percentage * copy.size());
	}
	else
	{
		index = std::round(percentage * copy.size());
	}

	return copy[std::min(index, copy.size() - 1u)];
}

const std::vector<double>& AverageStatistic::GetCollectedValues(void)
{
	return collectedValues;
}

double AverageStatistic::GetLastValue(void) const
{
	if (collectedValues.size() == 0u)
	{
		Logging::Panic("No values were collected");
	}

	return collectedValues[collectedValues.size() - 1u];
}

};
};
