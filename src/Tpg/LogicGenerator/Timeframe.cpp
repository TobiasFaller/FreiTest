#include "Tpg/LogicGenerator/Timeframe.hpp"

#include <queue>
#include <type_traits>

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/IContainerInterceptor.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
Timeframe<PinData>::Timeframe(size_t id, size_t nodeCount):
	_id(id),
	_tagged(false),
	_generated(false),
	_nodeTags(nodeCount),
	_pinData(nodeCount)
{
}

template<typename PinData>
Timeframe<PinData>::~Timeframe(void) = default;

template<typename PinData>
size_t Timeframe<PinData>::GetId(void) const
{
	return _id;
}

template<typename PinData>
template<typename Tag>
void Timeframe<PinData>::ClearTags(size_t nodeId)
{
	_nodeTags[nodeId] = { };
}

template<typename PinData>
template<typename Tag>
void Timeframe<PinData>::AddTag(size_t nodeId)
{
	auto& tags = _nodeTags[nodeId];
	if (auto it = std::find(tags.begin(), tags.end(), std::type_index(typeid(Tag))); it == tags.end())
	{
		tags.push_back(std::type_index(typeid(Tag)));
	}
}

template<typename PinData>
template<typename Tag>
void Timeframe<PinData>::RemoveTag(size_t nodeId)
{
	auto& tags = _nodeTags[nodeId];
	if (auto it = std::find(tags.begin(), tags.end(), std::type_index(typeid(Tag))); it != tags.end())
	{
		tags.erase(it);
	}
}

template<typename PinData>
template<typename Tag>
bool Timeframe<PinData>::HasTag(size_t nodeId) const
{
	auto& tags = _nodeTags[nodeId];
	return std::find(tags.begin(), tags.end(), std::type_index(typeid(Tag))) != tags.end();
}

template<typename PinData>
template<typename Tag>
void Timeframe<PinData>::TagInputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes)
{
	std::priority_queue<int64_t, std::vector<int64_t>, std::less<int64_t>> pq(nodes.begin(), nodes.end());

	const auto& circuit = context.GetMappedCircuit();
	int64_t lastNodeId = -1;
	while (!pq.empty())
	{
		const int64_t currentNodeId = pq.top();
		pq.pop();

		if (currentNodeId == lastNodeId)
		{
			continue;
		}

		lastNodeId = currentNodeId;
		AddTag<Tag>(currentNodeId);

		const auto* currentNode = circuit.GetNode(currentNodeId);
		for (auto next : currentNode->GetInputs())
		{
			if (next == nullptr)
			{
				continue;
			}

			pq.emplace(next->GetNodeId());
		}
	}
}

template<typename PinData>
template<typename Tag>
void Timeframe<PinData>::TagOutputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes)
{
	std::priority_queue<int64_t, std::vector<int64_t>, std::greater<int64_t>> pq(nodes.begin(), nodes.end());

	const auto& circuit = context.GetMappedCircuit();
	int64_t lastNodeId = -1;
	while (!pq.empty())
	{
		const int64_t currentNodeId = pq.top();
		pq.pop();

		if (currentNodeId == lastNodeId)
		{
			continue;
		}

		lastNodeId = currentNodeId;
		AddTag<Tag>(currentNodeId);

		const auto* currentNode = circuit.GetNode(currentNodeId);
		for (auto next : currentNode->GetSuccessors())
		{
			if (next == nullptr)
			{
				continue;
			}

			pq.emplace(next->GetNodeId());
		}
	}
}

template<typename PinData>
template<typename Tag>
get_pin_data_container_t<PinData, Tag> Timeframe<PinData>::GetContainer(
	GeneratorContext<PinData>& context, size_t nodeId, PortType portType, size_t portId, AllowContainerInterceptor allowInterceptor) const
{
	using LogicContainer = get_pin_data_container_t<PinData, Tag>;
	if (allowInterceptor == AllowContainerInterceptor::Enable)
	{
		Tag tag { };
		for (auto& interceptor : context.GetContainerInterceptors())
		{
			auto container = reinterpret_cast<const LogicContainer*>(
				interceptor->GetContainer(context, tag, GetId(), nodeId, portType, portId));
			if (container != nullptr)
			{
				return *container;
			}
		}
	}

	// We encode only output nodes, so use the driving output value for the input.
	if (portType == PortType::Input)
	{
		if (const auto* inputNode = context.GetMappedCircuit().GetNode(nodeId)->GetInput(portId);
			inputNode != nullptr)
		{
			return GetContainer<Tag>(context, inputNode->GetNodeId(), PortType::Output, 0u, allowInterceptor);
		}

		Logging::Panic("Trying to get non-existant logic container");
	}
	else if constexpr (has_pin_data_encoding_v<PinData, Tag>)
	{
		if (HasTag<Tag>(nodeId))
		{
			return GetInternalContainer<Tag>(context, nodeId);
		}
	}

	// TODO: Externalize fallbacks / virtual containers in the future
	if constexpr (std::is_same_v<Tag, BadTag> && has_good_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			return GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
		}
	}
	else if constexpr (std::is_same_v<Tag, Bad2Tag> && has_good_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			return GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
		}
	}
	else if constexpr (std::is_same_v<Tag, DiffTag> && has_good_tag_v<PinData> && has_bad_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			auto goodContainer = GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
			auto badContainer = GetContainer<BadTag>(context, nodeId, portType, portId, allowInterceptor);
			return context.GetEncoder().EncodeLogic01DifferenceDetector(goodContainer, badContainer);
		}
	}
	else if constexpr (std::is_same_v<Tag, Diff2Tag> && has_good_tag_v<PinData> && has_bad_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			auto goodContainer = GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
			auto badContainer = GetContainer<Bad2Tag>(context, nodeId, portType, portId, allowInterceptor);
			return context.GetEncoder().EncodeLogic01DifferenceDetector(goodContainer, badContainer);
		}
	}
	else if constexpr (std::is_same_v<Tag, LDiffTag> && has_good_tag_v<PinData> && has_bad_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			auto goodContainer = GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
			auto badContainer = GetContainer<BadTag>(context, nodeId, portType, portId, allowInterceptor);
			return -context.GetEncoder().EncodeLogicEquivalenceDetector(goodContainer, badContainer);
		}
	}
	else if constexpr (std::is_same_v<Tag, LDiff2Tag> && has_good_tag_v<PinData> && has_bad2_tag_v<PinData>)
	{
		if (HasTag<GoodTag>(nodeId))
		{
			auto goodContainer = GetContainer<GoodTag>(context, nodeId, portType, portId, allowInterceptor);
			auto badContainer = GetContainer<Bad2Tag>(context, nodeId, portType, portId, allowInterceptor);
			return -context.GetEncoder().EncodeLogicEquivalenceDetector(goodContainer, badContainer);
		}
	}

	// Logging::Panic("Trying to get non-existant logic container");
	return LogicContainer();
}

template<typename PinData>
template<typename Tag>
std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>, get_pin_data_container_t<PinData, Tag>>&
Timeframe<PinData>::GetInternalContainer(GeneratorContext<PinData>& context, size_t nodeId)
{
	return Tpg::GetContainer<Tag>(_pinData[nodeId]);
}

template<typename PinData>
template<typename Tag>
std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>, get_pin_data_container_t<PinData, Tag>> const&
Timeframe<PinData>::GetInternalContainer(GeneratorContext<PinData>& context, size_t nodeId) const
{
	return Tpg::GetContainer<Tag>(_pinData[nodeId]);
}

template<typename PinData>
void Timeframe<PinData>::Reset(void)
{
	std::fill(_nodeTags.begin(), _nodeTags.end(), std::vector<std::type_index> { });
	std::fill(_pinData.begin(), _pinData.end(), PinData());
}

#define FOR_PINDATA(PINDATA) template class Timeframe<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA
#undef FOR_PINDATA

#define FOR_TAG(PINDATA, TAG) \
	template get_pin_data_container_t<PINDATA, TAG> Timeframe<PINDATA>::GetContainer<TAG>(GeneratorContext<PINDATA>& context, size_t nodeId, PortType portType, size_t portId, AllowContainerInterceptor interceptor) const;
INSTANTIATE_FOR_ALL_TAGS
#undef FOR_TAG

#define FOR_TAG(PINDATA, TAG) \
	template get_pin_data_container_t<PINDATA, TAG>& Timeframe<PINDATA>::GetInternalContainer<TAG>(GeneratorContext<PINDATA>& context, size_t nodeId); \
	template get_pin_data_container_t<PINDATA, TAG> const& Timeframe<PINDATA>::GetInternalContainer<TAG>(GeneratorContext<PINDATA>& context, size_t nodeId) const;
INSTANTIATE_FOR_ALL_ENCODINGS
#undef FOR_TAG

#define FOR_TAG(PINDATA, TAG) \
	template void Timeframe<PINDATA>::AddTag<TAG>(size_t nodeId); \
	template void Timeframe<PINDATA>::RemoveTag<TAG>(size_t nodeId); \
	template void Timeframe<PINDATA>::ClearTags<TAG>(size_t nodeId); \
	template bool Timeframe<PINDATA>::HasTag<TAG>(size_t nodeId) const; \
	template void Timeframe<PINDATA>::TagInputCone<TAG>(GeneratorContext<PINDATA>& context, const std::vector<size_t>& nodes); \
	template void Timeframe<PINDATA>::TagOutputCone<TAG>(GeneratorContext<PINDATA>& context, const std::vector<size_t>& nodes);
INSTANTIATE_FOR_ALL_TAGGING
#undef FOR_TAG

};
};
