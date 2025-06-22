#pragma once

#include <string>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class BackwardDChainConnectionEncoder: public ICircuitEncoder<PinData>
{
public:
	BackwardDChainConnectionEncoder(void);
	virtual ~BackwardDChainConnectionEncoder(void);

	std::string GetName(void) const override;
	bool EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

};

};
};
