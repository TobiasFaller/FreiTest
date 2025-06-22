#include "Tpg/LogicGenerator/Constraint/CustomConstraintEncoder.hpp"

#include <type_traits>
#include <optional>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(Logic logic)
{
	return {
		[logic](const GeneratorContext<PinData>& context, size_t nodeId) {
			auto& mappedCircuit = context.GetMappedCircuit();
			auto const* node = mappedCircuit.GetNode(nodeId);
			if (!mappedCircuit.IsSecondaryInput(node)) {
				return LogicConstraint::NO_CONSTRAINT;
			}

			return GetLogicConstraintForLogic(logic);
		},
		ClauseType::BmcTarget
	};
}

template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(LogicConstraint constraint)
{
	return {
		[constraint](const GeneratorContext<PinData>& context, size_t nodeId) {
			auto& mappedCircuit = context.GetMappedCircuit();
			auto const* node = mappedCircuit.GetNode(nodeId);
			if (!mappedCircuit.IsSecondaryInput(node)) {
				return LogicConstraint::NO_CONSTRAINT;
			}

			return constraint;
		},
		ClauseType::BmcTarget
	};
}

template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(std::vector<Logic> values)
{
	return {
		[values](const GeneratorContext<PinData>& context, size_t nodeId) {
			auto& mappedCircuit = context.GetMappedCircuit();
			auto const* node = mappedCircuit.GetNode(nodeId);
			if (!mappedCircuit.IsSecondaryInput(node)) {
				return LogicConstraint::NO_CONSTRAINT;
			}

			return GetLogicConstraintForLogic(values[mappedCircuit.GetPrimaryInputNumber(node)]);
		},
		ClauseType::BmcTarget
	};
}

template<typename PinData>
CustomConstraintSpec<PinData> MakeConstantSequentialTarget(std::vector<LogicConstraint> constraints)
{
	return {
		[constraints](const GeneratorContext<PinData>& context, size_t nodeId) {
			auto& mappedCircuit = context.GetMappedCircuit();
			auto const* node = mappedCircuit.GetNode(nodeId);
			if (!mappedCircuit.IsSecondaryInput(node)) {
				return LogicConstraint::NO_CONSTRAINT;
			}

			return constraints[mappedCircuit.GetPrimaryInputNumber(node)];
		},
		ClauseType::BmcTarget
	};
}

template<typename PinData, typename Tag>
CustomConstraintEncoder<PinData, Tag>::CustomConstraintEncoder(CustomConstraintEncoderCallback<PinData> generator, ClauseType clauseType, ConstraintType type, ConstraintTarget target):
	IConstraintEncoder<PinData>(),
	_generator(generator),
	_type(type),
	_target(target),
	_clauseType(clauseType),
	_weight(1u)
{
}

template<typename PinData, typename Tag>
CustomConstraintEncoder<PinData, Tag>::CustomConstraintEncoder(CustomConstraintSpec<PinData> generator, ConstraintType type, ConstraintTarget target):
	CustomConstraintEncoder<PinData, Tag>(std::get<0>(generator), std::get<1>(generator), type, target)
{
}

template<typename PinData, typename Tag>
CustomConstraintEncoder<PinData, Tag>::~CustomConstraintEncoder(void) = default;

template<typename PinData, typename Tag>
std::string CustomConstraintEncoder<PinData, Tag>::GetName(void) const
{
	return "CustomConstraintEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
void CustomConstraintEncoder<PinData, Tag>::SetGenerator(CustomConstraintEncoderCallback<PinData> generator)
{
	_generator = generator;
}

template<typename PinData, typename Tag>
CustomConstraintEncoderCallback<PinData> const& CustomConstraintEncoder<PinData, Tag>::GetGenerator(void) const
{
	return _generator;
}

template<typename PinData, typename Tag>
bool CustomConstraintEncoder<PinData, Tag>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto maxSatSolver = std::dynamic_pointer_cast<MaxSatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	auto interceptor = (_target == ConstraintTarget::PreFaultModel)
		? AllowContainerInterceptor::Disable : AllowContainerInterceptor::Enable;

	const MappedCircuit& circuit = context.GetMappedCircuit();
	LogicEncoder& encoder = context.GetEncoder();

	auto encode_requirement = [&](const auto& container, LogicConstraint constraint) -> void {
		if (_type == ConstraintType::MaximizationConstraint)
		{
			LOG_IF(!maxSatSolver, FATAL) << "A Max-SAT solver is required to do soft constraint encoding";
			auto maximizationLiteral = encoder.EncodeLogicValueConstraintDetector(container, constraint);
			maxSatSolver->CommitSoftClause(maximizationLiteral.l0, _weight);
			return;
		}

		if (satSolver)
		{
			if (context.IsIncremental())
			{
				auto assumption { encoder.EncodeLogicValueConstraintDetector(container, constraint) };
				satSolver->AddAssumption(assumption.l0);
			}
			else
			{
				encoder.EncodeLogicValueConstraint(container, constraint);
			}
		}

		if (bmcSolver)
		{
			encoder.EncodeLogicValueConstraint(container, constraint);
		}
	};

	const auto constrain_timeframe = [&](size_t timeframeId) {
		Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			const auto constraint = _generator(context, nodeId);
			if (constraint == LogicConstraint::NO_CONSTRAINT)
			{
				continue;
			}

			DVLOG(3) << "Constraining node " << circuit.GetNode(nodeId)->GetName()
				<< " / " << get_tag_name_v<Tag> << " with " << to_string(constraint);
			auto container = timeframe.template GetContainer<Tag>(
				context, nodeId, PortType::Output, 0u, interceptor);
			// It can happen that for SAT-instances some parts are not encoded.
			// As we don't want to burden the developer of the workflow excluding
			// the non-encoded parts manually we do this automatically here.
			if (container.IsSet())
			{
				encode_requirement(container, constraint);
			}
		}
	};

	if (satSolver)
	{
		ASSERT(_clauseType == ClauseType::Sat) << "Can not specify BMC clause types for SAT solving!";

		// Encode for all timeframes as it is manually unrolled
		const size_t timeframes = context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding);
		for (size_t timeframe = 0u; timeframe < timeframes; ++timeframe)
		{
			constrain_timeframe(timeframe);
		}
	}

	if (bmcSolver)
	{
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		switch(_clauseType)
		{
			case ClauseType::BmcInitial:
				bmcSolver->SetTargetClauseType(SolverProxy::Bmc::ClauseType::Initial);
				break;
			case ClauseType::BmcTransition:
				bmcSolver->SetTargetClauseType(SolverProxy::Bmc::ClauseType::Transition);
				break;
			case ClauseType::BmcTarget:
				bmcSolver->SetTargetClauseType(SolverProxy::Bmc::ClauseType::Target);
				break;
			default:
				Logging::Panic("Found invalid clause type");
				break;
		}

		constrain_timeframe(0u);
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class CustomConstraintEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

#define FOR_PINDATA(PINDATA) \
	template CustomConstraintSpec<PINDATA> MakeConstantSequentialTarget(Logic logic); \
	template CustomConstraintSpec<PINDATA> MakeConstantSequentialTarget(LogicConstraint constraint); \
	template CustomConstraintSpec<PINDATA> MakeConstantSequentialTarget(std::vector<Logic> values); \
	template CustomConstraintSpec<PINDATA> MakeConstantSequentialTarget(std::vector<LogicConstraint> constraints);
INSTANTIATE_FOR_ALL_PINDATA

};
};
