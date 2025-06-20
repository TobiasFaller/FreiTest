#pragma once

#include <cstdint>
#include <string>

#include "Basic/Iterator/RawIterable.hpp"
#include "Basic/Iterator/RawEnumerable.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Circuit/Port.hpp"

namespace FreiTest
{
namespace Circuit
{

namespace Builder { class CircuitBuilder; };

class CircuitEnvironment;
class MappedCircuit;
class MappedNode;

using MappedInputList = RawIterable<const MappedNode *const, const size_t>;
using MappedSuccessorList = RawIterable<const MappedNode *const, const size_t>;
using MappedSuccessorPinList = RawIterable<const size_t, const size_t>;
using MappedInputEnumeration = RawEnumerable<const MappedNode *const, const size_t>;
using MappedSuccessorEnumeration = RawEnumerable<const MappedNode *const, const size_t>;
using MappedSuccessorPinEnumeration = RawEnumerable<const size_t, const size_t>;

class MappedNode
{
public:
	virtual ~MappedNode(void);

    // ----------------------------------------------
    // Node specific interface
    // ----------------------------------------------

	size_t GetNodeId(void) const;
	const std::string& GetName(void) const;
	const std::string& GetOutputSignalName(void) const;
	CellType GetCellType(void) const;
	CellCategory GetCellCategory(void) const;

    // ----------------------------------------------
    // Connection specific interface
    // ----------------------------------------------

	size_t GetNumberOfInputs(void) const;
	size_t GetNumberOfSuccessors(void) const;

	size_t GetInputPinId(size_t input) const;
	size_t GetOutputPinId(void) const;
	size_t GetPortPinId(const Port& port) const;

	size_t GetInputConnectionId(size_t input) const;
	size_t GetOutputConnectionId(void) const;
	size_t GetPortConnectionId(const Port& port) const;

	const MappedNode* GetInput(size_t input) const;
	const MappedNode* GetSuccessor(size_t successor) const;
	size_t GetSuccessorInputPinNumber(size_t successor) const;

    // Returns the input / output node-list connected to this node.
    // Usage:
    //
    //     for (auto node : node.GetInputs()) { ... }
    //     for (auto successor : circuit.GetSuccessors()) { ... }
    //
	const MappedInputList GetInputs(void) const;
	const MappedSuccessorList GetSuccessors(void) const;
	const MappedSuccessorPinList GetSuccessorInputPinNumbers(void) const;

    // Returns the input / output node-list with indices connected to this node.
    // Usage:
    //
    //     for (auto [inputId, node] : node.GetInputs()) { ... }
    //     for (auto [successorId, node] : circuit.GetSuccessors()) { ... }
    //
	const MappedInputEnumeration EnumerateInputs(void) const;
	const MappedSuccessorEnumeration EnumerateSuccessors(void) const;
	const MappedSuccessorPinList EnumerateSuccessorInputPinNumbers(void) const;

	const MappedNode* GetDriverForPort(const Port& port) const;

	friend CircuitEnvironment;
	friend MappedCircuit;
	friend Builder::CircuitBuilder;

private:
	MappedNode(void);

	MappedNode** in;
	MappedNode** out;
	size_t* successorInDataID;

	size_t* inDataID;
	size_t outDataID;

	size_t* inConnectionID;
	size_t outConnectionID;

	size_t tsort;
	CellType type;
	CellCategory mainType;
	size_t numberOfIns;
	size_t numberOfOuts;

	std::string name;
	std::string signalName;

};

};
};
