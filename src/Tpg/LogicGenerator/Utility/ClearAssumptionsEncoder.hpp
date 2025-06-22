#pragma once

#include <string>
#include <vector>

#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class ClearAssumptionsEncoder: public ICircuitEncoder<PinData>
{
public:
	ClearAssumptionsEncoder(void);
	virtual ~ClearAssumptionsEncoder(void);

	std::string GetName(void) const override;
	bool PreEncodeCircuit(GeneratorContext<PinData>& context) override;

};

};
};
