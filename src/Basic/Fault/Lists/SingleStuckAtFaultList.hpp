#pragma once

#include <type_traits>

#include "Basic/Fault/ConcurrentFaultList.hpp"
#include "Basic/Fault/FaultMetaData.hpp"
#include "Basic/Fault/Faults/SingleStuckAtFault.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Fault
{

/**
 * @brief Stores basic metadata about stuck-at faults.
 *
 * The properties include data about patterns that detect the fault,
 * as well as the node, timeframe and values that are expected for detection.
 */
class SingleStuckAtFaultMetaData: public TargetedFaultMetaData
{
public:
	SingleStuckAtFaultMetaData(void);
	virtual ~SingleStuckAtFaultMetaData(void);

	size_t detectingPatternId;
	Circuit::MappedCircuit::NodeAndPort detectingNode;
	size_t detectingTimeframe;
	Basic::Logic detectingOutputGood;
	Basic::Logic detectingOutputBad;
};

using SingleStuckAtFaultList = ConcurrentFaultList<SingleStuckAtFault, SingleStuckAtFaultMetaData>;

/**
 * @brief Provides possible reductions that can be specified to the ReduceStuckAtFaultList method.
 *
 * Multiple reductions can be combined with the | (or) operator: RemoveSequentialClock | RemoveSequentialSetReset
 * The & (and) operator can be used to test for the presence of a reduction: (value & RemoveEquivalent) == RemoveEquivalent
 *
 * - RemoveNone: Does nothing
 * - RemoveEquivalent: Merges equivalent faults and sorts the fault list according to the SortStuckAtFaultList method.
 * - RemoveCellInternal: Removes faults that are not at the boundary of a cell.
 * - RemoveSequentialClock: Removes faults at the clock pin of sequential elements.
 * - RemoveSequentialSetReset: Removes faults at the set and reset lines of sequential elements.
 * - RemoveConnectedToDontCare: Removes faults that are at input ports connected to a constant don't care logic signal.
 * - RemoveConnectedToUnknown: Removes faults that are at input ports connected to a constant unknown logic signal.
 */
enum class StuckAtFaultReduction: size_t
{
	RemoveNone = 0u,
	RemoveEquivalent = 1u,
	RemoveCellInternal = 2u,
	RemoveSequentialClock = 4u,
	RemoveSequentialSetReset = 8u,
	RemoveConnectedToDontCare = 16u,
	RemoveConnectedToUnknown = 32u
};

StuckAtFaultReduction operator|(StuckAtFaultReduction lhs, StuckAtFaultReduction rhs);
StuckAtFaultReduction operator&(StuckAtFaultReduction lhs, StuckAtFaultReduction rhs);
StuckAtFaultReduction& operator|=(StuckAtFaultReduction& lhs, StuckAtFaultReduction rhs);
StuckAtFaultReduction& operator&=(StuckAtFaultReduction& lhs, StuckAtFaultReduction rhs);

/**
 * @brief Generates a new stuck-at fault list for the gates in the circuit.
 *
 * The generated fault list includes internal faults.
 * If you want to exclude specific faults use the ReduceStuckAtFaultList method.
 *
 * @param circuit The circuit to base the fault list on.
 * @return The newly generated fault list with all stuck-at faults.
 */
std::vector<SingleStuckAtFault> GenerateStuckAtFaultList(const Circuit::CircuitEnvironment& circuit);

/**
 * @brief Sorts the stuck-at fault list and returns the indices the new fault list should have.
 *
 * Sorts the fault list according to the following properties.
 * 1. Smallest to largest node id
 * 2. Input then output faults
 * 3. Smallest to largest pin id
 *
 * The mapping to the fault list is from original indices to the new location.
 * The original stuck-at fault list is not modified.
 *
 * @param faultList The fault list to use as a starting point.
 * @return The mapping from original indices to the new location.
 */
std::vector<size_t> GetSortIndicesForStuckAtFaultList(const std::vector<SingleStuckAtFault>& faultList);

/**
 * @brief Sorts the stuck-at fault list and returns the new list and the mapping.
 *
 * Sorts the fault list according to the following properties.
 * 1. Smallest to largest node id
 * 2. Input then output faults
 * 3. Smallest to largest pin id
 *
 * The mapping to the fault list is from original indices to the new location.
 *
 * @param faultList The fault list to use as a starting point.
 * @return The new fault list together with the mapping from original indices to the new location.
 */
std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> SortStuckAtFaultList(const std::vector<SingleStuckAtFault>& faultList);

/**
 * @brief Removes unwanted faults from the fault list.
 *
 * By using the reduction parameter different sets of faults can be removed / merged.
 * The new fault list comprises of the remaining faults.
 * The second returned element is the mapping from the original fault list indices to
 * the new fault list. Multiple faults from the original fault list can be merged together
 * into one fault in the reduced fault list. If a fault has been removed it is assigned
 * the new index equal to REMOVED_FAULT.
 *
 * @param circuitEnvironment The circuit to use for fault reduction.
 * @param reduction The reduction steps to take. Multiple reductions can be selected at the same time.
 * @param faultList The original stuck-at fault list to use as a starting point.
 * @return The reduced fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> ReduceStuckAtFaultList(const Circuit::CircuitEnvironment& circuitEnvironment, StuckAtFaultReduction reduction, const std::vector<SingleStuckAtFault>& faultList);

/**
 * @brief Removes faults that do not match the pattern given.
 *
 * @param circuitEnvironment The circuit to use for fault reduction.
 * @param pattern The name pattern in Regex syntax to filter stuck-at faults for.
 * @param faultList The original stuck-at fault list to use as a starting point.
 * @return The reduced fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> FilterStuckAtFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<SingleStuckAtFault>& faultList);

/**
 * @brief Removes faults that are not in the node list provided.
 *
 * @param faultList The original fault list.
 * @param nodes The list of nodes to filter for.
 * @return The reduced fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<SingleStuckAtFault>, std::vector<size_t>> FilterStuckAtFaultListByNodes(const std::vector<SingleStuckAtFault>& faultList, const std::vector<Circuit::MappedNode*>& nodes);

template<typename T>
class SingleStuckAtFaultListDebug
{
public:
	SingleStuckAtFaultListDebug(const ConcurrentFaultList<SingleStuckAtFault, T>& faultList, const Circuit::MappedCircuit& circuit);
	operator std::string(void) const;

	template<typename U>
	friend std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultListDebug<U>& debug);

private:
	const ConcurrentFaultList<SingleStuckAtFault, T>& faultList;
	const Circuit::MappedCircuit& circuit;
};

template<typename T>
class SingleStuckAtFaultMetaDataDebug
{
public:
	SingleStuckAtFaultMetaDataDebug(const T& metaData, const Circuit::MappedCircuit& circuit);
	operator std::string(void) const;

	template<typename U>
	friend std::ostream& operator <<(std::ostream& stream, const SingleStuckAtFaultMetaDataDebug<U>& debug);

private:
	const T& metaData;
	const Circuit::MappedCircuit& circuit;
};

template<typename T>
SingleStuckAtFaultListDebug<T> to_debug(const ConcurrentFaultList<SingleStuckAtFault, T>& faultList, const Circuit::MappedCircuit& circuit);

template<typename T, std::enable_if_t<std::is_base_of_v<SingleStuckAtFaultMetaData, T>>>
SingleStuckAtFaultMetaDataDebug<T> to_debug(const T& metaData, const Circuit::MappedCircuit& circuit);

};
};
