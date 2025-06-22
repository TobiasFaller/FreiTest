#include "Tpg/LogicGenerator/Tagger/Generic/FanOutCircuitTagger.hpp"

#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
FanOutCircuitTagger<PinData, Tag>::FanOutCircuitTagger(Nodes::NodeSelector<PinData> nodes):
	_targetNodes(nodes)
{
}

template<typename PinData, typename Tag>
FanOutCircuitTagger<PinData, Tag>::~FanOutCircuitTagger(void) = default;

template<typename PinData, typename Tag>
std::string FanOutCircuitTagger<PinData, Tag>::GetName(void) const
{
	return "FanOutCircuitTagger";
}

template<typename PinData, typename Tag>
bool FanOutCircuitTagger<PinData, Tag>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	if (direction != TaggingDirection::Forward)
	{
		return true;
	}

	const auto& circuit = context.GetMappedCircuit();
	auto& timeframe = context.GetTimeframe(timeframeId);

	// Tag the selected nodes
	timeframe.template TagOutputCone<Tag>(context, _targetNodes(context, timeframeId));

	if (timeframeId != 0u)
	{
		// Extend the tagging from the secondary outputs of the previous timeframe
		// to reach circuit elements in this timeframe.
		const auto& previousTimeframe = context.GetTimeframe(timeframeId - 1u);
		std::vector<size_t> nodes;

		for (size_t secondaryOutput = 0u; secondaryOutput < circuit.GetNumberOfSecondaryOutputs(); ++secondaryOutput)
		{
			const auto* outputNode = circuit.GetSecondaryOutput(secondaryOutput);
			const auto* inputNode = circuit.GetSecondaryInputForSecondaryOutput(outputNode);

			if (previousTimeframe.template HasTag<Tag>(outputNode->GetNodeId()))
			{
				timeframe.template AddTag<Tag>(inputNode->GetNodeId());
				nodes.push_back(inputNode->GetNodeId());
			}
		}

		timeframe.template TagOutputCone<Tag>(context, std::move(nodes));
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class FanOutCircuitTagger<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
