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
class BackwardDChainEncoder: public ICircuitEncoder<PinData>
{
public:
	BackwardDChainEncoder(void);
	virtual ~BackwardDChainEncoder(void);

	std::string GetName(void) const override;
	bool EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

};

};
};
