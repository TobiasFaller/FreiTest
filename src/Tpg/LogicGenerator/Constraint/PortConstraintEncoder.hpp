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

enum class ConstrainedPorts
{
	None,
	Inputs,
	Outputs,
	InputAndOutputs
};

using PortConstraintEncoderCallback = std::function<Basic::LogicConstraint(size_t timeframe, size_t primaryPort)>;
using PortConstraintSpec = std::tuple<PortConstraintEncoderCallback, TimeframeDependency>;

PortConstraintSpec MakeConstantPortConstraint(Basic::Logic logic);
PortConstraintSpec MakeConstantPortConstraint(Basic::LogicConstraint constraint);
PortConstraintSpec MakeConstantPortConstraint(std::vector<Basic::Logic> values);
PortConstraintSpec MakeConstantPortConstraint(std::vector<Basic::LogicConstraint> constraints);
PortConstraintSpec MakePredefinedPortConstraint(std::vector<std::vector<Basic::Logic>> values, Basic::LogicConstraint undefined = Basic::LogicConstraint::ONLY_LOGIC_DONT_CARE);
PortConstraintSpec MakePredefinedPortConstraint(std::vector<std::vector<Basic::LogicConstraint>> constraints, Basic::LogicConstraint undefined = Basic::LogicConstraint::ONLY_LOGIC_DONT_CARE);

template<typename PinData, typename Tag>
class PortConstraintEncoder: public ConstraintEncoder<PinData, Tag>
{
public:
	PortConstraintEncoder(PortConstraintEncoderCallback generator, TimeframeDependency dependency, ConstrainedPorts portType, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	PortConstraintEncoder(PortConstraintSpec specification, ConstrainedPorts portType, ConstraintType type = ConstraintType::HardConstraint, ConstraintTarget target = ConstraintTarget::PreFaultModel);
	PortConstraintEncoder(const PortConstraintEncoder& other);
	PortConstraintEncoder(PortConstraintEncoder&& other);
	virtual ~PortConstraintEncoder(void);

	PortConstraintEncoder& operator=(const PortConstraintEncoder& other);
	PortConstraintEncoder& operator=(PortConstraintEncoder&& other);

	void SetConstraintEncoder(PortConstraintEncoderCallback generator);
	PortConstraintEncoderCallback const& GetConstraintEncoder(void) const;

	std::string GetName(void) const override;

protected:
	ConstraintEncoderCallback<PinData> WrapCallback(void) const;
	PortConstraintEncoderCallback _portConstraintEncoder;
	ConstrainedPorts _portType;

};

};
};
