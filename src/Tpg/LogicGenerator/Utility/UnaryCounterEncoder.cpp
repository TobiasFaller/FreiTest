#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "Tpg/LogicGenerator/UnaryCounter.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
UnaryCounterEncoder<PinData>::UnaryCounterEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
UnaryCounterEncoder<PinData>::~UnaryCounterEncoder(void) = default;

template<typename PinData>
std::string UnaryCounterEncoder<PinData>::GetName(void) const
{
	return "UnaryCounterEncoder";
}

template<typename PinData>
bool UnaryCounterEncoder<PinData>::PreEncodeCircuit(GeneratorContext<PinData>& context)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		UnaryCounter& counter { context.GetUnaryCounter() };
		LogicEncoder& encoder { context.GetEncoder() };
		const size_t initialTimeframes { counter.GetBits() };

		// Resize the unary counter to match the maximum number of timeframes.
		// Add an additional timeframe so that the last timeframe can be detected
		// by XORing the last state variables.
		// Use the number of timeframes for constraint generation here as the unary counter is only
		// used for constraint encoding and requires as many indicator variables as there are timeframes
		// for the constraint encoding.
		const size_t requiredBits { context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) };
		counter.SetBits(requiredBits);

		// Encode the necessary state variables that have not been encoded yet.
		// Continue the encoding when more variables are required because the number of timeframes have been increased.
		for (size_t stateIndex { initialTimeframes }; stateIndex < requiredBits + 1u; ++stateIndex)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);

			LogicContainer01 state = encoder.template NewLogicContainer<LogicContainer01>();
			counter.SetStateForIndex(stateIndex, state);

			// The first counter variable starts with LOGIC_ONE.
			// All other variables start with LOGIC_ZERO.
			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(state, (stateIndex == 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

			/* Encode the walking "1" front of the unary counter.
			 * Each new timeframe the solver is forced to assign an additional
			 * "1" value to the next state variable.
             *
			 * State:       S0 S1 S2 S3 S4
			 * Timeframe 1: 1  0  0  0  0
			 * Timeframe 2: 1  1  0  0  0
			 * Timeframe 3: 1  1  1  0  0
			 * Timeframe 4: 1  1  1  1  0
			 */
			LogicContainer01 previousState = (stateIndex > 0u)
				// The state variable receives the logic value from
				// the previous state variable in previous timeframe.
				? counter.GetStateForIndex(stateIndex - 1u)
				// The first state variable is constant one for simplicity.
				: encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE);
			bmcSolver->CommitTimeframeClause( previousState.l0, 0u, -state.l0, 1u);
			bmcSolver->CommitTimeframeClause(-previousState.l0, 0u,  state.l0, 1u);
		}

		for (size_t indicatorIndex = initialTimeframes; indicatorIndex < requiredBits; ++indicatorIndex)
		{
			/*
			 * State:       S0 S1 S2 S3 S4
			 * Timeframe 1: 1  0  0  0  0
			 * Timeframe 2: 1  1  0  0  0
			 * Timeframe 3: 1  1  1  0  0
			 * Timeframe 4: 1  1  1  1  0
			 *
			 * Use an XOR-gate to generate a logic 1 if the state is active:
			 * Indicator:   I0 I1 I2 I3
			 * Timeframe 1: 1  0  0  0
			 * Timeframe 2: 0  1  0  0
			 * Timeframe 3: 0  0  1  0
			 * Timeframe 4: 0  0  0  1
			 */

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);

			LogicContainer01 indicator = encoder.NewLogicContainer<LogicContainer01>();
			counter.SetIndicatorForIndex(indicatorIndex, indicator);

			// The first indicator variable starts with LOGIC_ONE.
			// All other variables start with LOGIC_ZERO.
			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(indicator, (indicatorIndex == 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

			// The new state of the indicator variable is the XOR of the two adjacent state variables (of the previous timeframe).
			// We don't use an XOR here as the same effect is achieved by shifting the single bit.
			LogicContainer01 newState = (indicatorIndex == 0u)
				? encoder.NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO)
				: counter.GetIndicatorForIndex(indicatorIndex - 1u);
			bmcSolver->CommitTimeframeClause( newState.l0, 0u, -indicator.l0, 1u);
			bmcSolver->CommitTimeframeClause(-newState.l0, 0u,  indicator.l0, 1u);
		}
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class UnaryCounterEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
