#pragma once

#include <vector>

#include "Tpg/Container/LogicContainer.hpp"

namespace FreiTest
{
namespace Tpg
{

class UnaryCounter
{
public:
	UnaryCounter(void);
	virtual ~UnaryCounter(void);

	void SetBits(size_t bitCount);
	size_t GetBits(void) const;

	void SetStateForIndex(size_t index, Tpg::LogicContainer01 value);
	Tpg::LogicContainer01 GetStateForIndex(size_t index) const;

	void SetIndicatorForIndex(size_t index, Tpg::LogicContainer01 value);
	Tpg::LogicContainer01 GetIndicatorForIndex(size_t index) const;

private:
	std::vector<Tpg::LogicContainer01> state;
	std::vector<Tpg::LogicContainer01> indicators;

};

};
};
