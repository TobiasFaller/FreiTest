#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/MultiTransitionDelayFault.hpp"
#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

MultiTransitionDelayFault::MultiTransitionDelayFault(std::vector<TransitionDelayFault> transitionDelays):
	transitionDelays(transitionDelays)
{
}

MultiTransitionDelayFault::~MultiTransitionDelayFault(void) = default;

const std::vector<TransitionDelayFault>& MultiTransitionDelayFault::GetTransitionDelays(void) const
{
	return transitionDelays;
}

std::string to_string(const MultiTransitionDelayFault& fault)
{
	std::string result = "Multi Transition Delay Fault[";
	for (size_t index { 0u }; index < fault.transitionDelays.size(); index++)
	{
		if (index != 0u) result += ", ";
		result += to_string(fault.transitionDelays[index]);
	}
	result += "]";
	return result;
}

};
};
