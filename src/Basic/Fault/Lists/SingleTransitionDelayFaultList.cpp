#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"

#include <algorithm>
#include <vector>
#include <utility>
#include <numeric>
#include <regex>

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Fault
{

SingleTransitionDelayFaultMetaData::SingleTransitionDelayFaultMetaData(void):
	TargetedFaultMetaData(),
	detectingPatternId(std::numeric_limits<size_t>::max()),
	detectingNode({ nullptr, { Circuit::PortType::Output, 0u } }),
	detectingTimeframe(std::numeric_limits<size_t>::max()),
	detectingOutputGood(Logic::LOGIC_INVALID),
	detectingOutputBad(Logic::LOGIC_INVALID)
{
}

SingleTransitionDelayFaultMetaData::~SingleTransitionDelayFaultMetaData(void) = default;

std::vector<SingleTransitionDelayFault> GenerateTransitionDelayFaultList(const CircuitEnvironment& circuit)
{
	const auto& mappedCircuit { circuit.GetMappedCircuit() };

	// Check the connectivity of the circuit first to not include faults of unconnected inputs / outputs
	std::vector<bool> forwardConnection(mappedCircuit.GetNumberOfNodes(), false);
	std::vector<bool> backwardConnection(mappedCircuit.GetNumberOfNodes(), false);

	// Find the start nodes
	for(auto [nodeId, node] : mappedCircuit.EnumerateNodes())
	{
		if (node->GetCellCategory() == CellCategory::MAIN_IN
			|| (node->GetCellCategory() == CellCategory::MAIN_CONSTANT && (node->GetCellType() == CellType::PRESET_0 || node->GetCellType() == CellType::PRESET_1)))
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
			const MappedNode* node = mappedCircuit.GetNode(nodeId - 1u);
			for (auto input : node->GetInputs())
			{
				backwardConnection[input->GetNodeId()] = true;
			}
		}
	}

	// Now create the fault list
	std::vector<SingleTransitionDelayFault> faultList;
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

				faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Input, inputId } }, TransitionDelayFaultType::SLOW_TO_RISE);
				faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Input, inputId } }, TransitionDelayFaultType::SLOW_TO_FALL);
			}
		}

		if (node->GetCellCategory() != MAIN_OUT && node->GetCellCategory() != MAIN_CONSTANT)
		{
			faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Output, 0u } }, TransitionDelayFaultType::SLOW_TO_RISE);
			faultList.emplace_back(MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { PortType::Output, 0u } }, TransitionDelayFaultType::SLOW_TO_FALL);
		}
	}

	return faultList;
}

std::vector<size_t> GetSortIndicesForTransitionDelayFaultList(const std::vector<SingleTransitionDelayFault>& faultList)
{
	// Sort the index list by node-id to arrange consecutive faults after each other
	std::vector<size_t> newFaultIndices(faultList.size());
	std::iota(newFaultIndices.begin(), newFaultIndices.end(), 0u);
	std::stable_sort(newFaultIndices.begin(), newFaultIndices.end(), [&faultList](size_t index1, size_t index2) {
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

	return newFaultIndices;
}

std::pair<std::vector<SingleTransitionDelayFault>, std::vector<size_t>> SortTransitionDelayFaultList(const std::vector<SingleTransitionDelayFault>& faultList)
{
	auto newFaultIndices = GetSortIndicesForTransitionDelayFaultList(faultList);

	std::vector<SingleTransitionDelayFault> newFaultList;
	newFaultList.reserve(faultList.size());
	for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
	{
		newFaultList[newFaultIndices[index]] = faultList[index];
	}

	return { newFaultList, newFaultIndices };
}

std::pair<std::vector<SingleTransitionDelayFault>, std::vector<size_t>> ReduceTransitionDelayFaultList(const CircuitEnvironment& circuitEnvironment, TransitionDelayFaultReduction reduction, const std::vector<SingleTransitionDelayFault>& faultList)
{
	auto invert_fault_type = [](TransitionDelayFaultType type, bool invert) -> TransitionDelayFaultType {
		switch (type)
		{
		case TransitionDelayFaultType::SLOW_TO_RISE:
			return invert ? TransitionDelayFaultType::SLOW_TO_FALL : type;
		case TransitionDelayFaultType::SLOW_TO_FALL:
			return invert ? TransitionDelayFaultType::SLOW_TO_RISE : type;
		case TransitionDelayFaultType::SLOW_TO_TRANSITION:
			return type; // Nothing to invert
		default:
			Logging::Panic("Invalid transition delay fault type detected");
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

	if (reduction == TransitionDelayFaultReduction::RemoveNone)
	{
		std::vector<size_t> unitMapping(faultList.size());
		std::iota(unitMapping.begin(), unitMapping.end(), 0u);
		return { faultList, unitMapping };
	}

	const MappedCircuit& mappedCircuit = circuitEnvironment.GetMappedCircuit();
	const CircuitMetaData& metaData = circuitEnvironment.GetMetaData();

	std::vector<bool> faultMergedOrRemoved(faultList.size(), false);
	std::vector<size_t> newFaultIndices(faultList.size());
	std::iota(newFaultIndices.begin(), newFaultIndices.end(), 0u);

	if ((reduction & TransitionDelayFaultReduction::RemoveEquivalent) == TransitionDelayFaultReduction::RemoveEquivalent)
	{
		// Sort the index list by node-id to arrange consecutive faults after each other
		newFaultIndices = GetSortIndicesForTransitionDelayFaultList(faultList);

		// Build a reverse mapping to be able to traverse the fault list in sorted order.
		// This mapping is not updated to enable the lookup if a fault has been remapped or not.
		std::vector<size_t> reverseFaultIndices(newFaultIndices.size());
		for (size_t index = 0u; index < newFaultIndices.size(); ++index)
		{
			reverseFaultIndices[newFaultIndices[index]] = index;
		}

		// Start index and end index + 1u for the faults for each node in the reverseFaultIndices array.
		// This is used as an optimization to search for faults that share a specific node.
		std::vector<std::pair<size_t, size_t>> nodeFaults(mappedCircuit.GetNumberOfNodes(), std::make_pair(0u, 0u));
		for (size_t startIndex { 0u }; startIndex < reverseFaultIndices.size();)
		{
			const size_t nodeId = faultList[reverseFaultIndices[startIndex]].GetNode()->GetNodeId();

			size_t nextIndex = startIndex + 1u;
			for (; nextIndex < reverseFaultIndices.size(); ++nextIndex)
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

		// Now filter the faults by removing duplicate entries.
		// Use the sorted indices as a look-up table to get clusters of faults for the same node.
		for (size_t revIndex = 0u; revIndex < reverseFaultIndices.size(); ++revIndex)
		{
			const size_t faultIndex = reverseFaultIndices[revIndex];
			const auto& fault { faultList[faultIndex] };

			// Case 1: Fan-outs: Fault f1 at output and f2 at input of single successor are equivalent.
			// Condition: The parent node only has one successor.
			// Since we are iterating the faults in node order from inputs to outputs
			// we will always merge the faults at the input pins (node with larger id).
			// => Keep only fault f1 and declare f2 as equivalent.
			if (fault.GetPort().portType == PortType::Input)
			{
				const MappedNode* parentNode = fault.GetNode()->GetInput(fault.GetPort().portNumber);
				const size_t parentNodeId = parentNode->GetNodeId();
				if (parentNode->GetNumberOfSuccessors() == 1)
				{
					// Check all faults at the parent node for a match
					const auto [parentRevFaultsStart, parentRevFaultsEnd] = nodeFaults[parentNodeId];
					for (size_t parentRevFaultIndex = parentRevFaultsStart; parentRevFaultIndex <= parentRevFaultsEnd; ++parentRevFaultIndex)
					{
						const size_t parentFaultIndex = reverseFaultIndices[parentRevFaultIndex];
						const SingleTransitionDelayFault& parentFault = faultList[parentFaultIndex];
						if (parentFault.GetType() == fault.GetType()
							&& parentFault.GetPort().portType == PortType::Output)
						{
							faultMergedOrRemoved[faultIndex] = true;
							newFaultIndices[faultIndex] = newFaultIndices[parentFaultIndex];
							// Do not update the reverse mapping as it is still used.
							goto nextFault;
						}
					}
				}
			}

			// Case 2: Controlling values at the inputs of an AND / NAND / OR / NOR / BUF / INV gate
			//         and the resulting output values are equivalent (keep only the input fault).
			if (fault.GetPort().portType == PortType::Output)
			{
				if ((fault.GetNode()->GetCellCategory() == CellCategory::MAIN_AND && fault.GetTransitionDelay() == TransitionDelayFaultType::SLOW_TO_RISE)
					|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_NAND && fault.GetTransitionDelay() == TransitionDelayFaultType::SLOW_TO_FALL)
					|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_OR && fault.GetTransitionDelay() == TransitionDelayFaultType::SLOW_TO_FALL)
					|| (fault.GetNode()->GetCellCategory() == CellCategory::MAIN_NOR && fault.GetTransitionDelay() == TransitionDelayFaultType::SLOW_TO_RISE)
					|| fault.GetNode()->GetCellCategory() == CellCategory::MAIN_BUF || fault.GetNode()->GetCellCategory() == CellCategory::MAIN_INV)
				{
					// Check all faults that are earlier in the fault list of the current node for a match
					const auto [nodeRevFaultsStart, nodeRevFaultsEnd] = nodeFaults[fault.GetNode()->GetNodeId()];
					for (size_t nodeRevFaultIndex = nodeRevFaultsStart; nodeRevFaultIndex < revIndex; ++nodeRevFaultIndex)
					{
						const size_t nodeFaultIndex = reverseFaultIndices[nodeRevFaultIndex];
						const SingleTransitionDelayFault& nodeFault = faultList[nodeFaultIndex];
						if (nodeFault.GetTransitionDelay() == invert_fault_type(fault.GetTransitionDelay(), is_inverting_gate(fault.GetNode()->GetCellCategory()))
							&& nodeFault.GetPort().portType == PortType::Input)
						{
							faultMergedOrRemoved[faultIndex] = true;
							newFaultIndices[faultIndex] = newFaultIndices[nodeFaultIndex];
							// Do not update the reverse mapping as it is still used.
							goto nextFault;
						}
					}
				}
			}

		nextFault:

			; // Continue the loop
		}
	}

	if ((reduction & TransitionDelayFaultReduction::RemoveCellInternal) == TransitionDelayFaultReduction::RemoveCellInternal)
	{
		for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
		{
			if (faultMergedOrRemoved[index])
			{
				continue;
			}

			auto const& fault { faultList[newFaultIndices[index]] };
			auto const* group { metaData.GetGroup(fault.GetNode()) };
			auto const* cell { group->GetParent() };
			if (cell == nullptr)
			{
				faultMergedOrRemoved[index] = true;
				newFaultIndices[index] = REMOVED_FAULT;
				continue;
			}

			auto const& sourceInfo { cell->GetSourceInfo() };
			if (!sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
			{
				faultMergedOrRemoved[index] = true;
				newFaultIndices[index] = REMOVED_FAULT;
				continue;
			}

			const size_t connectionId { fault.GetNode()->GetPortConnectionId(fault.GetPort()) };

			bool connectedToPort = false;
			for (auto port: cell->GetPorts())
			{
				ASSERT(port.GetConnections().size() == 1u) << "Found a cell with a port that has more than a single wire in the bus.";
				if (port.GetConnections()[0u] == nullptr)
				{
					continue;
				}

				if (port.GetConnections()[0u]->GetConnectionId() == connectionId)
				{
					connectedToPort = true;
					break;
				}
			}

			if (!connectedToPort)
			{
				faultMergedOrRemoved[index] = true;
				newFaultIndices[index] = REMOVED_FAULT;
				continue;
			}
		}
	}

	if ((reduction & TransitionDelayFaultReduction::RemoveSequentialClock) == TransitionDelayFaultReduction::RemoveSequentialClock)
	{
		for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
		{
			if (faultMergedOrRemoved[index])
			{
				continue;
			}

			auto const& fault { faultList[newFaultIndices[index]] };
			if (fault.GetPort().portType != PortType::Input
				|| fault.GetPort().portNumber != 1u)
			{
				continue;
			}

			auto const* mappedNode { fault.GetNode() };
			if (mappedNode->GetCellType() != CellType::S_OUT_CLK)
			{
				continue;
			}

			faultMergedOrRemoved[index] = true;
			newFaultIndices[index] = REMOVED_FAULT;
		}
	}

	if ((reduction & TransitionDelayFaultReduction::RemoveSequentialSetReset) == TransitionDelayFaultReduction::RemoveSequentialSetReset)
	{
		for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
		{
			if (faultMergedOrRemoved[index])
			{
				continue;
			}

			auto const& fault = faultList[newFaultIndices[index]];
			if (fault.GetPort().portType != PortType::Input
				|| (fault.GetPort().portNumber < 2u) || (fault.GetPort().portNumber > 3u))
			{
				continue;
			}

			auto const* mappedNode { fault.GetNode() };
			if (mappedNode->GetCellType() != CellType::S_OUT
				&& mappedNode->GetCellType() != CellType::S_OUT_CLK
				&& mappedNode->GetCellType() != CellType::S_OUT_EN)
			{
				continue;
			}

			faultMergedOrRemoved[index] = true;
			newFaultIndices[index] = REMOVED_FAULT;
		}
	}

	if ((reduction & TransitionDelayFaultReduction::RemoveConnectedToDontCare) == TransitionDelayFaultReduction::RemoveConnectedToDontCare)
	{
		for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
		{
			if (faultMergedOrRemoved[index])
			{
				continue;
			}

			auto const& fault { faultList[newFaultIndices[index]] };
			if (fault.GetPort().portType != PortType::Input)
			{
				continue;
			}

			auto const* driverNode { fault.GetNode()->GetInput(fault.GetPort().portNumber) };
			if (driverNode->GetCellType() != CellType::PRESET_X)
			{
				continue;
			}

			faultMergedOrRemoved[index] = true;
			newFaultIndices[index] = REMOVED_FAULT;
		}
	}

	if ((reduction & TransitionDelayFaultReduction::RemoveConnectedToUnknown) == TransitionDelayFaultReduction::RemoveConnectedToUnknown)
	{
		for (size_t index { 0u }; index < newFaultIndices.size(); ++index)
		{
			if (faultMergedOrRemoved[index])
			{
				continue;
			}

			auto const& fault { faultList[newFaultIndices[index]] };
			if (fault.GetPort().portType != PortType::Input)
			{
				continue;
			}

			auto const* driverNode { fault.GetNode()->GetInput(fault.GetPort().portNumber) };
			if (driverNode->GetCellType() != CellType::PRESET_U)
			{
				continue;
			}

			faultMergedOrRemoved[index] = true;
			newFaultIndices[index] = REMOVED_FAULT;
		}
	}

	// Construct the new fault list by copying the untouched elements.
	std::vector<SingleTransitionDelayFault> newFaultList;
	newFaultList.reserve(std::count_if(faultMergedOrRemoved.begin(), faultMergedOrRemoved.end(), [](bool faultMergedOrRemoved) {
		return !faultMergedOrRemoved;
	}));

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		if (!faultMergedOrRemoved[faultIndex])
		{
			// Update the fault index to the new location.
			newFaultIndices[faultIndex] = newFaultList.size();
			newFaultList.emplace_back(faultList[faultIndex]);
		}
		else if (newFaultIndices[faultIndex] != REMOVED_FAULT)
		{
			// Update the merged index to the new location.
			// This update method assumes that the referenced index has already
			// been resolved by the update above.
			newFaultIndices[faultIndex] = newFaultIndices[newFaultIndices[faultIndex]];
		}
	}

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		ASSERT(newFaultIndices[faultIndex] == REMOVED_FAULT
				|| newFaultIndices[faultIndex] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

std::pair<std::vector<SingleTransitionDelayFault>, std::vector<size_t>> FilterTransitionDelayFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<SingleTransitionDelayFault>& faultList)
{
	std::vector<bool> faultMergedOrRemoved(faultList.size(), false);
	std::vector<size_t> newFaultIndices(faultList.size());
	std::iota(newFaultIndices.begin(), newFaultIndices.end(), 0u);

	// Let an empty pattern always return the original fault list.
	if (pattern == "")
	{
		return { faultList, newFaultIndices };
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
			faultMergedOrRemoved[faultIndex] = true;
			newFaultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	// Construct the new fault list by copying the untouched elements.
	std::vector<SingleTransitionDelayFault> newFaultList;
	newFaultList.reserve(std::count_if(faultMergedOrRemoved.begin(), faultMergedOrRemoved.end(), [](bool faultMergedOrRemoved) {
		return !faultMergedOrRemoved;
	}));

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		if (!faultMergedOrRemoved[faultIndex])
		{
			// Update the fault index to the new location.
			newFaultIndices[faultIndex] = newFaultList.size();
			newFaultList.emplace_back(faultList[faultIndex]);
		}
		else if (newFaultIndices[faultIndex] != REMOVED_FAULT)
		{
			// Update the merged index to the new location.
			// This update method assumes that the referenced index has already
			// been resolved by the update above.
			newFaultIndices[faultIndex] = newFaultIndices[newFaultIndices[faultIndex]];
		}
	}

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		ASSERT(newFaultIndices[faultIndex] == REMOVED_FAULT
				|| newFaultIndices[faultIndex] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

std::pair<std::vector<SingleTransitionDelayFault>, std::vector<size_t>> FilterTransitionDelayFaultListByNodes(const std::vector<SingleTransitionDelayFault>& faultList, const std::vector<Circuit::MappedNode*>& nodes)
{
	std::vector<bool> faultMergedOrRemoved(faultList.size(), false);
	std::vector<size_t> newFaultIndices(faultList.size());
	std::iota(newFaultIndices.begin(), newFaultIndices.end(), 0u);

	std::vector<Circuit::MappedNode*> sortedDrivers { nodes };
	std::sort(sortedDrivers.begin(), sortedDrivers.end());

	for (size_t faultIndex { 0 }; faultIndex < faultList.size(); faultIndex++)
	{
		auto const& fault { faultList[faultIndex] };
		if (std::lower_bound(sortedDrivers.begin(), sortedDrivers.end(), fault.GetNode()) == sortedDrivers.end())
		{
			faultMergedOrRemoved[faultIndex] = true;
			newFaultIndices[faultIndex] = REMOVED_FAULT;
		}
	}

	// Construct the new fault list by copying the untouched elements.
	std::vector<SingleTransitionDelayFault> newFaultList;
	newFaultList.reserve(std::count_if(faultMergedOrRemoved.begin(), faultMergedOrRemoved.end(), [](bool faultMergedOrRemoved) {
		return !faultMergedOrRemoved;
	}));

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		if (!faultMergedOrRemoved[faultIndex])
		{
			// Update the fault index to the new location.
			newFaultIndices[faultIndex] = newFaultList.size();
			newFaultList.emplace_back(faultList[faultIndex]);
		}
		else if (newFaultIndices[faultIndex] != REMOVED_FAULT)
		{
			// Update the merged index to the new location.
			// This update method assumes that the referenced index has already
			// been resolved by the update above.
			newFaultIndices[faultIndex] = newFaultIndices[newFaultIndices[faultIndex]];
		}
	}

	for (size_t faultIndex { 0u }; faultIndex < faultList.size(); ++faultIndex)
	{
		ASSERT(newFaultIndices[faultIndex] == REMOVED_FAULT
				|| newFaultIndices[faultIndex] < newFaultList.size())
			<< "The mapping has either point to a valid entry of the new fault list "
			<< "or be equal to REMOVED_FAULT.";
	}

	return { newFaultList, newFaultIndices };
}

TransitionDelayFaultReduction operator|(TransitionDelayFaultReduction lhs, TransitionDelayFaultReduction rhs)
{
	return static_cast<TransitionDelayFaultReduction>(
		static_cast<std::underlying_type_t<TransitionDelayFaultReduction>>(lhs)
		| static_cast<std::underlying_type_t<TransitionDelayFaultReduction>>(rhs)
	);
}

TransitionDelayFaultReduction operator&(TransitionDelayFaultReduction lhs, TransitionDelayFaultReduction rhs)
{
	return static_cast<TransitionDelayFaultReduction>(
		static_cast<std::underlying_type_t<TransitionDelayFaultReduction>>(lhs)
		& static_cast<std::underlying_type_t<TransitionDelayFaultReduction>>(rhs)
	);
}

TransitionDelayFaultReduction& operator&=(TransitionDelayFaultReduction& lhs, TransitionDelayFaultReduction rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

TransitionDelayFaultReduction& operator|=(TransitionDelayFaultReduction& lhs, TransitionDelayFaultReduction rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

template<typename T>
SingleTransitionDelayFaultListDebug<T>::SingleTransitionDelayFaultListDebug(const ConcurrentFaultList<SingleTransitionDelayFault, T>& faultList, const MappedCircuit& circuit):
	faultList(faultList),
	circuit(circuit)
{
}


template<typename T>
SingleTransitionDelayFaultListDebug<T>::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

template<typename T>
std::ostream& operator <<(std::ostream& stream, const SingleTransitionDelayFaultListDebug<T>& debug)
{
	for (auto const [fault, metadata] : debug.faultList)
	{
		stream << "Fault: " << to_string(*fault) << std::endl;
	}
	return stream;
}

template<typename T>
SingleTransitionDelayFaultMetaDataDebug<T>::SingleTransitionDelayFaultMetaDataDebug(const T& metaData, const MappedCircuit& circuit):
	metaData(metaData),
	circuit(circuit)
{
}


template<typename T>
SingleTransitionDelayFaultMetaDataDebug<T>::operator std::string(void) const
{
	std::ostringstream stream;
	stream << *this;
	return stream.str();
}

template<typename T>
std::ostream& operator <<(std::ostream& stream, const SingleTransitionDelayFaultMetaDataDebug<T>& debug)
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
SingleTransitionDelayFaultListDebug<T> to_debug(const ConcurrentFaultList<SingleTransitionDelayFault, T>& faultList, const Circuit::MappedCircuit& circuit)
{
	return SingleTransitionDelayFaultListDebug { faultList, circuit };
}

template<typename T>
SingleTransitionDelayFaultMetaDataDebug<T> to_debug(const T& metaData, const Circuit::MappedCircuit& circuit)
{
	return SingleTransitionDelayFaultMetaDataDebug { metaData, circuit };
}

template class SingleTransitionDelayFaultListDebug<SingleTransitionDelayFaultMetaData>;
template SingleTransitionDelayFaultListDebug<SingleTransitionDelayFaultMetaData> to_debug(const ConcurrentFaultList<SingleTransitionDelayFault, SingleTransitionDelayFaultMetaData>& faultList, const Circuit::MappedCircuit& circuit);
template std::ostream& operator <<(std::ostream& stream, const SingleTransitionDelayFaultListDebug<SingleTransitionDelayFaultMetaData>& debug);

template class SingleTransitionDelayFaultMetaDataDebug<SingleTransitionDelayFaultMetaData>;
template SingleTransitionDelayFaultMetaDataDebug<SingleTransitionDelayFaultMetaData> to_debug(const SingleTransitionDelayFaultMetaData& metaData, const Circuit::MappedCircuit& circuit);
template std::ostream& operator <<(std::ostream& stream, const SingleTransitionDelayFaultMetaDataDebug<SingleTransitionDelayFaultMetaData>& debug);

};
};
