#pragma once

#include <string>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
class InputLogicEncoder: public ICircuitEncoder<PinData>
{
public:
	InputLogicEncoder(Basic::LogicConstraint constraint);
	virtual ~InputLogicEncoder(void);

	std::string GetName(void) const override;
	bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) override;

private:
	Basic::LogicConstraint _constraint;

};

};
};
