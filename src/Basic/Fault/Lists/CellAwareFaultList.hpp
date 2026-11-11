#pragma once

#include <string>
#include <optional>
#include <iostream>

#include "Basic/Fault/ConcurrentFaultList.hpp"
#include "Basic/Fault/FaultMetaData.hpp"
#include "Basic/Fault/Faults/CellAwareFault.hpp"
#include "Basic/Fault/Specification/CellAwareSpecification.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Fault
{

struct CellAwareMetaData:
	public TargetedFaultMetaData
{
public:
	CellAwareMetaData(void);
	virtual ~CellAwareMetaData(void);

	size_t detectingPatternId;
	Circuit::MappedCircuit::NodeAndPort detectingNode;
	size_t detectingTimeframe;
	Basic::Logic detectingOutputGood;
	Basic::Logic detectingOutputBad;
};

using CellAwareFaultList = ConcurrentFaultList<CellAwareFault, CellAwareMetaData>;

/**
 * @brief Provides possible reductions that can be specified to the ReduceCellAwareFaultList method.
 *
 * Multiple reductions can be combined with the | (or) operator: RemoveSequentialClock | RemoveSequentialSetReset
 * The & (and) operator can be used to test for the presence of a reduction: (value & RemoveEquivalent) == RemoveEquivalent
 *
 * - RemoveNone: Does nothing
 */
enum class CellAwareFaultReduction: size_t
{
	RemoveNone = 0u
};

CellAwareFaultReduction operator|(CellAwareFaultReduction lhs, CellAwareFaultReduction rhs);
CellAwareFaultReduction operator&(CellAwareFaultReduction lhs, CellAwareFaultReduction rhs);
CellAwareFaultReduction& operator|=(CellAwareFaultReduction& lhs, CellAwareFaultReduction rhs);
CellAwareFaultReduction& operator&=(CellAwareFaultReduction& lhs, CellAwareFaultReduction rhs);

/**
 * @brief Generates a new cell-aware fault list for the cells in the circuit.
 *
 * @param circuit The circuit to base the fault list on.
 * @return The newly generated fault list with all cell-aware faults.
 */
std::vector<CellAwareFault> GenerateCellAwareFaultList(const Circuit::CircuitEnvironment& circuit, const CellAwareModelSpecification& specification);

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
std::pair<std::vector<CellAwareFault>, std::vector<size_t>> ReduceCellAwareFaultList(const Circuit::CircuitEnvironment& circuitEnvironment, CellAwareFaultReduction reduction, const std::vector<CellAwareFault>& faultList);

/**
 * @brief Removes faults that do not match the pattern given.
 *
 * @param circuitEnvironment The circuit to use for fault reduction.
 * @param pattern The name pattern in Regex syntax to filter cell-aware faults for.
 * @param faultList The original cell-aware fault list to use as a starting point.
 * @return The reduced fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<CellAwareFault>, std::vector<size_t>> FilterCellAwareFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<CellAwareFault>& faultList);

/**
 * @brief Keeps only a random selection of faults from the fault list.
 *
 * @param faultList The original fault list.
 * @param samples The number of samples to choose from the fault list
 * @return The sampled fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<CellAwareFault>, std::vector<size_t>> SampleCellAwareFaultList(const std::vector<CellAwareFault>& faultList, size_t samples);

class CellAwareFaultListDebug
{
public:
	CellAwareFaultListDebug(const CellAwareFaultList &faultList, const Circuit::MappedCircuit& circuit);
	operator std::string(void) const;

	friend std::ostream& operator <<(std::ostream& stream, const CellAwareFaultListDebug& debug);

private:
	const CellAwareFaultList &faultList;
	const Circuit::MappedCircuit& circuit;
};

CellAwareFaultListDebug to_debug(const CellAwareFaultList &faultList, const Circuit::MappedCircuit& circuit);

};
};
