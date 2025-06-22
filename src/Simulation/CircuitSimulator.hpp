#pragma once

#include <cstdint>
#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/SequentialConfiguration.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Simulation/CircuitSimulationResult.hpp"

namespace FreiTest
{
namespace Simulation
{

enum class SequentialMode { FullScan, Functional };

struct SimulationConfig
{
	Basic::SequentialConfig sequentialConfig;
	SequentialMode sequentialMode;
};

SimulationConfig MakeSimulationConfig(Basic::SequentialConfig sequentialConfig);

template<typename FaultModel>
void SimulateTestPatternNaive(
	const Circuit::MappedCircuit& circuit, const Pattern::TestPattern& pattern,
	const FaultModel& faultModel, SimulationResult& result, const SimulationConfig& config
);

template<typename FaultModel>
void SimulateTestPatternEventDriven(
	const Circuit::MappedCircuit& circuit, const Pattern::TestPattern& pattern, const FaultModel& faultModel,
	SimulationResult& result, const SimulationConfig& config
);

template<typename FaultModel>
void SimulateTestPatternEventDrivenIncremental(
	const Circuit::MappedCircuit& circuit, const Pattern::TestPattern& pattern, const FaultModel& faultModel,
	const SimulationResult& base, SimulationResult& result, const SimulationConfig& config
);

};
};
