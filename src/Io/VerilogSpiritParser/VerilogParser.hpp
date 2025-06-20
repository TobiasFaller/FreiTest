#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest {
namespace Io {
namespace Verilog {

class VerilogParser
{
public:

	VerilogParser(void);
	virtual ~VerilogParser(void);

	bool Parse(std::istream& input);
	void Reset(void);
	ModuleCollection& GetModules(void);

private:
	ModuleCollection _modules;

};

};
};
};
