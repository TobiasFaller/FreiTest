#pragma once

#include <string>
#include <vector>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class UnaryCounterEncoder: public ICircuitEncoder<PinData>
{
public:
	UnaryCounterEncoder(void);
	virtual ~UnaryCounterEncoder(void);

	std::string GetName(void) const override;
	bool PreEncodeCircuit(GeneratorContext<PinData>& context) override;

};

};
};
