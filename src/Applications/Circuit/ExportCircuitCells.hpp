#pragma once

#include <string>
#include <ostream>

#include "Applications/BaseApplication.hpp"
#include "Circuit/CircuitMetaData.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

class ExportCircuitCells: public BaseApplication
{
public:
	ExportCircuitCells(void);
	virtual ~ExportCircuitCells(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

};

};
};
};
