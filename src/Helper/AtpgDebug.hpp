#pragma once

#include <string>
#include <iostream>

#include "Circuit/CircuitEnvironment.hpp"
#include "Simulation/CircuitSimulationResult.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Helper
{

struct DebugSimulationGood { };
struct DebugSimulationGoodBad { };
struct DebugSimulationVsCheck { };
struct DebugSimulationVsAtpg { };

template<typename DebugType>
struct SimulationDebug
{
	const Circuit::CircuitEnvironment& circuit;
	const Simulation::SimulationResult& resultGoodSim;
	const Simulation::SimulationResult& resultBadSim;
	const Simulation::SimulationResult& resultGoodOther;
	const Simulation::SimulationResult& resultBadOther;
};

template<typename DebugType>
struct SimulationLocationDebug
{
	const size_t nodeId;
	const Circuit::CircuitEnvironment& circuit;
	const Simulation::SimulationResult& resultGoodSim;
	const Simulation::SimulationResult& resultBadSim;
	const Simulation::SimulationResult& resultGoodOther;
	const Simulation::SimulationResult& resultBadOther;
};

template<typename DebugType, typename FaultModel>
struct FaultLocationDebug
{
	const FaultModel& fault;
	const Circuit::CircuitEnvironment& circuit;
	const Simulation::SimulationResult& resultGoodSim;
	const Simulation::SimulationResult& resultBadSim;
	const Simulation::SimulationResult& resultGoodOther;
	const Simulation::SimulationResult& resultBadOther;
};

template<typename PinData>
struct EncodingDebug
{
	Tpg::GeneratorContext<PinData>& context;
};

template<typename DebugType>
SimulationDebug<DebugType> DebugSimulation(
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
);

template<typename DebugType>
SimulationLocationDebug<DebugType> DebugSimulationLocation(
	const size_t nodeId,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
);

template<typename DebugType, typename FaultModel>
FaultLocationDebug<DebugType, FaultModel> DebugFaultLocation(
	const FaultModel& fault,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& resultGoodSim,
	const Simulation::SimulationResult& resultBadSim,
	const Simulation::SimulationResult& resultGoodOther,
	const Simulation::SimulationResult& resultBadOther
);

template<typename PinData>
EncodingDebug<PinData> DebugEncoding(
	Tpg::GeneratorContext<PinData>& context
);

void DebugExportSimulation(
	const std::string& name,
	const Circuit::CircuitEnvironment& circuit,
	const Simulation::SimulationResult& result
);

template<typename DebugType>
std::ostream& operator<<(std::ostream& stream, const SimulationDebug<DebugType>& debug);
template<typename DebugType>
std::ostream& operator<<(std::ostream& stream, const SimulationLocationDebug<DebugType>& debug);
template<typename DebugType, typename FaultModel>
std::ostream& operator<<(std::ostream& stream, const FaultLocationDebug<DebugType, FaultModel>& debug);
template<typename PinData>
std::ostream& operator<<(std::ostream& stream, const EncodingDebug<PinData>& debug);

}
}
