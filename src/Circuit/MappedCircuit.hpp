#pragma once

#include <sparsepp/spp.h>
#include <tsl/htrie_map.h>

#include <cstdint>
#include <string>
#include <vector>
#include <limits>

#include "Basic/Logging.hpp"
#include "Basic/Iterator/RawEnumerable.hpp"
#include "Basic/Iterator/RawIterable.hpp"
#include "Circuit/MappedNode.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };

class CircuitEnvironment;

using NodeList = RawIterable<const MappedNode *const, const size_t>;
using NodeEnumeration = RawEnumerable<const MappedNode *const, const size_t>;

class MappedCircuit
{
public:

	static constexpr size_t NO_CONNECTION = std::numeric_limits<size_t>::max();

	struct NodeAndPort {
		const MappedNode* node;
		Port port;

		// Compare in the following order: nodeId, portType (input < output), portNumber
		inline bool operator<(const FreiTest::Circuit::MappedCircuit::NodeAndPort& comp) const
		{
			return (node->GetNodeId() < comp.node->GetNodeId())
				|| (node->GetNodeId() == comp.node->GetNodeId() && port < comp.port);
		}

		inline bool operator==(const FreiTest::Circuit::MappedCircuit::NodeAndPort& comp) const
		{
			return (node->GetNodeId() == comp.node->GetNodeId()) && (port == comp.port);
		}
	};

	// Main constructor is protected
	virtual ~MappedCircuit(void);

	// Disallow move and copy
	MappedCircuit(MappedCircuit&& other) = delete;
	MappedCircuit(const MappedCircuit& other) = delete;
	MappedCircuit& operator=(MappedCircuit&& other) = delete;
	MappedCircuit& operator=(const MappedCircuit& other) = delete;

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	std::vector<const MappedNode*>::const_iterator begin(void) const;
	std::vector<const MappedNode*>::const_iterator end(void) const;

	std::vector<const MappedNode*>::const_iterator cbegin(void) const;
	std::vector<const MappedNode*>::const_iterator cend(void) const;

	size_t GetNumberOfSecondaryInputs(void) const;
	size_t GetNumberOfPrimaryInputs(void) const;
	size_t GetNumberOfInputs(void) const;
	size_t GetNumberOfSecondaryOutputs(void) const;
	size_t GetNumberOfPrimaryOutputs(void) const;
	size_t GetNumberOfOutputs(void) const;
	size_t GetNumberOfNodes(void) const;
	size_t GetNumberOfPins(void) const;

	const MappedNode* GetNode(size_t nodeId) const;
	const MappedNode* GetInput(size_t inputNr) const;
	const MappedNode* GetPrimaryInput(size_t inputNr) const;
	const MappedNode* GetSecondaryInput(size_t inputNr) const;
	const MappedNode* GetOutput(size_t outputNr) const;
	const MappedNode* GetPrimaryOutput(size_t outputNr) const;
	const MappedNode* GetSecondaryOutput(size_t outputNr) const;

    // Returns an iterable (for-loop) list of nodes in the circuit.
    // Each element in this list is a "const MappedNode*".
    // Usage:
    //
    //     for (auto node : circuit.GetNodes()) { ... }
    //     for (auto node : circuit.GetPrimaryInputs()) { ... }
    //     for (auto node : circuit.GetSecondaryOutputs()) { ... }
    //
	NodeList GetNodes(void) const;
	NodeList GetInputs(void) const;
	NodeList GetPrimaryInputs(void) const;
	NodeList GetSecondaryInputs(void) const;
	NodeList GetOutputs(void) const;
	NodeList GetPrimaryOutputs(void) const;
	NodeList GetSecondaryOutputs(void) const;

    // Returns an iterable (for-loop) list of nodes and index in the circuit.
    // Each element in this enumeration is a "const MappedNode*".
    // Usage:
    //
    //     for (auto [nodeId, node] : circuit.EnumerateNodes()) { ... }
    //     for (auto [inputId, node] : circuit.EnumeratePrimaryInputs()) { ... }
    //     for (auto [outputId, node] : circuit.EnumerateSecondaryOutputs()) { ... }
    //
	NodeEnumeration EnumerateNodes(void) const;
	NodeEnumeration EnumerateInputs(void) const;
	NodeEnumeration EnumeratePrimaryInputs(void) const;
	NodeEnumeration EnumerateSecondaryInputs(void) const;
	NodeEnumeration EnumerateOutputs(void) const;
	NodeEnumeration EnumeratePrimaryOutputs(void) const;
	NodeEnumeration EnumerateSecondaryOutputs(void) const;

	bool IsInput(size_t nodeId) const;
	bool IsOutput(size_t nodeId) const;
	bool IsPrimaryInput(size_t nodeId) const;
	bool IsPrimaryOutput(size_t nodeId) const;
	bool IsSecondaryInput(size_t nodeId) const;
	bool IsSecondaryOutput(size_t nodeId) const;

	bool IsInput(const MappedNode* node) const;
	bool IsOutput(const MappedNode* node) const;
	bool IsPrimaryInput(const MappedNode* node) const;
	bool IsPrimaryOutput(const MappedNode* node) const;
	bool IsSecondaryInput(const MappedNode* node) const;
	bool IsSecondaryOutput(const MappedNode* node) const;

	size_t GetInputNumber(const MappedNode* input) const;
	size_t GetPrimaryInputNumber(const MappedNode* primaryInput) const;
	size_t GetSecondaryInputNumber(const MappedNode* secondaryInput) const;
	size_t GetOutputNumber(const MappedNode* output) const;
	size_t GetPrimaryOutputNumber(const MappedNode* primaryOutput) const;
	size_t GetSecondaryOutputNumber(const MappedNode* secondaryOutput) const;

	const MappedNode* GetSecondaryInputForSecondaryOutput(const MappedNode* secondaryInput) const;
	const MappedNode* GetSecondaryOutputForSecondaryInput(const MappedNode* secondaryOutput) const;

	const MappedNode* GetNodeWithName(std::string name) const;
	const MappedNode* GetNodeWithOutputSignal(std::string name) const;
	std::vector<NodeAndPort> GetNodesWithInputSignal(std::string name) const;

	std::vector<const MappedNode*> GetNodesWithNamePrefix(std::string name) const;
	std::vector<const MappedNode*> GetNodesWithOutputSignalPrefix(std::string name) const;
	std::vector<NodeAndPort> GetNodesWithInputSignalPrefix(std::string name) const;

	const MappedNode* GetNodeForPinId(size_t pinId) const;
	Port GetPortForPinId(size_t pinId) const;
	NodeAndPort GetNodeAndPortForPinId(size_t pinId) const;

	const MappedNode* GetDriverForPort(const NodeAndPort& nodeAndPort) const;

	// These are classes which are allowed to change the circuit
	friend CircuitEnvironment;
	friend MappedNode;
	friend Builder::CircuitBuilder;

private:
	struct NodeIdAndPort {
		size_t nodeId;
		Port port;
	};

	MappedCircuit(void);

	std::vector<MappedNode> _nodeContainer;

	std::vector<const MappedNode*> _nodes;
	std::vector<const MappedNode*> _inputs;
	std::vector<const MappedNode*> _primInputs;
	std::vector<const MappedNode*> _secInputs;
	std::vector<const MappedNode*> _outputs;
	std::vector<const MappedNode*> _primOutputs;
	std::vector<const MappedNode*> _secOutputs;

	// mapping from unique pin id to node id and pin number
	std::vector<NodeIdAndPort> _pinIdToNodeIdAndPort;

	// mapping from node name to node id
	tsl::htrie_map<char, size_t> _nodeNameToNodeId;
	tsl::htrie_map<char, size_t> _wireNameToNodeId;

	spp::sparse_hash_map<size_t, MappedNode*> _sinToSoutMap;
	spp::sparse_hash_map<size_t, MappedNode*> _soutToSinMap;

	spp::sparse_hash_map<size_t, size_t> _primInputNrMap;
	spp::sparse_hash_map<size_t, size_t> _secInputNrMap;
	spp::sparse_hash_map<size_t, size_t> _inputNrMap;

	spp::sparse_hash_map<size_t, size_t> _primOutputNrMap;
	spp::sparse_hash_map<size_t, size_t> _secOutputNrMap;
	spp::sparse_hash_map<size_t, size_t> _outputNrMap;

	// Storage for nodes variable-sized fields.
	// This saves the need to allocate the fields for every node on the heap
	// which would require more memory and would yield a worse cache locality.
	std::vector<size_t> _nodePinData;
	std::vector<size_t> _nodeConnectionData;
	std::vector<MappedNode*> _nodeIn;
	std::vector<MappedNode*> _nodeOut;
	std::vector<size_t> _nodeSuccessorInData;

};

class MappedCircuitDebug
{
public:
	MappedCircuitDebug(const MappedCircuit& circuit, Logging::Verbose verbose);
	operator std::string(void) const;

	friend std::ostream& operator <<(std::ostream& stream, const MappedCircuitDebug& debug);

private:
	const MappedCircuit& circuit;
};

MappedCircuitDebug to_debug(const MappedCircuit& circuit, Logging::Verbose verbose = Logging::Verbose::No);

std::string to_string(const Circuit::MappedCircuit::NodeAndPort& nodeAndPort);

};
};

