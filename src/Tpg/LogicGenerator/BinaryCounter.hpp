#pragma once

#include <vector>

#include "Tpg/Container/LogicContainer.hpp"

namespace FreiTest
{
namespace Tpg
{

class BinaryCounter
{
public:
	BinaryCounter(void);
	virtual ~BinaryCounter(void);

	void SetBits(size_t bitCount);
	size_t GetBits(void) const;

	void SetBit(size_t index, Tpg::LogicContainer01 value);
	Tpg::LogicContainer01 GetBit(size_t index) const;

	void SetCarry(size_t index, Tpg::LogicContainer01 value);
	Tpg::LogicContainer01 GetCarry(size_t index) const;

private:
	std::vector<Tpg::LogicContainer01> bits;
	std::vector<Tpg::LogicContainer01> carries;

};

};
};
