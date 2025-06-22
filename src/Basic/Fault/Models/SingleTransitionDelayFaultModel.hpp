#pragma once

#include <memory>

#include "Basic/Fault/Faults/SingleTransitionDelayFault.hpp"

namespace FreiTest
{
namespace Fault
{

class SingleTransitionDelayFaultModel
{
public:
	SingleTransitionDelayFaultModel(std::shared_ptr<Fault::SingleTransitionDelayFault> fault);
	virtual ~SingleTransitionDelayFaultModel(void);

	void SetFault(std::shared_ptr<Fault::SingleTransitionDelayFault> fault);
	const std::shared_ptr<Fault::SingleTransitionDelayFault>& GetFault(void) const;

private:
	std::shared_ptr<Fault::SingleTransitionDelayFault> _fault;

};

};
};
