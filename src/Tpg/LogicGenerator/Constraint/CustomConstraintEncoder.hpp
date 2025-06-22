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

enum class ClauseType
{
	Sat,
	BmcInitial,
	BmcTransition,
	BmcTarget
};

template<typename PinData>
using CustomConstraintEncoderCallback = std::function<Basic::LogicConstraint(GeneratorContext<PinData>&, size_t)>;

template<typename PinData>
using CustomConstraintSpec = std::tuple<CustomConstraintEncoderCallback<PinData>, ClauseType>;

template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(Basic::Logic logic);
template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(Basic::LogicConstraint constraint);
template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(std::vector<Basic::Logic> values);
template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(std::vector<Basic::LogicConstraint> constraints);

template<typename PinData, typename Tag>
class CustomConstraintEncoder: public IConstraintEncoder<PinData>
{
public:
	CustomConstraintEncoder(CustomConstraintEncoderCallback<PinData> generator, ClauseType clauseType, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	CustomConstraintEncoder(CustomConstraintSpec<PinData> generator, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	virtual ~CustomConstraintEncoder(void);

	void SetGenerator(CustomConstraintEncoderCallback<PinData> generator);
	CustomConstraintEncoderCallback<PinData> const& GetGenerator(void) const;
	void SetWeight(size_t weight);
	size_t GetWeitht(void) const;

	std::string GetName(void) const override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

protected:
	CustomConstraintEncoderCallback<PinData> _generator;
	ConstraintType _type;
	ConstraintTarget _target;
	ClauseType _clauseType;
	size_t _weight;

};

};
};
