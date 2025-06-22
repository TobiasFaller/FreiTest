#include "Tpg/LogicGenerator/UnaryCounter.hpp"

#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Tpg
{

UnaryCounter::UnaryCounter(void) = default;
UnaryCounter::~UnaryCounter(void) = default;

void UnaryCounter::SetBits(size_t bitCount)
{
	indicators.resize(bitCount);
	state.resize(bitCount + 1u);
}

size_t UnaryCounter::GetBits(void) const
{
	return indicators.size();
}

void UnaryCounter::SetIndicatorForIndex(size_t index, LogicContainer01 value)
{
	indicators[index] = value;
}

LogicContainer01 UnaryCounter::GetIndicatorForIndex(size_t index) const
{
	return indicators[index];
}

void UnaryCounter::SetStateForIndex(size_t index, LogicContainer01 value)
{
	state[index] = value;
}

LogicContainer01 UnaryCounter::GetStateForIndex(size_t index) const
{
	return state[index];
}

};
};
