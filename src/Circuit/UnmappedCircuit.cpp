#include "Circuit/UnmappedCircuit.hpp"

namespace FreiTest
{
namespace Circuit
{

UnmappedCircuit::UnmappedCircuit(void) = default;
UnmappedCircuit::~UnmappedCircuit() = default;

std::vector<const UnmappedNode*>::const_iterator UnmappedCircuit::begin(void) const
{
	return _nodes.begin();
}

std::vector<const UnmappedNode*>::const_iterator UnmappedCircuit::end(void) const
{
	return _nodes.end();
}

std::vector<const UnmappedNode*>::const_iterator UnmappedCircuit::cbegin(void) const
{
	return _nodes.cbegin();
}

std::vector<const UnmappedNode*>::const_iterator UnmappedCircuit::cend(void) const
{
	return _nodes.cend();
}

size_t UnmappedCircuit::GetNumberOfPrimaryInputs(void) const
{
	return _primInputs.size();
}

size_t UnmappedCircuit::GetNumberOfPrimaryOutputs(void) const
{
	return _primOutputs.size();
}

size_t UnmappedCircuit::GetNumberOfNodes(void) const
{
	return _nodeContainer.size();
}

size_t UnmappedCircuit::GetNumberOfPins(void) const
{
	return _pinIdToNodeIdAndPort.size();
}

const UnmappedNode* UnmappedCircuit::GetNode(size_t nodeId) const
{
	DASSERT(nodeId < _nodeContainer.size()) << "Invalid node id requested: " << nodeId;
	return &(_nodeContainer[nodeId]);
}

const UnmappedNode* UnmappedCircuit::GetPrimaryInput(size_t inputNr) const
{
	DASSERT(inputNr < _primInputs.size()) << "Invalid primary input requested: " << inputNr;
	return _primInputs[inputNr];
}

const UnmappedNode* UnmappedCircuit::GetPrimaryOutput(size_t outputNr) const
{
	DASSERT(outputNr < _primOutputs.size()) << "Invalid primary output requested: " << outputNr;
	return _primOutputs[outputNr];
}

UnmappedNodeList UnmappedCircuit::GetNodes(void) const
{
	return UnmappedNodeList(&_nodes[0u], _nodes.size());
}

UnmappedNodeList UnmappedCircuit::GetPrimaryInputs(void) const
{
	return UnmappedNodeList(&_primInputs[0u], _primInputs.size());
}

UnmappedNodeList UnmappedCircuit::GetPrimaryOutputs(void) const
{
	return UnmappedNodeList(&_primOutputs[0u], _primOutputs.size());
}

UnmappedNodeEnumeration UnmappedCircuit::EnumerateNodes(void) const
{
	return UnmappedNodeEnumeration(&_nodes[0u], _nodes.size());
}

UnmappedNodeEnumeration UnmappedCircuit::EnumeratePrimaryInputs(void) const
{
	return UnmappedNodeEnumeration(&_primInputs[0u], _primInputs.size());
}

UnmappedNodeEnumeration UnmappedCircuit::EnumeratePrimaryOutputs(void) const
{
	return UnmappedNodeEnumeration(&_primOutputs[0u], _primOutputs.size());
}

bool UnmappedCircuit::IsPrimaryInput(const UnmappedNode* node) const
{
	return _primInputNrMap.contains(node->GetNodeId());
}

bool UnmappedCircuit::IsPrimaryOutput(const UnmappedNode* node) const
{
	return _primOutputNrMap.contains(node->GetNodeId());
}

bool UnmappedCircuit::IsPrimaryInput(size_t nodeId) const
{
	return IsPrimaryInput(GetNode(nodeId));
}

bool UnmappedCircuit::IsPrimaryOutput(size_t nodeId) const
{
	return IsPrimaryOutput(GetNode(nodeId));
}

size_t UnmappedCircuit::GetPrimaryInputNumber(const UnmappedNode* node) const
{
	DASSERT(_primInputNrMap.contains(node->GetNodeId()));
	return _primInputNrMap.at(node->GetNodeId());
}

size_t UnmappedCircuit::GetPrimaryOutputNumber(const UnmappedNode* node) const
{
	DASSERT(_primOutputNrMap.contains(node->GetNodeId()));
	return _primOutputNrMap.at(node->GetNodeId());
}

const UnmappedNode* UnmappedCircuit::GetNodeWithName(std::string name) const
{
	if (auto it = _nodeNameToNodeId.find(name); it != _nodeNameToNodeId.end())
	{
		return GetNode(*it);
	}

	return nullptr;
}

UnmappedCircuit::NodeAndPort UnmappedCircuit::GetNodeWithOutputSignal(std::string name) const
{
	if (auto it = _wireNameToNodeId.find(name); it != _wireNameToNodeId.end())
	{
		auto [nodeId, pinIndex ] = *it;
		return { GetNode(nodeId), { PortType::Output, pinIndex } };
	}

	return { nullptr, { PortType::Output, 0u} };
}

std::vector<UnmappedCircuit::NodeAndPort> UnmappedCircuit::GetNodesWithInputSignal(std::string name) const
{
	if (auto it = _wireNameToNodeId.find(name); it != _wireNameToNodeId.end())
	{
		auto [nodeId, pinIndex ] = *it;

		std::vector<NodeAndPort> result;
		const UnmappedNode* node = GetNode(nodeId);
		const size_t wireId = node->GetOutputPinData(pinIndex)->PinWireId;
		for (size_t successor = 0u; successor < node->numberOfOuts[pinIndex]; ++successor)
		{
			// Find the (possible multiple) driven input ports of the successor
			const auto* successorNode = node->GetSuccessor(pinIndex, successor);
			for (size_t inputIndex = 0u; inputIndex < successorNode->GetNumberOfInputs(); ++inputIndex)
			{
				if (successorNode->GetInputPinData(inputIndex)->PinWireId == wireId)
				{
					// Emplace back only supported in C++ 20 (P0960)
					result.push_back({ successorNode, { PortType::Input, inputIndex } });
				}
			}
		}

		return result;
	}

	return { };
}

std::vector<const UnmappedNode*> UnmappedCircuit::GetNodesWithNamePrefix(std::string name) const
{
	auto [start, end] = _nodeNameToNodeId.equal_prefix_range(name);

	std::vector<const UnmappedNode*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(GetNode(*element));
	}
	return result;
}

std::vector<UnmappedCircuit::NodeAndPort> UnmappedCircuit::GetNodesWithOutputSignalPrefix(std::string name) const
{
	auto [start, end] = _wireNameToNodeId.equal_prefix_range(name);

	std::vector<NodeAndPort> result;
	for (auto element = start; element != end; ++element)
	{
		auto [nodeId, pinIndex] = *element;

		// Emplace back only supported in C++ 20 (P0960)
		result.push_back({ GetNode(nodeId), { PortType::Output, pinIndex } });
	}

	return result;
}

std::vector<UnmappedCircuit::NodeAndPort> UnmappedCircuit::GetNodesWithInputSignalPrefix(std::string name) const
{
	auto [start, end] = _wireNameToNodeId.equal_prefix_range(name);

	std::vector<NodeAndPort> result;
	for (auto element = start; element != end; ++element)
	{
		auto [nodeId, pinIndex] = *element;

		const auto* node = GetNode(nodeId);
		const size_t wireId = node->GetOutputPinData(pinIndex)->PinWireId;
		for (size_t successor = 0u; successor < node->numberOfOuts[pinIndex]; ++successor)
		{
			// Find the (possible multiple) driven input ports of the successor
			const auto* successorNode = node->GetSuccessor(pinIndex, successor);
			for (size_t inputIndex = 0u; inputIndex < successorNode->GetNumberOfInputs(); ++inputIndex)
			{
				if (successorNode->GetInputPinData(inputIndex)->PinWireId == wireId)
				{
					// Emplace back only supported in C++ 20 (P0960)
					result.push_back({ successorNode, { PortType::Input, inputIndex } });
				}
			}
		}
	}

	return result;
}

const UnmappedPinData* UnmappedCircuit::GetPinData(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	auto [nodeId, port] = _pinIdToNodeIdAndPort[pinId];
	return GetPinData(nodeId, port);
}

const UnmappedPinData* UnmappedCircuit::GetPinData(size_t nodeId, Port port) const
{
	DASSERT(nodeId < _nodeContainer.size()) << "Invalid node id requested: " << nodeId;
	return GetPinData(nodeId, port.portType, port.portNumber);
}

const UnmappedPinData* UnmappedCircuit::GetPinData(size_t nodeId, PortType portType, size_t portNumber) const
{
	DASSERT(nodeId < _nodeContainer.size()) << "Invalid node id requested: " << nodeId;
	return (portType == PortType::Input)
		? GetNode(nodeId)->GetInputPinData(portNumber)
		: GetNode(nodeId)->GetOutputPinData(portNumber);
}

const UnmappedNode* UnmappedCircuit::GetNodeForPin(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	return GetNode(_pinIdToNodeIdAndPort[pinId].nodeId);
}

Port UnmappedCircuit::GetPortForPin(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	return _pinIdToNodeIdAndPort[pinId].port;
}

UnmappedCircuit::NodeAndPort UnmappedCircuit::GetNodeAndPortForPin(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	auto [nodeId, port] = _pinIdToNodeIdAndPort[pinId];
	return { GetNode(nodeId), port };
}

UnmappedCircuit::NodeAndPort UnmappedCircuit::GetDriverForPort(const UnmappedCircuit::NodeAndPort& nodeAndPort) const
{
	auto [node, port] = nodeAndPort;
	switch (port.portType)
	{
		case PortType::Input:
		{
			const auto* driverNode = node->GetInput(port.portNumber);
			ASSERT(driverNode) << "No node is connected to input " << std::to_string(port.portNumber) << " of node " << node->GetName();
			const auto* pinData = node->GetInputPinData(port.portNumber);
			for (size_t outputPortNumber = 0u; outputPortNumber < driverNode->GetNumberOfOutputs(); ++outputPortNumber)
			{
				const auto* outputPinData = driverNode->GetOutputPinData(outputPortNumber);
				if (pinData->PinWireId == outputPinData->PinWireId && pinData->PinWireName == outputPinData->PinWireName)
				{
					return { driverNode, { PortType::Output, outputPortNumber } };
				}
			}
			LOG(FATAL) << "Failed to find driver for Port " << to_string(port) << " of Unmapped Node " << node->GetName();
		}
		case PortType::Output:
			return nodeAndPort;
		default:
			LOG(FATAL) << "Unknown portType";
			__builtin_unreachable();
	}
}

UnmappedCircuitDebug::UnmappedCircuitDebug(const UnmappedCircuit& circuit, Logging::Verbose verbose):
	circuit(circuit)
{
}

UnmappedCircuitDebug::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

std::ostream& operator <<(std::ostream& stream, const UnmappedCircuitDebug& debug)
{
	for (auto [nodeId, node] : debug.circuit.EnumerateNodes())
	{
		std::string inputs;
		std::string outputs;

		for (auto [i, input] : node->EnumerateInputs())
		{
			if (i != 0) inputs += ", ";
			inputs += (input == nullptr) ? "unconnected" : input->GetName();
		}

		for (auto [i, output] : node->EnumerateOutputs())
		{
			if (i != 0) outputs += ", ";

			outputs += "[";
			for (auto [j, successor] : node->EnumerateSuccessors(i))
			{
				if (j != 0) outputs += ", ";
				outputs += (successor == nullptr) ? "unconnected" : successor->GetName();
			}
			outputs += "]";
		}

		stream << "Unmapped Node " << nodeId << " " << node->GetName() << ": "
			<< node->GetType() << " (" << inputs << ") -> (" << outputs << ")" << std::endl;
	}

	std::string primaryInputs;
	for (auto [i, primaryInput] : debug.circuit.EnumeratePrimaryInputs())
	{
		if (i != 0u) primaryInputs += ", ";
		primaryInputs += primaryInput->GetName();
	}
	stream << "Unmapped Primary Inputs: " << primaryInputs << std::endl;

	std::string primaryOutputs;
	for (auto [i, primaryOutput] : debug.circuit.EnumeratePrimaryOutputs())
	{
		if (i != 0u) primaryOutputs += ", ";
		primaryOutputs += primaryOutput->GetName();
	}
	stream << "Unmapped Primary Outputs: " << primaryOutputs << std::endl;

	return stream;
}

UnmappedCircuitDebug to_debug(const UnmappedCircuit& circuit, Logging::Verbose verbose)
{
	return UnmappedCircuitDebug { circuit, verbose };
}

std::string to_string(const Circuit::UnmappedCircuit::NodeAndPort& nodeAndPort)
{
	return "Unmapped Node: " + nodeAndPort.node->GetName() + " (" + std::to_string(nodeAndPort.node->GetNodeId()) +  "), Port: " + to_string(nodeAndPort.port);
}

};
};
