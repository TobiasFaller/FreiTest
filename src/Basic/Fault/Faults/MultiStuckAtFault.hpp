#pragma once

#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/StuckAtFault.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

struct MultiStuckAtFault
{
public:
	MultiStuckAtFault(std::vector<StuckAtFault> faults);
	virtual ~MultiStuckAtFault(void);

	const std::vector<StuckAtFault>& GetStuckAts(void) const;

	friend std::string to_string(const MultiStuckAtFault& fault);

private:
	std::vector<StuckAtFault> stuckAts;

};

std::string to_string(const MultiStuckAtFault& fault);

};
};
