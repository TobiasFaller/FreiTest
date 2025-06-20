#pragma once

#include <sparsepp/spp.h>
#include <tsl/htrie_map.h>

#include <cstdint>
#include <string>
#include <vector>
#include <limits>

#include "Basic/Logging.hpp"
#include "Basic/Iterator/RawIterable.hpp"
#include "Basic/Iterator/RawEnumerable.hpp"
#include "Circuit/UnmappedNode.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };

class CircuitEnvironment;

using UnmappedNodeList = RawIterable<const UnmappedNode *const, const size_t>;
using UnmappedNodeEnumeration = RawEnumerable<const UnmappedNode *const, const size_t>;

class UnmappedCircuit
{
public:

	static constexpr size_t NO_CONNECTION = std::numeric_limits<size_t>::max();

	struct NodeAndPort {
		const UnmappedNode* node;
		Port port;

		// Compare in the following order: nodeId, portType (input < output), portNumber
		inline bool operator<(const FreiTest::Circuit::UnmappedCircuit::NodeAndPort& comp) const
		{
			return node->GetNodeId() < comp.node->GetNodeId() || (node->GetNodeId() == comp.node->GetNodeId() && port < comp.port);
		}

		inline bool operator==(const FreiTest::Circuit::UnmappedCircuit::NodeAndPort& comp) const
		{
			return node->GetNodeId() == comp.node->GetNodeId() && port == comp.port;
		}
	};

	// Main constructor is protected
	virtual ~UnmappedCircuit(void);

	// Disallow move and copy
	UnmappedCircuit(UnmappedCircuit&& other) = delete;
	UnmappedCircuit(const UnmappedCircuit& other) = delete;
	UnmappedCircuit& operator=(UnmappedCircuit&& other) = delete;
	UnmappedCircuit& operator=(const UnmappedCircuit& other) = delete;

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	std::vector<const UnmappedNode*>::const_iterator begin(void) const;
	std::vector<const UnmappedNode*>::const_iterator end(void) const;

	std::vector<const UnmappedNode*>::const_iterator cbegin(void) const;
	std::vector<const UnmappedNode*>::const_iterator cend(void) const;

	size_t GetNumberOfPrimaryInputs(void) const;
	size_t GetNumberOfPrimaryOutputs(void) const;
	size_t GetNumberOfNodes(void) const;
	size_t GetNumberOfPins(void) const;

    // Returns an iterable (for-loop) list of nodes in the circuit.
    // Each element in this list is a "const UnmappedNode*".
    // Usage:
    //
    //     for (auto node : circuit.GetNodes()) { ... }
    //     for (auto node : circuit.GetPrimaryInputs()) { ... }
    //     for (auto node : circuit.GetPrimaryOutputs()) { ... }
    //
	UnmappedNodeList GetNodes(void) const;
	UnmappedNodeList GetPrimaryInputs(void) const;
	UnmappedNodeList GetPrimaryOutputs(void) const;

    // Returns an iterable (for-loop) list of nodes and index in the circuit.
    // Each element in this enumeration is a "const UnmappedNode*".
    // Usage:
    //
    //     for (auto [nodeId, node] : circuit.EnumerateNodes()) { ... }
    //     for (auto [inputId, node] : circuit.EnumeratePrimaryInputs()) { ... }
    //     for (auto [outputId, node] : circuit.EnumeratePrimaryOutputs()) { ... }
    //
	UnmappedNodeEnumeration EnumerateNodes(void) const;
	UnmappedNodeEnumeration EnumeratePrimaryInputs(void) const;
	UnmappedNodeEnumeration EnumeratePrimaryOutputs(void) const;

	bool IsPrimaryInput(size_t nodeId) const;
	bool IsPrimaryOutput(size_t nodeId) const;

	bool IsPrimaryInput(const UnmappedNode* node) const;
	bool IsPrimaryOutput(const UnmappedNode* node) const;

	size_t GetPrimaryInputNumber(const UnmappedNode* primaryInput) const;
	size_t GetPrimaryOutputNumber(const UnmappedNode* primaryOutput) const;

    // Returns a single node or primary port (a node too).
	const UnmappedNode* GetNode(size_t nodeId) const;
	const UnmappedNode* GetPrimaryInput(size_t inputNr) const;
	const UnmappedNode* GetPrimaryOutput(size_t outputNr) const;

	const UnmappedNode* GetNodeWithName(std::string name) const;
	NodeAndPort GetNodeWithOutputSignal(std::string name) const;
	std::vector<NodeAndPort> GetNodesWithInputSignal(std::string name) const;

	std::vector<const UnmappedNode*> GetNodesWithNamePrefix(std::string name) const;
	std::vector<NodeAndPort> GetNodesWithOutputSignalPrefix(std::string name) const;
	std::vector<NodeAndPort> GetNodesWithInputSignalPrefix(std::string name) const;

	const UnmappedPinData* GetPinData(size_t pinId) const;
	const UnmappedPinData* GetPinData(size_t nodeId, Port port) const;
	const UnmappedPinData* GetPinData(size_t nodeId, PortType portType, size_t portNumber) const;
	const UnmappedNode* GetNodeForPin(size_t pinId) const;
	Port GetPortForPin(size_t pinId) const;
	NodeAndPort GetNodeAndPortForPin(size_t pinId) const;

	NodeAndPort GetDriverForPort(const NodeAndPort& nodeAndPort) const;

	friend CircuitEnvironment;
	friend UnmappedNode;
	friend Builder::CircuitBuilder;

private:
	// Internal structure that does store node id an port and port type.
	// The port type has to be encoded to know if to access an input or output.
	struct NodeIdAndPort {
		size_t nodeId;
		Port port;
	};

	// We only store output pins in the _wireNameToNodeId collection
	// as we can already infer the successor input pins from this.
	struct NodeIdAndPinIndex {
		size_t nodeId;
		size_t pinIndex;
	};

	UnmappedCircuit(void);

	std::vector<UnmappedNode> _nodeContainer;

	std::vector<const UnmappedNode*> _nodes;
	std::vector<const UnmappedNode*> _primInputs;
	std::vector<const UnmappedNode*> _primOutputs;

	// mapping from unique pin id to node id and pin number
	std::vector<NodeIdAndPort> _pinIdToNodeIdAndPort;

	// mapping from names to nodes
	tsl::htrie_map<char, size_t> _nodeNameToNodeId;
	tsl::htrie_map<char, NodeIdAndPinIndex> _wireNameToNodeId;

	spp::sparse_hash_map<size_t, size_t> _primInputNrMap;
	spp::sparse_hash_map<size_t, size_t> _primOutputNrMap;

};

class UnmappedCircuitDebug
{
public:
	UnmappedCircuitDebug(const UnmappedCircuit& circuit, Logging::Verbose verbose);
	operator std::string(void) const;

	friend std::ostream& operator <<(std::ostream& stream, const UnmappedCircuitDebug& debug);

private:
	const UnmappedCircuit& circuit;
};

UnmappedCircuitDebug to_debug(const UnmappedCircuit& circuit, Logging::Verbose verbose = Logging::Verbose::No);

std::string to_string(const Circuit::UnmappedCircuit::NodeAndPort& nodeAndPort);

};
};

