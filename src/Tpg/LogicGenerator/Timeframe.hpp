#pragma once

#include <vector>
#include <string>
#include <typeindex>

#include "Circuit/Port.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class GeneratorContext;

enum class AllowContainerInterceptor
{
	Enable,
	Disable
};

template<typename PinData>
class Timeframe
{

public:
	Timeframe(size_t id, size_t nodeCount);
	virtual ~Timeframe(void);

	size_t GetId(void) const;
	void Reset(void);

	template<typename Tag> void AddTag(size_t nodeId);
	template<typename Tag> void RemoveTag(size_t nodeId);
	template<typename Tag> void ClearTags(size_t nodeId);
	template<typename Tag> bool HasTag(size_t nodeId) const;
	template<typename Tag> void TagInputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes);
	template<typename Tag> void TagOutputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes);

	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> void AddTag(size_t nodeId) { (AddTag<Tags>(nodeId), ...); }
	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> void RemoveTag(size_t nodeId) { (RemoveTag<Tags>(nodeId), ...); }
	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> void ClearTags(size_t nodeId) { (ClearTags<Tags>(nodeId), ...); }
	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> bool HasTag(size_t nodeId) const { return (HasTag<Tags>() & ...); }
	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> void TagInputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes) { (TagInputCone<Tags>(context, nodes), ...); }
	template<typename... Tags, typename enable = std::enable_if_t<(sizeof...(Tags) > 1), size_t>> void TagOutputCone(GeneratorContext<PinData>& context, const std::vector<size_t>& nodes) { (TagOutputCone<Tags>(context, nodes), ...); }

	template<typename Tag>
	get_pin_data_container_t<PinData, Tag> GetContainer(
		GeneratorContext<PinData>& context, size_t nodeId, Circuit::PortType portType,
		size_t portId, AllowContainerInterceptor allowInterceptor = AllowContainerInterceptor::Enable) const;

	template<typename Tag>
	std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>, get_pin_data_container_t<PinData, Tag>>&
	GetInternalContainer(GeneratorContext<PinData>& context, size_t nodeId);

	template<typename Tag>
	std::enable_if_t<has_pin_data_encoding_v<PinData, Tag>, get_pin_data_container_t<PinData, Tag>> const&
	GetInternalContainer(GeneratorContext<PinData>& context, size_t nodeId) const;

private:
	size_t _id;
	bool _tagged;
	bool _generated;
	std::vector<std::vector<std::type_index>> _nodeTags;
	std::vector<PinData> _pinData;

	friend GeneratorContext<PinData>;

};

};
};
