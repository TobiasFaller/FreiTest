#pragma once

#include <cstddef>
#include <type_traits>

namespace FreiTest
{
namespace Tpg
{

enum TaggingDirection
{
	Forward,
	Backward
};

template<typename PinData>
class GeneratorContext;

template<typename PinData>
class ICircuitTagger
{
public:
	virtual std::string GetName(void) const = 0;
	virtual void Reset(void) { };

	virtual bool TagCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PreTagCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PostTagCircuit(GeneratorContext<PinData>& context) { return true; };

	virtual bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) { return true; };
	virtual bool PreTagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) { return true; };
	virtual bool PostTagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) { return true; };

};

template<typename Module>
struct is_circuit_tagger {
	template<typename PinData>
	static constexpr std::true_type test(const ICircuitTagger<PinData>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<Module*>())); \
};
template<typename Module>
using is_circuit_tagger_t = typename is_circuit_tagger<Module>::type;
template<typename Module>
constexpr bool is_circuit_tagger_v = is_circuit_tagger_t<Module>::value;

};
};
