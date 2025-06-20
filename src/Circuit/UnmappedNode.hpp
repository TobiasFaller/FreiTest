#pragma once

#include <cstdint>
#include <string>

#include "Basic/Iterator/RawIterable.hpp"
#include "Basic/Iterator/TwoDRawIterable.hpp"
#include "Basic/Iterator/RawEnumerable.hpp"
#include "Basic/Iterator/TwoDRawEnumerable.hpp"
#include "Circuit/Port.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };

class CircuitEnvironment;
class UnmappedCircuit;
class UnmappedNode;

struct UnmappedPinData
{
	size_t PinSort;
	std::string PinName;

	size_t PinWireId;
	std::string PinWireName;
};

using UnmappedInputList = RawIterable<const UnmappedNode *const, const size_t>;
using UnmappedOutputList = TwoDRawIterable<const UnmappedNode *const, const size_t>;
using UnmappedSuccessorList = RawIterable<const UnmappedNode *const, const size_t>;
using UnmappedPinDataList = RawIterable<const UnmappedPinData, const size_t>;

using UnmappedInputEnumeration = RawEnumerable<const UnmappedNode *const, const size_t>;
using UnmappedOutputEnumeration = TwoDRawEnumerable<const UnmappedNode *const, const size_t>;
using UnmappedSuccessorEnumeration = RawEnumerable<const UnmappedNode *const, const size_t>;
using UnmappedPinDataEnumeration = RawEnumerable<const UnmappedPinData, const size_t>;

class UnmappedNode
{
public:
	virtual ~UnmappedNode(void);

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	size_t GetNodeId(void) const;
	const std::string& GetName(void) const;
	const std::string& GetType(void) const;

    // ----------------------------------------------
    // Connection specific interface
    // ----------------------------------------------

	size_t GetNumberOfInputs(void) const;
	size_t GetNumberOfOutputs(void) const;
	size_t GetNumberOfSuccessors(size_t output) const;

	size_t GetInputPinId(size_t input) const;
	size_t GetOutputPinId(size_t output) const;
	size_t GetPortPinId(const Port& port) const;

	size_t GetInputConnectionId(size_t input) const;
	size_t GetOutputConnectionId(size_t input) const;
	size_t GetPortConnectionId(const Port& port) const;

	const UnmappedNode* GetInput(size_t input) const;
	const UnmappedSuccessorList GetOutput(size_t output) const;
	const UnmappedNode* GetSuccessor(size_t output, size_t successor) const;

    // Returns the input / output node-list connected to this node.
    // Usage:
    //
    //     for (auto node : node.GetInputs()) { ... }
    //     for (auto output : node.GetOutputs()) {
    //         for (auto successor : output) { ... }
    //     }
    //     for (auto successor : circuit.GetSuccessors(0u)) { ... }
    //
	const UnmappedInputList GetInputs(void) const;
	const UnmappedOutputList GetOutputs(void) const;
	const UnmappedSuccessorList GetSuccessors(size_t output) const;

    // Returns the input / output node-list with indices connected to this node.
    // Usage:
    //
    //     for (auto [inputId, node] : node.GetInputs()) { ... }
    //     for (auto [outputId, nodes] : node.GetOutputs()) {
    //         for (auto successor : nodes) { ... }
    //     }
    //     for (auto [successorId, node] : circuit.GetSuccessors(0u)) { ... }
    //
	const UnmappedInputEnumeration EnumerateInputs(void) const;
	const UnmappedOutputEnumeration EnumerateOutputs(void) const;
	const UnmappedSuccessorEnumeration EnumerateSuccessors(size_t output) const;

	const UnmappedPinData* GetInputPinData(size_t input) const;
	const UnmappedPinData* GetOutputPinData(size_t output) const;
	const UnmappedPinData* GetPinDataForPort(const Port& port) const;

	const UnmappedPinDataList GetInputPinData(void) const;
	const UnmappedPinDataList GetOutputPinData(void) const;
	const UnmappedPinDataEnumeration EnumerateInputPinData(void) const;
	const UnmappedPinDataEnumeration EnumerateOutputPinData(void) const;

	friend CircuitEnvironment;
	friend UnmappedCircuit;
	friend Builder::CircuitBuilder;

private:
	UnmappedNode(void);

	UnmappedNode** in;  // Node<PinData>
	UnmappedNode*** out; // Node<PinData> -> [OutNr][OutNrIndex]*
	UnmappedPinData* inData; // PinData
	UnmappedPinData* outData; // PinData

	size_t tsort;
	size_t numberOfIns;
	size_t* numberOfOuts;  // -> * -> Can have several outputs with different numbers of outs each!
	size_t numberOfOutputPins;
	std::string name;
	std::string typeName;

};

};
};
