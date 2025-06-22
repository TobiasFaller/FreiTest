#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"

#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
StuckAtFaultCircuitTagger<PinData, FaultModel>::StuckAtFaultCircuitTagger(std::shared_ptr<FaultModel> faultModel):
	_faultModel(faultModel),
	_taggingMode(StuckAtFaultTaggingMode::Functional)
{
}

template<typename PinData, typename FaultModel>
StuckAtFaultCircuitTagger<PinData, FaultModel>::~StuckAtFaultCircuitTagger(void) = default;

template<typename PinData, typename FaultModel>
void StuckAtFaultCircuitTagger<PinData, FaultModel>::SetTaggingMode(StuckAtFaultTaggingMode mode)
{
	_taggingMode = mode;
}

template<typename PinData, typename FaultModel>
const StuckAtFaultTaggingMode& StuckAtFaultCircuitTagger<PinData, FaultModel>::GetTaggingMode(void) const
{
	return _taggingMode;
}

template<typename PinData, typename FaultModel>
std::string StuckAtFaultCircuitTagger<PinData, FaultModel>::GetName(void) const
{
	return "StuckAtFaultCircuitTagger";
}

template<typename PinData, typename FaultModel>
bool StuckAtFaultCircuitTagger<PinData, FaultModel>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	const auto& circuit { context.GetMappedCircuit() };
	auto& timeframe { context.GetTimeframe(timeframeId) };
	auto const& fault { _faultModel->GetFault() };

	if (direction == TaggingDirection::Forward)
	{
		// When this generator runs in incremental mode, then we encode all secondary outputs
		// to enable a defined input for the next timeframe
		if (context.IsIncremental() && _taggingMode == StuckAtFaultTaggingMode::Functional)
		{
			std::vector<size_t> nodes;
			for (size_t secondaryOutput = 0u; secondaryOutput < circuit.GetNumberOfSecondaryOutputs(); ++secondaryOutput)
			{
				const auto* outputNode = circuit.GetSecondaryOutput(secondaryOutput);
				timeframe.template AddTag<GoodTag>(outputNode->GetNodeId());
				nodes.push_back(outputNode->GetNodeId());
			}
			timeframe.template TagInputCone<GoodTag>(context, nodes);
		}

		if (timeframeId != 0u && _taggingMode == StuckAtFaultTaggingMode::Functional)
		{
			// Extend the tag from the secondary outputs of the previous timeframe
			// to reach circuit elements in this timeframe.
			std::vector<size_t> badNodes;
			std::vector<size_t> goodNodes;
			const auto& previousTimeframe = context.GetTimeframe(timeframeId - 1u);
			for (size_t secondaryOutput = 0u; secondaryOutput < circuit.GetNumberOfSecondaryOutputs(); ++secondaryOutput)
			{
				const auto* outputNode = circuit.GetSecondaryOutput(secondaryOutput);
				const auto* inputNode = circuit.GetSecondaryInputForSecondaryOutput(outputNode);
				if (previousTimeframe.template HasTag<GoodTag>(outputNode->GetNodeId()))
				{
					timeframe.template AddTag<GoodTag, FaultSupportOutputConeTag>(inputNode->GetNodeId());
					goodNodes.push_back(inputNode->GetNodeId());
				}
				if (previousTimeframe.template HasTag<BadTag>(outputNode->GetNodeId()))
				{
					timeframe.template AddTag<BadTag, FaultSupportOutputConeTag>(inputNode->GetNodeId());
					badNodes.push_back(inputNode->GetNodeId());
				}
			}
			timeframe.template TagOutputCone<GoodTag, FaultSupportOutputConeTag>(context, goodNodes);
			timeframe.template TagOutputCone<BadTag, FaultSupportOutputConeTag>(context, badNodes);
		}

		// Tag nodes in forward direction from the faults
		std::vector<size_t> faultNodes;
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			auto stuckAt { fault->GetStuckAt() };
			timeframe.template AddTag<GoodTag, BadTag, FaultLocationTag>(stuckAt.GetNode()->GetNodeId());
			faultNodes.push_back(stuckAt.GetNode()->GetNodeId());
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
		{
			for (auto& stuckAt : fault->GetStuckAts())
			{
				timeframe.template AddTag<GoodTag, BadTag, FaultLocationTag>(stuckAt.GetNode()->GetNodeId());
				faultNodes.push_back(stuckAt.GetNode()->GetNodeId());
			}
		}
		timeframe.template TagOutputCone<GoodTag, BadTag, FaultOutputConeTag>(context, faultNodes);
	}

	if (direction == TaggingDirection::Backward)
	{
		if (timeframeId + 1u < context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging)
			&& _taggingMode == StuckAtFaultTaggingMode::Functional)
		{
			// Extend the tag from the secondary outputs of the previous timeframe
			// to reach circuit elements in this timeframe.
			std::vector<size_t> nodes;
			auto& previousTimeframe = context.GetTimeframe(timeframeId + 1u);
			for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
			{
				const auto* inputNode = circuit.GetSecondaryInput(secondaryInput);
				const auto* outputNode = circuit.GetSecondaryOutputForSecondaryInput(inputNode);
				if (previousTimeframe.template HasTag<GoodTag>(inputNode->GetNodeId()))
				{
					timeframe.template AddTag<GoodTag, FaultSupportInputConeTag>(outputNode->GetNodeId());
					nodes.push_back(outputNode->GetNodeId());
				}
			}
			timeframe.template TagInputCone<GoodTag, FaultSupportInputConeTag>(context, nodes);
		}

		// Tag nodes in backward direction from the faults
		std::vector<size_t> faultNodes;
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			auto stuckAt { fault->GetStuckAt() };
			faultNodes.push_back(stuckAt.GetNode()->GetNodeId());
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
		{
			for (auto& stuckAt : fault->GetStuckAts())
			{
				faultNodes.push_back(stuckAt.GetNode()->GetNodeId());
			}
		}
		timeframe.template TagInputCone<GoodTag, FaultInputConeTag>(context, faultNodes);

		// Encode the good input cone of all nodes with good / bad / fault encoding
		std::vector<size_t> nodes;
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			const auto* node = circuit.GetNode(nodeId);
			if (timeframe.template HasTag<FaultOutputConeTag>(node->GetNodeId())
				|| timeframe.template HasTag<FaultSupportOutputConeTag>(node->GetNodeId()))
			{
				nodes.push_back(node->GetNodeId());
			}
		}
		timeframe.template TagInputCone<GoodTag, FaultSupportInputConeTag>(context, nodes);
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) \
	template class StuckAtFaultCircuitTagger<PinDataGB<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtFaultCircuitTagger<PinDataGBD<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtFaultCircuitTagger<PinDataGB<CONTAINER>, Fault::MultiStuckAtFaultModel>; \
	template class StuckAtFaultCircuitTagger<PinDataGBD<CONTAINER>, Fault::MultiStuckAtFaultModel>;
INSTANTIATE_ALL_CONTAINER

};
};
