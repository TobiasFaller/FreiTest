#pragma once

#include <memory>

#include "Basic/Fault/Faults/SingleStuckAtFault.hpp"

namespace FreiTest
{
namespace Fault
{

class SingleStuckAtFaultModel
{
public:
	SingleStuckAtFaultModel(std::shared_ptr<Fault::SingleStuckAtFault> fault);
	virtual ~SingleStuckAtFaultModel(void);

	void SetFault(std::shared_ptr<Fault::SingleStuckAtFault> fault);
	const std::shared_ptr<Fault::SingleStuckAtFault>& GetFault(void) const;

private:
	std::shared_ptr<Fault::SingleStuckAtFault> _fault;

};

};
};
