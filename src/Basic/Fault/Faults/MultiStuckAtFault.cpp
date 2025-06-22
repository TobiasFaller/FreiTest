#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/MultiStuckAtFault.hpp"
#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

MultiStuckAtFault::MultiStuckAtFault(std::vector<StuckAtFault> stuckAts):
	stuckAts(stuckAts)
{
}

MultiStuckAtFault::~MultiStuckAtFault(void) = default;

const std::vector<StuckAtFault>& MultiStuckAtFault::GetStuckAts(void) const
{
	return stuckAts;
}

std::string to_string(const MultiStuckAtFault& fault)
{
	std::string result = "Multi Stuck-At Fault[";
	for (size_t index { 0u }; index < fault.stuckAts.size(); index++)
	{
		if (index != 0u) result += ", ";
		result += to_string(fault.stuckAts[index]);
	}
	result += "]";
	return result;
}

};
};
