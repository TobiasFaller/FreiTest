#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
FaultPropagationConstraintEncoder<PinData>::FaultPropagationConstraintEncoder(FaultPropagationTarget target, FaultPropagationTimeframe targetTimeframe):
    IConstraintEncoder<PinData>(),
    _target(target),
	_targetTimeframe(targetTimeframe),
	_allowEmptyPropagation(false),
    _primaryDifferenceOutputs(),
	_secondaryDifferenceOutputs()
{
}

template<typename PinData>
FaultPropagationConstraintEncoder<PinData>::~FaultPropagationConstraintEncoder(void) = default;

template<typename PinData>
void FaultPropagationConstraintEncoder<PinData>::SetAllowEmptyPropagation(bool enable)
{
	_allowEmptyPropagation = enable;
}

template<typename PinData>
bool FaultPropagationConstraintEncoder<PinData>::IsAllowEmptyPropagation(void) const
{
	return _allowEmptyPropagation;
}

template<typename PinData>
std::string FaultPropagationConstraintEncoder<PinData>::GetName(void) const
{
    return "FaultPropagationConstraintEncoder";
}

template<typename PinData>
void FaultPropagationConstraintEncoder<PinData>::Reset(void)
{
    _primaryDifferenceOutputs.clear();
    _secondaryDifferenceOutputs.clear();
}

template<typename PinData>
bool FaultPropagationConstraintEncoder<PinData>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if (context.IsTimeframeCompletedForTarget(timeframeId, GenerationTarget::ConstraintEncoding))
	{
		return true;
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		if (timeframeId != 0u)
		{
			return true;
		}
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

    const auto& circuit = context.GetMappedCircuit();
    auto& timeframe = context.GetTimeframe(timeframeId);
    auto& encoder = context.GetEncoder();

	auto encode_propagation_requirement = [&](size_t nodeId, std::vector<LogicContainer01>& target) {
		const auto container = timeframe.template GetContainer<DiffTag>(
			context, nodeId, PortType::Output, 0u);
		// It can happen that for SAT-instances some parts are not encoded.
		// As we don't want to burden the developer of the workflow excluding
		// the non-encoded parts manually we do this automatically here.
		if (container.IsSet())
		{
			target.push_back(container);
		}
	};

	std::vector<LogicContainer01> primaryDifferences;
	for (auto [index, primaryOutput] : circuit.EnumeratePrimaryOutputs())
	{
		DVLOG(6) << "Adding primary output " << index << ": " << primaryOutput->GetName();
		encode_propagation_requirement(primaryOutput->GetNodeId(), primaryDifferences);
	}

	std::vector<LogicContainer01> secondaryDifferences;
	for (auto [index, secondaryOutput] : circuit.EnumerateSecondaryOutputs())
	{
		DVLOG(6) << "Adding secondary output " << index << ": " << secondaryOutput->GetName();
		encode_propagation_requirement(secondaryOutput->GetNodeId(), secondaryDifferences);
	}

	VLOG(3) << "There are " << primaryDifferences.size() << " primary and " << secondaryDifferences.size()
		<< " secondary outputs that could show a difference in timeframe " << timeframeId;

	_primaryDifferenceOutputs.push_back((primaryDifferences.size() > 0u)
		// Require that at least one primary output shows a difference
		? encoder.EncodeOr(primaryDifferences)
		// This timeframe has no difference at the primary outputs,
		// but another time frame might have a difference output
		: encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO));

	_secondaryDifferenceOutputs.push_back((secondaryDifferences.size() > 0u)
		// Require that at least one secondary output shows a difference
		? encoder.EncodeOr(secondaryDifferences)
		// This timeframe has no difference at the secondary outputs,
		// but another time frame might have a difference output
		: encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO));

    return true;
}

template<typename PinData>
bool FaultPropagationConstraintEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	DASSERT(context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) > 0u) << "No timeframes have been generated";
	DASSERT(_primaryDifferenceOutputs.size() == context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		<< "The fault propagation module was not invoked for all timeframes";
	DASSERT(_secondaryDifferenceOutputs.size() == context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		<< "The fault propagation module was not invoked for all timeframes";

    LogicEncoder& encoder = context.GetEncoder();
	const LogicContainer01 constantZero = encoder.NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
	const auto is_all_zero = [&constantZero](auto begin, auto end) -> bool {
		return std::all_of(begin, end, [&constantZero](const LogicContainer01& value) {
			return value == constantZero;
		});
	};

	if (!_allowEmptyPropagation)
	{
		switch (_target)
		{
			case FaultPropagationTarget::PrimaryOutputsOnly:
				if (// If there is no primary output difference in the last timeframe this target is unsatisfiable.
					(_targetTimeframe == FaultPropagationTimeframe::Last && is_all_zero(_primaryDifferenceOutputs.cend() - 1u, _primaryDifferenceOutputs.cend()))
					// If there is no primary output difference in any timeframe this target is unsatisfiable.
					|| (_targetTimeframe == FaultPropagationTimeframe::Any && is_all_zero(_primaryDifferenceOutputs.cbegin(), _primaryDifferenceOutputs.cend())))
				{
					DVLOG(3) << "There are no primary outputs which could show a difference for the fault";
					return false;
				}
				break;

			case FaultPropagationTarget::SecondaryOutputsOnly:
				if (// If there is no secondary output difference in the last timeframe this target is unsatisfiable.
					(_targetTimeframe == FaultPropagationTimeframe::Last && is_all_zero(_secondaryDifferenceOutputs.cend() - 1u, _secondaryDifferenceOutputs.cend()))
					// If there is no secondary output difference in the any timeframe this target is unsatisfiable.
					|| (_targetTimeframe == FaultPropagationTimeframe::Any && is_all_zero(_secondaryDifferenceOutputs.cbegin(), _secondaryDifferenceOutputs.cend())))
				{
					DVLOG(3) << "There are no secondary outputs which could show a difference for the fault";
					return false;
				}
				break;

			case FaultPropagationTarget::PrimaryAndSecondaryOutputs:
				if (// If there is no primary or secondary output difference in the last timeframe this target is unsatisfiable.
					(_targetTimeframe == FaultPropagationTimeframe::Last
						&& is_all_zero(_primaryDifferenceOutputs.cend() - 1u, _primaryDifferenceOutputs.cend())
						&& is_all_zero(_secondaryDifferenceOutputs.cend() - 1u, _secondaryDifferenceOutputs.cend()))
					// If there is no primary or secondary output difference in the last timeframe this target is unsatisfiable.
					|| (_targetTimeframe == FaultPropagationTimeframe::Any && (
						is_all_zero(_primaryDifferenceOutputs.cbegin(), _primaryDifferenceOutputs.cend()))
						&& is_all_zero(_secondaryDifferenceOutputs.cbegin(), _secondaryDifferenceOutputs.cend())))
				{
					DVLOG(3) << "There are no primary or secondary outputs which could show a difference for the fault";
					return false;
				}
				break;

			default:
				Logging::Panic("Unsupported propagation target");
				break;
		}
	}

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver)
	{
		const auto force_logic1 = [&](LogicContainer01 constraint) {
			if (context.IsIncremental())
			{
				auto assumption { encoder.EncodeLogicValueDetector(constraint, Logic::LOGIC_ONE) };
				satSolver->AddAssumption(assumption.l0);
			}
			else
			{
				encoder.EncodeLogicValue(constraint, Logic::LOGIC_ONE);
			}
		};

		switch (_target)
		{
		case FaultPropagationTarget::PrimaryOutputsOnly:
			DVLOG(3) << "Forcing the timeframe " << (_primaryDifferenceOutputs.size() - 1u)
				<< " to have a difference at a primary output";
			force_logic1(_targetTimeframe == FaultPropagationTimeframe::Last
				// Force the last timeframe to have a difference at any primary output
				? _primaryDifferenceOutputs.back()
				// Force any timeframe to have a difference at any primary output
				: encoder.EncodeOr(_primaryDifferenceOutputs));
			break;

		case FaultPropagationTarget::SecondaryOutputsOnly:
			DVLOG(3) << "Forcing the timeframe " << (_primaryDifferenceOutputs.size() - 1u)
				<< " to have a difference at a secondary output";
			force_logic1(_targetTimeframe == FaultPropagationTimeframe::Last
				// Force the last timeframe to have a difference at any secondary output
				? _secondaryDifferenceOutputs.back()
				// Force any timeframe to have a difference at any secondary output
				: encoder.EncodeOr(_secondaryDifferenceOutputs));
			break;

		case FaultPropagationTarget::PrimaryAndSecondaryOutputs:
			DVLOG(3) << "Forcing the timeframe " << (_primaryDifferenceOutputs.size() - 1u)
				<< " to have a difference at a primary or secondary output";
			force_logic1(_targetTimeframe == FaultPropagationTimeframe::Last
				// Force the last timeframe to have a difference at any primary or secondary output
				? encoder.EncodeOr(_primaryDifferenceOutputs.back(), _secondaryDifferenceOutputs.back())
				// Force any timeframe to have a difference at any primary or secondary output
				: encoder.EncodeOr(
					encoder.EncodeOr(_primaryDifferenceOutputs),
					encoder.EncodeOr(_secondaryDifferenceOutputs)));
			break;

		default:
			Logging::Panic("Unsupported propagation target");
			break;
		}
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Latch);
		auto propagationLatch = encoder.NewLogicContainer<LogicContainer01>();

		// Generate OR-tree for the final propagation latch
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

		LogicContainer01 differenceOutput;
		switch (_target)
		{
		case FaultPropagationTarget::PrimaryOutputsOnly:
			differenceOutput = _primaryDifferenceOutputs.back();
			break;

		case FaultPropagationTarget::SecondaryOutputsOnly:
			differenceOutput = _secondaryDifferenceOutputs.back();
			break;

		case FaultPropagationTarget::PrimaryAndSecondaryOutputs:
			differenceOutput = encoder.EncodeOr(_primaryDifferenceOutputs.back(), _secondaryDifferenceOutputs.back());
			break;

		default:
			Logging::Panic("Unsupported propagation target");
			break;
		}

		if (_targetTimeframe == FaultPropagationTimeframe::Any)
		{
			// The propagagation does not need to happen in the last timeframe.
			// Generate OR-gate to include the previous output value of the latch.
			differenceOutput = encoder.EncodeOr(differenceOutput, propagationLatch);
		}

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		bmcSolver->CommitTimeframeClause( differenceOutput.l0, 0u, -propagationLatch.l0, 1u);
		bmcSolver->CommitTimeframeClause(-differenceOutput.l0, 0u,  propagationLatch.l0, 1u);

		// Initialize fault propagation to 0
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(propagationLatch, Logic::LOGIC_ZERO);

		// Require fault propagation to be 1
		bmcSolver->SetTargetClauseType(ClauseType::Target);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(propagationLatch, Logic::LOGIC_ONE);
	}

    return true;
}

template<typename PinData>
const std::vector<LogicContainer01>& FaultPropagationConstraintEncoder<PinData>::GetGeneratedPrimaryConstraints(void) const
{
	return _primaryDifferenceOutputs;
}

template<typename PinData>
const std::vector<LogicContainer01>& FaultPropagationConstraintEncoder<PinData>::GetGeneratedSecondaryConstraints(void) const
{
	return _secondaryDifferenceOutputs;
}

#define FOR_CONTAINER(CONTAINER) \
	template class FaultPropagationConstraintEncoder<PinDataGB<CONTAINER>>; \
	template class FaultPropagationConstraintEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
