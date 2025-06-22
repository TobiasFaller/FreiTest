#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"

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
SequentialInputLogicEncoder<PinData, Tag>::SequentialInputLogicEncoder(LogicConstraint constraint):
	ICircuitEncoder<PinData>(),
	_constraint(constraint)
{
}

template<typename PinData, typename Tag>
SequentialInputLogicEncoder<PinData, Tag>::~SequentialInputLogicEncoder(void) = default;

template<typename PinData, typename Tag>
std::string SequentialInputLogicEncoder<PinData, Tag>::GetName(void) const
{
	return "SequentialInputLogicEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool SequentialInputLogicEncoder<PinData, Tag>::EncodeNode(GeneratorContext<PinData>& context, size_t timeframeId, size_t nodeId)
{
	using LogicContainer = get_pin_data_container_t<PinData, Tag>;

	const auto& circuit = context.GetMappedCircuit();
	auto& timeframe = context.GetTimeframe(timeframeId);
	auto& encoder = context.GetEncoder();

	if (!circuit.IsSecondaryInput(nodeId))
	{
		return true;
	}
	if (!timeframe.template HasTag<Tag>(nodeId))
	{
		return true;
	}

	auto& outLogic = timeframe.template GetInternalContainer<Tag>(context, nodeId);
	outLogic = encoder.template NewStateContainer<LogicContainer>();
	encoder.EncodeLogicValueConstraint(outLogic, _constraint);

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class SequentialInputLogicEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_ENCODINGS

};
};
