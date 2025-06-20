#pragma once

#include "Circuit/CircuitBuilder.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogInstantiator/Primitive.hpp"
#include "Io/VerilogInstantiator/ModuleInstance.hpp"
#include "Io/VerilogInstantiator/VerilogModules.hpp"

#include <memory>

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

class VerilogInstantiator
{
public:
    VerilogInstantiator(void);
    virtual ~VerilogInstantiator(void);

    std::unique_ptr<Instance::ModuleInstance> Instantiate(const Modules& modules, const std::string& name) const;
	std::unique_ptr<Circuit::Builder::CircuitBuilder> CreateBuilder(const Modules& modules, const Instance::ModuleInstance& instance, const Circuit::Builder::BuildConfiguration& configuration) const;
	std::unique_ptr<Circuit::CircuitEnvironment> CreateCircuit(const Modules& modules, const Instance::ModuleInstance& instance, const Circuit::Builder::BuildConfiguration& configuration) const;

};

};
};
};
