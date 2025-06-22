#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"

#include <cstddef>

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CellLibrary.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

using namespace std;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename LogicContainer, typename Tag>
static LogicContainer EncodeConnection(LogicEncoder& encoder, CellType type, LogicContainer input, LogicContainer output);

template<typename PinData, typename Tag>
SequentialConnectionEncoder<PinData, Tag>::SequentialConnectionEncoder(void) = default;

template<typename PinData, typename Tag>
SequentialConnectionEncoder<PinData, Tag>::~SequentialConnectionEncoder(void) = default;

template<typename PinData, typename Tag>
std::string SequentialConnectionEncoder<PinData, Tag>::GetName(void) const
{
	return "SequentialConnectionEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool SequentialConnectionEncoder<PinData, Tag>::PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	const auto& circuit = context.GetMappedCircuit();
	auto& encoder = context.GetEncoder();

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver && timeframeId == 0u)
	{
		// The following timeframe will encode the connection to the previous timeframe.
		// As there is no previous timeframe in timeframe 0 we exit here.
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

	for (auto const& secondaryInput : circuit.GetSecondaryInputs())
	{
		const auto* secondaryOutput = circuit.GetSecondaryOutputForSecondaryInput(secondaryInput);
		if (!timeframe.template HasTag<Tag>(secondaryInput->GetNodeId()))
		{
			continue;
		}

		auto outLogic = previousTimeframe.template GetContainer<Tag>(
			context, secondaryOutput->GetNodeId(), PortType::Output, 0u);
		if (!outLogic.IsSet())
		{
			LOG(FATAL) << "Found secondary input " << secondaryInput->GetName() << " in timeframe " << timeframeId
				<< " for which no output value is defined for the secondary output " << secondaryOutput->GetName()
				<< " in timeframe " << (timeframeId - 1u) << " when using the " << get_tag_name_v<Tag> << " encoding."
				<< " Make sure that you have fully functional sequential modules in the module stack above.";
		}

		auto& inLogic = timeframe.template GetInternalContainer<Tag>(
			context, secondaryInput->GetNodeId());

		if (satSolver)
		{
			encoder.EncodeBuffer(outLogic, inLogic);
		}
		if (bmcSolver)
		{
			encoder.EncodeNextState(inLogic, outLogic);
		}
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class SequentialConnectionEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_ENCODINGS

};
};
