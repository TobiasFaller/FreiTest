#pragma once

#include <string>
#include <vector>

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Nodes
{

template<typename PinData>
using NodeSelector = std::function<std::vector<size_t>(const GeneratorContext<PinData>& context, size_t timeframe)>;

template<typename PinData, typename Tag>
constexpr NodeSelector<PinData> WithTag()
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();
		const Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);

		std::vector<size_t> nodes;
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			if (timeframe.template HasTag<Tag>(nodeId))
			{
				nodes.push_back(nodeId);
			}
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> PrimaryInputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t primaryInput = 0u; primaryInput < circuit.GetNumberOfPrimaryInputs(); ++primaryInput)
		{
			nodes.push_back(circuit.GetPrimaryInput(primaryInput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> SecondaryInputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			nodes.push_back(circuit.GetSecondaryInput(secondaryInput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> Inputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t primaryInput = 0u; primaryInput < circuit.GetNumberOfPrimaryInputs(); ++primaryInput)
		{
			nodes.push_back(circuit.GetPrimaryInput(primaryInput)->GetNodeId());
		}
		for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			nodes.push_back(circuit.GetSecondaryInput(secondaryInput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> PrimaryOutputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t primaryOutput = 0u; primaryOutput < circuit.GetNumberOfPrimaryOutputs(); ++primaryOutput)
		{
			nodes.push_back(circuit.GetPrimaryOutput(primaryOutput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> SecondaryOutputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t secondaryOutput = 0u; secondaryOutput < circuit.GetNumberOfSecondaryOutputs(); ++secondaryOutput)
		{
			nodes.push_back(circuit.GetSecondaryOutput(secondaryOutput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> Outputs(void)
{
	return [](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		std::vector<size_t> nodes;
		for (size_t primaryOutput = 0u; primaryOutput < circuit.GetNumberOfPrimaryOutputs(); ++primaryOutput)
		{
			nodes.push_back(circuit.GetPrimaryOutput(primaryOutput)->GetNodeId());
		}
		for (size_t secondaryOutput = 0u; secondaryOutput < circuit.GetNumberOfSecondaryOutputs(); ++secondaryOutput)
		{
			nodes.push_back(circuit.GetSecondaryOutput(secondaryOutput)->GetNodeId());
		}
		return nodes;
	};
}

template<typename PinData>
constexpr NodeSelector<PinData> FlipFlopsWithFaultDifference(const std::vector<Basic::Logic>& goodState, const std::vector<Basic::Logic>& badState)
{
	return [&goodState, &badState](const GeneratorContext<PinData>& context, size_t timeframeId) -> std::vector<size_t> {
		const auto& circuit = context.GetMappedCircuit();

		if (timeframeId != 0u)
		{
			return { };
		}

		std::vector<size_t> nodes;
		for (size_t secondaryInput = 0u; secondaryInput < circuit.GetNumberOfSecondaryInputs(); ++secondaryInput)
		{
			const Basic::Logic good = goodState[secondaryInput];
			const Basic::Logic bad = badState[secondaryInput];
			const bool difference = IsValidLogic01(good) && IsValidLogic01(bad) && (bad != good);
			if (difference)
			{
				nodes.push_back(circuit.GetSecondaryInput(secondaryInput)->GetNodeId());
			}
		}
		return nodes;
	};
}

};
};
};
