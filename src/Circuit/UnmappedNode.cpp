#include "Circuit/UnmappedNode.hpp"

#include <limits>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Circuit
{

UnmappedNode::UnmappedNode(void):
	in(nullptr),
	out(nullptr),
	inData(nullptr),
	outData(nullptr),
	tsort(std::numeric_limits<size_t>::max()),
	numberOfIns(0u),
	numberOfOuts(nullptr),
	numberOfOutputPins(0u),
	name(""),
	typeName("")
{
}

UnmappedNode::~UnmappedNode(void)
{
	for (size_t i=0; i < numberOfOutputPins; i++)
	{
		delete[] out[i];
	}

	delete[] in;
	delete[] inData;
	delete[] outData;
	delete[] out;
	delete[] numberOfOuts;
}

size_t UnmappedNode::GetNodeId(void) const
{
	return tsort;
}

const std::string& UnmappedNode::GetName(void) const
{
	return name;
}

const std::string& UnmappedNode::GetType(void) const
{
	return typeName;
}

size_t UnmappedNode::GetNumberOfInputs(void) const
{
	return numberOfIns;
}

size_t UnmappedNode::GetNumberOfOutputs(void) const
{
	return numberOfOutputPins;
}

size_t UnmappedNode::GetNumberOfSuccessors(size_t output) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	return numberOfOuts[output];
}

size_t UnmappedNode::GetInputPinId(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return inData[input].PinSort;
}

size_t UnmappedNode::GetOutputPinId(size_t output) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	return outData[output].PinSort;
}

size_t UnmappedNode::GetPortPinId(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInputPinId(port.portNumber);
		case PortType::Output:
			return GetOutputPinId(port.portNumber);
		default:
			LOG(FATAL) << "Unknown PortType";
			__builtin_unreachable();
	}
}

size_t UnmappedNode::GetInputConnectionId(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return inData[input].PinWireId;
}

size_t UnmappedNode::GetOutputConnectionId(size_t output) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	return outData[output].PinWireId;
}

size_t UnmappedNode::GetPortConnectionId(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInputConnectionId(port.portNumber);
		case PortType::Output:
			return GetOutputConnectionId(port.portNumber);
		default:
			LOG(FATAL) << "Unknown PortType";
			__builtin_unreachable();
	}
}

const UnmappedNode* UnmappedNode::GetInput(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return in[input];
}

const UnmappedSuccessorList UnmappedNode::GetOutput(size_t output) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	return UnmappedSuccessorList(out[output], numberOfOuts[output]);
}

const UnmappedNode* UnmappedNode::GetSuccessor(size_t output, size_t successor) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	DASSERT(successor < numberOfOuts[output]) << "The requested successor " << successor << " does not exist";
	return out[output][successor];
}

const UnmappedInputList UnmappedNode::GetInputs(void) const
{
	return UnmappedInputList(in, numberOfIns);
}

const UnmappedOutputList UnmappedNode::GetOutputs(void) const
{
	return UnmappedOutputList(out, numberOfOuts, numberOfOutputPins);
}

const UnmappedSuccessorList UnmappedNode::GetSuccessors(size_t output) const
{
	return UnmappedSuccessorList(out[output], numberOfOuts[output]);
}

const UnmappedInputEnumeration UnmappedNode::EnumerateInputs(void) const
{
	return UnmappedInputEnumeration(in, numberOfIns);
}

const UnmappedOutputEnumeration UnmappedNode::EnumerateOutputs(void) const
{
	return UnmappedOutputEnumeration(out, numberOfOuts, numberOfOutputPins);
}

const UnmappedSuccessorEnumeration UnmappedNode::EnumerateSuccessors(size_t output) const
{
	return UnmappedSuccessorEnumeration(out[output], numberOfOuts[output]);
}

const UnmappedPinData* UnmappedNode::GetInputPinData(size_t input) const
{
	DASSERT(input < numberOfIns) << "The requested input " << input << " does not exist";
	return &inData[input];
}

const UnmappedPinData* UnmappedNode::GetPinDataForPort(const Port& port) const
{
	switch (port.portType)
	{
		case PortType::Input:
			return GetInputPinData(port.portNumber);
		case PortType::Output:
			return GetOutputPinData(port.portNumber);
		default:
			Logging::Panic("Invalid port type specificed");
	}
}

const UnmappedPinData* UnmappedNode::GetOutputPinData(size_t output) const
{
	DASSERT(output < numberOfOutputPins) << "The requested output " << output << " does not exist";
	return &outData[output];
}

const UnmappedPinDataList UnmappedNode::GetInputPinData(void) const
{
	return UnmappedPinDataList(inData, numberOfIns);
}

const UnmappedPinDataList UnmappedNode::GetOutputPinData(void) const
{
	return UnmappedPinDataList(outData, numberOfOutputPins);
}

const UnmappedPinDataEnumeration UnmappedNode::EnumerateInputPinData(void) const
{
	return UnmappedPinDataEnumeration(inData, numberOfIns);
}

const UnmappedPinDataEnumeration UnmappedNode::EnumerateOutputPinData(void) const
{
	return UnmappedPinDataEnumeration(outData, numberOfOutputPins);
}

};
};
