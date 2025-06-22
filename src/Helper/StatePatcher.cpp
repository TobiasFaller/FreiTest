#include "Helper/StatePatcher.hpp"

#include "Basic/Logging.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Helper
{

StatePatcher::PatchValue::PatchValue(size_t flipFlop, Logic value):
	flipFlop(flipFlop),
	value(value)
{
}
StatePatcher::PatchValue::~PatchValue(void) = default;

StatePatcher::Patcher::Patcher(std::string name, std::vector<PatchValue> values):
	name(name),
	values(values)
{
}
StatePatcher::Patcher::~Patcher(void) = default;

StatePatcher::StatePatcher(void) = default;
StatePatcher::~StatePatcher(void) = default;

std::shared_ptr<StatePatcher::Patcher> StatePatcher::AddPatcher(std::string name, std::vector<PatchValue> patchValues)
{
	return patchers.emplace_back(std::make_shared<Patcher>(name, patchValues));
}

void StatePatcher::ApplyPatches(std::vector<Logic>& state) const
{
	for (auto& patcher : patchers)
	{
		for (auto& value : patcher->values)
		{
			ASSERT(value.flipFlop < state.size()) << "The selected flip-flop " << value.flipFlop << " for patch " << patcher->name << " does not exist!";
			state[value.flipFlop] = value.value;
		}
	}
}

};
};
