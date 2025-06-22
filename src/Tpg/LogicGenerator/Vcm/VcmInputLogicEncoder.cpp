#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"

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

#define NOT_IMPLEMENTED NotImplemented<LogicContainer>()

template<typename LogicContainer>
constexpr void NotImplemented(void)
{
	// Ugly hack to make static_assert dependent on the type used in LogicContainer template parameter
	static_assert(std::is_same_v<LogicContainer, int>, "The logic container is not implemented");
}

template<typename PinData, typename TargetPinData>
VcmInputLogicEncoder<PinData, TargetPinData>::VcmInputLogicEncoder(std::shared_ptr<LogicGenerator<TargetPinData>> targetGenerator, std::vector<Vcm::VcmInput> vcmInputs):
	ICircuitEncoder<PinData>(),
	_targetGenerator(targetGenerator),
	_vcmInputs(vcmInputs),
	_parameters()
{
}

template<typename PinData, typename TargetPinData>
VcmInputLogicEncoder<PinData, TargetPinData>::VcmInputLogicEncoder(std::shared_ptr<LogicGenerator<TargetPinData>> targetGenerator, std::vector<Vcm::VcmInput> vcmInputs, std::map<std::string, Vcm::VcmParameter> parameters):
	ICircuitEncoder<PinData>(),
	_targetGenerator(targetGenerator),
	_vcmInputs(vcmInputs),
	_parameters(parameters)
{
}

template<typename PinData, typename TargetPinData>
VcmInputLogicEncoder<PinData, TargetPinData>::~VcmInputLogicEncoder(void) = default;

template<typename PinData, typename TargetPinData>
std::string VcmInputLogicEncoder<PinData, TargetPinData>::GetName(void) const
{
	return "VcmInputLogicEncoder";
}

template<typename PinData, typename TargetPinData>
void VcmInputLogicEncoder<PinData, TargetPinData>::Reset(void)
{
	_encodedInputs.clear();
}

template<typename PinData, typename TargetPinData>
bool VcmInputLogicEncoder<PinData, TargetPinData>::PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId)
{
	auto& encoder = context.GetEncoder();
	auto& timeframe = context.GetTimeframe(timeframeId);
	auto& targetContext { _targetGenerator->GetContext() };
	auto& targetTimeframe { targetContext.GetTimeframe(timeframeId) };

	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());

	_encodedInputs.emplace_back();
	for (auto& vcmInput : _vcmInputs)
	{
		if (!timeframe.template HasTag<GoodTag>(vcmInput.vcmPort->GetNodeId()))
		{
			_encodedInputs[timeframeId].push_back(LogicContainer());
			continue;
		}

		auto& input = timeframe.template GetInternalContainer<GoodTag>(context, vcmInput.vcmPort->GetNodeId());
		auto const get_container = [&]<typename Tag>() -> auto {
			const auto container = targetTimeframe.template GetContainer<Tag>(
				targetContext, vcmInput.targetNode->GetNodeId(), Circuit::PortType::Output, 0u);
			if (!container.IsSet())
			{
				LOG(FATAL) << "The node " << vcmInput.targetNodeName
					<< " has not been encoded with " << get_tag_name_v<Tag>
					<< " encoding in target for timeframe " << timeframeId;
				__builtin_unreachable();
			}
			return container;
		};

		switch(vcmInput.vcmPortType)
		{
			case Vcm::VcmInputType::Good:
				DASSERT(vcmInput.targetNode != nullptr)
					<< "Could not find node " << vcmInput.targetNodeName << " in target circuit";

				if constexpr (has_good_tag_v<TargetPinData>)
				{
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						get_container.template operator()<GoodTag>());
				}
				else
				{
					Logging::Panic("Good tag not supported by PinData");
				}
				break;

			case Vcm::VcmInputType::Bad:
				DASSERT(vcmInput.targetNode != nullptr)
					<< "Could not find node " << vcmInput.targetNodeName << " in target circuit";

				if constexpr (has_bad_tag_v<TargetPinData>)
				{
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						get_container.template operator()<BadTag>());
				}
				else if constexpr (has_good_tag_v<TargetPinData>)
				{
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						get_container.template operator()<GoodTag>());
				}
				else
				{
					Logging::Panic("Bad tag not supported by PinData");
				}
				break;

			case Vcm::VcmInputType::Difference:
				DASSERT(vcmInput.targetNode != nullptr)
					<< "Could not find node " << vcmInput.targetNodeName << " in target circuit";

				if constexpr (has_diff_tag_v<TargetPinData>)
				{
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						get_container.template operator()<DiffTag>());
				}
				else if constexpr (has_good_tag_v<TargetPinData> && has_bad_tag_v<TargetPinData>)
				{
					auto goodContainer { get_container.template operator()<GoodTag>() };
					auto badContainer { get_container.template operator()<BadTag>() };
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						encoder.Encode01LogicDifferenceDetector(goodContainer, badContainer));
				}
				else if constexpr (has_good_tag_v<TargetPinData>)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
				}
				else
				{
					Logging::Panic("Diff tag not supported by PinData");
				}
				break;

			case Vcm::VcmInputType::GoodValid:
				DASSERT(vcmInput.targetNode != nullptr)
					<< "Could not find node " << vcmInput.targetNodeName << " in target circuit";

				if constexpr (has_good_tag_v<TargetPinData>)
				{
					auto const container { get_container.template operator()<GoodTag>() };
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						encoder.EncodeLogicValueConstraintDetector(container, LogicConstraint::ONLY_LOGIC_01));
				}
				else
				{
					Logging::Panic("Good tag not supported by PinData");
				}
				break;

			case Vcm::VcmInputType::BadValid:
				DASSERT(vcmInput.targetNode != nullptr)
					<< "Could not find node " << vcmInput.targetNodeName << " in target circuit";

				if constexpr (has_bad_tag_v<TargetPinData>)
				{
					auto const container { get_container.template operator()<BadTag>() };
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						encoder.EncodeLogicValueConstraintDetector(container, LogicConstraint::ONLY_LOGIC_01));
				}
				else if constexpr (has_good_tag_v<TargetPinData>)
				{
					auto const container { get_container.template operator()<GoodTag>() };
					input = encoder.template ConvertLogicContainer<LogicContainer>(
						encoder.EncodeLogicValueConstraintDetector(container, LogicConstraint::ONLY_LOGIC_01));
				}
				else
				{
					Logging::Panic("Bad tag not supported by PinData");
				}
				break;

			case Vcm::VcmInputType::Parameter:
				if (auto it = _parameters.find(vcmInput.parameterName); it != _parameters.end())
				{
					auto result = Vcm::GetVcmParameterValue(it->second, vcmInput.parameterIndex);
					if (!result.has_value())
					{
						LOG(FATAL) << "The vcm parameter " << vcmInput.parameterName << " bit "
							<< vcmInput.parameterIndex << " could not be encoded.";
						__builtin_unreachable();
					}
					input = encoder.template NewLogicContainer<LogicContainer>(result.value());
				}
				else
				{
					LOG(FATAL) << "The parameter " << vcmInput.parameterName << " has not been found inside the parameter list.";
					__builtin_unreachable();
				}
				break;

			case Vcm::VcmInputType::ConstantZero:
				input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
				break;

			case Vcm::VcmInputType::ConstantOne:
				input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
				break;

			case Vcm::VcmInputType::ConstantDontCare:
				if constexpr (is_01_encoding_v<LogicContainer>)
				{
					LOG(WARNING) << "Encountered PRESET_X (constX) VCM input, encoding it as LOGIC_ZERO as the container does not support LOGIC_DONT_CARE";
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
				}
				else if constexpr (is_01X_encoding_v<LogicContainer> || is_U01X_encoding_v<LogicContainer>
					|| is_01F_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
				}
				else
				{
					NOT_IMPLEMENTED;
				}
				break;

			case Vcm::VcmInputType::ConstantUnknown:
				if constexpr (is_01_encoding_v<LogicContainer>)
				{
					LOG(WARNING) << "Encountered UNDEFTYPE (constU) VCM input, encoding it as LOGIC_ZERO as the container does not support LOGIC_UNKNOWN";
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
				}
				else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
				{
					LOG(WARNING) << "Encountered UNDEFTYPE (constU) VCM input, encoding it as LOGIC_DONT_CARE as the container does not support LOGIC_UNKNOWN";
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
				}
				else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
				}
				else
				{
					NOT_IMPLEMENTED;
				}
				break;

			case Vcm::VcmInputType::FreeStatic:
				input = encoder.template NewStateContainer<LogicContainer>();
				if (bmcSolver || (satSolver && timeframeId > 0u)) {
					auto& previousTimeframe = context.GetTimeframe(timeframeId - 1u);
					auto output { previousTimeframe.template GetContainer<GoodTag>(
						context, vcmInput.vcmPort->GetNodeId(), Circuit::PortType::Output, 0u) };
					encoder.EncodeNextState(input, output);
				}
				break;

			case Vcm::VcmInputType::FreeDynamic:
				input = encoder.template NewInputContainer<LogicContainer>();
				break;

			case Vcm::VcmInputType::CounterInitial:
				if (satSolver)
				{
					input = encoder.template NewLogicContainer<LogicContainer>((timeframeId == 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);
				}
				if (bmcSolver)
				{
					input = encoder.template ConvertLogicContainer<LogicContainer>(context.GetUnaryCounter().GetIndicatorForIndex(0u));
				}
				break;

			case Vcm::VcmInputType::CounterBinary:
				if (satSolver)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(
						(timeframeId & (1u << vcmInput.vcmPortElementIndex)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO
					);
				}
				if (bmcSolver)
				{
					input = (vcmInput.vcmPortElementIndex >= context.GetBinaryCounter().GetBits())
						? encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO)
						: encoder.template ConvertLogicContainer<LogicContainer>(
							context.GetBinaryCounter().GetBit(vcmInput.vcmPortElementIndex)
						);
				}
				break;

			case Vcm::VcmInputType::CounterUnary:
				if (satSolver)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(
						(timeframeId >= vcmInput.vcmPortElementIndex) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO
					);
				}
				if (bmcSolver)
				{
					input = (vcmInput.vcmPortElementIndex >= context.GetUnaryCounter().GetBits())
						? encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO)
						: encoder.template ConvertLogicContainer<LogicContainer>(
							context.GetUnaryCounter().GetStateForIndex(vcmInput.vcmPortElementIndex)
						);
				}
				break;

			case Vcm::VcmInputType::CounterTimeframe:
				if (satSolver)
				{
					input = encoder.template NewLogicContainer<LogicContainer>(
						(timeframeId == vcmInput.vcmPortElementIndex) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO
					);
				}
				if (bmcSolver)
				{
					input = (vcmInput.vcmPortElementIndex >= context.GetUnaryCounter().GetBits())
						? encoder.template NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO)
						: encoder.template ConvertLogicContainer<LogicContainer>(
							context.GetUnaryCounter().GetIndicatorForIndex(vcmInput.vcmPortElementIndex)
						);
				}
				break;

			case Vcm::VcmInputType::Unknown:
				LOG(FATAL) << "The input node " << vcmInput.vcmPortName << " has an unknown type";
				__builtin_unreachable();

			default:
				Logging::Panic();
		}
		_encodedInputs[timeframeId].push_back(input);
	}

	return true;
}

template<typename PinData, typename TargetPinData>
std::map<std::string, Vcm::VcmParameter>& VcmInputLogicEncoder<PinData, TargetPinData>::GetParameters(void)
{
	return _parameters;
}

template<typename PinData, typename TargetPinData>
std::map<std::string, Vcm::VcmParameter> const& VcmInputLogicEncoder<PinData, TargetPinData>::GetParameters(void) const
{
	return _parameters;
}

template<typename PinData, typename TargetPinData>
std::vector<std::tuple<Vcm::VcmInput, std::vector<typename VcmInputLogicEncoder<PinData, TargetPinData>::LogicContainer>>> VcmInputLogicEncoder<PinData, TargetPinData>::GetEncodedInputs(InputFilter filterType) const
{
	std::vector<std::tuple<Vcm::VcmInput, std::vector<LogicContainer>>> result;
	for (auto& vcmInput : _vcmInputs)
	{
		auto filtered = [&]() -> bool {
			switch (vcmInput.vcmPortType)
			{
				case Vcm::VcmInputType::Good:
				case Vcm::VcmInputType::Bad:
				case Vcm::VcmInputType::Difference:
				case Vcm::VcmInputType::GoodValid:
				case Vcm::VcmInputType::BadValid:
					return (filterType == InputFilter::All) || (filterType == InputFilter::Miter);

				case Vcm::VcmInputType::Parameter:
					return (filterType == InputFilter::All) || (filterType == InputFilter::Parameter);

				case Vcm::VcmInputType::ConstantZero:
				case Vcm::VcmInputType::ConstantOne:
				case Vcm::VcmInputType::ConstantDontCare:
				case Vcm::VcmInputType::ConstantUnknown:
					return (filterType == InputFilter::All) || (filterType == InputFilter::Constant);

				case Vcm::VcmInputType::FreeStatic:
				case Vcm::VcmInputType::FreeDynamic:
					return (filterType == InputFilter::All) || (filterType == InputFilter::Free);

				case Vcm::VcmInputType::CounterInitial:
				case Vcm::VcmInputType::CounterUnary:
				case Vcm::VcmInputType::CounterBinary:
				case Vcm::VcmInputType::CounterTimeframe:
					return (filterType == InputFilter::All) || (filterType == InputFilter::Counter);

				default:
					return false;
			}
		}();
		if (!filtered) {
			result.push_back(std::make_tuple(vcmInput, _encodedInputs[result.size()]));
		}
	}
	return result;
}

#define FOR_PINDATA(PINDATA) template class VcmInputLogicEncoder<PinDataG<LogicContainer01>, PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA
#undef FOR_PINDATA
#define FOR_PINDATA(PINDATA) template class VcmInputLogicEncoder<PinDataG<LogicContainer01X>, PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA
#undef FOR_PINDATA
#define FOR_PINDATA(PINDATA) template class VcmInputLogicEncoder<PinDataG<LogicContainer01F>, PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA
#undef FOR_PINDATA
#define FOR_PINDATA(PINDATA) template class VcmInputLogicEncoder<PinDataG<LogicContainerU01X>, PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA
#undef FOR_PINDATA
#define FOR_PINDATA(PINDATA) template class VcmInputLogicEncoder<PinDataG<LogicContainerU01F>, PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
