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

class ExportCircuitHierarchy: public BaseApplication
{
public:
	ExportCircuitHierarchy(void);
	virtual ~ExportCircuitHierarchy(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

private:
	void ExportHierarchy(size_t level, const Circuit::GroupMetaData* group, std::ostream& out);

	std::string optionFilter = ".*";

};

};
};
};
