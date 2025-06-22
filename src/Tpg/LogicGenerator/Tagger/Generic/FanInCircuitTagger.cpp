#include "Tpg/LogicGenerator/Tagger/Generic/FanInCircuitTagger.hpp"

#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
FanInCircuitTagger<PinData, Tag>::FanInCircuitTagger(Nodes::NodeSelector<PinData> nodes):
	_targetNodes(nodes)
{
}

template<typename PinData, typename Tag>
FanInCircuitTagger<PinData, Tag>::~FanInCircuitTagger(void) = default;

template<typename PinData, typename Tag>
std::string FanInCircuitTagger<PinData, Tag>::GetName(void) const
{
	return "FanInCircuitTagger";
}

template<typename PinData, typename Tag>
bool FanInCircuitTagger<PinData, Tag>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	if (direction != TaggingDirection::Backward)
	{
		return true;
	}

	const auto& circuit = context.GetMappedCircuit();
	auto& timeframe = context.GetTimeframe(timeframeId);

	// Tag inputs for the given targets
	timeframe.template TagInputCone<Tag>(context, _targetNodes(context, timeframeId));

	// Extend the tagging from the secondary outputs of the previous timeframe
	// to reach circuit elements in this timeframe.
	if (timeframeId + 1u < context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging))
	{
		const auto& previousTimeframe = context.GetTimeframe(timeframeId + 1u);

		std::vector<size_t> nodes;
		for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			const auto* inputNode = circuit.GetSecondaryInput(secondaryInput);
			const auto* outputNode = circuit.GetSecondaryOutputForSecondaryInput(inputNode);

			if (previousTimeframe.template HasTag<Tag>(inputNode->GetNodeId()))
			{
				timeframe.template AddTag<Tag>(outputNode->GetNodeId());
				nodes.push_back(outputNode->GetNodeId());
			}
		}

		timeframe.template TagInputCone<Tag>(context, std::move(nodes));
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class FanInCircuitTagger<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
