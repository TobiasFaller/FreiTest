#include "Tpg/Vcm/VcmTypes.hpp"

#include <boost/algorithm/string.hpp>

#include "Basic/Overload.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

std::string to_string(const VcmInputType& type)
{
	const std::map<VcmInputType, std::string> options = {
		{ VcmInputType::Good, "Good" },
		{ VcmInputType::Bad, "Bad" },
		{ VcmInputType::Difference, "Difference" },
		{ VcmInputType::GoodValid, "GoodValid" },
		{ VcmInputType::BadValid, "BadValid" },
		{ VcmInputType::Parameter, "Parameter" },
		{ VcmInputType::ConstantOne, "ConstantZero" },
		{ VcmInputType::ConstantZero, "ConstantOne" },
		{ VcmInputType::ConstantDontCare, "ConstantDontCare" },
		{ VcmInputType::ConstantUnknown, "ConstantUnknown" },
		{ VcmInputType::FreeStatic, "FreeStatic" },
		{ VcmInputType::FreeDynamic, "FreeDynamic" },
		{ VcmInputType::CounterInitial, "CounterInitial" },
		{ VcmInputType::CounterUnary, "CounterUnary" },
		{ VcmInputType::CounterBinary, "CounterBinary" },
		{ VcmInputType::CounterTimeframe, "CounterTimeframe" },
	};

	if (auto it { options.find(type) }; it != options.end())
	{
		return it->second;
	}

	return "Unknown";
}

std::string to_string(const VcmOutputType& type)
{
	const std::map<VcmOutputType, std::string> options = {
		{ VcmOutputType::InitialConstraint, "InitialConstraint" },
		{ VcmOutputType::TransitionConstraint, "TransitionConstraint" },
		{ VcmOutputType::TargetConstraint, "TargetConstraint" },
		{ VcmOutputType::Result, "Result" },
	};

	if (auto it { options.find(type) }; it != options.end())
	{
		return it->second;
	}

	return "Unknown";
}

std::string to_string(const VcmParameter& parameter)
{
	return std::visit(overload(
		[&](const uint32_t& value) -> auto {
			return std::to_string(value);
		},
		[&](const uint64_t& value) -> auto {
			return std::to_string(value);
		},
		[&](const bool& value) -> auto {
			return std::to_string(value ? 1u : 0u);
		},
		[&](const std::vector<bool>& values) -> auto {
			std::string result;
			for (const auto& value : values) result += std::to_string(value ? 1u : 0u);
			return result;
		},
		[&](const Basic::Logic& value) -> auto {
			return to_string(value);
		},
		[&](const std::vector<Basic::Logic>& values) -> auto {
			std::string result;
			for (const auto& value : values) result += to_string(value);
			return result;
		}
	), parameter);
}

std::optional<Logic> GetVcmParameterValue(const VcmParameter& parameter, size_t index)
{
	return std::visit(overload(
		[&](const uint32_t& value) -> auto {
			return (index < 32u) ? std::optional<Logic> { ((value >> index) & 1) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO } : std::nullopt;
		},
		[&](const uint64_t& value) -> auto {
			return (index < 64u) ? std::optional<Logic> { ((value >> index) & 1) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO } : std::nullopt;
		},
		[&](const bool& value) -> auto {
			return (index == 0u) ? std::optional<Logic> { value ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO } : std::nullopt;
		},
		[&](const std::vector<bool>& value) -> auto {
			return (index < value.size()) ? std::optional<Logic> { value[index] ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO } : std::nullopt;
		},
		[&](const Basic::Logic& value) -> auto {
			return (index == 0u) ? std::optional<Logic> { value } : std::nullopt;
		},
		[&](const std::vector<Basic::Logic>& value) -> auto {
			return (index < value.size()) ? std::optional<Logic> { value[index] } : std::nullopt;
		}
	), parameter);
}

VcmInputType GetVcmInputType(const std::string& extension)
{
	const std::map<std::string, VcmInputType> options = {
		{ "g", VcmInputType::Good },
		{ "b", VcmInputType::Bad },
		{ "d", VcmInputType::Difference },
		{ "gv", VcmInputType::GoodValid },
		{ "bv", VcmInputType::BadValid },
		{ "param", VcmInputType::Parameter },
		{ "const0", VcmInputType::ConstantOne },
		{ "const1", VcmInputType::ConstantZero },
		{ "constx", VcmInputType::ConstantDontCare },
		{ "constu", VcmInputType::ConstantUnknown },
		{ "sfree", VcmInputType::FreeStatic },
		{ "dfree", VcmInputType::FreeDynamic },
		{ "init", VcmInputType::CounterInitial },
		{ "bcntr", VcmInputType::CounterBinary },
		{ "ucntr", VcmInputType::CounterUnary },
		{ "tf", VcmInputType::CounterTimeframe },
	};

	const auto value = boost::algorithm::to_lower_copy(extension);
	if (auto it { options.find(extension) }; it != options.end())
	{
		return it->second;
	}

	return VcmInputType::Unknown;
}

VcmOutputType GetVcmOutputType(const std::string& extension)
{
	const std::map<std::string, VcmOutputType> options = {
		{ "i", VcmOutputType::InitialConstraint },
		{ "c", VcmOutputType::TransitionConstraint },
		{ "t", VcmOutputType::TargetConstraint },
		{ "result", VcmOutputType::Result },
	};

	const auto value = boost::algorithm::to_lower_copy(extension);
	if (auto it { options.find(extension) }; it != options.end())
	{
		return it->second;
	}

	return VcmOutputType::Unknown;
};

VcmInput::VcmInput(const Circuit::MappedNode* vcmPort, std::string vcmPortName, VcmInputType vcmPortType, size_t vcmPortElementIndex, size_t vcmPortPrimaryIndex):
	vcmPort(vcmPort),
	vcmPortName(vcmPortName),
	vcmPortType(vcmPortType),
	vcmPortElementIndex(vcmPortElementIndex),
	vcmPortPrimaryIndex(vcmPortPrimaryIndex),
	targetNode(nullptr),
	targetNodeName(""),
	parameterName(""),
	parameterIndex(std::numeric_limits<size_t>::max())
{
}

VcmInput VcmInput::ConvertToParameter(std::string parameterName, size_t parameterIndex) const
{
	VcmInput vcmInput { vcmPort, vcmPortName, vcmPortType, vcmPortElementIndex, vcmPortPrimaryIndex };
	vcmInput.parameterName = parameterName;
	vcmInput.parameterIndex = parameterIndex;
	return vcmInput;
}

VcmInput VcmInput::ConvertToNode(const Circuit::MappedNode* targetNode, std::string targetNodeName) const
{
	VcmInput vcmInput { vcmPort, vcmPortName, vcmPortType, vcmPortElementIndex, vcmPortPrimaryIndex };
	vcmInput.targetNode = targetNode;
	vcmInput.targetNodeName = targetNodeName;
	return vcmInput;
}

VcmOutput::VcmOutput(const Circuit::MappedNode* vcmPort, std::string vcmPortName,VcmOutputType vcmPortType,  size_t vcmPortElementIndex, size_t vcmPortPrimaryIndex):
	vcmPort(vcmPort),
	vcmPortName(vcmPortName),
	vcmPortType(vcmPortType),
	vcmPortElementIndex(vcmPortElementIndex),
	vcmPortPrimaryIndex(vcmPortPrimaryIndex),
	resultName(""),
	resultIndex(std::numeric_limits<size_t>::max())
{
}

VcmOutput VcmOutput::ConvertToResult(std::string resultName, size_t resultIndex) const
{
	VcmOutput vcmOutput { vcmPort, vcmPortName, vcmPortType, vcmPortElementIndex, vcmPortPrimaryIndex };
	vcmOutput.resultName = resultName;
	vcmOutput.resultIndex = resultIndex;
	return vcmOutput;
}

};
};
};
