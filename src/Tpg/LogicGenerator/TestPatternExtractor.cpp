#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Pattern;

namespace FreiTest
{
namespace Tpg
{
namespace Extractor
{

template<typename PinData, typename Tag>
TestPattern ExtractTestPattern(GeneratorContext<PinData>& context, InputCapture capture)
{
	const MappedCircuit& circuit = context.GetMappedCircuit();
	const LogicEncoder& encoder = context.GetEncoder();
	const size_t timeframes = context.GetNumberOfTimeframesForTarget(GenerationTarget::PatternExtraction);
	const size_t primaryInputs = circuit.GetNumberOfPrimaryInputs();
	const size_t secondaryInputs = circuit.GetNumberOfSecondaryInputs();

	size_t capturePrimaryFrames = 0u;
	size_t captureSecondaryFrames = 0u;
	switch (capture)
	{
		case InputCapture::PrimaryInputsOnly:
			capturePrimaryFrames = timeframes;
			break;
		case InputCapture::SecondaryInputsOnly:
			captureSecondaryFrames = timeframes;
			break;
		case InputCapture::PrimaryAndSecondaryInputs:
			capturePrimaryFrames = timeframes;
			captureSecondaryFrames = timeframes;
			break;
		case InputCapture::PrimaryAndInitialSecondaryInputs:
			capturePrimaryFrames = timeframes;
			captureSecondaryFrames = 1u;
			break;
		default:
			Logging::Panic("The input catpure type is not supported");
			break;
	}

	TestPattern pattern(timeframes, primaryInputs, secondaryInputs, Logic::LOGIC_DONT_CARE);

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
		for (size_t primaryInput = 0u; timeframeId < capturePrimaryFrames && primaryInput < primaryInputs; ++primaryInput)
		{
			const auto* node = circuit.GetPrimaryInput(primaryInput);
			const auto nodeId = node->GetNodeId();
			const auto container = timeframe.template GetContainer<Tag>(
				context, nodeId, PortType::Output, 0u);
			if (!container.IsSet())
			{
				DVLOG(6) << "Node " << nodeId << " " << node->GetName() << " has no " << get_tag_name_v<Tag> << " encoding";
				continue;
			}

			Logic value = encoder.GetSolvedLogicValue(container);
			DVLOG(6) << "Node " << nodeId << " " << node->GetName() << " has " << get_tag_name_v<Tag> << " value " << to_string(value);
			if (value == Logic::LOGIC_ONE || value == Logic::LOGIC_ZERO)
			{
				pattern.SetPrimaryInput(timeframeId, primaryInput, value);
			}
		}

		for (size_t secondaryInput = 0u; timeframeId < captureSecondaryFrames && secondaryInput < secondaryInputs; ++secondaryInput)
		{
			const auto* node = circuit.GetSecondaryInput(secondaryInput);
			const auto container = timeframe.template GetContainer<Tag>(
				context, node->GetNodeId(), PortType::Output, 0u);
			if (!container.IsSet())
			{
				DVLOG(6) << "Node " << node->GetNodeId() << " " << node->GetName() << " has no " << get_tag_name_v<Tag> << " value";
				continue;
			}

			Logic value = encoder.GetSolvedLogicValue(container);
			DVLOG(6) << "Node " << node->GetNodeId() << " " << node->GetName() << " has " << get_tag_name_v<Tag> << " value " << to_string(value);
			if (value == Logic::LOGIC_ONE || value == Logic::LOGIC_ZERO)
			{
				pattern.SetSecondaryInput(timeframeId, secondaryInput, value);
			}
		}
	}

	return pattern;
}

#define FOR_TAG(PINDATA, TAG) template TestPattern ExtractTestPattern<PINDATA, TAG>(GeneratorContext<PINDATA>& context, InputCapture capture);
INSTANTIATE_FOR_ALL_TAGS

};
};
};
