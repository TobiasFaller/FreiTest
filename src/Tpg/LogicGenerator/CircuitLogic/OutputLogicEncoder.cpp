#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
OutputLogicEncoder<PinData, Tag>::OutputLogicEncoder(void): ICircuitEncoder<PinData>()
{
}

template<typename PinData, typename Tag>
OutputLogicEncoder<PinData, Tag>::~OutputLogicEncoder(void) = default;

template<typename PinData, typename Tag>
std::string OutputLogicEncoder<PinData, Tag>::GetName(void) const
{
	return "OutputLogicEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool OutputLogicEncoder<PinData, Tag>::EncodeNode(GeneratorContext<PinData>& context, size_t timeframeId, size_t nodeId)
{
	const auto& circuit = context.GetMappedCircuit();
	auto& timeframe = context.GetTimeframe(timeframeId);
	auto& encoder = context.GetEncoder();

	if (!circuit.IsPrimaryOutput(nodeId))
	{
		return true;
	}
	if (!timeframe.template HasTag<Tag>(nodeId))
	{
		return true;
	}

	const auto inLogic = timeframe.template GetContainer<Tag>(context, nodeId, PortType::Input, 0u);
		if (!inLogic.IsSet())
	{
		LOG(FATAL) << "Found node without input logic container for color " << get_tag_name_v<Tag> << ".";
	}

	auto& outLogic = timeframe.template GetInternalContainer<Tag>(context, nodeId);

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver) bmcSolver->SetTargetClauseType(ClauseType::Transition);
	if (bmcSolver) bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	outLogic = encoder.EncodeBuffer(inLogic);
	return true;
}

#define FOR_TAG(PINDATA, TAG) template class OutputLogicEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_ENCODINGS

};
};
