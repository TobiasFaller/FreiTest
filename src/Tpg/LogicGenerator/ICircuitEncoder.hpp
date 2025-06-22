#pragma once

#include <cstddef>
#include <type_traits>

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class GeneratorContext;

template<typename PinData>
class ICircuitEncoder
{
public:
	virtual std::string GetName(void) const = 0;
	virtual void Reset(void) { };

	virtual bool EncodeCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PreEncodeCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PostEncodeCircuit(GeneratorContext<PinData>& context) { return true; };

	virtual bool EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };
	virtual bool PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };
	virtual bool PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };

	virtual bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) { return true; };
	virtual bool PreEncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) { return true; };
	virtual bool PostEncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) { return true; };

};

template<typename Module>
struct is_circuit_encoder {
	template<typename PinData>
	static constexpr std::true_type test(const ICircuitEncoder<PinData>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<Module*>())); \
};
template<typename Module>
using is_circuit_encoder_t = typename is_circuit_encoder<Module>::type;
template<typename Module>
constexpr bool is_circuit_encoder_v = is_circuit_encoder_t<Module>::value;

};
};
