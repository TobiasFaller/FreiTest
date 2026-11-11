#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <numeric>
#include <random>
#include <regex>

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

static std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> RemapFaults(const std::vector<SingleStuckAtFault>& faultList, const std::vector<size_t>& faultIndices);
static std::vector<size_t> GetSortIndices(const std::vector<SingleStuckAtFault>& faultList);

SingleStuckAtFaultMetaData::SingleStuckAtFaultMetaData(void):
	TargetedFaultMetaData(),
	detectingPatternId(std::numeric_limits<size_t>::max()),
	detectingNode({ nullptr, { Circuit::PortType::Output, 0u } }),
	detectingTimeframe(std::numeric_limits<size_t>::max()),
	detectingOutputGood(Logic::LOGIC_INVALID),
	detectingOutputBad(Logic::LOGIC_INVALID)
{
}

SingleStuckAtFaultMetaData::~SingleStuckAtFaultMetaData(void) = default;

std::vector<SingleStuckAtFault> GenerateStuckAtFaultList(const CircuitEnvironment& circuit)
{
	const auto& mappedCircuit { circuit.GetMappedCircuit() };

	// Check the connectivity of the circuit first to not include faults of unconnected inputs / outputs
	std::vector<bool> forwardConnection(mappedCircuit.GetNumberOfNodes(), false);
	std::vector<bool> backwardConnection(mappedCircuit.GetNumberOfNodes(), false);

	// Find the start nodes
	for(auto [nodeId, node] : mappedCircuit.EnumerateNodes())
	{
		if (node->GetCellCategory() == CellCategory::MAIN_IN
			|| node->GetCellCategory() == CellCategory::MAIN_CONSTANT)
		{
			forwardConnection[nodeId] = true;
		}

		if (node->GetCellCategory() == CellCategory::MAIN_OUT)
		{
			backwardConnection[nodeId] = true;
		}
	}

	// Propagate the connectivity information to all connected nodes
	for(auto [nodeId, node] : mappedCircuit.EnumerateNodes())
	{
		if (forwardConnection[nodeId])
		{
			for (auto successor : node->GetSuccessors())
			{
				forwardConnection[successor->GetNodeId()] = true;
			}
		}
	}
	for(size_t nodeId = mappedCircuit.GetNumberOfNodes(); nodeId > 0u; nodeId--)
	{
		if (backwardConnection[nodeId - 1u])
		{
			const auto* node = mappedCircuit.GetNode(nodeId - 1u);
			for (auto input : node->GetInputs())
			{
				backwardConnection[input->GetNodeId()] = true;
			}
		}
	}

	// Now create the fault list
	std::vector<SingleStuckAtFault> faultList;
	for(auto [nodeId, node] : mappedCircuit.EnumerateNodes())
	{
		if(!forwardConnection[nodeId] || !backwardConnection[nodeId])
		{
			continue;
		}

		if (node->GetCellCategory() != MAIN_IN && node->GetCellCategory() != MAIN_CONSTANT)
		{
			for(auto [inputId, input] : node->EnumerateInputs())
			{
				// Don't include inputs that are unconnected
				if (input == nullptr || !forwardConnection[input->GetNodeId()])
				{
					continue;
				}

				faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Input, inputId } }, StuckAtFaultType::STUCK_AT_0);
				faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Input, inputId } }, StuckAtFaultType::STUCK_AT_1);
			}
		}

		if (node->GetCellCategory() != MAIN_OUT && node->GetCellCategory() != MAIN_CONSTANT)
		{
			faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Output, 0u } }, StuckAtFaultType::STUCK_AT_0);
			faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Output, 0u } }, StuckAtFaultType::STUCK_AT_1);
		}
	}

	return faultList;
}

static std::vector<size_t> GetSortIndices(const std::vector<SingleStuckAtFault>& faultList)
{
	// Sort the index list by node-id to arrange consecutive faults after each other
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);
	std::stable_sort(faultIndices.begin(), faultIndices.end(), [&faultList](size_t index1, size_t index2) {
		const auto& f1 { faultList[index1] };
		const auto& f2 { faultList[index2] };

		// Sort with the following order:
		// 1. Smallest to largest node id
		// 2. Input then output faults
		// 3. Smallest to largest pin id
		return (f1.GetNode()->GetNodeId() != f2.GetNode()->GetNodeId())
			? (f1.GetNode()->GetNodeId() < f2.GetNode()->GetNodeId())
			: (f1.GetPort() < f2.GetPort());
	});

	return faultIndices;
}

std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> ReduceStuckAtFaultList(const CircuitEnvironment& circuitEnvironment, StuckAtFaultReduction reduction, const std::vector<SingleStuckAtFault>& faultList)
{
	auto invert_fault_type = [](StuckAtFaultType type, bool invert) -> StuckAtFaultType {
		switch (type)
		{
		case StuckAtFaultType::STUCK_AT_0:
			return invert ? StuckAtFaultType::STUCK_AT_1 : type;
		case StuckAtFaultType::STUCK_AT_1:
			return invert ? StuckAtFaultType::STUCK_AT_0 : type;
		case StuckAtFaultType::STUCK_AT_X:
		case StuckAtFaultType::STUCK_AT_U:
			return type; // Nothing to invert
		default:
			Logging::Panic("Invalid stuck-at fault type detected");
		}
	};
	auto is_inverting_gate = [](CellCategory cellType) -> bool {
		switch (cellType)
		{
		case CellCategory::MAIN_BUF:
		case CellCategory::MAIN_AND:
		case CellCategory::MAIN_OR:
			return false;

		case CellCategory::MAIN_INV:
		case CellCategory::MAIN_NAND:
		case CellCategory::MAIN_NOR:
			return true;

		default:
			Logging::Panic("Invalid cell type detected");
		}
	};

	const auto& mappedCircuit = circuitEnvironment.GetMappedCircuit();
	const auto& metaData = circuitEnvironment.GetMetaData();

	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	if ((reduction & StuckAtFaultReduction::RemoveCellInternal) == StuckAtFaultReduction::RemoveCellInternal)
	{
		for (size_t index { 0u }; index < faultIndices.size(); ++index)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault { faultList[faultIndices[index]] };
			auto const* group { metaData.GetGroup(fault.GetNode()) };
			auto const* cell { group->GetParent() };
			if (cell == nullptr)
			{
				continue;
			}

			auto const& sourceInfo { cell->GetSourceInfo() };
			if (!sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
			{
				continue;
			}

			const auto connectionId { fault.GetNode()->GetPortConnectionId(fault.GetPort()) };

			bool connectedToPort = false;
			for (auto port: cell->GetPorts())
			{
				ASSERT(port.GetConnections().size() == 1u) << "Found a cell with a port that has more than a single wire in the bus.";
				if (port.GetConnections()[0u] == nullptr)
				{
					continue;
				}

				if (port.GetConnections()[0u]->GetConnectionId() != connectionId)
				{
					continue;
				}

				const auto portType = port.GetSourceInfo().GetProperty<std::string>("port-type").value_or("unknown");
				if ((fault.GetPort().portType == PortType::Input && portType == "input")
						|| (fault.GetPort().portType == PortType::Output && portType == "output")) {
					connectedToPort = true;
					break;
				}
			}

			if (!connectedToPort)
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	if ((reduction & StuckAtFaultReduction::RemoveSequentialClock) == StuckAtFaultReduction::RemoveSequentialClock)
	{
		for (size_t index { 0u }; index < faultList.size(); ++index)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault { faultList[faultIndices[index]] };
			auto const* node { fault.GetNode() };
			if (node->GetCellType() != CellType::S_OUT
				&& node->GetCellType() != CellType::S_OUT_CLK
				&& node->GetCellType() != CellType::S_OUT_EN)
			{
				continue;
			}

			if (fault.GetPort().portType == PortType::Input
				&& fault.GetPort().portNumber == 1u)
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	if ((reduction & StuckAtFaultReduction::RemoveSequentialSetReset) == StuckAtFaultReduction::RemoveSequentialSetReset)
	{
		for (size_t index { 0u }; index < faultList.size(); index++)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault = faultList[faultIndices[index]];
			auto const* node { fault.GetNode() };
			if (node->GetCellType() != CellType::S_OUT
				&& node->GetCellType() != CellType::S_OUT_CLK
				&& node->GetCellType() != CellType::S_OUT_EN)
			{
				continue;
			}

			if (fault.GetPort().portType == PortType::Input
				&& fault.GetPort().portNumber >= 2u
				&& fault.GetPort().portNumber <= 3u)
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	if ((reduction & StuckAtFaultReduction::RemoveConnectedToDontCare) == StuckAtFaultReduction::RemoveConnectedToDontCare)
	{
		for (size_t index { 0u }; index < faultList.size(); index++)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault { faultList[faultIndices[index]] };
			if (fault.GetPort().portType != PortType::Input)
			{
				continue;
			}

			auto const* driverNode { fault.GetNode()->GetInput(fault.GetPort().portNumber) };
			if (driverNode->GetCellType() == CellType::PRESET_X)
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	if ((reduction & StuckAtFaultReduction::RemoveConnectedToUnknown) == StuckAtFaultReduction::RemoveConnectedToUnknown)
	{
		for (size_t index { 0u }; index < faultList.size(); index++)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault { faultList[faultIndices[index]] };
			if (fault.GetPort().portType != PortType::Input)
			{
				continue;
			}

			auto const* driverNode { fault.GetNode()->GetInput(fault.GetPort().portNumber) };
			if (driverNode->GetCellType() == CellType::PRESET_U)
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	if ((reduction & StuckAtFaultReduction::RemoveCircuitPort) == StuckAtFaultReduction::RemoveCircuitPort)
	{
		for (size_t index { 0u }; index < faultList.size(); index++)
		{
			if (faultIndices[index] == REMOVED_FAULT)
			{
				continue;
			}

			auto const& fault { faultList[faultIndices[index]] };
			auto const* node { fault.GetNode() };
			if (mappedCircuit.IsPrimaryInput(node) || mappedCircuit.IsPrimaryOutput(node))
			{
				faultIndices[index] = REMOVED_FAULT;
			}
		}
	}

	// Now remap remaining faults. If we remap first and then remove the original faults
	// the remapped faults will point to a removed fault.
	if ((reduction & StuckAtFaultReduction::RemoveEquivalent) == StuckAtFaultReduction::RemoveEquivalent)
	{
		// Sort the index list by node-id to arrange consecutive faults after each other
		auto forwardFaultIndices = GetSortIndices(faultList);

		// Build a reverse mapping to be able to traverse the fault list in sorted order.
		std::vector<size_t> reverseFaultIndices(faultList.size());
		for (size_t index { 0u }; index < faultList.size(); index++)
		{
			reverseFaultIndices[forwardFaultIndices[index]] = index;
		}

		// Start index and end index + 1u for the faults for each node in the reverseFaultIndices array.
		// This is used as an optimization to search for faults that share a specific node.
		std::vector<std::pair<size_t, size_t>> nodeFaults(mappedCircuit.GetNumberOfNodes(), std::make_pair(0u, 0u));
		for (size_t startIndex { 0u }; startIndex < faultList.size(); )
		{
			const auto nodeId = faultList[reverseFaultIndices[startIndex]].GetNode()->GetNodeId();
			auto nextIndex = startIndex + 1u;
			for (; nextIndex < reverseFaultIndices.size(); nextIndex++)
			{
				if (faultList[reverseFaultIndices[nextIndex]].GetNode()->GetNodeId() != nodeId)
				{
					break;
				}
			}

			// The number of consecutive faults that share the same node
			nodeFaults[nodeId] = { startIndex, nextIndex - 1u };
			startIndex = nextIndex;
		}

		// Now filter the fault list by removing equivalent faults.
		// Use the sorted indices as a look-up table to get clusters of faults for the same node
		// and have a defined order of iteration through the circuit graph (from outputs to inputs).
		for (size_t revIndex { faultList.size() }; revIndex > 0u; revIndex--)
		{
			const auto faultIndex = reverseFaultIndices[revIndex - 1u];
			if (faultIndices[faultIndex] == REMOVED_FAULT)
			{
				continue;
			}

			const auto& fault { faultList[faultIndex] };
			if (fault.GetPort().portType != PortType::Output)
			{
				continue;
			}

			// Case 1: Fan-outs: Fault f1 at output and f2 at input of single successor are equivalent.
			// Condition: The parent node only has one successor.
			// Since we are iterating the faults in reverse node order from outputs to inputs
			// we will always merge the faults at the output pins (node with smaller id).
			// => Keep only fault f2 and declare f1 as equivalent.
			if (fault.GetNode()->GetNumberOfSuccessors() == 1)
			{
				const auto* successor = fault.GetNode()->GetSuccessor(0);
				const auto [startIndex, endIndex] = nodeFaults[successor->GetNodeId()];
				for (size_t otherIndex { startIndex }; otherIndex <= endIndex; otherIndex++)
				{
					const auto successorFaultIndex = reverseFaultIndices[otherIndex];
					const auto& successorFault = faultList[successorFaultIndex];
					if (successorFault.GetPort().portType == PortType::Input
						&& successorFault.GetNode()->GetInput(successorFault.GetPort().portNumber) == fault.GetNode()
						&& successorFault.GetType() == fault.GetType()
						&& faultIndices[successorFaultIndex] != REMOVED_FAULT)
					{
						faultIndices[faultIndex] = faultIndices[successorFaultIndex];
						break;
					}
				}
			}

			// Case 2: Equivalent Fault Collapsing (EFC)
			// Controlling values at the inputs of an AND / NAND / OR / NOR / BUF / INV gate
			// and the resulting output values are equivalent (keep only the output fault).
			if ((fault.GetNode()->GetCellCategory() == CellCategory::MAIN_AND && fault.GetStuckAt() == StuckAtFaultType::STUCK_AT_0)
				|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_NAND && fault.GetStuckAt() == StuckAtFaultType::STUCK_AT_1)
				|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_OR && fault.GetStuckAt() == StuckAtFaultType::STUCK_AT_1)
				|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_NOR && fault.GetStuckAt() == StuckAtFaultType::STUCK_AT_0)
				|| fault.GetNode()->GetCellCategory() == CellCategory::MAIN_BUF
				|| fault.GetNode()->GetCellCategory() == CellCategory::MAIN_INV)
			{
				const auto [startIndex, endIndex] = nodeFaults[fault.GetNode()->GetNodeId()];
				for (size_t otherIndex { startIndex }; otherIndex <= endIndex; otherIndex++)
				{
					const auto inputFaultIndex = reverseFaultIndices[otherIndex];
					const auto& inputFault = faultList[inputFaultIndex];
					if (inputFault.GetPort().portType == PortType::Input
						&& inputFault.GetStuckAt() == invert_fault_type(fault.GetStuckAt(), is_inverting_gate(fault.GetNode()->GetCellCategory()))
						&& faultIndices[inputFaultIndex] != REMOVED_FAULT)
					{
						faultIndices[inputFaultIndex] = faultIndices[faultIndex];
					}
				}
			}
		}
	}

	return RemapFaults(faultList, faultIndices);
}

std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> FilterStuckAtFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<SingleStuckAtFault>& faultList)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	// Let an empty pattern always return the original fault list.
	if (pattern == "")
	{
		return { faultList, faultIndices };
	}

	const auto& metaData { circuitEnvironment.GetMetaData() };
	std::regex patternRegex { pattern, std::regex_constants::ECMAScript };
	for (size_t faultIndex { 0 }; faultIndex < faultList.size(); faultIndex++)
	{
		auto const* group { metaData.GetGroup(faultList[faultIndex].GetNode()) };
		auto const result { std::regex_match(group->GetHierarchyName(), patternRegex) };
		if ((type == FilterPatternType::Normal && !result)
			|| (type == FilterPatternType::Inverted && result))
		{
			faultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	return RemapFaults(faultList, faultIndices);
}

std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> FilterStuckAtFaultListByNodes(const std::vector<SingleStuckAtFault>& faultList, const std::vector<Circuit::MappedNode*>& nodes)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	std::vector<Circuit::MappedNode*> sortedDrivers { nodes };
	std::sort(sortedDrivers.begin(), sortedDrivers.end());

	for (size_t faultIndex { 0 }; faultIndex < faultList.size(); faultIndex++)
	{
		auto const& fault { faultList[faultIndex] };
		if (std::lower_bound(sortedDrivers.begin(), sortedDrivers.end(), fault.GetNode()) == sortedDrivers.end())
		{
			faultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	return RemapFaults(faultList, faultIndices);
}

std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> SampleStuckAtFaultList(const std::vector<SingleStuckAtFault>& faultList, size_t samples)
{
	std::vector<size_t> faultIndices(faultList.size());
	std::iota(faultIndices.begin(), faultIndices.end(), 0u);

	if (samples == 0u)
	{
		return { faultList, faultIndices };
	}

	std::vector<size_t> sampledIndices;
	sampledIndices.reserve(samples);

	std::random_device randomDevice {};
	std::mt19937 randomSource { randomDevice() };
	std::sample(faultIndices.begin(), faultIndices.end(), std::back_inserter(sampledIndices), samples, randomSource);

	for (auto& fault : faultIndices)
	{
		fault = REMOVED_FAULT;
	}
	for (auto const& fault : sampledIndices)
	{
		faultIndices[fault] = fault;
	}

	return RemapFaults(faultList, faultIndices);
}

static std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> RemapFaults(const std::vector<SingleStuckAtFault>& faultList, const std::vector<size_t>& faultIndices)
{
	ASSERT(faultList.size() == faultIndices.size())
		<< "Faults and indices list sizes have to match.";

	size_t newFaultCount { 0u };
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] == index)
		{
			newFaultCount++;
		}
	}

	std::vector<size_t> newFaultIndices { };
	std::vector<SingleStuckAtFault> newFaultList { };

	// First re-index all the faults that have been kept.
	newFaultList.reserve(newFaultCount);
	newFaultIndices.resize(faultIndices.size(), REMOVED_FAULT);
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] == index)
		{
			newFaultIndices[index] = newFaultList.size();
			newFaultList.emplace_back(faultList[index]);
		}
	}

	// Now re-index all the faults that have been merged or removed.
	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		if (faultIndices[index] != REMOVED_FAULT && faultIndices[index] != index)
		{
			ASSERT(newFaultIndices[faultIndices[index]] != REMOVED_FAULT);
			newFaultIndices[index] = newFaultIndices[faultIndices[index]];
		}
	}

	for (size_t index { 0u }; index < faultIndices.size(); index++)
	{
		ASSERT(newFaultIndices[index] == REMOVED_FAULT
				|| newFaultIndices[index] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

StuckAtFaultReduction operator|(StuckAtFaultReduction lhs, StuckAtFaultReduction rhs)
{
	return static_cast<StuckAtFaultReduction>(
		static_cast<std::underlying_type_t<StuckAtFaultReduction>>(lhs)
		| static_cast<std::underlying_type_t<StuckAtFaultReduction>>(rhs)
	);
}

StuckAtFaultReduction operator&(StuckAtFaultReduction lhs, StuckAtFaultReduction rhs)
{
	return static_cast<StuckAtFaultReduction>(
		static_cast<std::underlying_type_t<StuckAtFaultReduction>>(lhs)
		& static_cast<std::underlying_type_t<StuckAtFaultReduction>>(rhs)
	);
}

StuckAtFaultReduction& operator&=(StuckAtFaultReduction& lhs, StuckAtFaultReduction rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

StuckAtFaultReduction& operator|=(StuckAtFaultReduction& lhs, StuckAtFaultReduction rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template<typename T>
SingleStuckAtFaultListDebug<T>::SingleStuckAtFaultListDebug(const ConcurrentFaultList<SingleStuckAtFault, T>& faultList, const MappedCircuit& circuit):
	faultList(faultList),
	circuit(circuit)
{
}


template<typename T>
SingleStuckAtFaultListDebug<T>::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

template<typename T>
std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultListDebug<T>& debug)
{
	for (auto const [fault, metadata] : debug.faultList)
	{
		stream << "Fault: " << to_string(*fault) << std::endl;
	}
	return stream;
}

template<typename T>
SingleStuckAtFaultMetaDataDebug<T>::SingleStuckAtFaultMetaDataDebug(const T& metaData, const MappedCircuit& circuit):
	metaData(metaData),
	circuit(circuit)
{
}


template<typename T>
SingleStuckAtFaultMetaDataDebug<T>::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

template<typename T>
std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultMetaDataDebug<T>& debug)
{
	stream << "detectingPatternId: " << debug.metaData.detectingPatternId
			<< ", detectingNode: " << to_string(debug.metaData.detectingNode)
			<< ", detectingTimeframe: " << debug.metaData.detectingTimeframe
			<< ", detectingOutputGood: " << debug.metaData.detectingOutputGood
			<< ", detectingOutputBad: " << debug.metaData.detectingOutputBad
			<< std::endl;

	return stream;
}

template<typename T>
SingleStuckAtFaultListDebug<T> to_debug(const ConcurrentFaultList<SingleStuckAtFault, T>& faultList, const Circuit::MappedCircuit& circuit)
{
	return SingleStuckAtFaultListDebug { faultList, circuit };
}

template<typename T>
SingleStuckAtFaultMetaDataDebug<T> to_debug(const T& metaData, const Circuit::MappedCircuit& circuit)
{
	return SingleStuckAtFaultMetaDataDebug { metaData, circuit };
}

template class SingleStuckAtFaultListDebug<SingleStuckAtFaultMetaData>;
template SingleStuckAtFaultListDebug<SingleStuckAtFaultMetaData> to_debug(const ConcurrentFaultList<SingleStuckAtFault, SingleStuckAtFaultMetaData>& faultList, const Circuit::MappedCircuit& circuit);
template std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultListDebug<SingleStuckAtFaultMetaData>& debug);

template class SingleStuckAtFaultMetaDataDebug<SingleStuckAtFaultMetaData>;
template SingleStuckAtFaultMetaDataDebug<SingleStuckAtFaultMetaData> to_debug(const SingleStuckAtFaultMetaData& metaData, const Circuit::MappedCircuit& circuit);
template std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultMetaDataDebug<SingleStuckAtFaultMetaData>& debug);

};
};
