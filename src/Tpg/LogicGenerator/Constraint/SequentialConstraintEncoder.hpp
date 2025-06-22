#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/Constraint/ConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class ConstrainedSequentials
{
	None,
	Inputs,
	Outputs,
	InputAndOutputs
};

using SequentialConstraintEncoderCallback = std::function<Basic::LogicConstraint(size_t timeframe, size_t secondaryInput)>;
using SequentialConstraintSpec = std::tuple<SequentialConstraintEncoderCallback, TimeframeDependency>;

SequentialConstraintSpec MakeConstantSequentialInitializer(Basic::Logic logic);
SequentialConstraintSpec MakeConstantSequentialInitializer(Basic::LogicConstraint constraint);
SequentialConstraintSpec MakeConstantSequentialInitializer(std::vector<Basic::Logic> values);
SequentialConstraintSpec MakeConstantSequentialInitializer(std::vector<Basic::LogicConstraint> constraints);
SequentialConstraintSpec MakeConstantSequentialConstraint(Basic::Logic logic);
SequentialConstraintSpec MakeConstantSequentialConstraint(Basic::LogicConstraint constraint);
SequentialConstraintSpec MakeConstantSequentialConstraint(std::vector<Basic::Logic> values);
SequentialConstraintSpec MakeConstantSequentialConstraint(std::vector<Basic::LogicConstraint> constraints);
SequentialConstraintSpec MakeSequentialConstraints(std::vector<std::vector<Basic::Logic>> values);
SequentialConstraintSpec MakeSequentialConstraints(std::vector<std::vector<Basic::LogicConstraint>> constraints);

template<typename PinData, typename Tag>
class SequentialConstraintEncoder: public ConstraintEncoder<PinData, Tag>
{
public:
	SequentialConstraintEncoder(SequentialConstraintEncoderCallback generator, TimeframeDependency dependency, ConstrainedSequentials portType, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	SequentialConstraintEncoder(SequentialConstraintSpec generator, ConstrainedSequentials portType, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	SequentialConstraintEncoder(const SequentialConstraintEncoder& other);
	SequentialConstraintEncoder(SequentialConstraintEncoder&& other);
	virtual ~SequentialConstraintEncoder(void);

	SequentialConstraintEncoder& operator=(const SequentialConstraintEncoder& other);
	SequentialConstraintEncoder& operator=(SequentialConstraintEncoder&& other);

	void SetConstraintEncoder(SequentialConstraintEncoderCallback generator);
	SequentialConstraintEncoderCallback const& GetConstraintEncoder(void) const;

	std::string GetName(void) const override;

private:
	ConstraintEncoderCallback<PinData> WrapCallback(void) const;
	SequentialConstraintEncoderCallback _sequentialConstraintEncoder;
	ConstrainedSequentials _portType;

};

};
};
