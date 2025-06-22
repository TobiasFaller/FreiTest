#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <optional>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

using VcmParameter = std::variant<uint32_t, uint64_t, bool, std::vector<bool>, Basic::Logic, std::vector<Basic::Logic>>;

std::string to_string(const VcmParameter& parameter);
std::optional<Basic::Logic> GetVcmParameterValue(const VcmParameter& parameter, size_t index);

enum class VcmInputType
{
	Good,
	Bad,
	Difference,
	GoodValid,
	BadValid,
	Parameter,
	ConstantZero,
	ConstantOne,
	ConstantDontCare,
	ConstantUnknown,
	FreeStatic,
	FreeDynamic,
	CounterInitial,
	CounterUnary,
	CounterBinary,
	CounterTimeframe,
	Unknown
};
enum class VcmOutputType
{
	InitialConstraint,
	TransitionConstraint,
	TargetConstraint,
	Result,
	Unknown
};

std::string to_string(const VcmInputType& type);
std::string to_string(const VcmOutputType& type);

struct VcmInput
{
	VcmInput(const Circuit::MappedNode* vcmPort, std::string vcmPortName, VcmInputType vcmPortType, size_t vcmPortElementIndex, size_t vcmPortPrimaryIndex);
	VcmInput ConvertToParameter(std::string parameterName, size_t parameterIndex) const;
	VcmInput ConvertToNode(const Circuit::MappedNode* targetNode, std::string targetNodeName) const;

	const Circuit::MappedNode* vcmPort;
	std::string vcmPortName;
	VcmInputType vcmPortType;
	size_t vcmPortElementIndex;
	size_t vcmPortPrimaryIndex;

	const Circuit::MappedNode* targetNode;
	std::string targetNodeName;

	std::string parameterName;
	size_t parameterIndex;
};

struct VcmOutput
{
	VcmOutput(const Circuit::MappedNode* vcmPort, std::string vcmPortName, VcmOutputType vcmPortType, size_t vcmPortElementIndex, size_t vcmPortPrimaryIndex);
	VcmOutput ConvertToResult(std::string resultName, size_t resultIndex) const;

	const Circuit::MappedNode* vcmPort;
	std::string vcmPortName;
	VcmOutputType vcmPortType;
	size_t vcmPortElementIndex;
	size_t vcmPortPrimaryIndex;

	std::string resultName;
	size_t resultIndex;
};

VcmInputType GetVcmInputType(const std::string& extension);
VcmOutputType GetVcmOutputType(const std::string& extension);

};
};
};
