#pragma once

#include <string>
#include <iostream>

#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

class VerilogExporter
{
public:
	VerilogExporter(std::ostream& output);
	virtual ~VerilogExporter(void);

	bool ExportModule(const Verilog::Module& module);

private:
	std::ostream& _output;

};

};
};
};
