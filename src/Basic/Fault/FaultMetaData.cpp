#include "Basic/Fault/FaultMetaData.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Fault
{

BaseFaultMetaData::BaseFaultMetaData(void):
	faultStatus(FaultStatus::FAULT_STATUS_UNCLASSIFIED)
{
}

BaseFaultMetaData::~BaseFaultMetaData(void) = default;

TargetedFaultMetaData::TargetedFaultMetaData(void):
	BaseFaultMetaData(),
	targetedFaultStatus(TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED)
{
}

TargetedFaultMetaData::~TargetedFaultMetaData(void) = default;

std::string to_string(const FaultStatus& status)
{
	switch (status)
	{
		case FaultStatus::FAULT_STATUS_UNCLASSIFIED:
			return "FAULT_STATUS_UNCLASSIFIED";
		case FaultStatus::FAULT_STATUS_DETECTED:
			return "FAULT_STATUS_DETECTED";
		case FaultStatus::FAULT_STATUS_UNDETECTED:
			return "FAULT_STATUS_UNDETECTED";
		case FaultStatus::FAULT_STATUS_EXTENDED:
			return "FAULT_STATUS_EXTENDED";
		default:
			Logging::Panic();
	}
}

std::string to_string(const TargetedFaultStatus& status)
{
	switch(status)
	{
		case TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED:
			return "FAULT_STATUS_UNCLASSIFIED";
		case TargetedFaultStatus::FAULT_STATUS_TESTABLE:
			return "FAULT_STATUS_TESTABLE";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE:
			return "FAULT_STATUS_UNTESTABLE";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL:
			return "FAULT_STATUS_UNTESTABLE_COMBINATIONAL";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_FUNCTIONAL:
			return "FAULT_STATUS_UNTESTABLE_FUNCTIONAL";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_STRUCTURAL:
			return "FAULT_STATUS_UNTESTABLE_STRUCTURAL";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE:
			return "FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE";
		case TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE:
			return "FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT:
			return "FAULT_STATUS_ABORTED_TIMEOUT";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE:
			return "FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE:
			return "FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS:
			return "FAULT_STATUS_ABORTED_MAX_ITERATIONS";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE:
			return "FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE";
		case TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE:
			return "FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE";
		case TargetedFaultStatus::FAULT_STATUS_DEFERRED:
			return "FAULT_STATUS_DEFERRED";
		case TargetedFaultStatus::FAULT_STATUS_DEFERRED_TIMEOUT:
			return "FAULT_STATUS_DEFERRED_TIMEOUT";
		case TargetedFaultStatus::FAULT_STATUS_DEFERRED_MAX_ITERATIONS:
			return "FAULT_STATUS_DEFERRED_MAX_ITERATIONS";
		case TargetedFaultStatus::FAULT_STATUS_DEFERRED_UNREACHABLE:
			return "FAULT_STATUS_DEFERRED_UNREACHABLE";
		case TargetedFaultStatus::FAULT_STATUS_EQUIVALENT:
			return "FAULT_STATUS_EQUIVALENT";
		default:
			Logging::Panic();
	}
}

};
};
