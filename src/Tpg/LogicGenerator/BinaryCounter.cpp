#include "Tpg/LogicGenerator/BinaryCounter.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Tpg
{

BinaryCounter::BinaryCounter(void) = default;
BinaryCounter::~BinaryCounter(void) = default;

void BinaryCounter::SetBits(size_t bitCount)
{
	bits.resize(bitCount);
	carries.resize(bitCount);
}

size_t BinaryCounter::GetBits(void) const
{
	return bits.size();
}

void BinaryCounter::SetBit(size_t index, LogicContainer01 value)
{
	bits[index] = value;
}

LogicContainer01 BinaryCounter::GetBit(size_t index) const
{
	return bits[index];
}

void BinaryCounter::SetCarry(size_t index, LogicContainer01 value)
{
	carries[index] = value;
}

LogicContainer01 BinaryCounter::GetCarry(size_t index) const
{
	return carries[index];
}

};
};
