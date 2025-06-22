#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
DChainCircuitTagger<PinData>::DChainCircuitTagger(void) = default;

template<typename PinData>
DChainCircuitTagger<PinData>::~DChainCircuitTagger(void) = default;

template<typename PinData>
std::string DChainCircuitTagger<PinData>::GetName(void) const
{
	return "DChainCircuitTagger";
}

template<typename PinData>
bool DChainCircuitTagger<PinData>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	const MappedCircuit& circuit = context.GetMappedCircuit();
	Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);

	for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
	{
		if (timeframe.template HasTag<GoodTag>(nodeId) && timeframe.template HasTag<BadTag>(nodeId))
		{
			timeframe.template AddTag<DiffTag>(nodeId);
		}
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class DChainCircuitTagger<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
