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

template<typename PinData, typename Tag>
class DefaultLogicEncoder: public ICircuitEncoder<PinData>
{
public:
	DefaultLogicEncoder(Basic::LogicConstraint constraint);
	virtual ~DefaultLogicEncoder(void);

	std::string GetName(void) const override;
	bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) override;

private:
	Basic::LogicConstraint _constraint;

};

};
};
