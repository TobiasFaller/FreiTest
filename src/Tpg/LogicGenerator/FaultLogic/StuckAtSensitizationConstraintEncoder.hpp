#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class StuckAtSensitizationTimeframe
{
	Any,
	Last
};
enum class StuckAtSensitizationType
{
	All,
	One
};

template<typename PinData, typename FaultModel>
class StuckAtSensitizationConstraintEncoder:
	public IConstraintEncoder<PinData>
{
public:
	StuckAtSensitizationConstraintEncoder(std::shared_ptr<FaultModel> faultModel, StuckAtSensitizationTimeframe targetTimeframe = StuckAtSensitizationTimeframe::Any);
	virtual ~StuckAtSensitizationConstraintEncoder(void);

	void SetAllowEmptySensitization(bool enable);
	const bool& IsAllowEmptySensitization(void) const;

	void SetStuckAtSensitizationType(StuckAtSensitizationType type);
	const StuckAtSensitizationType& GetStuckAtSensitizationType(void) const;

	std::string GetName(void) const override;
	void Reset(void) override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

protected:
	std::shared_ptr<FaultModel> _faultModel;
	StuckAtSensitizationTimeframe _targetTimeframe;
	bool _allowEmptySensitization;
	StuckAtSensitizationType _sensitizationType;

	std::vector<Tpg::LogicContainer01> _sensitizations;

};

};
};
