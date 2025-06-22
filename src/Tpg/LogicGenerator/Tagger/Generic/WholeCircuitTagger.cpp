#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
WholeCircuitTagger<PinData, Tag>::WholeCircuitTagger(void) = default;

template<typename PinData, typename Tag>
WholeCircuitTagger<PinData, Tag>::~WholeCircuitTagger(void) = default;

template<typename PinData, typename Tag>
std::string WholeCircuitTagger<PinData, Tag>::GetName(void) const
{
	return "WholeCircuitTagger<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool WholeCircuitTagger<PinData, Tag>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	// Check if we are tagging in the forward direction such that we
	// do not change the tagging that has been modified by the following modules.
	if (direction == TaggingDirection::Forward)
	{
		const auto& circuit = context.GetMappedCircuit();
		auto& timeframe = context.GetTimeframe(timeframeId);
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			timeframe.template AddTag<Tag>(nodeId);
		}
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class WholeCircuitTagger<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
