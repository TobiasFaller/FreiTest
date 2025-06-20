#pragma once

#include "Io/VerilogInstantiator/Primitive.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

struct Modules
{
	Modules(const Primitives::PrimitiveCollection& primitives, const Verilog::ModuleCollection& modules);
	virtual ~Modules(void);

	const Primitives::PrimitiveCollection& primitives;
	const Verilog::ModuleCollection& modules;
};

};
};
};
