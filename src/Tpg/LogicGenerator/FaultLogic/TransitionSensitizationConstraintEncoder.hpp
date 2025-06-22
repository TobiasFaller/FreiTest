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

enum class TransitionSensitizationTimeframe
{
	Any,
	Last
};
enum class TransitionSensitizationType
{
	All,
	One
};

template<typename PinData, typename FaultModel>
class TransitionSensitizationConstraintEncoder:
	public IConstraintEncoder<PinData>
{
public:
	TransitionSensitizationConstraintEncoder(std::shared_ptr<FaultModel> faultModel, TransitionSensitizationTimeframe targetTimeframe = TransitionSensitizationTimeframe::Any);
	virtual ~TransitionSensitizationConstraintEncoder(void);

	void SetAllowEmptySensitization(bool enable);
	const bool& IsAllowEmptySensitization(void) const;

	void SetTransitionSensitizationType(TransitionSensitizationType type);
	const TransitionSensitizationType& GetTransitionSensitizationType(void) const;

	std::string GetName(void) const override;
	void Reset(void) override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

protected:
	std::shared_ptr<FaultModel> _faultModel;
	TransitionSensitizationTimeframe _targetTimeframe;
	bool _allowEmptySensitization;
	TransitionSensitizationType _sensitizationType;

	std::vector<std::vector<Tpg::LogicContainer01>> _transitionInitialValidForTimeframe;
	std::vector<std::vector<Tpg::LogicContainer01>> _transitionInitialValueForTimeframe;
	std::vector<Tpg::LogicContainer01> _sensitizations;

};

};
};
