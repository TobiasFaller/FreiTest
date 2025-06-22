#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CellLibrary.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

using namespace SolverProxy::Bmc;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
BackwardDChainEncoder<PinData>::BackwardDChainEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
BackwardDChainEncoder<PinData>::~BackwardDChainEncoder(void) = default;

template<typename PinData>
std::string BackwardDChainEncoder<PinData>::GetName(void) const
{
	return "BackwardDChainEncoder";
}

template<typename PinData>
bool BackwardDChainEncoder<PinData>::EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	using DiffLogicContainer = get_pin_data_container_t<PinData, DiffTag>;

	const MappedCircuit& circuit = context.GetMappedCircuit();
	Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);
	LogicEncoder& encoder = context.GetEncoder();

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	// Link the D-Chain backwards to the inputs by using implications
	for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
	{
		// Don't encode anything for nodes which don't have a D-Chain encoding.
		if (!timeframe.template HasTag<DiffTag>(nodeId))
		{
			continue;
		}

		const auto* node = circuit.GetNode(nodeId);

		std::vector<DiffLogicContainer> differences;
		for (size_t inputId = 0u; inputId < node->GetNumberOfInputs(); ++inputId)
		{
			if (!timeframe.template HasTag<DiffTag>(node->GetInput(inputId)->GetNodeId()))
			{
				continue;
			}

			const auto diff = timeframe.template GetContainer<DiffTag>(
				context, nodeId, PortType::Input, inputId);
			if (!diff.IsSet())
			{
				LOG(FATAL) << "Found difference color but no difference literal";
			}
			differences.emplace_back(diff);
		}

		if (differences.size() > 0u)
		{
			// Encode the backward chain that forces a difference for at least one of the predecessor nodes.
			// Encode implication like diff => (d1 v d2 v ... v dn)
			// Directly use the diff container stored in the pin data since the fault generator may
			// intercept the internal difference literal.

			// The fault generator is expected to intercept the original difference logic container.
			// Hence we can simply disregard this special case as the implications further
			// down the d-chain will never reach the internal difference literal.
			//
			// Example:
			//
			// The implication will force the D-Chain to propagate from D3 to D1 or D2 for the AND gate.
			// However, since the fault generator proxies the output of the gate with difference literal D4
			// the difference literal D3 will never be reached by the implications of the BUF gate following.
			//
			// D1: Difference literal of AND input I1
			// D2: Difference literal of AND input I2
			// D3: Difference literal of AND output O (unused)
			// D4: Difference literal of AND output O (new)
			// D5: Difference literal of BUF output O
			//
			//                          SA-1 Fault
			//                          at output O
			//       +--------------+      |    +-------------+
			//       |     AND      |      |    |     BUF     |
			//  -----+I1            |      |    |             |
			//    D1 | <------.     |      V    |             |
			//  -----+I2       |   O+----  1----+I           O+----
			//    D2 | <-------+--- | D3   D4   | <---------- | D5
			//       |   Implies    |  A        |   Implies   |
			//       |              |  |        |             |
			//       +--------------+  |        +-------------+
			//                    Unused logic
			//                      container
			//               (is always intercepted)
			//
			auto& diff = timeframe.template GetInternalContainer<DiffTag>(context, nodeId);
			encoder.EncodeImplication(diff, encoder.EncodeOr(differences));
		}
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class BackwardDChainEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
