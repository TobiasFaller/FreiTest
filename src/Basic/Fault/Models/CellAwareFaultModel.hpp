#pragma once

#include <memory>

#include "Basic/Fault/Faults/CellAwareFault.hpp"

namespace FreiTest
{
namespace Fault
{

class CellAwareFaultModel
{
public:
	CellAwareFaultModel(std::shared_ptr<Fault::CellAwareFault> fault);
	virtual ~CellAwareFaultModel(void);

	const std::shared_ptr<Fault::CellAwareFault>& GetFault(void) const;
	void SetFault(std::shared_ptr<Fault::CellAwareFault> fault);

private:
	std::shared_ptr<Fault::CellAwareFault> _fault;

};

};
};
