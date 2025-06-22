#pragma once

#include <string>
#include <optional>
#include <iostream>

#include "Basic/Fault/ConcurrentFaultList.hpp"
#include "Basic/Fault/FaultMetaData.hpp"
#include "Basic/Fault/Faults/CellAwareFault.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

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

/**
 * @brief Maps the UDFM fault to the cell specified.
 *
 * @param circuit The circuit to base the fault on.
 * @param cell The target cell which is used for mapping the UDFM fault.
 * @param fault The UDFM fault to map to the target cell.
 * @return The newly generated cell-aware fault for the UDFM fault.
 */
std::optional<CellAwareFault> MapUdfmFaultToCell(const Circuit::CircuitEnvironment& circuit, const Circuit::GroupMetaData* cell, const std::shared_ptr<Io::Udfm::UdfmFault>& fault);

/**
 * @brief Generates a new cell-aware fault list for the cells in the circuit.
 *
 * @param circuit The circuit to base the fault list on.
 * @return The newly generated fault list with all cell-aware faults.
 */
std::vector<CellAwareFault> GenerateCellAwareFaultList(const Circuit::CircuitEnvironment& circuit, const Io::Udfm::UdfmModel& udfm);

/**
 * @brief Removes faults that do not match the pattern given.
 *
 * @param circuitEnvironment The circuit to use for fault reduction.
 * @param pattern The name pattern in Regex syntax to filter cell-aware faults for.
 * @param faultList The original cell-aware fault list to use as a starting point.
 * @return The reduced fault list and the mapping of the original faults to the new fault list.
 */
std::pair<std::vector<CellAwareFault>, std::vector<size_t>> FilterCellAwareFaultListByPattern(const Circuit::CircuitEnvironment& circuitEnvironment, const std::string& pattern, const FilterPatternType& type, const std::vector<CellAwareFault>& faultList);

};
};
