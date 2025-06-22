#pragma once

#include <string>
#include <vector>

#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/NodeSelectors.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
class FanOutCircuitTagger: public ICircuitTagger<PinData>
{
public:
	FanOutCircuitTagger(Nodes::NodeSelector<PinData> nodes);
	virtual ~FanOutCircuitTagger(void);

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

private:
	Nodes::NodeSelector<PinData> _targetNodes;

};

};
};
