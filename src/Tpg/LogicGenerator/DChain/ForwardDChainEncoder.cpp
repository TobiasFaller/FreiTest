#include "Tpg/LogicGenerator/DChain/ForwardDChainEncoder.hpp"

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
ForwardDChainEncoder<PinData>::ForwardDChainEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
ForwardDChainEncoder<PinData>::~ForwardDChainEncoder(void) = default;

template<typename PinData>
std::string ForwardDChainEncoder<PinData>::GetName(void) const
{
	return "ForwardDChainEncoder";
}

template<typename PinData>
bool ForwardDChainEncoder<PinData>::EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
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

	// Link the D-Chain forwards to the outputs by using implications
	for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
	{
		// Don't encode anything for nodes which don't have a D-Chain encoding.
		if (!timeframe.template HasTag<DiffTag>(nodeId))
		{
			continue;
		}

		const auto* node = circuit.GetNode(nodeId);

		std::vector<DiffLogicContainer> differences;
		for (auto const* successor : node->GetSuccessors())
		{
			const auto diff = timeframe.template GetInternalContainer<DiffTag>(
				context, successor->GetNodeId());
			ASSERT (diff.IsSet()) << "Found difference color but no difference literal";
			differences.emplace_back(diff);
		}

		if (differences.size() > 0u)
		{
			// Encode the forward chain that forces a difference in at least one successor node,
			// if this node has a difference (the fault has to be propagated).
			// Encode an implication like diff => (d1 v d2 v ... v dn)
			// Directly use the d1 to dn containers stored in the pin data since the fault generator
			// may intercept the internal difference literal and terminate a propagation path.
			const auto diff = timeframe.template GetContainer<DiffTag>(
				context, nodeId, PortType::Output, 0u);
			encoder.EncodeImplication(diff, encoder.EncodeOr(differences));
		}
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class ForwardDChainEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
