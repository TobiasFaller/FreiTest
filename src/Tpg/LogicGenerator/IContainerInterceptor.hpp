#pragma once

#include <cstdint>
#include <type_traits>

#include "Circuit/Port.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class GeneratorContext;

// This is an ugly interface and not type-safe but at least it compiles.
// void* is required as a work-around here since virtual functions with
// function-specific templates are not allowed in the current C++ standard
template<typename PinData>
class IContainerInterceptor
{

public:
	virtual const void* GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframe, size_t nodeId, Circuit::PortType portType, size_t portId) = 0;

};

template<typename Module>
struct is_container_interceptor {
	template<typename PinData>
	static constexpr std::true_type test(const IContainerInterceptor<PinData>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<Module*>())); \
};
template<typename Module>
using is_container_interceptor_t = typename is_container_interceptor<Module>::type;
template<typename Module>
constexpr bool is_container_interceptor_v = is_container_interceptor_t<Module>::value;

};
};
