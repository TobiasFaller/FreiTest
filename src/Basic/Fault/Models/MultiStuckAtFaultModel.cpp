#include "Basic/Fault/Models/MultiStuckAtFaultModel.hpp"

namespace FreiTest
{
namespace Fault
{

MultiStuckAtFaultModel::MultiStuckAtFaultModel(std::shared_ptr<MultiStuckAtFault> fault):
	_fault(fault)
{
}

MultiStuckAtFaultModel::~MultiStuckAtFaultModel(void) = default;

void MultiStuckAtFaultModel::SetFault(std::shared_ptr<MultiStuckAtFault> fault)
{
	_fault = fault;
}

const std::shared_ptr<MultiStuckAtFault>& MultiStuckAtFaultModel::GetFault(void) const
{
	return _fault;
}

};
};
