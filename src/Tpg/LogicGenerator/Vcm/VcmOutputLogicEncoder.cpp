#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"

#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace SolverProxy::Sat;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
VcmOutputLogicEncoder<PinData>::VcmOutputLogicEncoder(std::vector<Vcm::VcmOutput> vcmOutputs):
	_vcmOutputs(vcmOutputs)
{
}

template<typename PinData>
VcmOutputLogicEncoder<PinData>::~VcmOutputLogicEncoder(void) = default;

template<typename PinData>
std::string VcmOutputLogicEncoder<PinData>::GetName(void) const
{
	return "VcmOutputLogicEncoder";
}

template<typename PinData>
void VcmOutputLogicEncoder<PinData>::Reset(void)
{
	_encodedOutputs.clear();
}

template<typename PinData>
bool VcmOutputLogicEncoder<PinData>::PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	auto& encoder = context.GetEncoder();
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	auto& timeframe { context.GetTimeframe(timeframeId) };

	_encodedOutputs.emplace_back();
	for (auto& vcmOutput : _vcmOutputs)
	{
		if (!timeframe.template HasTag<GoodTag>(vcmOutput.vcmPort->GetNodeId()))
		{
			_encodedOutputs[timeframeId].push_back(LogicContainer());
			continue;
		}

		const auto container { timeframe.template GetContainer<GoodTag>(
			context, vcmOutput.vcmPort->GetNodeId(), Circuit::PortType::Output, 0u) };
		switch (vcmOutput.vcmPortType)
		{
			case Vcm::VcmOutputType::InitialConstraint:
			case Vcm::VcmOutputType::TransitionConstraint:
			case Vcm::VcmOutputType::TargetConstraint:
			{
				auto const condition {
					encoder.template ConvertLogicContainer<LogicContainer>(
					encoder.EncodeLogicValueDetector(container, Logic::LOGIC_ONE))
				};

				if (satSolver)
				{
					_encodedOutputs[timeframeId].push_back(condition);
				}
				if (bmcSolver)
				{
					auto const latch { encoder.template NewStateContainer<LogicContainer>() };
					auto const firstTimeframe { encoder.template ConvertLogicContainer<LogicContainer>(
						context.GetUnaryCounter().GetIndicatorForIndex(0u)) };
					assert(firstTimeframe.IsSet());
					auto const output = [&]() {
						switch (vcmOutput.vcmPortType)
						{
							case Vcm::VcmOutputType::InitialConstraint:
								return encoder.EncodeMultiplexer(latch, condition, firstTimeframe);
							case Vcm::VcmOutputType::TransitionConstraint:
								return encoder.EncodeMultiplexer(encoder.EncodeAnd(latch, condition), condition, firstTimeframe);
							case Vcm::VcmOutputType::TargetConstraint:
								return condition;
							default:
								__builtin_unreachable();
						}
					}();
					encoder.EncodeInitialStateValue(latch, Logic::LOGIC_ZERO);
					encoder.EncodeNextState(latch, output);
					_encodedOutputs[timeframeId].push_back(latch);
				}
				break;
			}

			case Vcm::VcmOutputType::Result:
				_encodedOutputs[timeframeId].push_back(container);
				break;

			default:
				Logging::Panic();
		}
	}

	return true;
}

template<typename PinData>
bool VcmOutputLogicEncoder<PinData>::ConstrainCircuit(GeneratorContext<PinData>& context)
{
	auto& encoder = context.GetEncoder();
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());

	auto const constrain = [&](auto constraint) {
		if (satSolver && context.IsIncremental())
		{
			satSolver->AddAssumption(encoder.EncodeLogicValueDetector(constraint, Logic::LOGIC_ONE).l0);
		}
		else if (satSolver && !context.IsIncremental())
		{
			encoder.EncodeLogicValue(constraint, Logic::LOGIC_ONE);
		}
		else if (bmcSolver)
		{
			encoder.EncodeTargetStateValue(constraint, Logic::LOGIC_ONE);
		}
	};

	for (size_t index { 0u }; index < _vcmOutputs.size(); index++)
	{
		switch (_vcmOutputs[index].vcmPortType)
		{
			case Vcm::VcmOutputType::InitialConstraint:
				constrain(_encodedOutputs.front()[index]);
				break;

			case Vcm::VcmOutputType::TransitionConstraint:
				for (size_t timeframe { 0u }; timeframe < _encodedOutputs.size(); timeframe++)
				{
					constrain(_encodedOutputs[timeframe][index]);
				}
				break;

			case Vcm::VcmOutputType::TargetConstraint:
				constrain(_encodedOutputs.back()[index]);
				break;

			case Vcm::VcmOutputType::Result:
				break;

			default:
				Logging::Panic();
		}
	}

	return true;
}

template<typename PinData>
std::vector<std::tuple<Vcm::VcmOutput, std::vector<typename VcmOutputLogicEncoder<PinData>::LogicContainer>>> VcmOutputLogicEncoder<PinData>::GetEncodedOutputs(OutputFilter filterType) const
{
	std::vector<std::tuple<Vcm::VcmOutput, std::vector<LogicContainer>>> result;
	for (auto& vcmOutput : _vcmOutputs)
	{
		auto filtered = [&]() -> bool {
			switch (vcmOutput.vcmPortType)
			{
				case Vcm::VcmOutputType::InitialConstraint:
				case Vcm::VcmOutputType::TransitionConstraint:
				case Vcm::VcmOutputType::TargetConstraint:
					return (filterType == OutputFilter::All) || (filterType == OutputFilter::Constraint);

				case Vcm::VcmOutputType::Result:
					return (filterType == OutputFilter::All) || (filterType == OutputFilter::Result);

				default:
					return false;
			}
		}();
		if (!filtered) {
			result.push_back(std::make_tuple(vcmOutput, _encodedOutputs[result.size()]));
		}
	}
	return result;
}

#define FOR_PINDATA(PINDATA) template class VcmOutputLogicEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
