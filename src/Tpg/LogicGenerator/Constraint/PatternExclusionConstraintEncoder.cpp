#include "Tpg/LogicGenerator/Constraint/PatternExclusionConstraintEncoder.hpp"

#include <vector>
#include <utility>

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
using namespace FreiTest::Pattern;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
PatternExclusionConstraintEncoder<PinData>::PatternExclusionConstraintEncoder(std::shared_ptr<std::vector<TestPattern>> excludedPatterns, Pattern::InputCapture inputCapture):
	IConstraintEncoder<PinData>(),
	_excludedPatterns(excludedPatterns),
	_inputCapture(inputCapture)
{
}

template<typename PinData>
PatternExclusionConstraintEncoder<PinData>::~PatternExclusionConstraintEncoder(void) = default;

template<typename PinData>
std::string PatternExclusionConstraintEncoder<PinData>::GetName(void) const
{
	return "PatternExclusionConstraintEncoder";
}

template<typename PinData>
bool PatternExclusionConstraintEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	if (_excludedPatterns->size() == 0u)
	{
		return true;
	}

	const auto& circuit = context.GetMappedCircuit();
	auto& encoder = context.GetEncoder();

	auto get_constraints_for_timeframe = [&](const TestPattern& pattern, size_t timeframeId) {
		auto& timeframe = context.GetTimeframe(timeframeId);

		size_t maxEncodePrimaryInputsTimeframe = pattern.GetNumberOfTimeframes();
		size_t maxEncodeSecondaryInputsTimeframe = pattern.GetNumberOfTimeframes();
		switch (_inputCapture)
		{
			case Pattern::InputCapture::PrimaryInputsOnly:
				maxEncodeSecondaryInputsTimeframe = 0u;
				break;
			case Pattern::InputCapture::SecondaryInputsOnly:
				maxEncodePrimaryInputsTimeframe = 0u;
				break;
			case Pattern::InputCapture::PrimaryAndInitialSecondaryInputs:
				maxEncodeSecondaryInputsTimeframe = 1u;
				break;
			case Pattern::InputCapture::PrimaryAndSecondaryInputs:
				break;
			default:
				Logging::Panic();
				break;
		}

		std::vector<LogicContainer01> constraints;
		for (size_t primaryIndex = 0u; timeframeId < maxEncodePrimaryInputsTimeframe && primaryIndex < circuit.GetNumberOfPrimaryInputs(); ++primaryIndex)
		{
			size_t nodeId = circuit.GetPrimaryInput(primaryIndex)->GetNodeId();
			if (!timeframe.template HasTag<GoodTag>(nodeId))
			{
				continue;
			}

			Logic value = pattern.GetPrimaryInput(timeframeId, primaryIndex);
			if (value != Logic::LOGIC_ZERO && value != Logic::LOGIC_ONE)
			{
				continue;
			}

			auto goodContainer = timeframe.template GetContainer<GoodTag>(context, nodeId, PortType::Output, 0u);
			constraints.push_back(encoder.EncodeLogicValueDetector(goodContainer, value));
		}

		for (size_t secondaryIndex = 0u; timeframeId < maxEncodeSecondaryInputsTimeframe && secondaryIndex < circuit.GetNumberOfSecondaryInputs(); ++secondaryIndex)
		{
			size_t nodeId = circuit.GetSecondaryInput(secondaryIndex)->GetNodeId();
			if (!timeframe.template HasTag<GoodTag>(nodeId))
			{
				continue;
			}

			Logic value = pattern.GetSecondaryInput(timeframeId, secondaryIndex);
			if (value != Logic::LOGIC_ZERO && value != Logic::LOGIC_ONE)
			{
				continue;
			}

			auto goodContainer = timeframe.template GetContainer<GoodTag>(context, nodeId, PortType::Output, 0u);
			constraints.push_back(encoder.EncodeLogicValueDetector(goodContainer, value));
		}

		return constraints;
	};

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	for (const TestPattern& pattern : *_excludedPatterns)
	{
		if (pattern.GetNumberOfTimeframes() > context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		{
			continue;
		}

		if (satSolver)
		{
			std::vector<LogicContainer01> constraints;
			for (size_t timeframeId = 0u; timeframeId < pattern.GetNumberOfTimeframes(); ++timeframeId)
			{
				auto timeframeConstraints = get_constraints_for_timeframe(pattern, timeframeId);
				if (timeframeConstraints.size() > 0u)
				{
					constraints.push_back(encoder.EncodeAnd(timeframeConstraints));
				}
			}

			if (context.IsIncremental())
			{
				auto assumption { -encoder.EncodeAnd(constraints) };
				satSolver->AddAssumption(assumption.l0);
			}
			else
			{
				encoder.EncodeLogicValue(encoder.EncodeAnd(constraints), Logic::LOGIC_ZERO);
			}
		}

		if (bmcSolver)
		{
			UnaryCounter& counter = context.GetUnaryCounter();

			std::vector<LogicContainer01> constraints;
			for (size_t timeframeId = 0u; timeframeId < pattern.GetNumberOfTimeframes(); ++timeframeId)
			{
				auto timeframeConstraints = get_constraints_for_timeframe(pattern, timeframeId);
				if (timeframeConstraints.size() > 0u)
				{
					// This timeframe is active if the unary counter is 1 at the timeframe index and 0 at the next index.
					auto timeframeActive = encoder.EncodeXor(
							counter.GetIndicatorForIndex(timeframeId),
							counter.GetIndicatorForIndex(timeframeId + 1u)
						);
					constraints.push_back(encoder.EncodeAnd(timeframeActive, encoder.EncodeAnd(timeframeConstraints)));
				}
			}

			// Encode that the current pattern is disallowed
			encoder.EncodeLogicValue(encoder.EncodeAnd(constraints), Logic::LOGIC_ZERO);
		}
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class PatternExclusionConstraintEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
