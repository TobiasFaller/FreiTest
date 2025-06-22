#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"

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
DChainBaseEncoder<PinData>::DChainBaseEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
DChainBaseEncoder<PinData>::~DChainBaseEncoder(void) = default;

template<typename PinData>
std::string DChainBaseEncoder<PinData>::GetName(void) const
{
	return "DChainBaseEncoder";
}

template<typename PinData>
bool DChainBaseEncoder<PinData>::EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	if constexpr (has_good_tag_v<PinData> && has_bad_tag_v<PinData> && has_diff_encoding_v<PinData>)
	{
		auto& timeframe = context.GetTimeframe(timeframeId);
		auto& encoder = context.GetEncoder();

		// Encode the differences for all nodes
		const MappedCircuit& circuit = context.GetMappedCircuit();
		for (auto [nodeId, node] : circuit.EnumerateNodes())
		{
			if (!timeframe.template HasTag<DiffTag>(nodeId))
			{
				continue;
			}

			// Disable the logic interceptor here since we have to encode for the original logic containers.
			// We are not using GetInternalContainer for good and bad since the bad container might not exist
			// and the GetContainer handles this case.
			const auto good = timeframe.template GetContainer<GoodTag>(context, nodeId, PortType::Output, 0u, AllowContainerInterceptor::Disable);
			const auto bad = timeframe.template GetContainer<BadTag>(context, nodeId, PortType::Output, 0u, AllowContainerInterceptor::Disable);
			auto& diff = timeframe.template GetInternalContainer<DiffTag>(context, nodeId);

			auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
			if (bmcSolver)
			{
				if (circuit.IsSecondaryInput(node))
				{
					// Encode a latch here. The logic value of the latch is forwarded
					// from the corresponding secondary output by the D-Chain Connection Generator.
					bmcSolver->SetTargetClauseType(ClauseType::Transition);
					bmcSolver->SetTargetVariableType(VariableType::Latch);
					diff = encoder.template NewLogicContainer<LogicContainer01>();

					// There can not be an initial difference as the secondary input node
					// has no inputs and hence no input difference in the first timeframe.
					// However, to be able to constrain the initial state of the flip-flops
					// we need to encode a difference here. This is for example required to
					// constrain the starting state to be equal via constraining
					// the difference to be LOGIC_ZERO.
					bmcSolver->SetTargetClauseType(ClauseType::Initial);
					bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
					encoder.EncodeLogic01DifferenceDetector(good, bad, diff);

					bmcSolver->SetTargetClauseType(ClauseType::Transition);
					bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
					encoder.EncodeLogic01DifferenceDetector(good, bad, diff);
					continue;
				}

				bmcSolver->SetTargetClauseType(ClauseType::Transition);
				bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
			}

			diff = encoder.EncodeLogic01DifferenceDetector(good, bad);
		}
	}
	else
	{
		static_assert(std::is_same_v<PinData, float>, "This pin data type is not supported!");
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class DChainBaseEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
