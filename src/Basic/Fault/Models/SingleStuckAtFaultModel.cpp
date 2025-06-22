#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"

namespace FreiTest
{
namespace Fault
{

SingleStuckAtFaultModel::SingleStuckAtFaultModel(std::shared_ptr<SingleStuckAtFault> fault):
	_fault(fault)
{
}

SingleStuckAtFaultModel::~SingleStuckAtFaultModel(void) = default;

void SingleStuckAtFaultModel::SetFault(std::shared_ptr<SingleStuckAtFault> fault)
{
	_fault = fault;
}

const std::shared_ptr<SingleStuckAtFault>& SingleStuckAtFaultModel::GetFault(void) const
{
	return _fault;
}

};
};
