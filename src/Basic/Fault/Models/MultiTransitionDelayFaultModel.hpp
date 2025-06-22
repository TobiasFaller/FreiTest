#pragma once

#include <memory>

#include "Basic/Fault/Faults/MultiTransitionDelayFault.hpp"

namespace FreiTest
{
namespace Fault
{

class MultiTransitionDelayFaultModel
{
public:
	MultiTransitionDelayFaultModel(std::shared_ptr<Fault::MultiTransitionDelayFault> fault);
	virtual ~MultiTransitionDelayFaultModel(void);

	void SetFault(std::shared_ptr<Fault::MultiTransitionDelayFault> fault);
	const std::shared_ptr<Fault::MultiTransitionDelayFault>& GetFault(void) const;

private:
	std::shared_ptr<Fault::MultiTransitionDelayFault> _fault;

};

};
};
