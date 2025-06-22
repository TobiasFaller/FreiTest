#pragma once

#include <cstdint>
#include <string>

#include "Circuit/CircuitEnvironment.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

VcmInput GetVcmInput(const Circuit::CircuitEnvironment& vcmCircuit, const Circuit::MappedNode* vcmNode, const Circuit::CircuitEnvironment& targetCircuit);
VcmOutput GetVcmOutput(const Circuit::CircuitEnvironment& vcmCircuit, const Circuit::MappedNode* vcmNode);

};
};
};
