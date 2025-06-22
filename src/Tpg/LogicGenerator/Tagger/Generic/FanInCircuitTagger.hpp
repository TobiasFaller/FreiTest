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
class FanInCircuitTagger: public ICircuitTagger<PinData>
{
public:
	FanInCircuitTagger(Nodes::NodeSelector<PinData> nodes);
	virtual ~FanInCircuitTagger(void);

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

private:
	Nodes::NodeSelector<PinData> _targetNodes;

};

};
};
