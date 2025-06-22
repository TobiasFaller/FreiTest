#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Fault;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::StuckAtSensitizationConstraintEncoder(std::shared_ptr<FaultModel> faultModel, StuckAtSensitizationTimeframe targetTimeframe):
	IConstraintEncoder<PinData>(),
	_faultModel(faultModel),
	_targetTimeframe(targetTimeframe),
	_allowEmptySensitization(false),
	_sensitizationType(StuckAtSensitizationType::All),
	_sensitizations()
{
}

template<typename PinData, typename FaultModel>
StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::~StuckAtSensitizationConstraintEncoder(void) = default;

template<typename PinData, typename FaultModel>
void StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::SetAllowEmptySensitization(bool enable)
{
	_allowEmptySensitization = enable;
}

template<typename PinData, typename FaultModel>
const bool& StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::IsAllowEmptySensitization(void) const
{
	return _allowEmptySensitization;
}

template<typename PinData, typename FaultModel>
void StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::SetStuckAtSensitizationType(StuckAtSensitizationType type)
{
	_sensitizationType = type;
}

template<typename PinData, typename FaultModel>
const StuckAtSensitizationType& StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::GetStuckAtSensitizationType(void) const
{
	return _sensitizationType;
}

template<typename PinData, typename FaultModel>
std::string StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::GetName(void) const
{
	return "StuckAtSensitizationConstraintEncoder";
}

template<typename PinData, typename FaultModel>
void StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::Reset(void)
{
	_sensitizations.clear();
}

template<typename PinData, typename FaultModel>
bool StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if (context.IsTimeframeCompletedForTarget(timeframeId, GenerationTarget::ConstraintEncoding))
	{
		return true;
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		if (timeframeId != 0)
		{
			return true;
		}

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	auto& encoder { context.GetEncoder() };
	auto const& fault { _faultModel->GetFault() };

	auto get_stuck_ats = [&]() -> std::vector<Fault::StuckAtFault> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			return { fault->GetStuckAt() };
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
		{
			return fault->GetStuckAts();
		}
	};

	std::vector<LogicContainer01> constraints;
	for (auto const& stuckAt : get_stuck_ats())
	{
		auto& timeframe = context.GetTimeframe(timeframeId);
		auto& [node, port] = stuckAt.GetNodeAndPort();
		auto logicContainer = timeframe.template GetContainer<BadTag>(
			context, node->GetNodeId(), port.portType, port.portNumber,
			AllowContainerInterceptor::Disable);

		switch(stuckAt.GetType())
		{
			case StuckAtFaultType::STUCK_AT_0:
				constraints.push_back(encoder.EncodeLogicValueDetector(logicContainer, Logic::LOGIC_ONE));
				break;

			case StuckAtFaultType::STUCK_AT_1:
				constraints.push_back(encoder.EncodeLogicValueDetector(logicContainer, Logic::LOGIC_ZERO));
				break;

			case StuckAtFaultType::STUCK_AT_X:
			case StuckAtFaultType::STUCK_AT_U:
			case StuckAtFaultType::STUCK_AT_FREE:
				LOG(FATAL) << "The stuck-at fault type " << to_string(stuckAt.GetType())
					<< " is currently not supported by the StuckAtSensitizationConstraintEncoder";
				__builtin_unreachable();
				break;
		}
	}

	switch (_sensitizationType)
	{
		case StuckAtSensitizationType::All:
			_sensitizations.push_back(encoder.EncodeAnd(constraints));
			break;
		case StuckAtSensitizationType::One:
			_sensitizations.push_back(encoder.EncodeOr(constraints));
			break;
		default: Logging::Panic("Unknown sensitization type");
	}

	return true;
}

template<typename PinData, typename FaultModel>
bool StuckAtSensitizationConstraintEncoder<PinData, FaultModel>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	DASSERT(context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) > 0u) << "No timeframes have been generated";
	DASSERT(_sensitizations.size() == context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		<< "The fault sensitization module was not invoked for all timeframes";

	LogicEncoder& encoder = context.GetEncoder();
	if (!_allowEmptySensitization)
	{
		const LogicContainer01 constantZero = encoder.NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
		if ((_targetTimeframe == StuckAtSensitizationTimeframe::Last && _sensitizations.back() == constantZero)
			|| std::all_of(_sensitizations.cbegin(), _sensitizations.cend(), [&constantZero](const LogicContainer01& value) {
				return value == constantZero;
			}))
		{
			DVLOG(3) << "There are no gates that have a difference for the fault";
			return false;
		}
	}

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver)
	{
		DVLOG(3) << "Forcing the timeframe " << (_sensitizations.size() - 1u)
			<< " to have a difference at a the fault locations";
		if (context.IsIncremental())
		{
			auto assumption {
				encoder.EncodeLogicValueDetector(
					_targetTimeframe == StuckAtSensitizationTimeframe::Last
						? _sensitizations.back()
						: encoder.EncodeOr(_sensitizations),
					Logic::LOGIC_ONE
				)
			};
			satSolver->AddAssumption(assumption.l0);
		}
		else
		{
			encoder.EncodeLogicValue(
				_targetTimeframe == StuckAtSensitizationTimeframe::Last
					? _sensitizations.back()
					: encoder.EncodeOr(_sensitizations),
				Logic::LOGIC_ONE);
		}
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Latch);
		auto sensitizationLatch = encoder.NewLogicContainer<LogicContainer01>();

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		auto differenceOutput = (_targetTimeframe == StuckAtSensitizationTimeframe::Last)
			? _sensitizations.back()
			// The sensitization does not need to happen in the last timeframe.
			// Generate OR-gate to include the previous output value of the latch.
			: encoder.EncodeOr(_sensitizations.back(), sensitizationLatch);

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		bmcSolver->CommitTimeframeClause( differenceOutput.l0, 0u, -sensitizationLatch.l0, 1u);
		bmcSolver->CommitTimeframeClause(-differenceOutput.l0, 0u,  sensitizationLatch.l0, 1u);

		// Initialize fault sensitization to 0
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ZERO);

		// Require fault sensitization to be 1
		bmcSolver->SetTargetClauseType(ClauseType::Target);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ONE);
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) \
	template class StuckAtSensitizationConstraintEncoder<PinDataGB<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtSensitizationConstraintEncoder<PinDataGBD<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtSensitizationConstraintEncoder<PinDataGB<CONTAINER>, Fault::MultiStuckAtFaultModel>; \
	template class StuckAtSensitizationConstraintEncoder<PinDataGBD<CONTAINER>, Fault::MultiStuckAtFaultModel>;
INSTANTIATE_ALL_CONTAINER

};
};
