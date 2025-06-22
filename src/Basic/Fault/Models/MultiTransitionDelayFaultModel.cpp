#include "Basic/Fault/Models/MultiTransitionDelayFaultModel.hpp"

namespace FreiTest
{
namespace Fault
{

MultiTransitionDelayFaultModel::MultiTransitionDelayFaultModel(std::shared_ptr<MultiTransitionDelayFault> fault):
	_fault(fault)
{
}

MultiTransitionDelayFaultModel::~MultiTransitionDelayFaultModel(void) = default;

void MultiTransitionDelayFaultModel::SetFault(std::shared_ptr<MultiTransitionDelayFault> fault)
{
	_fault = fault;
}

const std::shared_ptr<MultiTransitionDelayFault>& MultiTransitionDelayFaultModel::GetFault(void) const
{
	return _fault;
}

};
};
