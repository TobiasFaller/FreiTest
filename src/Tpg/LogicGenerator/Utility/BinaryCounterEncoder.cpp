#include "Tpg/LogicGenerator/Utility/BinaryCounterEncoder.hpp"

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/BinaryCounter.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

#include <cmath>

using namespace FreiTest::Basic;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
BinaryCounterEncoder<PinData>::BinaryCounterEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
BinaryCounterEncoder<PinData>::~BinaryCounterEncoder(void) = default;

template<typename PinData>
std::string BinaryCounterEncoder<PinData>::GetName(void) const
{
	return "BinaryCounterEncoder";
}

template<typename PinData>
bool BinaryCounterEncoder<PinData>::PreEncodeCircuit(GeneratorContext<PinData>& context)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		auto& counter { context.GetBinaryCounter() };
		auto& encoder { context.GetEncoder() };

		// Resize the binary counter to match the maximum number of timeframes.
		// Use the number of timeframes for constraint generation here as the binary counter is only
		// used for constraint encoding and requires as many indicator variables as there are timeframes
		// for the constraint encoding.
		const auto timeframes { context.GetNumberOfTimeframesForTarget(GenerationTarget::ConstraintEncoding) };
		const auto initialBits { counter.GetBits() };
		const auto requiredBits { std::max((timeframes <= 1u) ? timeframes : static_cast<size_t>(std::log2(timeframes - 1u) + 1u), initialBits) };
		counter.SetBits(requiredBits);

		// Encode the necessary bits that have not been encoded yet.
		// Continue the encoding when more variables are required because the number of timeframes have been increased.
		for (auto bitIndex { initialBits }; bitIndex < requiredBits; ++bitIndex)
		{
			// ----------------------------------------------------------------
			// Initial State
			// ----------------------------------------------------------------

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Latch);
			auto bit = encoder.template NewLogicContainer<LogicContainer01>();
			counter.SetBit(bitIndex, bit);

			// The counter variables start with LOGIC_ZERO.
			bmcSolver->SetTargetClauseType(ClauseType::Initial);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			encoder.EncodeLogicValue(bit, Logic::LOGIC_ZERO);

			// ----------------------------------------------------------------
			// Next State
			// ----------------------------------------------------------------

			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

			auto sumIn = bit;
			auto carryIn = (bitIndex == 0u)
				? encoder.template NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE)
				: counter.GetCarry(bitIndex - 1u);

			auto sumOut = encoder.EncodeXor(carryIn, sumIn);
			auto carryOut = encoder.EncodeAnd(carryIn, sumIn);
			bmcSolver->CommitTimeframeClause( sumOut.l0, 0u, -bit.l0, 1u);
			bmcSolver->CommitTimeframeClause(-sumOut.l0, 0u,  bit.l0, 1u);
			counter.SetCarry(bitIndex, carryOut);
		}
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class BinaryCounterEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
