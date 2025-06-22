#pragma once

#include <cstdint>
#include <string>

#include "Basic/Logic.hpp"
#include "Basic/SequentialConfiguration.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
class SequentialOutputLogicEncoder: public ICircuitEncoder<PinData>
{
public:
	SequentialOutputLogicEncoder(Basic::SequentialConfig sequentialConfig);
	virtual ~SequentialOutputLogicEncoder(void);

	std::string GetName(void) const override;
	bool EncodeNode(GeneratorContext<PinData>& context, size_t timeframe, size_t node) override;

private:
	Basic::SequentialConfig _sequentialConfig;

};

};
};
