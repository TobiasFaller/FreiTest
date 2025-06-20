#include "Circuit/MappedNode.hpp"

#include <limits>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Circuit
{

MappedNode::MappedNode(void):
	in(nullptr),
	out(nullptr),
	successorInDataID(nullptr),
	inDataID(nullptr),
	outDataID(std::numeric_limits<size_t>::max()),
	inConnectionID(nullptr),
	outConnectionID(std::numeric_limits<size_t>::max()),
	tsort(std::numeric_limits<size_t>::max()),
	type(CellType::UNDEFTYPE),
	mainType(CellCategory::MAIN_UNKNOWN),
	numberOfIns(0u),
	numberOfOuts(0u),
	name(""),
	signalName("")
{
}

MappedNode::~MappedNode(void) = default;

size_t MappedNode::GetNodeId(void) const
{
	return tsort;
}

const std::string& MappedNode::GetName(void) const
{
	return name;
}

const std::string& MappedNode::GetOutputSignalName(void) const
{
	return signalName;
}

CellType MappedNode::GetCellType(void) const
{
	return type;
}

CellCategory MappedNode::GetCellCategory(void) const
{
	return mainType;
}

size_t MappedNode::GetNumberOfInputs(void) const
{
	return numberOfIns;
}

size_t MappedNode::GetNumberOfSuccessors(void) const
{
	return numberOfOuts;
}

size_t MappedNode::GetInputPinId(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return inDataID[input];
}

size_t MappedNode::GetOutputPinId(void) const
{
	return outDataID;
}

size_t MappedNode::GetPortPinId(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInputPinId(port.portNumber);
		case PortType::Output:
			return GetOutputPinId();
		default:
			LOG(FATAL) << "Unknown PortType";
			__builtin_unreachable();
	}
}

size_t MappedNode::GetInputConnectionId(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return inConnectionID[input];
}

size_t MappedNode::GetOutputConnectionId(void) const
{
	return outConnectionID;
}

size_t MappedNode::GetPortConnectionId(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInputConnectionId(port.portNumber);
		case PortType::Output:
			return GetOutputConnectionId();
		default:
			LOG(FATAL) << "Unknown PortType";
			__builtin_unreachable();
	}
}

const MappedNode* MappedNode::GetInput(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return in[input];
}

const MappedNode* MappedNode::GetSuccessor(size_t successor) const
{
	DASSERT(successor < numberOfOuts) << "The requested successor " << successor << " does not exist";
	return out[successor];
}

size_t MappedNode::GetSuccessorInputPinNumber(size_t successor) const
{
	DASSERT(successor < numberOfOuts) << "The requested successor " << successor << " does not exist";
	return successorInDataID[successor];
}

const MappedInputList MappedNode::GetInputs(void) const
{
	return MappedInputList(in, numberOfIns);
}

const MappedSuccessorList MappedNode::GetSuccessors(void) const
{
	return MappedSuccessorList(out, numberOfOuts);
}

const MappedSuccessorPinList MappedNode::GetSuccessorInputPinNumbers(void) const
{
	return MappedSuccessorPinList(successorInDataID, numberOfOuts);
}

const MappedInputEnumeration MappedNode::EnumerateInputs(void) const
{
	return MappedInputEnumeration(in, numberOfIns);
}

const MappedSuccessorEnumeration MappedNode::EnumerateSuccessors(void) const
{
	return MappedSuccessorEnumeration(out, numberOfOuts);
}

const MappedSuccessorPinList MappedNode::EnumerateSuccessorInputPinNumbers(void) const
{
	return MappedSuccessorPinList(successorInDataID, numberOfOuts);
}

const MappedNode* MappedNode::GetDriverForPort(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInput(port.portNumber);
		case PortType::Output:
			return this;
		default:
			LOG(FATAL) << "Unknown PortType";
			__builtin_unreachable();
	}
}

};
};
