#include "Tpg/LogicGenerator/Constraint/ConstraintEncoder.hpp"

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

template<typename PinData, typename Tag>
ConstraintEncoder<PinData, Tag>::ConstraintEncoder(ConstraintEncoderCallback<PinData> generator, TimeframeDependency dependency, ConstraintType type, ConstraintTarget target):
	IConstraintEncoder<PinData>(),
	_generator(generator),
	_dependency(dependency),
	_type(type),
	_target(target),
	_weight(1u)
{
}

template<typename PinData, typename Tag>
ConstraintEncoder<PinData, Tag>::ConstraintEncoder(ConstraintSpec<PinData> generator, ConstraintType type, ConstraintTarget target):
	ConstraintEncoder<PinData, Tag>(std::get<0>(generator), std::get<1>(generator), type, target)
{
}

template<typename PinData, typename Tag>
ConstraintEncoder<PinData, Tag>::~ConstraintEncoder(void) = default;

template<typename PinData, typename Tag>
std::string ConstraintEncoder<PinData, Tag>::GetName(void) const
{
	return "ConstraintEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
void ConstraintEncoder<PinData, Tag>::SetGenerator(ConstraintEncoderCallback<PinData> generator)
{
	_generator = generator;
}

template<typename PinData, typename Tag>
ConstraintEncoderCallback<PinData> const& ConstraintEncoder<PinData, Tag>::GetGenerator(void) const
{
	return _generator;
}

template<typename PinData, typename Tag>
void ConstraintEncoder<PinData, Tag>::SetWeight(size_t weight)
{
	_weight = weight;
}

template<typename PinData, typename Tag>
size_t ConstraintEncoder<PinData, Tag>::GetWeight(void) const
{
	return _weight;
}

template<typename PinData, typename Tag>
bool ConstraintEncoder<PinData, Tag>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto maxSatSolver = std::dynamic_pointer_cast<MaxSatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	auto interceptor = (_target == ConstraintTarget::PreFaultModel)
		? AllowContainerInterceptor::Disable : AllowContainerInterceptor::Enable;

	const auto& circuit = context.GetMappedCircuit();
	auto& encoder = context.GetEncoder();

	enum UnaryCounterDependency
	{
		None,
		SpecifiedTimeframe,
		AllOtherTimeframes
	};

	auto encode_requirement = [&](size_t counterTimeframeId, const auto& container, LogicConstraint constraint, UnaryCounterDependency dependency) -> void {
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
			switch (dependency)
			{
				case UnaryCounterDependency::None:
					encoder.EncodeLogicValueConstraint(container, constraint);
					break;

				case UnaryCounterDependency::SpecifiedTimeframe:
				case UnaryCounterDependency::AllOtherTimeframes:
				{
					auto indicator = context.GetUnaryCounter().GetIndicatorForIndex(counterTimeframeId);

					encoder.EncodeLogicValue(
						encoder.EncodeAnd(
							encoder.EncodeLogicValueConstraintDetector(container, constraint),
							(dependency == UnaryCounterDependency::SpecifiedTimeframe)
								? indicator : -indicator
						), Logic::LOGIC_ONE);
					break;
				}
			}
		}
	};

	const size_t timeframes = context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding);
	const auto constrain_timeframe = [&](size_t counterTimeframeId, size_t generatorTimeframeId, UnaryCounterDependency dependency) {
		Timeframe<PinData>& timeframe = context.GetTimeframe(counterTimeframeId);
		for (auto [nodeId, node] : circuit.EnumerateNodes())
		{
			const auto constraint = _generator(context, generatorTimeframeId, nodeId);
			if (constraint == LogicConstraint::NO_CONSTRAINT)
			{
				continue;
			}

			DVLOG(3) << "Constraining node " << node->GetName() << " / " << get_tag_name_v<Tag>
				<< " with " << to_string(constraint) << " in tf " << std::to_string(counterTimeframeId);
			auto container = timeframe.template GetContainer<Tag>(
				context, nodeId, PortType::Output, 0u, interceptor);
			// It can happen that for SAT-instances some parts are not encoded.
			// As we don't want to burden the developer of the workflow excluding
			// the non-encoded parts manually we do this automatically here.
			if (container.IsSet())
			{
				encode_requirement(counterTimeframeId, container, constraint, dependency);
			}
		}
	};

	if (satSolver)
	{
		// Encode for all timeframes as it is manually unrolled
		for (size_t timeframe = 0u; timeframe < timeframes; ++timeframe)
		{
			constrain_timeframe(timeframe, timeframe, UnaryCounterDependency::SpecifiedTimeframe);
		}
	}

	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		constrain_timeframe(0u, 0u, UnaryCounterDependency::None);

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		switch (_dependency)
		{
		case TimeframeDependency::TimeframeDependent:
			for (size_t timeframe = 1u; timeframe < timeframes; ++timeframe)
			{
				constrain_timeframe(timeframe, timeframe, UnaryCounterDependency::SpecifiedTimeframe);
			}
			break;

		case TimeframeDependency::InitThenIndependent:
			constrain_timeframe(0u, 1u, UnaryCounterDependency::AllOtherTimeframes);
			break;

		case TimeframeDependency::TimeframeIndependent:
			constrain_timeframe(0u, 0u, UnaryCounterDependency::None);
			break;
		}
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class ConstraintEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_TAGS

};
};
