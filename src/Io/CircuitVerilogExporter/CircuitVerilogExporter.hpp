#pragma once

#include <iostream>
#include <set>

#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogExporter/VerilogExporter.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest
{
namespace Io
{

class CircuitVerilogExporter
{

public:
	CircuitVerilogExporter(void);
	virtual ~CircuitVerilogExporter(void);

	void ExportCircuit(const Circuit::CircuitEnvironment& circuitEnvironment, std::ostream& out);

private:
	void BuildModule(const Circuit::CircuitEnvironment& circuitEnvironment, const Circuit::GroupMetaData* group);

	std::vector<Verilog::Module> modules;

};

};
};
