#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "Basic/Settings.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

class VerilogConverter
{
public:
	VerilogConverter(void);
	virtual ~VerilogConverter(void);

	std::unique_ptr<Circuit::CircuitEnvironment> LoadCircuit(std::shared_ptr<Settings> settings) const;

	bool PreprocessFiles(std::shared_ptr<Settings>& settings, std::ostream& output) const;
	std::unique_ptr<ModuleCollection> ParseVerilog(std::shared_ptr<Settings>& settings, std::istream& input) const;
	std::unique_ptr<Circuit::CircuitEnvironment> InstantiateModule(std::shared_ptr<Settings> settings, const ModuleCollection& verilogModules) const;
};

};
};
};
