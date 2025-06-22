#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{
namespace Extractor
{

template<typename PinData, typename Tag>
Simulation::SimulationResult ExtractSimulationResult(GeneratorContext<PinData>& context)
{
	const MappedCircuit& circuit = context.GetMappedCircuit();
	const LogicEncoder& encoder = context.GetEncoder();
	const size_t timeframes = context.GetNumberOfTimeframesForTarget(GenerationTarget::PatternExtraction);

	Simulation::SimulationResult result { timeframes, circuit.GetNumberOfNodes(), Logic::LOGIC_INVALID };

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	for (size_t timeframeId = 0u; timeframeId < timeframes; ++timeframeId)
	{
		const Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);
		if (bmcSolver)
		{
			bmcSolver->SetTargetTimeframe(timeframeId);
		}

		DVLOG(6) << "Extracting timeframe " << timeframeId;
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			const auto* node = circuit.GetNode(nodeId);
			const auto container = timeframe.template GetContainer<Tag>(
				context, nodeId, PortType::Output, 0u);
			if (!container.IsSet())
			{
				DVLOG(6) << "Node " << nodeId << " " << node->GetName() << " has no " <<  get_tag_name_v<Tag> << " encoding";
				continue;
			}
			Logic value = encoder.GetSolvedLogicValue(container);
			result[timeframeId][nodeId] = value;
			DVLOG(6) << "Node " << nodeId << " " << node->GetName() << " has " << get_tag_name_v<Tag> << " value " << to_string(value);
		}
	}

	return result;
}

#define FOR_TAG(PINDATA, TAG) template Simulation::SimulationResult ExtractSimulationResult<PINDATA, TAG>(GeneratorContext<PINDATA>& context);
INSTANTIATE_FOR_ALL_TAGS

};
};
};
