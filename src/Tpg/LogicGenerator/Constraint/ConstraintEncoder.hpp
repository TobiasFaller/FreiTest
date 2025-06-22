#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class TimeframeDependency
{
	TimeframeDependent,
	TimeframeIndependent,
	InitThenIndependent
};

template<typename PinData>
using ConstraintEncoderCallback = std::function<Basic::LogicConstraint(GeneratorContext<PinData>&, size_t, size_t)>;

template<typename PinData>
using ConstraintSpec = std::tuple<ConstraintEncoderCallback<PinData>, TimeframeDependency>;

template<typename PinData, typename Tag>
class ConstraintEncoder: public IConstraintEncoder<PinData>
{
public:
	ConstraintEncoder(ConstraintEncoderCallback<PinData> generator, TimeframeDependency dependency, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	ConstraintEncoder(ConstraintSpec<PinData> generator, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	virtual ~ConstraintEncoder(void);

	void SetGenerator(ConstraintEncoderCallback<PinData> generator);
	ConstraintEncoderCallback<PinData> const& GetGenerator(void) const;
	void SetWeight(size_t weight);
	size_t GetWeight(void) const;

	std::string GetName(void) const override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

protected:
	ConstraintEncoderCallback<PinData> _generator;
	TimeframeDependency _dependency;
	ConstraintType _type;
	ConstraintTarget _target;
	size_t _weight;

};

};
};
