#pragma once

#include <string>
#include <type_traits>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class ForwardDChainEncoder: public ICircuitEncoder<PinData>
{
public:
	ForwardDChainEncoder(void);
	virtual ~ForwardDChainEncoder(void);

	std::string GetName(void) const override;
	bool EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

};

};
};
