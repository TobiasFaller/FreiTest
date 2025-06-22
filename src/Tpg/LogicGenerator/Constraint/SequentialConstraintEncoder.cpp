#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"

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

SequentialConstraintSpec MakeConstantSequentialInitializer(Logic logic)
{
	return {
		[logic](size_t timeframe, size_t flipflop) {
			return (timeframe == 0) ? GetLogicConstraintForLogic(logic) : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::InitThenIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialInitializer(LogicConstraint constraint)
{
	return {
		[constraint](size_t timeframe, size_t flipflop) {
			return (timeframe == 0) ? constraint : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::InitThenIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialInitializer(std::vector<Logic> values)
{
	return {
		[values](size_t timeframe, size_t flipflop) {
			return (timeframe == 0) ? GetLogicConstraintForLogic(values[flipflop]) : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::InitThenIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialInitializer(std::vector<LogicConstraint> constraints)
{
	return {
		[constraints](size_t timeframe, size_t flipflop) {
			return (timeframe == 0) ? constraints[flipflop] : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::InitThenIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialConstraint(Logic logic)
{
	return {
		[logic](size_t timeframe, size_t flipflop) {
			return GetLogicConstraintForLogic(logic);
		},
		TimeframeDependency::TimeframeIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialConstraint(LogicConstraint constraint)
{
	return {
		[constraint](size_t timeframe, size_t flipflop) {
			return constraint;
		},
		TimeframeDependency::TimeframeIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialConstraint(std::vector<Logic> values)
{
	return {
		[values](size_t timeframe, size_t flipflop) {
			return GetLogicConstraintForLogic(values[flipflop]);
		},
		TimeframeDependency::TimeframeIndependent
	};
}

SequentialConstraintSpec MakeConstantSequentialConstraint(std::vector<LogicConstraint> constraints)
{
	return {
		[constraints](size_t timeframe, size_t flipflop) {
			return constraints[flipflop];
		},
		TimeframeDependency::TimeframeIndependent
	};
}

SequentialConstraintSpec MakeSequentialConstraints(std::vector<std::vector<Logic>> values)
{
	return {
		[values](size_t timeframe, size_t flipflop) {
			return (timeframe >= values.size()) ? GetLogicConstraintForLogic(values[timeframe][flipflop]) : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::TimeframeDependent
	};
}

SequentialConstraintSpec MakeSequentialConstraints(std::vector<std::vector<LogicConstraint>> constraints)
{
	return {
		[constraints](size_t timeframe, size_t flipflop) {
			return (timeframe >= constraints.size()) ? constraints[timeframe][flipflop] : LogicConstraint::NO_CONSTRAINT;
		},
		TimeframeDependency::TimeframeDependent
	};
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>::SequentialConstraintEncoder(SequentialConstraintEncoderCallback generator, TimeframeDependency dependency, ConstrainedSequentials portType, ConstraintType type, ConstraintTarget target):
	ConstraintEncoder<PinData, Tag>(WrapCallback(), dependency, type, target),
	_sequentialConstraintEncoder(generator),
	_portType(portType)
{
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>::SequentialConstraintEncoder(SequentialConstraintSpec generator, ConstrainedSequentials portType, ConstraintType type, ConstraintTarget target):
	SequentialConstraintEncoder<PinData, Tag>(std::get<0>(generator), std::get<1>(generator), portType, type, target)
{
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>::SequentialConstraintEncoder(const SequentialConstraintEncoder<PinData, Tag>& other):
	// The callback is bound to the this pointer of the SequentialConstraintEncoder current instance.
	// Do not use the callback from the other SequentialConstraintEncoder instance.
	ConstraintEncoder<PinData, Tag>(WrapCallback(), other._dependency, other._type, other._target),
	_sequentialConstraintEncoder(other._sequentialConstraintEncoder),
	_portType(other._portType)
{
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>::SequentialConstraintEncoder(SequentialConstraintEncoder<PinData, Tag>&& other):
	// The callback is bound to the this pointer of the SequentialConstraintEncoder current instance.
	// Do not use the callback from the other SequentialConstraintEncoder instance.
	ConstraintEncoder<PinData, Tag>(WrapCallback(), other._dependency, other._type, other._target),
	_sequentialConstraintEncoder(std::move(other._sequentialConstraintEncoder)),
	_portType(other._portType)
{
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>::~SequentialConstraintEncoder(void) = default;


template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>& SequentialConstraintEncoder<PinData, Tag>::operator=(const SequentialConstraintEncoder<PinData, Tag>& other)
{
	this->_sequentialConstraintEncoder = other._sequentialConstraintEncoder;
	// Do not copy the ConstraintEncoderCallback of the _generator object.
	// This callback is bound to the specific instance of the SequentialConstraintEncoder.
	this->_dependency = other._dependency;
	this->_target = other._target;
	this->_portType = other._portType;
	return *this;
}

template<typename PinData, typename Tag>
SequentialConstraintEncoder<PinData, Tag>& SequentialConstraintEncoder<PinData, Tag>::operator=(SequentialConstraintEncoder<PinData, Tag>&& other)
{
	this->_sequentialConstraintEncoder = other._sequentialConstraintEncoder;
	// Do not copy the ConstraintEncoderCallback of the _generator object.
	// This callback is bound to the specific instance of the SequentialConstraintEncoder.
	this->_dependency = other._dependency;
	this->_target = other._target;
	this->_portType = other._portType;
	return *this;
}

template<typename PinData, typename Tag>
std::string SequentialConstraintEncoder<PinData, Tag>::GetName(void) const
{
	return "SequentialConstraintEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
void SequentialConstraintEncoder<PinData, Tag>::SetConstraintEncoder(SequentialConstraintEncoderCallback generator)
{
	_sequentialConstraintEncoder = generator;
}

template<typename PinData, typename Tag>
SequentialConstraintEncoderCallback const& SequentialConstraintEncoder<PinData, Tag>::GetConstraintEncoder(void) const
{
	return _sequentialConstraintEncoder;
}

template<typename PinData, typename Tag>
ConstraintEncoderCallback<PinData> SequentialConstraintEncoder<PinData, Tag>::WrapCallback(void) const
{
	return [this](GeneratorContext<PinData>& context, size_t timeframe, size_t nodeId) -> LogicConstraint {
		const auto& circuit = context.GetMappedCircuit();
		const auto* node = circuit.GetNode(nodeId);
		switch (_portType)
		{
			case ConstrainedSequentials::None:
				return LogicConstraint::NO_CONSTRAINT;

			case ConstrainedSequentials::Inputs:
				return circuit.IsSecondaryInput(node)
					? _sequentialConstraintEncoder(timeframe, circuit.GetSecondaryInputNumber(node))
					: LogicConstraint::NO_CONSTRAINT;

			case ConstrainedSequentials::Outputs:
				return circuit.IsSecondaryOutput(node)
					? _sequentialConstraintEncoder(timeframe, circuit.GetSecondaryOutputNumber(node))
					: LogicConstraint::NO_CONSTRAINT;

			case ConstrainedSequentials::InputAndOutputs:
				if (circuit.IsSecondaryInput(node)) {
					return _sequentialConstraintEncoder(timeframe, circuit.GetSecondaryInputNumber(node));
				}
				if (circuit.IsSecondaryOutput(node)) {
					auto secondaryInputs = circuit.GetNumberOfSecondaryInputs();
					return _sequentialConstraintEncoder(timeframe, secondaryInputs + circuit.GetSecondaryOutputNumber(node));
				}
				return LogicConstraint::NO_CONSTRAINT;

			default:
				Logging::Panic("Unsupported port type!");
		}
	};
}

#define FOR_TAG(PINDATA, TAG) template class SequentialConstraintEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
