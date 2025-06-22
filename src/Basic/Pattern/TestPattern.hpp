#pragma once

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Pattern
{

class TestPattern
{
public:
	TestPattern();
	TestPattern(size_t timeframes, size_t primaryInputs, size_t secondaryInputs, Basic::Logic fill = Basic::Logic::LOGIC_DONT_CARE);
	TestPattern(const TestPattern& other) = default;
	virtual ~TestPattern(void);

	void SetPrimaryInput(size_t timeframe, size_t inputNumber, Basic::Logic value);
	void SetSecondaryInput(size_t timeframe, size_t inputNumber, Basic::Logic value);

	Basic::Logic GetPrimaryInput(size_t timeframe, size_t inputNumber) const;
	Basic::Logic GetSecondaryInput(size_t timeframe, size_t inputNumber) const;

	std::vector<Basic::Logic> const& GetPrimaryInputs(size_t timeframe) const;
	std::vector<Basic::Logic> const& GetSecondaryInputs(size_t timeframe) const;
	std::vector<Basic::Logic>& GetPrimaryInputs(size_t timeframe);
	std::vector<Basic::Logic>& GetSecondaryInputs(size_t timeframe);

	size_t GetNumberOfTimeframes(void) const;
	size_t GetNumberOfPrimaryInputs(void) const;
	size_t GetNumberOfSecondaryInputs(void) const;

	bool IsCompatible(const TestPattern& comparePattern) const;
	bool Compact(const TestPattern& compactPattern);
	void Append(const TestPattern& appendedPattern);

	friend std::ostream& operator<<(std::ostream& os, const TestPattern& pattern);

private:
	size_t _primaryInputCount;
	size_t _secondaryInputCount;

	std::vector<std::vector<Basic::Logic>> _primaryInputs;
	std::vector<std::vector<Basic::Logic>> _secondaryInputs;

};

enum class TestPatternFormat {
	PrimaryOnly,
	SecondaryOnly,
	PrimaryAndSecondary
};

std::string to_string(const TestPattern& pattern, TestPatternFormat format = TestPatternFormat::PrimaryAndSecondary);

};
};
