#pragma once

#include <cstddef>
#include <type_traits>

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class GeneratorContext;

enum class ConstraintTarget
{
	PreFaultModel,
	PostFaultModel
};
enum class ConstraintType
{
	HardConstraint,
	MaximizationConstraint
};

template<typename PinData>
class IConstraintEncoder
{
public:
	virtual std::string GetName(void) const = 0;
	virtual void Reset(void) { };

	virtual bool ConstrainCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PreConstrainCircuit(GeneratorContext<PinData>& context) { return true; };
	virtual bool PostConstrainCircuit(GeneratorContext<PinData>& context) { return true; };

	virtual bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };
	virtual bool PreConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };
	virtual bool PostConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) { return true; };

};

template<typename Module>
struct is_constraint_encoder {
	template<typename PinData>
	static constexpr std::true_type test(const IConstraintEncoder<PinData>*);
	static constexpr std::false_type test(...);
	using type = decltype(test(std::declval<Module*>())); \
};
template<typename Module>
using is_constraint_encoder_t = typename is_constraint_encoder<Module>::type;
template<typename Module>
constexpr bool is_constraint_encoder_v = is_constraint_encoder_t<Module>::value;

};
};
