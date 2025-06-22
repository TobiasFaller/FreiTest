
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"

namespace FreiTest
{
namespace Fault
{

CellAwareFaultModel::CellAwareFaultModel(std::shared_ptr<Fault::CellAwareFault> fault):
	_fault(fault)
{
}

CellAwareFaultModel::~CellAwareFaultModel(void) = default;

void CellAwareFaultModel::SetFault(std::shared_ptr<Fault::CellAwareFault> fault)
{
	_fault = fault;
}

const std::shared_ptr<Fault::CellAwareFault>& CellAwareFaultModel::GetFault(void) const
{
	return _fault;
}

};
};
