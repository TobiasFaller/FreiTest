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

class ExportCircuitPorts: public BaseApplication
{
public:
	ExportCircuitPorts(void);
	virtual ~ExportCircuitPorts(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

};

};
};
};
