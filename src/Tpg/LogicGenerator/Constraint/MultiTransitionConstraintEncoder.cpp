#include "Tpg/LogicGenerator/Constraint/MultiTransitionConstraintEncoder.hpp"

#include <type_traits>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace FreiTest::Io::Udfm;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

TransitionTarget::TransitionTarget(Circuit::MappedCircuit::NodeAndPort target, size_t toggleCount):
	target(target),
	toggleCount(toggleCount)
{
}
TransitionTarget::~TransitionTarget(void) = default;

const Circuit::MappedCircuit::NodeAndPort& TransitionTarget::GetTarget(void) const
{
	return target;
}

const size_t& TransitionTarget::GetTransitionCount() const
{
	return toggleCount;
}

template<typename PinData>
MultiTransitionConstraintEncoder<PinData>::MultiTransitionConstraintEncoder(Circuit::MappedCircuit::NodeAndPort target, size_t toggleCount):
	IConstraintEncoder<PinData>(),
	targets({ { target, toggleCount } }),
	toggleCountLiterals()
{
}

template<typename PinData>
MultiTransitionConstraintEncoder<PinData>::MultiTransitionConstraintEncoder(std::vector<TransitionTarget> targets):
	IConstraintEncoder<PinData>(),
	targets(targets),
	toggleCountLiterals()
{
}

template<typename PinData >
MultiTransitionConstraintEncoder<PinData>::~MultiTransitionConstraintEncoder(void) = default;

template<typename PinData >
std::string MultiTransitionConstraintEncoder<PinData>::GetName(void) const
{
	return "MultiTransitionConstraintEncoder";
}

static std::vector<Tpg::LogicContainer01> ConditionalIncrement(LogicEncoder& encoder, const std::vector<Tpg::LogicContainer01>& value, Tpg::LogicContainer01 increment)
{
	std::vector<LogicContainer01> result;
	result.reserve(value.size() + 1u);

	for (size_t index { 0u }; index < value.size() + 1u; index++)
	{
		result.push_back(encoder.EncodeMultiplexer(
			(index == value.size()) ? encoder.NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO) : value[index     ], // Prepend a zero if not incrementing
			(index == 0u)           ? encoder.NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE)  : value[index - 1u], // Shift by one and append a one if incrementing
			increment
		));
	}

	return result;
}

template<typename PinData>
bool MultiTransitionConstraintEncoder<PinData>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	using GoodContainerType = get_pin_data_container_t<PinData, GoodTag>;

	auto encoder = context.GetEncoder();
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());

	auto const timeframes = context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding);
	auto const& timeframe = context.GetTimeframe(timeframeId);

	// ------------------------------------------------------------------------
	// Extract the previous values from the preceding timeframes.
	// ------------------------------------------------------------------------
	std::vector<GoodContainerType> previousValues;
	previousValues.reserve(targets.size());

	if (satSolver)
	{
		for (const auto& target : targets)
		{
			const auto [node, port] = target.GetTarget();
			const auto& previousTimeframe = (timeframeId != 0u) ? context.GetTimeframe(timeframeId - 1u) : timeframe;
			const auto literal = previousTimeframe.template GetContainer<GoodTag>(
				context, node->GetNodeId(), port.portType, port.portNumber);
			ASSERT(literal.IsSet());
			previousValues.push_back(literal);
		}
	}

	if (bmcSolver)
	{
		if (timeframeId != 0u)
		{
			return true;
		}

		for (const auto& target : targets)
		{
			const auto [node, port] = target.GetTarget();
			const auto container = timeframe.template GetContainer<GoodTag>(
				context, node->GetNodeId(), port.portType, port.portNumber);
			ASSERT(container.IsSet());

			// We use a Latch to store the target's INITIAL value
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);
			auto const& previousValueLatch = previousValues.emplace_back(encoder.template NewLogicContainer<GoodContainerType>());

			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			if constexpr (is_01_encoding_v<GoodContainerType>)
			{
				encoder.EncodeLogicValue(previousValueLatch, Basic::Logic::LOGIC_ZERO);
			}
			else if constexpr (is_01X_encoding_v<GoodContainerType> || is_U01X_encoding_v<GoodContainerType>)
			{
				encoder.EncodeLogicValue(previousValueLatch, Basic::Logic::LOGIC_DONT_CARE);
			}

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			if constexpr (is_01_encoding_v<GoodContainerType>)
			{
				bmcSolver->CommitTimeframeClause( container.l0, 0u, -previousValueLatch.l0, 1u);
				bmcSolver->CommitTimeframeClause(-container.l0, 0u,  previousValueLatch.l0, 1u);
			}
			else if constexpr (is_01X_encoding_v<GoodContainerType> || is_U01X_encoding_v<GoodContainerType>)
			{
				bmcSolver->CommitTimeframeClause( container.l0, 0u, -previousValueLatch.l0, 1u);
				bmcSolver->CommitTimeframeClause(-container.l0, 0u,  previousValueLatch.l0, 1u);
				bmcSolver->CommitTimeframeClause( container.l1, 0u, -previousValueLatch.l1, 1u);
				bmcSolver->CommitTimeframeClause(-container.l1, 0u,  previousValueLatch.l1, 1u);
			}
		}
	}

	// ------------------------------------------------------------------------
	// Encode Unary Incrementers to count transitions.
	// ------------------------------------------------------------------------
	if (toggleCountLiterals.size() <= timeframeId)
	{
		toggleCountLiterals.resize(timeframeId + 1u, { });
	}
	if (auto& counterContainers = toggleCountLiterals[timeframeId]; counterContainers.size() != targets.size())
	{
		counterContainers.resize(targets.size(), { });
		for (size_t targetIndex { 0u }; targetIndex < targets.size(); targetIndex++)
		{
			// Create latches to store the transition counter's value in case we are doing BMC
			if (bmcSolver)
			{
				counterContainers[targetIndex].resize(timeframes + 1u);
				for (size_t index { 0u }; index < timeframes + 1u; index++)
				{
					bmcSolver->SetTargetClauseType(ClauseType::Transition);
					bmcSolver->SetTargetVariableType(VariableType::Latch);
					counterContainers[targetIndex][index] = encoder.template NewLogicContainer<LogicContainer01>();

					bmcSolver->SetTargetClauseType(ClauseType::Initial);
					bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
					encoder.EncodeLogicValue(counterContainers[targetIndex][index], Basic::Logic::LOGIC_ZERO);
				}
			}

			std::vector<LogicContainer01> previousCounterBits;
			if (satSolver)
			{
				if (timeframeId != 0u)
				{
					// When using SAT solving the counter dynamically grows and always uses the
					// previous literals of the previous timeframe.
					previousCounterBits = toggleCountLiterals[timeframeId - 1u][targetIndex];
				}
			}
			if (bmcSolver)
			{
				// When using BMC solving the counter's containers are the same for this single timeframe
				previousCounterBits = counterContainers[targetIndex];
			}

			auto [node, port] = targets[targetIndex].GetTarget();
			const auto container = timeframe.template GetContainer<GoodTag>(
				context, node->GetNodeId(), port.portType, port.portNumber);
			ASSERT(container.IsSet());

			if (bmcSolver)
			{
				// Change the clause type to transition (important) for the following calls to
				// generate the correct types.
				bmcSolver->SetTargetClauseType(ClauseType::Transition);
				bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			}

			auto toggle = encoder.EncodeLogic01DifferenceDetector(container, previousValues[targetIndex]);
			if (bmcSolver)
			{
				// We want to prohibit the BMC solver to detect a "toggle" just because
				// we initialized the latche's values with LOGIC_ZERO in the first timeframe.
				// This required since the 01 encoding doesn't support LOGIC_DONT_CARE
				// as the initial value for those latches.
				auto indicator = context.GetUnaryCounter().GetIndicatorForIndex(0u);
				toggle = encoder.EncodeMultiplexer(toggle, encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO), indicator);
			}

			auto nextCounterValue = ConditionalIncrement(encoder, previousCounterBits, toggle);

			if (satSolver)
			{
				counterContainers[targetIndex] = nextCounterValue;
			}
			if (bmcSolver)
			{
				for (size_t index { 0u }; index < timeframes + 1u; index++)
				{
					bmcSolver->CommitTimeframeClause( nextCounterValue[index].l0, 0u, -counterContainers[targetIndex][index].l0, 1u);
					bmcSolver->CommitTimeframeClause(-nextCounterValue[index].l0, 0u,  counterContainers[targetIndex][index].l0, 1u);
				}
			}
		}
	}

	return true;
}

template<typename PinData>
bool MultiTransitionConstraintEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto encoder = context.GetEncoder();
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());

	for (size_t targetIndex { 0u }; targetIndex < targets.size(); targetIndex++)
	{
		auto const targetTransitionCount = targets[targetIndex].GetTransitionCount();
		auto const& targetLiterals = toggleCountLiterals.back()[targetIndex];
		if (targetTransitionCount + 1u >= targetLiterals.size())
		{
			return false;
		}

		auto const& targetLiteral = targetLiterals[(targetTransitionCount != 0u) ? (targetTransitionCount - 1u) : 0u];
		auto const targetValue = (targetTransitionCount != 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;

		if (satSolver)
		{
			if (context.IsIncremental())
			{
				auto assumption { encoder.EncodeLogicValueDetector(targetLiteral, targetValue) };
				satSolver->AddAssumption(assumption.l0);
			}
			else
			{
				encoder.EncodeLogicValue(targetLiteral, targetValue);
			}
		}

		if (bmcSolver)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Target);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(targetLiteral, targetValue);
		}
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class MultiTransitionConstraintEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
