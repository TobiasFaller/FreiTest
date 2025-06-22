#pragma once

#include <memory>
#include <string>

#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class TransitionDelayFaultTaggingMode { FullScan, Functional };

template<typename PinData, typename FaultModel>
class TransitionDelayFaultCircuitTagger: public ICircuitTagger<PinData>
{
public:
	TransitionDelayFaultCircuitTagger(std::shared_ptr<FaultModel> faultModel);
	virtual ~TransitionDelayFaultCircuitTagger(void);

	void SetTaggingMode(TransitionDelayFaultTaggingMode mode);
	const TransitionDelayFaultTaggingMode& GetTaggingMode(void) const;

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

private:
	std::shared_ptr<FaultModel> _faultModel;
	TransitionDelayFaultTaggingMode _taggingMode;

};

};
};
