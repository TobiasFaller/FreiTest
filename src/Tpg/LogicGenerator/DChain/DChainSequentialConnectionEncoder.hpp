#pragma once

#include <string>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class DChainSequentialConnectionEncoder: public ICircuitEncoder<PinData>
{
public:
	DChainSequentialConnectionEncoder(void);
	virtual ~DChainSequentialConnectionEncoder(void);

	std::string GetName(void) const override;
	bool EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

};

};
};
