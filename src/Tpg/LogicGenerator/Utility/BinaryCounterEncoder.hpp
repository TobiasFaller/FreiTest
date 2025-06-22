#pragma once

#include <string>
#include <vector>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class BinaryCounterEncoder: public ICircuitEncoder<PinData>
{
public:
	BinaryCounterEncoder(void);
	virtual ~BinaryCounterEncoder(void);

	std::string GetName(void) const override;
	bool PreEncodeCircuit(GeneratorContext<PinData>& context) override;

};

};
};
