#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Circuit
{

MappedCircuit::MappedCircuit(void) = default;
MappedCircuit::~MappedCircuit(void) = default;

std::vector<const MappedNode*>::const_iterator MappedCircuit::begin(void) const
{
	return _nodes.begin();
}

std::vector<const MappedNode*>::const_iterator MappedCircuit::end(void) const
{
	return _nodes.end();
}

std::vector<const MappedNode*>::const_iterator MappedCircuit::cbegin(void) const
{
	return _nodes.cbegin();
}

std::vector<const MappedNode*>::const_iterator MappedCircuit::cend(void) const
{
	return _nodes.cend();
}

size_t MappedCircuit::GetNumberOfSecondaryInputs(void) const
{
	return _secInputs.size();
}

size_t MappedCircuit::GetNumberOfPrimaryInputs(void) const
{
	return _primInputs.size();
}

size_t MappedCircuit::GetNumberOfInputs(void) const
{
	return _inputs.size();
}

size_t MappedCircuit::GetNumberOfSecondaryOutputs(void) const
{
	return _secOutputs.size();
}

size_t MappedCircuit::GetNumberOfPrimaryOutputs(void) const
{
	return _primOutputs.size();
}

size_t MappedCircuit::GetNumberOfOutputs(void) const
{
	return _outputs.size();
}

size_t MappedCircuit::GetNumberOfNodes(void) const
{
	return _nodeContainer.size();
}

size_t MappedCircuit::GetNumberOfPins(void) const
{
	return _pinIdToNodeIdAndPort.size();
}

const MappedNode* MappedCircuit::GetInput(size_t inputNr) const
{
	ASSERT(inputNr < _inputs.size());
	return _inputs[inputNr];
}

const MappedNode* MappedCircuit::GetPrimaryInput(size_t inputNr) const
{
	ASSERT(inputNr < _primInputs.size());
	return _primInputs[inputNr];
}

const MappedNode* MappedCircuit::GetSecondaryInput(size_t inputNr) const
{
	ASSERT(inputNr < _secInputs.size());
	return _secInputs[inputNr];
}

const MappedNode* MappedCircuit::GetOutput(size_t outputNr) const
{
	ASSERT(outputNr < _outputs.size());
	return _outputs[outputNr];
}

const MappedNode* MappedCircuit::GetPrimaryOutput(size_t outputNr) const
{
	ASSERT(outputNr < _outputs.size());
	return _primOutputs[outputNr];
}

const MappedNode* MappedCircuit::GetSecondaryOutput(size_t outputNr) const
{
	ASSERT(outputNr < _secOutputs.size());
	return _secOutputs[outputNr];
}

NodeList MappedCircuit::GetNodes(void) const
{
	if (__builtin_expect(_nodes.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_nodes[0u], _nodes.size());
}

NodeList MappedCircuit::GetInputs(void) const
{
	if (__builtin_expect(_inputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_inputs[0u], _inputs.size());
}

NodeList MappedCircuit::GetPrimaryInputs(void) const
{
	if (__builtin_expect(_primInputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_primInputs[0u], _primInputs.size());
}

NodeList MappedCircuit::GetSecondaryInputs(void) const
{
	if (__builtin_expect(_secInputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_secInputs[0u], _secInputs.size());
}

NodeList MappedCircuit::GetOutputs(void) const
{
	if (__builtin_expect(_outputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_outputs[0u], _outputs.size());
}

NodeList MappedCircuit::GetPrimaryOutputs(void) const
{
	if (__builtin_expect(_primOutputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_primOutputs[0u], _primOutputs.size());
}

NodeList MappedCircuit::GetSecondaryOutputs(void) const
{
	if (__builtin_expect(_secOutputs.size() == 0u, false))
	{
		return NodeList(nullptr, 0u);
	}

	return NodeList(&_secOutputs[0u], _secOutputs.size());
}

NodeEnumeration MappedCircuit::EnumerateNodes(void) const
{
	if (__builtin_expect(_nodes.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_nodes[0u], _nodes.size());
}

NodeEnumeration MappedCircuit::EnumerateInputs(void) const
{
	if (__builtin_expect(_inputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_inputs[0u], _inputs.size());
}

NodeEnumeration MappedCircuit::EnumeratePrimaryInputs(void) const
{
	if (__builtin_expect(_primInputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_primInputs[0u], _primInputs.size());
}

NodeEnumeration MappedCircuit::EnumerateSecondaryInputs(void) const
{
	if (__builtin_expect(_secInputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_secInputs[0u], _secInputs.size());
}

NodeEnumeration MappedCircuit::EnumerateOutputs(void) const
{
	if (__builtin_expect(_outputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_outputs[0u], _outputs.size());
}

NodeEnumeration MappedCircuit::EnumeratePrimaryOutputs(void) const
{
	if (__builtin_expect(_primOutputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_primOutputs[0u], _primOutputs.size());
}

NodeEnumeration MappedCircuit::EnumerateSecondaryOutputs(void) const
{
	if (__builtin_expect(_secOutputs.size() == 0u, false))
	{
		return NodeEnumeration(nullptr, 0u);
	}

	return NodeEnumeration(&_secOutputs[0u], _secOutputs.size());
}

bool MappedCircuit::IsInput(const MappedNode* node) const
{
	return node->mainType == CellCategory::MAIN_IN;
}

bool MappedCircuit::IsOutput(const MappedNode* node) const
{
	return node->mainType == CellCategory::MAIN_OUT;
}

bool MappedCircuit::IsPrimaryInput(const MappedNode* node) const
{
	return node->mainType == CellCategory::MAIN_IN && node->type == CellType::P_IN;
}

bool MappedCircuit::IsPrimaryOutput(const MappedNode* node) const
{
	return node->mainType == CellCategory::MAIN_OUT && node->type == CellType::P_OUT;
}

bool MappedCircuit::IsSecondaryInput(const MappedNode* node) const
{
	return node->mainType == CellCategory::MAIN_IN && node->type == CellType::S_IN;
}

bool MappedCircuit::IsSecondaryOutput(const MappedNode* node) const
{
	if (node->mainType != CellCategory::MAIN_OUT)
	{
		return false;
	}

	switch (node->type)
	{
	case CellType::S_OUT:
	case CellType::S_OUT_CLK:
	case CellType::S_OUT_EN:
		return true;
	default:
		return false;
	}
}

bool MappedCircuit::IsInput(size_t nodeId) const
{
	return IsInput(GetNode(nodeId));
}

bool MappedCircuit::IsOutput(size_t nodeId) const
{
	return IsOutput(GetNode(nodeId));
}

bool MappedCircuit::IsPrimaryInput(size_t nodeId) const
{
	return IsPrimaryInput(GetNode(nodeId));
}

bool MappedCircuit::IsPrimaryOutput(size_t nodeId) const
{
	return IsPrimaryOutput(GetNode(nodeId));
}

bool MappedCircuit::IsSecondaryInput(size_t nodeId) const
{
	return IsSecondaryInput(GetNode(nodeId));
}

bool MappedCircuit::IsSecondaryOutput(size_t nodeId) const
{
	return IsSecondaryOutput(GetNode(nodeId));
}

size_t MappedCircuit::GetInputNumber(const MappedNode* node) const
{
	DASSERT(_inputNrMap.contains(node->GetNodeId()));
	return _inputNrMap.at(node->GetNodeId());
}

size_t MappedCircuit::GetPrimaryInputNumber(const MappedNode* node) const
{
	DASSERT(_primInputNrMap.contains(node->GetNodeId()));
	return _primInputNrMap.at(node->GetNodeId());
}

size_t MappedCircuit::GetSecondaryInputNumber(const MappedNode* node) const
{
	DASSERT(_secInputNrMap.contains(node->GetNodeId()));
	return _secInputNrMap.at(node->GetNodeId());
}

size_t MappedCircuit::GetOutputNumber(const MappedNode* node) const
{
	DASSERT(_outputNrMap.contains(node->GetNodeId()));
	return _outputNrMap.at(node->GetNodeId());
}

size_t MappedCircuit::GetPrimaryOutputNumber(const MappedNode* node) const
{
	DASSERT(_primOutputNrMap.contains(node->GetNodeId()));
	return _primOutputNrMap.at(node->GetNodeId());
}

size_t MappedCircuit::GetSecondaryOutputNumber(const MappedNode* node) const
{
	DASSERT(_secOutputNrMap.contains(node->GetNodeId()));
	return _secOutputNrMap.at(node->GetNodeId());
}

const MappedNode* MappedCircuit::GetSecondaryInputForSecondaryOutput(const MappedNode* node) const
{
	DASSERT(_soutToSinMap.contains(node->GetNodeId()));
	return _soutToSinMap.at(node->GetNodeId());
}

const MappedNode* MappedCircuit::GetSecondaryOutputForSecondaryInput(const MappedNode* node) const
{
	DASSERT(_sinToSoutMap.contains(node->GetNodeId()));
	return _sinToSoutMap.at(node->GetNodeId());
}

const MappedNode* MappedCircuit::GetNode(size_t nodeId) const
{
	DASSERT(nodeId < _nodeContainer.size()) << "Invalid node id requested: " << nodeId;
	return &(_nodeContainer[nodeId]);
}

const MappedNode* MappedCircuit::GetNodeWithName(std::string name) const
{
	if (auto it = _nodeNameToNodeId.find(name); it != _nodeNameToNodeId.end())
	{
		return GetNode(*it);
	}

	return nullptr;
}

const MappedNode* MappedCircuit::GetNodeWithOutputSignal(std::string name) const
{
	if (auto it = _wireNameToNodeId.find(name); it != _wireNameToNodeId.end())
	{
		return GetNode(*it);
	}

	return nullptr;
}

std::vector<MappedCircuit::NodeAndPort> MappedCircuit::GetNodesWithInputSignal(std::string name) const
{
	if (auto it = _wireNameToNodeId.find(name); it != _wireNameToNodeId.end())
	{
		auto node = GetNode(*it);

		std::vector<NodeAndPort> result;
		for (size_t successor = 0u; successor < node->numberOfOuts; ++successor)
		{
			// Emplace back only supported in C++ 20 (P0960)
			result.push_back({ node->GetSuccessor(successor), { PortType::Input, node->successorInDataID[successor] } });
		}

		return result;
	}

	return { };
}

std::vector<const MappedNode*> MappedCircuit::GetNodesWithNamePrefix(std::string name) const
{
	auto [start, end] = _nodeNameToNodeId.equal_prefix_range(name);

	std::vector<const MappedNode*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(GetNode(*element));
	}
	return result;
}

std::vector<const MappedNode*> MappedCircuit::GetNodesWithOutputSignalPrefix(std::string name) const
{
	auto [start, end] = _wireNameToNodeId.equal_prefix_range(name);

	std::vector<const MappedNode*> result;
	for (auto element = start; element != end; ++element)
	{
		result.emplace_back(GetNode(*element));
	}
	return result;
}

std::vector<MappedCircuit::NodeAndPort> MappedCircuit::GetNodesWithInputSignalPrefix(std::string name) const
{
	auto [start, end] = _wireNameToNodeId.equal_prefix_range(name);

	std::vector<NodeAndPort> result;
	for (auto element = start; element != end; ++element)
	{
		const auto* node = GetNode(*element);
		for (size_t successor = 0u; successor < node->numberOfOuts; ++successor)
		{
			// Emplace back only supported in C++ 20 (P0960)
			result.push_back({ node->GetSuccessor(successor), { PortType::Input, node->successorInDataID[successor] } });
		}
	}
	return result;
}

const MappedNode* MappedCircuit::GetNodeForPinId(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	return GetNode(_pinIdToNodeIdAndPort[pinId].nodeId);
}

Port MappedCircuit::GetPortForPinId(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	return _pinIdToNodeIdAndPort[pinId].port;
}

MappedCircuit::NodeAndPort MappedCircuit::GetNodeAndPortForPinId(size_t pinId) const
{
	DASSERT(pinId < _pinIdToNodeIdAndPort.size()) << "Invalid pin id requested: " << pinId;
	auto [nodeId, port] = _pinIdToNodeIdAndPort[pinId];
	return { GetNode(nodeId), port };
}

const MappedNode* MappedCircuit::GetDriverForPort(const NodeAndPort& nodeAndPort) const
{
	return nodeAndPort.node->GetDriverForPort(nodeAndPort.port);
}

MappedCircuitDebug::MappedCircuitDebug(const MappedCircuit& circuit, Logging::Verbose verbose):
	circuit(circuit)
{
}

MappedCircuitDebug::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

std::ostream& operator <<(std::ostream& stream, const MappedCircuitDebug& debug)
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

		for (auto [i, successor] : node->EnumerateSuccessors())
		{
			if (i != 0) outputs += ", ";
			outputs += (successor == nullptr) ? "unconnected" : successor->GetName();
		}

		stream << "Mapped Node " << nodeId << " " << node->GetName() << ": "
			<< to_string(node->GetCellType()) << " (" << inputs << ") -> [" << outputs << "]" << std::endl;
	}

	std::string primaryInputs;
	for (auto [i, primaryInput] : debug.circuit.EnumeratePrimaryInputs())
	{
		if (i != 0u) primaryInputs += ", ";
		primaryInputs += primaryInput->GetName();
	}
	stream << "Mapped Primary Inputs: " << primaryInputs << std::endl;

	std::string primaryOutputs;
	for (auto [i, primaryOutput] : debug.circuit.EnumeratePrimaryOutputs())
	{
		if (i != 0u) primaryOutputs += ", ";
		primaryOutputs += primaryOutput->GetName();
	}
	stream << "Mapped Primary Outputs: " << primaryOutputs << std::endl;

	std::string secondaryInputs;
	for (auto [i, secondaryInput] : debug.circuit.EnumerateSecondaryInputs())
	{
		if (i != 0u) secondaryInputs += ", ";
		secondaryInputs += secondaryInput->GetName();
	}
	stream << "Mapped Secondary Inputs: " << secondaryInputs << std::endl;

	std::string secondaryOutputs;
	for (auto [i, secondaryOutput] : debug.circuit.EnumerateSecondaryOutputs())
	{
		if (i != 0u) secondaryOutputs += ", ";
		secondaryOutputs += secondaryOutput->GetName();
	}
	stream << "Mapped Secondary Outputs: " << secondaryOutputs << std::endl;

	return stream;
}

MappedCircuitDebug to_debug(const MappedCircuit& circuit, Logging::Verbose verbose)
{
	return MappedCircuitDebug { circuit, verbose };
}

std::string to_string(const Circuit::MappedCircuit::NodeAndPort& nodeAndPort)
{
	return "Mapped Node: " + nodeAndPort.node->GetName() + " (nodeId " + std::to_string(nodeAndPort.node->GetNodeId()) +  "), Port: " + to_string(nodeAndPort.port);
}

};
};
