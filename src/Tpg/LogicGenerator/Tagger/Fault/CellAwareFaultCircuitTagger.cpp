#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"

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
CellAwareFaultCircuitTagger<PinData>::CellAwareFaultCircuitTagger(std::shared_ptr<Fault::CellAwareFaultModel> faultModel):
	_faultModel(faultModel),
	_taggingMode(CellAwareFaultTaggingMode::Functional)
{
}

template<typename PinData>
CellAwareFaultCircuitTagger<PinData>::~CellAwareFaultCircuitTagger(void) = default;

template<typename PinData>
void CellAwareFaultCircuitTagger<PinData>::SetTaggingMode(CellAwareFaultTaggingMode mode)
{
	_taggingMode = mode;
}

template<typename PinData>
const CellAwareFaultTaggingMode& CellAwareFaultCircuitTagger<PinData>::GetTaggingMode(void) const
{
	return _taggingMode;
}

template<typename PinData>
std::string CellAwareFaultCircuitTagger<PinData>::GetName(void) const
{
	return "CellAwareFaultCircuitTagger";
}

template<typename PinData>
bool CellAwareFaultCircuitTagger<PinData>::TagTimeframe(GeneratorContext<PinData>& context, size_t timeframeId, TaggingDirection direction)
{
	auto const& circuit { context.GetMappedCircuit() };
	auto& timeframe { context.GetTimeframe(timeframeId) };
	auto const& fault { _faultModel->GetFault() };

	if (direction == TaggingDirection::Forward)
	{
		// When this generator runs in incremental mode, then we encode all secondary outputs
		// to enable a defined input for the next timeframe
		if (context.IsIncremental() && _taggingMode == CellAwareFaultTaggingMode::Functional)
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

		if (timeframeId != 0u && _taggingMode == CellAwareFaultTaggingMode::Functional)
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

		std::vector<size_t> faultNodes;
		for (const auto& [node, port] : fault->GetEffectNodesAndPorts())
		{
			timeframe.template AddTag<GoodTag, BadTag, FaultLocationTag>(node->GetNodeId());
			faultNodes.push_back(node->GetNodeId());
		}
		ASSERT(faultNodes.size() > 0) << "No fault effect found to tag";
		timeframe.template TagOutputCone<GoodTag, BadTag, FaultOutputConeTag>(context, faultNodes);
	}

	if (direction == TaggingDirection::Backward)
	{
		if (timeframeId + 1u < context.GetNumberOfTimeframesForTarget(GenerationTarget::CircuitTagging)
			&& _taggingMode == CellAwareFaultTaggingMode::Functional)
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

		// Tag nodes in backward direction from the fault
		std::vector<size_t> faultNodes;
		for (const auto& [node, port] : fault->GetConditionNodesAndPorts())
		{
			timeframe.template AddTag<GoodTag, FaultLocationTag>(node->GetNodeId());
			faultNodes.push_back(node->GetNodeId());
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
	template class CellAwareFaultCircuitTagger<PinDataGB<CONTAINER>>; \
	template class CellAwareFaultCircuitTagger<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
