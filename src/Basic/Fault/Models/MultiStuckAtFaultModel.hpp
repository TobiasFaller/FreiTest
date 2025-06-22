#pragma once

#include <memory>

#include "Basic/Fault/Faults/MultiStuckAtFault.hpp"

namespace FreiTest
{
namespace Fault
{

/*!
 * \brief A container to hold the multi stuck-at faults used in other classes.
 *
 * Currently, only permanent faults are accounted for which are modeled by
 * multiple single stuck-at faults.
 */
class MultiStuckAtFaultModel
{
public:
	MultiStuckAtFaultModel(std::shared_ptr<Fault::MultiStuckAtFault> fault);
	virtual ~MultiStuckAtFaultModel(void);

	void SetFault(std::shared_ptr<Fault::MultiStuckAtFault> fault);
	const std::shared_ptr<Fault::MultiStuckAtFault>& GetFault(void) const;

private:
	std::shared_ptr<Fault::MultiStuckAtFault> _fault;

};

};
};
