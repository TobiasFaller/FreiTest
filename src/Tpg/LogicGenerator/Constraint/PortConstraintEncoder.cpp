#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

PortConstraintSpec MakeConstantPortConstraint(Logic value)
{
	return MakeConstantPortConstraint(GetLogicConstraintForLogic(value));
}

PortConstraintSpec MakeConstantPortConstraint(LogicConstraint constraint)
{
	return {
		[constraint](size_t timeframe, size_t port) -> LogicConstraint {
			return constraint;
		},
		TimeframeDependency::TimeframeIndependent
	};
}

PortConstraintSpec MakeConstantPortConstraint(std::vector<Logic> values)
{
	return {
		[values](size_t timeframe, size_t port) -> LogicConstraint {
			return GetLogicConstraintForLogic(values[port]);
		},
		TimeframeDependency::TimeframeIndependent
	};
}

PortConstraintSpec MakeConstantPortConstraint(std::vector<LogicConstraint> constraints)
{
	return {
		[constraints](size_t timeframe, size_t port) -> LogicConstraint {
			return constraints[port];
		},
		TimeframeDependency::TimeframeIndependent
	};
}

PortConstraintSpec MakePredefinedPortConstraint(std::vector<std::vector<Logic>> values, LogicConstraint undefined)
{
	return {
		[values, undefined](size_t timeframe, size_t port) -> LogicConstraint {
			return (timeframe < values.size()) ? GetLogicConstraintForLogic(values[timeframe][port]) : undefined;
		},
		TimeframeDependency::TimeframeDependent
	};
}

PortConstraintSpec MakePredefinedPortConstraint(std::vector<std::vector<LogicConstraint>> constraints, LogicConstraint undefined)
{
	return {
		[constraints, undefined](size_t timeframe, size_t port) -> LogicConstraint {
			return (timeframe < constraints.size()) ? constraints[timeframe][port] : undefined;
		},
		TimeframeDependency::TimeframeDependent
	};
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>::PortConstraintEncoder(PortConstraintEncoderCallback generator, TimeframeDependency dependency, ConstrainedPorts portType, ConstraintType type, ConstraintTarget target):
	ConstraintEncoder<PinData, Tag>(WrapCallback(), dependency, type, target),
	_portConstraintEncoder(generator),
	_portType(portType)
{
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>::PortConstraintEncoder(std::tuple<PortConstraintEncoderCallback, TimeframeDependency> specification, ConstrainedPorts portType, ConstraintType type, ConstraintTarget target):
	PortConstraintEncoder<PinData, Tag>(std::get<0>(specification), std::get<1>(specification), portType, type, target)
{
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>::PortConstraintEncoder(const PortConstraintEncoder<PinData, Tag>& other):
	// The callback is bound to the this pointer of the PortConstraintEncoder current instance.
	// Do not use the callback from the other PortConstraintEncoder instance.
	ConstraintEncoder<PinData, Tag>(WrapCallback(), other._dependency, other._type, other._target),
	_portConstraintEncoder(other._portConstraintEncoder),
	_portType(other._portType)
{
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>::PortConstraintEncoder(PortConstraintEncoder<PinData, Tag>&& other):
	// The callback is bound to the this pointer of the PortConstraintEncoder current instance.
	// Do not use the callback from the other PortConstraintEncoder instance.
	ConstraintEncoder<PinData, Tag>(WrapCallback(), other._dependency, other._type, other._target),
	_portConstraintEncoder(std::move(other._portConstraintEncoder)),
	_portType(other._portType)
{
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>::~PortConstraintEncoder(void) = default;

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>& PortConstraintEncoder<PinData, Tag>::operator=(const PortConstraintEncoder<PinData, Tag>& other)
{
	this->_portConstraintEncoder = other._portConstraintEncoder;
	// Do not copy the ConstraintEncoderCallback of the _generator object.
	// This callback is bound to the specific instance of the PortConstraintEncoder.
	this->_dependency = other._dependency;
	this->_target = other._target;
	this->_portType = other._portType;
	return *this;
}

template<typename PinData, typename Tag>
PortConstraintEncoder<PinData, Tag>& PortConstraintEncoder<PinData, Tag>::operator=(PortConstraintEncoder<PinData, Tag>&& other)
{
	this->_portConstraintEncoder = std::move(other._portConstraintEncoder);
	// Do not move the ConstraintEncoderCallback of the _generator object.
	// This callback is bound to the specific instance of the PortConstraintEncoder.
	this->_dependency = other._dependency;
	this->_target = other._target;
	this->_portType = other._portType;
	return *this;
}

template<typename PinData, typename Tag>
std::string PortConstraintEncoder<PinData, Tag>::GetName(void) const
{
	return "PortConstraintEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
void PortConstraintEncoder<PinData, Tag>::SetConstraintEncoder(PortConstraintEncoderCallback generator)
{
	_portConstraintEncoder = generator;
}

template<typename PinData, typename Tag>
PortConstraintEncoderCallback const& PortConstraintEncoder<PinData, Tag>::GetConstraintEncoder(void) const
{
	return _portConstraintEncoder;
}
template<typename PinData, typename Tag>
ConstraintEncoderCallback<PinData> PortConstraintEncoder<PinData, Tag>::WrapCallback(void) const
{
	return [this](GeneratorContext<PinData>& context, size_t timeframe, size_t nodeId) -> LogicConstraint {
		const auto& circuit = context.GetMappedCircuit();
		const auto* node = circuit.GetNode(nodeId);
		switch (_portType)
		{
			case ConstrainedPorts::None:
				return LogicConstraint::NO_CONSTRAINT;

			case ConstrainedPorts::Inputs:
				return circuit.IsPrimaryInput(node)
					? _portConstraintEncoder(timeframe, circuit.GetPrimaryInputNumber(node))
					: LogicConstraint::NO_CONSTRAINT;

			case ConstrainedPorts::Outputs:
				return circuit.IsPrimaryOutput(node)
					? _portConstraintEncoder(timeframe, circuit.GetPrimaryOutputNumber(node))
					: LogicConstraint::NO_CONSTRAINT;

			case ConstrainedPorts::InputAndOutputs:
				if (circuit.IsPrimaryInput(node)) {
					return _portConstraintEncoder(timeframe, circuit.GetPrimaryInputNumber(node));
				}
				if (circuit.IsPrimaryOutput(node)) {
					auto primaryInputs = circuit.GetNumberOfPrimaryInputs();
					return _portConstraintEncoder(timeframe, primaryInputs + circuit.GetPrimaryOutputNumber(node));
				}
				return LogicConstraint::NO_CONSTRAINT;

			default:
				Logging::Panic("Unsupported port type!");
		}
	};
}

#define FOR_TAG(PINDATA, TAG) template class PortConstraintEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
