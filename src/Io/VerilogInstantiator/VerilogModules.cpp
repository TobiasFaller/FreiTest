#include "Io/VerilogInstantiator/VerilogModules.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

Modules::Modules(const Primitives::PrimitiveCollection& primitives, const ModuleCollection& modules):
	primitives(primitives),
	modules(modules)
{
}

Modules::~Modules(void) = default;

};
};
};
