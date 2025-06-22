#include "Basic/Pattern/TestPattern.hpp"

#include <cstdlib>
#include <fstream>
#include <algorithm>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Pattern
{

TestPattern::TestPattern():
	_primaryInputCount(0u),
	_secondaryInputCount(0u),
	_primaryInputs(0u, std::vector<Basic::Logic> {}),
	_secondaryInputs(0u, std::vector<Basic::Logic> {})
{
}

TestPattern::TestPattern(size_t timeframes, size_t primaryInputs, size_t secondaryInputs, Logic fill):
	_primaryInputCount(primaryInputs),
	_secondaryInputCount(secondaryInputs),
	_primaryInputs(timeframes, std::vector<Logic>(primaryInputs, fill)),
	_secondaryInputs(timeframes, std::vector<Logic>(secondaryInputs, fill))
{
}

TestPattern::~TestPattern(void) = default;

void TestPattern::SetPrimaryInput(size_t timeframe, size_t inputNumber, Logic value)
{
	ASSERT(_primaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	ASSERT(_primaryInputs[timeframe].size() > inputNumber) << "Timeframe does not include inputNumber " << std::to_string(inputNumber);
	_primaryInputs[timeframe][inputNumber] = value;
}

void TestPattern::SetSecondaryInput(size_t timeframe, size_t inputNumber, Logic value)
{
	ASSERT(_secondaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	ASSERT(_secondaryInputs[timeframe].size() > inputNumber) << "Timeframe does not include inputNumber " << std::to_string(inputNumber);
	_secondaryInputs[timeframe][inputNumber] = value;
}

Logic TestPattern::GetPrimaryInput(size_t timeframe, size_t inputNumber) const
{
	ASSERT(_primaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	ASSERT(_primaryInputs[timeframe].size() > inputNumber) << "Timeframe does not include inputNumber " << std::to_string(inputNumber);
	return _primaryInputs[timeframe][inputNumber];
}

Logic TestPattern::GetSecondaryInput(size_t timeframe, size_t inputNumber) const
{
	ASSERT(_secondaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	ASSERT(_secondaryInputs[timeframe].size() > inputNumber) << "Timeframe does not include inputNumber " << std::to_string(inputNumber);
	return _secondaryInputs[timeframe][inputNumber];
}

std::vector<Logic> const& TestPattern::GetPrimaryInputs(size_t timeframe) const
{
	ASSERT(_primaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	return _primaryInputs[timeframe];
}

std::vector<Logic> const& TestPattern::GetSecondaryInputs(size_t timeframe) const
{
	ASSERT(_secondaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	return _secondaryInputs[timeframe];
}

std::vector<Logic>& TestPattern::GetPrimaryInputs(size_t timeframe)
{
	ASSERT(_primaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	return _primaryInputs[timeframe];
}

std::vector<Logic>& TestPattern::GetSecondaryInputs(size_t timeframe)
{
	ASSERT(_secondaryInputs.size() > timeframe) << "Testpattern does not include this timeframe " << std::to_string(timeframe);
	return _secondaryInputs[timeframe];
}

size_t TestPattern::GetNumberOfTimeframes(void) const
{
	return _primaryInputs.size();
}

size_t TestPattern::GetNumberOfPrimaryInputs(void) const
{
	return _primaryInputCount;
}

size_t TestPattern::GetNumberOfSecondaryInputs(void) const
{
	return _secondaryInputCount;
}

bool TestPattern::IsCompatible(const TestPattern& pattern) const
{
	if (GetNumberOfTimeframes() < pattern.GetNumberOfTimeframes())
	{
		return false;
	}
	if (GetNumberOfPrimaryInputs() < pattern.GetNumberOfPrimaryInputs())
	{
		return false;
	}
	if (GetNumberOfSecondaryInputs() < pattern.GetNumberOfSecondaryInputs())
	{
		return false;
	}

	for (size_t timeframe = 0; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
	{
		for (size_t primaryInput = 0; primaryInput < pattern.GetNumberOfPrimaryInputs(); ++primaryInput)
		{
			if (GetPrimaryInput(timeframe, primaryInput) == Logic::LOGIC_ONE
				&& pattern.GetPrimaryInput(timeframe, primaryInput) == Logic::LOGIC_ZERO)
			{
				return false;
			}
			if (GetPrimaryInput(timeframe, primaryInput) == Logic::LOGIC_ZERO
				&& pattern.GetPrimaryInput(timeframe, primaryInput) == Logic::LOGIC_ONE)
			{
				return false;
			}
		}

		for (size_t secondaryInput = 0; secondaryInput < pattern.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			if (GetSecondaryInput(timeframe, secondaryInput) == Logic::LOGIC_ONE
				&& pattern.GetSecondaryInput(timeframe, secondaryInput) == Logic::LOGIC_ZERO)
			{
				return false;
			}
			if (GetSecondaryInput(timeframe, secondaryInput) == Logic::LOGIC_ZERO
				&& pattern.GetSecondaryInput(timeframe, secondaryInput) == Logic::LOGIC_ONE)
			{
				return false;
			}
		}
	}
	return true;
}

bool TestPattern::Compact(const TestPattern& pattern)
{
	if (!IsCompatible(pattern))
	{
		return false;
	}

	for (size_t timeframe = 0; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
	{
		for (size_t primaryInput = 0; primaryInput < pattern.GetNumberOfPrimaryInputs(); ++primaryInput)
		{
			if (_primaryInputs[timeframe][primaryInput] == Logic::LOGIC_DONT_CARE)
			{
				_primaryInputs[timeframe][primaryInput] = pattern.GetPrimaryInput(timeframe, primaryInput);
			}
		}

		for (size_t secondaryInput = 0; secondaryInput < pattern.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			if (_secondaryInputs[timeframe][secondaryInput] == Logic::LOGIC_DONT_CARE)
			{
				_secondaryInputs[timeframe][secondaryInput] = pattern.GetSecondaryInput(timeframe, secondaryInput);
			}
		}
	}
	return true;
}

void TestPattern::Append(const TestPattern& pattern)
{
	ASSERT(_primaryInputCount == pattern._primaryInputCount) << "Patterns have different numbers primary inputs";
	ASSERT(_secondaryInputCount == pattern._secondaryInputCount) << "Patterns have different numbers secondary inputs";

	std::copy(pattern._primaryInputs.begin(), pattern._primaryInputs.end(), std::back_inserter(_primaryInputs));
	std::copy(pattern._secondaryInputs.begin(), pattern._secondaryInputs.end(), std::back_inserter(_secondaryInputs));
}


std::ostream& operator<<(std::ostream& os, const TestPattern& pattern)
{
	return os << to_string(pattern);
}

std::string to_string(const TestPattern& pattern, TestPatternFormat format)
{
	std::string result;
	switch (format)
	{
		case TestPatternFormat::PrimaryOnly: result = "PI: "; break;
		case TestPatternFormat::SecondaryOnly: result = "SI: "; break;
		case TestPatternFormat::PrimaryAndSecondary: result = "PI/SI: "; break;
	}

	for (size_t timeframe = 0u; timeframe < pattern.GetNumberOfTimeframes(); ++timeframe)
	{
		std::string primaryInputs;
		std::string secondaryInputs;

		for (size_t primaryInput = 0u; primaryInput < pattern.GetNumberOfPrimaryInputs(); ++primaryInput)
		{
			primaryInputs += to_string(pattern.GetPrimaryInput(timeframe, primaryInput));
		}
		for (size_t secondaryInput = 0u; secondaryInput < pattern.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			secondaryInputs += to_string(pattern.GetSecondaryInput(timeframe, secondaryInput));
		}

		if (timeframe != 0)
		{
			result += " -> ";
		}

		switch (format)
		{
			case TestPatternFormat::PrimaryOnly:result += primaryInputs; break;
			case TestPatternFormat::SecondaryOnly: result += secondaryInputs; break;
			case TestPatternFormat::PrimaryAndSecondary:
				result += primaryInputs;
				result += "/";
				result += secondaryInputs;
				break;
		}
	}
	return result;
}

};
};
