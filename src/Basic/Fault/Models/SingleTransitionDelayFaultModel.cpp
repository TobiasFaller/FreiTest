#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"

namespace FreiTest
{
namespace Fault
{

SingleTransitionDelayFaultModel::SingleTransitionDelayFaultModel(std::shared_ptr<SingleTransitionDelayFault> fault):
	_fault(fault)
{
}

SingleTransitionDelayFaultModel::~SingleTransitionDelayFaultModel(void) = default;

void SingleTransitionDelayFaultModel::SetFault(std::shared_ptr<SingleTransitionDelayFault> fault)
{
	_fault = fault;
}

const std::shared_ptr<SingleTransitionDelayFault>& SingleTransitionDelayFaultModel::GetFault(void) const
{
	return _fault;
}

};
};
