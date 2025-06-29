#pragma once

#include <string>

namespace FreiTest
{
namespace Fault
{

enum class FaultStatus
{
	FAULT_STATUS_UNCLASSIFIED,
	FAULT_STATUS_DETECTED,
	FAULT_STATUS_UNDETECTED,
	FAULT_STATUS_EXTENDED
};

enum class TargetedFaultStatus
{
	FAULT_STATUS_UNCLASSIFIED,
	FAULT_STATUS_TESTABLE,
	FAULT_STATUS_UNTESTABLE,
	FAULT_STATUS_UNTESTABLE_COMBINATIONAL,
	FAULT_STATUS_UNTESTABLE_FUNCTIONAL,
	FAULT_STATUS_UNTESTABLE_STRUCTURAL,
	FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE,
	FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE,
	FAULT_STATUS_ABORTED_TIMEOUT,
	FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE,
	FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE,
	FAULT_STATUS_ABORTED_MAX_ITERATIONS,
	FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE,
	FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE,
	FAULT_STATUS_DEFERRED,
	FAULT_STATUS_DEFERRED_TIMEOUT,
	FAULT_STATUS_DEFERRED_MAX_ITERATIONS,
	FAULT_STATUS_DEFERRED_UNREACHABLE,
	FAULT_STATUS_EQUIVALENT
};

class BaseFaultMetaData
{
public:
	BaseFaultMetaData(void);
	virtual ~BaseFaultMetaData(void);

	FaultStatus faultStatus;
};

class TargetedFaultMetaData: public BaseFaultMetaData
{
public:
	TargetedFaultMetaData(void);
	virtual ~TargetedFaultMetaData(void);

	TargetedFaultStatus targetedFaultStatus;
};

std::string to_string(const FaultStatus& status);
std::string to_string(const TargetedFaultStatus& status);

};
};
