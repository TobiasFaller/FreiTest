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

enum class FaultPropagationTarget
{
	PrimaryOutputsOnly,
	SecondaryOutputsOnly,
	PrimaryAndSecondaryOutputs
};

enum class FaultPropagationTimeframe
{
	Any,
	Last
};
template<typename PinData>
class FaultPropagationConstraintEncoder:
	public IConstraintEncoder<PinData>
{
public:
	FaultPropagationConstraintEncoder(FaultPropagationTarget target, FaultPropagationTimeframe targetFrame = FaultPropagationTimeframe::Any);
	virtual ~FaultPropagationConstraintEncoder(void);

	void SetAllowEmptyPropagation(bool enable);
	bool IsAllowEmptyPropagation(void) const;

	std::string GetName(void) const override;
	void Reset(void) override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

	const std::vector<Tpg::LogicContainer01>& GetGeneratedPrimaryConstraints(void) const;
	const std::vector<Tpg::LogicContainer01>& GetGeneratedSecondaryConstraints(void) const;

protected:
	FaultPropagationTarget _target;
	FaultPropagationTimeframe _targetTimeframe;
	bool _allowEmptyPropagation;
	std::vector<Tpg::LogicContainer01> _primaryDifferenceOutputs;
	std::vector<Tpg::LogicContainer01> _secondaryDifferenceOutputs;

};

};
};
