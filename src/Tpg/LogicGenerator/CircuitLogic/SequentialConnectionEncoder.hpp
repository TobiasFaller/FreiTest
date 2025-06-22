#pragma once

#include <string>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
class SequentialConnectionEncoder: public ICircuitEncoder<PinData>
{
public:
	SequentialConnectionEncoder(void);
	virtual ~SequentialConnectionEncoder(void);

	std::string GetName(void) const override;
	bool PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

};

};
};
