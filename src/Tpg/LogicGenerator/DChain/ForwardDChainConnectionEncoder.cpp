#include "Tpg/LogicGenerator/DChain/ForwardDChainConnectionEncoder.hpp"

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
ForwardDChainConnectionEncoder<PinData>::ForwardDChainConnectionEncoder(void) = default;

template<typename PinData>
ForwardDChainConnectionEncoder<PinData>::~ForwardDChainConnectionEncoder(void) = default;

template<typename PinData>
std::string ForwardDChainConnectionEncoder<PinData>::GetName(void) const
{
	return "ForwardDChainConnectionEncoder";
}

template<typename PinData>
bool ForwardDChainConnectionEncoder<PinData>::EncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	using LogicContainer = get_pin_data_container_t<PinData, DiffTag>;

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

	Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);
	Timeframe<PinData>& previousTimeframe = context.GetTimeframe(timeframeId - 1u);
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
		if (satSolver)
		{
			// We encode a buffer here as there is no need to use an implication (Only one input that has already been validated).
			encoder.EncodeBuffer(diff, outLogic);
		}

		if (bmcSolver)
		{
			bmcSolver->SetTargetClauseType(ClauseType::Transition);
			bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

			// Connect output in timeframe 0 (SOUT) to input in timeframe 1 (SIN).
			// We encode a buffer here as there is no need to use an implication (Only one input that has already been validated).
			if constexpr (is_01_encoding_v<LogicContainer>)
			{
				bmcSolver->CommitTimeframeClause(-outLogic.l0, timeframeId,  diff.l0, timeframeId + 1u);
				bmcSolver->CommitTimeframeClause( outLogic.l0, timeframeId, -diff.l0, timeframeId + 1u);
			}
			else if (is_01X_encoding_v<LogicContainer> || is_U01X_encoding_v<LogicContainer>)
			{
				bmcSolver->CommitTimeframeClause(-outLogic.l0, timeframeId,  diff.l0, timeframeId + 1u);
				bmcSolver->CommitTimeframeClause( outLogic.l0, timeframeId, -diff.l0, timeframeId + 1u);
				bmcSolver->CommitTimeframeClause(-outLogic.l1, timeframeId,  diff.l1, timeframeId + 1u);
				bmcSolver->CommitTimeframeClause( outLogic.l1, timeframeId, -diff.l1, timeframeId + 1u);
			}
		}
	}

	return true;
}

#define FOR_CONTAINER(CONTAINER) template class ForwardDChainConnectionEncoder<PinDataGBD<CONTAINER>>;
INSTANTIATE_ALL_CONTAINER

};
};
