#pragma once

#include <string>

#include "Applications/BaseApplication.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

class ExportCircuitNodes: public BaseApplication
{
public:
	ExportCircuitNodes(void);
	virtual ~ExportCircuitNodes(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

private:
	std::string optionFilter = ".*";

};

};
};
};
