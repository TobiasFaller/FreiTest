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
class SequentialInputLogicEncoder: public ICircuitEncoder<PinData>
{
public:
	SequentialInputLogicEncoder(Basic::LogicConstraint constraint);
	virtual ~SequentialInputLogicEncoder(void);

	std::string GetName(void) const override;
	bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) override;

private:
	Basic::LogicConstraint _constraint;

};

};
};
