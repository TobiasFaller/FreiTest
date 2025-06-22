#pragma once

#include <memory>
#include <string>

#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class StuckAtFaultTaggingMode { FullScan, Functional };

template<typename PinData, typename FaultModel>
class StuckAtFaultCircuitTagger: public ICircuitTagger<PinData>
{
public:
	StuckAtFaultCircuitTagger(std::shared_ptr<FaultModel> faultModel);
	virtual ~StuckAtFaultCircuitTagger(void);

	void SetTaggingMode(StuckAtFaultTaggingMode mode);
	const StuckAtFaultTaggingMode& GetTaggingMode(void) const;

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

private:
	std::shared_ptr<FaultModel> _faultModel;
	StuckAtFaultTaggingMode _taggingMode;

};

};
};
