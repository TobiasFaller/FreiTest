#pragma once

#include <memory>
#include <string>

#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class CellAwareFaultTaggingMode { FullScan, Functional };

template<typename PinData>
class CellAwareFaultCircuitTagger: public ICircuitTagger<PinData>
{
public:
	CellAwareFaultCircuitTagger(std::shared_ptr<Fault::CellAwareFaultModel> faultModel);
	virtual ~CellAwareFaultCircuitTagger(void);

	void SetTaggingMode(CellAwareFaultTaggingMode mode);
	const CellAwareFaultTaggingMode& GetTaggingMode(void) const;

	std::string GetName(void) const override;
	bool TagTimeframe(GeneratorContext<PinData>& context, size_t timeframe, TaggingDirection direction) override;

private:
	std::shared_ptr<Fault::CellAwareFaultModel> _faultModel;
	CellAwareFaultTaggingMode _taggingMode;

};

};
};
