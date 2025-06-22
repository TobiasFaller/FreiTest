#pragma once

#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Fault/Faults/TransitionDelayFault.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Fault
{

struct MultiTransitionDelayFault
{
public:
	MultiTransitionDelayFault(std::vector<TransitionDelayFault> faults);
	virtual ~MultiTransitionDelayFault(void);

	const std::vector<TransitionDelayFault>& GetTransitionDelays(void) const;

	friend std::string to_string(const MultiTransitionDelayFault& fault);

private:
	std::vector<TransitionDelayFault> transitionDelays;

};

std::string to_string(const MultiTransitionDelayFault& fault);

};
};
