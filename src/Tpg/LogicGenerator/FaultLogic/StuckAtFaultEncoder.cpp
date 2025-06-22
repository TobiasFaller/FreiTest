#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
StuckAtFaultEncoder<PinData, FaultModel>::StuckAtFaultEncoder(std::shared_ptr<FaultModel> faultModel):
	_faultModel(faultModel),
	_badContainerForTimeframe(),
	_diffContainerForTimeframe()
{
}

template<typename PinData, typename FaultModel>
StuckAtFaultEncoder<PinData, FaultModel>::~StuckAtFaultEncoder(void) = default;

template<typename PinData, typename FaultModel>
std::string StuckAtFaultEncoder<PinData, FaultModel>::GetName(void) const
{
	return "StuckAtFaultEncoder";
}

template<typename PinData, typename FaultModel>
void StuckAtFaultEncoder<PinData, FaultModel>::Reset(void)
{
	_badContainerForTimeframe = { };
	_diffContainerForTimeframe = { };
}

template<typename PinData, typename FaultModel>
bool StuckAtFaultEncoder<PinData, FaultModel>::PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	auto const& fault { _faultModel->GetFault() };
	auto const get_stuck_at_count = [&]() -> size_t {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			return 1u;
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
		{
			return fault->GetStuckAts().size();
		}
	};

	auto const stuckAtCount = get_stuck_at_count();
	while (_badContainerForTimeframe.size() <= timeframeId)
	{
		_badContainerForTimeframe.emplace_back(std::vector<BadContainer>(stuckAtCount));
	}
	while (_diffContainerForTimeframe.size() <= timeframeId)
	{
		_diffContainerForTimeframe.emplace_back(std::vector<DiffContainer>(stuckAtCount));
	}
	return true;
}

template<typename PinData, typename FaultModel>
const void* StuckAtFaultEncoder<PinData, FaultModel>::GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframeId, size_t nodeId, PortType portType, size_t portId)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	auto& encoder { context.GetEncoder() };

	const auto get_stuck_ats = [&]() -> std::vector<Fault::StuckAtFault> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>)
		{
			return { _faultModel->GetFault()->GetStuckAt() };
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::MultiStuckAtFaultModel>)
		{
			return _faultModel->GetFault()->GetStuckAts();
		}
	};
	const auto get_constant = [&](const Fault::StuckAtFaultType& type) -> BadContainer {
		switch (type)
		{
			case Fault::StuckAtFaultType::STUCK_AT_0:
				return encoder.template NewLogicContainer<BadContainer>(Logic::LOGIC_ZERO);
			case Fault::StuckAtFaultType::STUCK_AT_1:
				return encoder.template NewLogicContainer<BadContainer>(Logic::LOGIC_ONE);
			case Fault::StuckAtFaultType::STUCK_AT_X:
				return encoder.template NewLogicContainer<BadContainer>(Logic::LOGIC_DONT_CARE);
			case Fault::StuckAtFaultType::STUCK_AT_U:
				return encoder.template NewLogicContainer<BadContainer>(Logic::LOGIC_UNKNOWN);
			case Fault::StuckAtFaultType::STUCK_AT_FREE:
				return encoder.template NewLogicContainer<BadContainer>();
			default:
				Logging::Panic("Encountered invalid fault type");
		}
	};

	if constexpr (has_bad_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(BadTag))
		{
			size_t index { 0u };
			for (auto const& stuckAt : get_stuck_ats())
			{
				if (stuckAt.GetNode()->GetNodeId() == nodeId
					&& stuckAt.GetPort().portType == portType
					&& stuckAt.GetPort().portNumber == portId)
				{
					if (!_badContainerForTimeframe[timeframeId][index].IsSet())
					{
						_badContainerForTimeframe[timeframeId][index] = get_constant(stuckAt.GetType());
					}
					return &_badContainerForTimeframe[timeframeId][index];
				}

				index++;
			}
		}
	}

	if constexpr (has_diff_tag_v<PinData>)
	{
		if (typeid(tag) == typeid(DiffTag))
		{
			// The difference container has to be overriden by the fault injector too.
			// This is because otherwise the D-Chain will link the inputs of the fault location
			// to the outputs even though the stuck-at fault should break the implication chain.
			size_t index { 0u };
			for (auto const& stuckAt : get_stuck_ats())
			{
				if (stuckAt.GetNode()->GetNodeId() == nodeId
					&& stuckAt.GetPort().portType == portType
					&& stuckAt.GetPort().portNumber == portId)
				{
					if (!_diffContainerForTimeframe[timeframeId][index].IsSet())
					{
						_diffContainerForTimeframe[timeframeId][index] = context.GetEncoder().EncodeLogic01DifferenceDetector(
							context.GetTimeframe(timeframeId).template GetContainer<GoodTag>(
								context, nodeId, portType, portId, AllowContainerInterceptor::Enable),
							context.GetTimeframe(timeframeId).template GetContainer<BadTag>(
								context, nodeId, portType, portId, AllowContainerInterceptor::Enable)
						);
					}
					return &_diffContainerForTimeframe[timeframeId][index];
				}

				index++;
			}
		}
	}

	return nullptr;
}

#define FOR_CONTAINER(CONTAINER) \
	template class StuckAtFaultEncoder<PinDataGB<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtFaultEncoder<PinDataGBD<CONTAINER>, Fault::SingleStuckAtFaultModel>; \
	template class StuckAtFaultEncoder<PinDataGB<CONTAINER>, Fault::MultiStuckAtFaultModel>; \
	template class StuckAtFaultEncoder<PinDataGBD<CONTAINER>, Fault::MultiStuckAtFaultModel>;
INSTANTIATE_ALL_CONTAINER

};
};
