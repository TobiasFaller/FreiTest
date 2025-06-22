#pragma once

#include <string>

#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class DChainCircuitTagger: public ICircuitTagger<PinData>
{
public:
	DChainCircuitTagger(void);
	virtual ~DChainCircuitTagger(void);

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

};

};
};
