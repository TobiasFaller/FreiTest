#include "Tpg/LogicGenerator/Constraint/NetSensitizationConstraintEncoder.hpp"

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

template<typename PinData>
NetSensitizationConstraintEncoder<PinData>::NetSensitizationConstraintEncoder(Circuit::MappedCircuit::NodeAndPort target, Basic::LogicConstraint logicConstraint, NetSensitizationTimeframe targetTimeframe):
	IConstraintEncoder<PinData>(),
	_target(target),
	_logicConstraint(logicConstraint),
	_targetTimeframe(targetTimeframe),
	_constraints()
{
}

template<typename PinData >
NetSensitizationConstraintEncoder<PinData>::~NetSensitizationConstraintEncoder(void) = default;

template<typename PinData >
std::string NetSensitizationConstraintEncoder<PinData>::GetName(void) const
{
	return "NetSensitizationConstraintEncoder";
}

template<typename PinData >
void NetSensitizationConstraintEncoder<PinData>::Reset(void)
{
	_constraints.clear();
}

template<typename PinData >
bool NetSensitizationConstraintEncoder<PinData>::ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if (context.IsTimeframeCompletedForTarget(timeframeId, GenerationTarget::ConstraintEncoding))
	{
		return true;
	}

	auto& timeframe = context.GetTimeframe(timeframeId);
	auto& encoder = context.GetEncoder();

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	auto& [node, port] = _target;
	auto logicContainer = timeframe.template GetContainer<GoodTag>(
		context, node->GetNodeId(), port.portType, port.portNumber);
	_constraints.push_back(encoder.EncodeLogicValueConstraintDetector(logicContainer, _logicConstraint));
	return true;
}

template<typename PinData>
bool NetSensitizationConstraintEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	DASSERT(context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) > 0u) << "No timeframes have been generated";
	DASSERT(_constraints.size() == context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding))
		<< "The net sensitization module was not invoked for all timeframes";

	LogicEncoder& encoder = context.GetEncoder();

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver)
	{
		if (context.IsIncremental())
		{
			auto assumption {
				encoder.EncodeLogicValueDetector(
					_targetTimeframe == NetSensitizationTimeframe::Last
						? _constraints.back()
						: encoder.EncodeOr(_constraints),
					Logic::LOGIC_ONE
				)
			};
			satSolver->AddAssumption(assumption.l0);
		}
		else
		{
			encoder.EncodeLogicValue(
				_targetTimeframe == NetSensitizationTimeframe::Last
					? _constraints.back()
					: encoder.EncodeOr(_constraints),
				Logic::LOGIC_ONE);
		}
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Latch);
		LogicContainer01 sensitizationLatch = encoder.NewLogicContainer<LogicContainer01>();

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		LogicContainer01 sensitizedOutput = (_targetTimeframe == NetSensitizationTimeframe::Last)
			? _constraints.back()
			// The sensitization does not need to happen in the last timeframe.
			// Generate OR-gate to include the previous output value of the latch.
			: encoder.EncodeOr(_constraints.back(), sensitizationLatch);

		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		bmcSolver->CommitTimeframeClause( sensitizedOutput.l0, 0u, -sensitizationLatch.l0, 1u);
		bmcSolver->CommitTimeframeClause(-sensitizedOutput.l0, 0u,  sensitizationLatch.l0, 1u);

		// Initialize net sensitization to 0
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ZERO);

		// Require net sensitization to be 1
		bmcSolver->SetTargetClauseType(ClauseType::Target);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
		encoder.EncodeLogicValue(sensitizationLatch, Logic::LOGIC_ONE);
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class NetSensitizationConstraintEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
