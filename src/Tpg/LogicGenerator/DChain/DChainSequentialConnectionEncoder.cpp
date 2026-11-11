#include "Tpg/LogicGenerator/DChain/DChainSequentialConnectionEncoder.hpp"

#include "Circuit/MappedCircuit.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"

using namespace std;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename LogicContainer>
static LogicContainer EncodeConnection(LogicEncoder& encoder, CellType type, LogicContainer input, LogicContainer output);

template<typename PinData>
DChainSequentialConnectionEncoder<PinData>::DChainSequentialConnectionEncoder(void) = default;

template<typename PinData>
DChainSequentialConnectionEncoder<PinData>::~DChainSequentialConnectionEncoder(void) = default;

template<typename PinData>
std::string DChainSequentialConnectionEncoder<PinData>::GetName(void) const
{
	return "DChainSequentialConnectionEncoder";
}

template<typename PinData>
bool DChainSequentialConnectionEncoder<PinData>::EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	const MappedCircuit& circuit = context.GetMappedCircuit();
	LogicEncoder& encoder = context.GetEncoder();

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver && timeframeId == 0u)
	{
		// For SAT-problems we connect the timeframes from the current to the previous one.
		// Therefore, we skip the first timeframe as it has no previous element.
		return true;
	}

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver && timeframeId != 0u)
	{
		// For BMC-problems we only have one timeframe to encode that holds our circuit logic.
		// The other timeframes are only used to encode constraints.
		return true;
	}

	auto& timeframe = context.GetTimeframe(timeframeId);
	auto& previousTimeframe = context.GetTimeframe(timeframeId - 1u);
	for (auto secondaryInput : circuit.GetSecondaryInputs())
	{
		if (!timeframe.template HasTag<DiffTag>(secondaryInput->GetNodeId()))
		{
			continue;
		}

		const auto* secondaryOutput = circuit.GetSecondaryOutputForSecondaryInput(secondaryInput);
		auto outLogic = previousTimeframe.template GetContainer<DiffTag>(
			context, secondaryOutput->GetNodeId(), PortType::Output, 0u);
		if (!outLogic.IsSet())
		{
			LOG(FATAL) << "Found secondary input " << secondaryInput->GetName() << " in timeframe 0"
				<< " for which no output value is defined for the secondary output " << secondaryOutput->GetName()
				<< " when using the diff encoding.";
		}

		auto& diff = timeframe.template GetInternalContainer<DiffTag>(context, secondaryInput->GetNodeId());
		encoder.EncodeNextState(diff, outLogic);
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class DChainSequentialConnectionEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
