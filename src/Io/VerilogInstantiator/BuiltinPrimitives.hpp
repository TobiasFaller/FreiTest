#pragma once

#include <string>
#include <vector>

#include "Io/VerilogInstantiator/Primitive.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{
namespace Primitives
{

void DefineConstantPrimitives(PrimitiveCollection& primitives);
void DefinePrimaryPortPrimitives(PrimitiveCollection& primitives);
void DefineBufferPrimitives(PrimitiveCollection& primitives);
void DefineCombinatorialPrimitives(PrimitiveCollection& primitives);
void DefineMultiplexerPrimitives(PrimitiveCollection& primitives);
void DefineTristatePrimitives(PrimitiveCollection& primitives);
void DefineSequentialPrimitives(PrimitiveCollection& primitives);

};
};
};
};
