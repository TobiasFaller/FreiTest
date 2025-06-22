#pragma once

#include "Simulation/CircuitSimulationResult.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Extractor
{

template<typename PinData, typename Tag>
Simulation::SimulationResult ExtractSimulationResult(GeneratorContext<PinData>& context);

};
};
};
