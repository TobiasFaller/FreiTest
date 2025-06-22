#pragma once

#include <string>

#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
class OutputLogicEncoder: public ICircuitEncoder<PinData>
{
public:
	OutputLogicEncoder(void);
	virtual ~OutputLogicEncoder(void);

	std::string GetName(void) const override;
	bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) override;

};

};
};
