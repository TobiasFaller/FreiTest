#include "Io/FaultListParser/FaultListParser.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
#include <utility>

#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/CircuitGuard/CircuitGuard.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Io
{

FaultTypeException::FaultTypeException(std::string expected, std::string detected)
{
	expectedFault = ConvertStringToFaultType(expected);
	detectedFault = ConvertStringToFaultType(detected);
}

FaultTypeException::~FaultTypeException(void) = default;

const FaultType& FaultTypeException::GetExpectedFaultType(void) const
{
	return expectedFault;
}

const FaultType& FaultTypeException::GetDetectedFaultType(void) const
{
	return detectedFault;
}

FaultType FaultTypeException::ConvertStringToFaultType(std::string type) const
{
	if (type == "single-stuck-at-fault")
	{
		return FaultType::SingleStuckAtFault;
	}
	else if (type == "single-transition-delay-fault")
	{
		return FaultType::SingleTransitionDelayFault;
	}
	else if (type == "cell-aware-fault")
	{
		return FaultType::CellAwareFault;
	}
	else
	{
		LOG(ERROR) << "Got unknown fault type string \"" << type << "\"";
		return FaultType::UnknownFault;
	}
}

std::string ConvertFaultTypeToString(const FaultType& faultType)
{
	switch (faultType)
	{
		case FaultType::SingleStuckAtFault:
			return "single-stuck-at-fault";
		case FaultType::SingleTransitionDelayFault:
			return "single-transition-delay-fault";
		case FaultType::CellAwareFault:
			return "cell-aware-fault";
		default:
			return "unkown-fault";
	}
}

template<typename FaultT>
static std::string GetFaultTypeString(void)
{
	if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>)
	{
		return "single-stuck-at-fault";
	}
	else if constexpr (std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
	{
		return "single-transition-delay-fault";
	}
	else if constexpr (std::is_same_v<FaultT, Fault::CellAwareFault>)
	{
		return "cell-aware-fault";
	}
	else
	{
		static_assert(std::is_same_v<FaultT, Fault::SingleStuckAtFault>, "Unsupported fault type given");
	}
}

template<typename FaultTypeT>
static FaultTypeT ConvertStringToFaultSubType(std::string faultSubType)
{
	if constexpr (std::is_same_v<FaultTypeT, Fault::StuckAtFaultType>)
	{
		if (faultSubType == "stuck-at-0")
		{
			return Fault::StuckAtFaultType::STUCK_AT_0;
		}
		else if (faultSubType == "stuck-at-1")
		{
			return Fault::StuckAtFaultType::STUCK_AT_1;
		}
		else if (faultSubType == "stuck-at-x")
		{
			return Fault::StuckAtFaultType::STUCK_AT_X;
		}
		else if (faultSubType == "stuck-at-u")
		{
			return Fault::StuckAtFaultType::STUCK_AT_U;
		}

		throw boost::property_tree::ptree_bad_data("Invalid fault sub-type given for single stuck-at fault model", faultSubType);
	}
	else if constexpr (std::is_same_v<FaultTypeT, Fault::TransitionDelayFaultType>)
	{
		if (faultSubType == "slow-to-rise")
		{
			return Fault::TransitionDelayFaultType::SLOW_TO_RISE;
		}
		else if (faultSubType == "slow-to-fall")
		{
			return Fault::TransitionDelayFaultType::SLOW_TO_FALL;
		}
		else if (faultSubType == "slow-to-transition")
		{
			return Fault::TransitionDelayFaultType::SLOW_TO_TRANSITION;
		}

		throw boost::property_tree::ptree_bad_data("Invalid fault sub-type given for single stuck-at fault model", faultSubType);
	}
	else
	{
		static_assert(std::is_same_v<FaultTypeT, Fault::StuckAtFaultType>, "Unsupported fault type given");
	}
}

template<typename FaultTypeT>
static std::string ConvertFaultSubTypeToString(FaultTypeT faultSubType)
{
	if constexpr (std::is_same_v<FaultTypeT, Fault::StuckAtFaultType>)
	{
		switch (faultSubType)
		{
			case Fault::StuckAtFaultType::STUCK_AT_0:
				return "stuck-at-0";
			case Fault::StuckAtFaultType::STUCK_AT_1:
				return "stuck-at-1";
			case Fault::StuckAtFaultType::STUCK_AT_X:
				return "stuck-at-x";
			case Fault::StuckAtFaultType::STUCK_AT_U:
				return "stuck-at-u";
			default:
				throw boost::property_tree::ptree_bad_data("Invalid fault sub-type given for single stuck-at fault model", faultSubType);
		}
	}
	else if constexpr (std::is_same_v<FaultTypeT, Fault::TransitionDelayFaultType>)
	{
		switch (faultSubType)
		{
			case Fault::TransitionDelayFaultType::SLOW_TO_RISE:
				return "slow-to-rise";
			case Fault::TransitionDelayFaultType::SLOW_TO_FALL:
				return "slow-to-fall";
			case Fault::TransitionDelayFaultType::SLOW_TO_TRANSITION:
				return "slow-to-transition";
			default:
				throw boost::property_tree::ptree_bad_data("Invalid fault sub-type given for single transition delay fault model", faultSubType);
		}
	}
	else
	{
		static_assert(std::is_same_v<FaultTypeT, Fault::StuckAtFaultType>, "Unsupported fault type given");
	}
}

static std::string ConvertFaultStatusToString(Fault::FaultStatus status)
{
	switch (status)
	{
		case Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED:
			return "unclassified";
		case Fault::FaultStatus::FAULT_STATUS_DETECTED:
			return "detected";
		case Fault::FaultStatus::FAULT_STATUS_UNDETECTED:
			return "undetected";
		case Fault::FaultStatus::FAULT_STATUS_EXTENDED:
			return "extended";
		default:
			return "invalid";
	}
}

static Fault::FaultStatus ConvertStringToFaultStatus(std::string status)
{
	const std::vector<std::pair<std::string, Fault::FaultStatus>> mappings =
	{
		{ "unclassified", Fault::FaultStatus::FAULT_STATUS_UNCLASSIFIED },
		{ "detected", Fault::FaultStatus::FAULT_STATUS_DETECTED },
		{ "undetected", Fault::FaultStatus::FAULT_STATUS_UNDETECTED },
		{ "extended", Fault::FaultStatus::FAULT_STATUS_EXTENDED },
	};
	for (auto& [name, value] : mappings)
	{
		if (status == name)
		{
			return value;
		}
	}

	throw boost::property_tree::ptree_bad_data("Invalid fault status", status);
}

static std::string ConvertTargetedFaultStatusToString(Fault::TargetedFaultStatus status)
{
	switch (status)
	{
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED:
		return "unclassified";
	case Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE:
		return "testable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE:
		return "untestable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL:
		return "untestable-combinational";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_FUNCTIONAL:
		return "untestable-functional";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_STRUCTURAL:
		return "untestable-structural";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE:
		return "untestable-unsensitizable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE:
		return "untestable-unpropagatable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT:
		return "aborted-timeout";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE:
		return "aborted-timeout-unsensitizable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE:
		return "aborted-timeout-unpropagatable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS:
		return "aborted-max-iterations";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE:
		return "aborted-max-iterations-unsensitizable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE:
		return "aborted-max-iterations-unpropagatable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED:
		return "deferred";
	case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_TIMEOUT:
		return "deferred-timeout";
	case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_MAX_ITERATIONS:
		return "deferred-max-iterations";
	case Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_UNREACHABLE:
		return "deferred-unreachable";
	case Fault::TargetedFaultStatus::FAULT_STATUS_EQUIVALENT:
		return "equivalent-testable";
	default:
		throw boost::property_tree::ptree_bad_data("Invalid fault status", status);
	}
}

static Fault::TargetedFaultStatus ConvertStringToTargetedFaultStatus(std::string status)
{
	const std::vector<std::pair<std::string, Fault::TargetedFaultStatus>> mappings =
	{
		{ "unclassified", Fault::TargetedFaultStatus::FAULT_STATUS_UNCLASSIFIED },
		{ "testable", Fault::TargetedFaultStatus::FAULT_STATUS_TESTABLE },
		{ "untestable", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE },
		{ "untestable-combinational", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_COMBINATIONAL },
		{ "untestable-functional", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_FUNCTIONAL },
		{ "untestable-structural", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_STRUCTURAL },
		{ "untestable-unsensitizable", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNSENSITIZABLE },
		{ "untestable-unpropagatable", Fault::TargetedFaultStatus::FAULT_STATUS_UNTESTABLE_UNPROPAGATABLE },
		{ "aborted-timeout", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT },
		{ "aborted-timeout-unsensitizable", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNSENSITIZABLE },
		{ "aborted-timeout-unpropagatable", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_TIMEOUT_UNPROPAGATABLE },
		{ "aborted-max-iterations", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS },
		{ "aborted-max-iterations-unsensitizable", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNSENSITIZABLE },
		{ "aborted-max-iterations-unpropagatable", Fault::TargetedFaultStatus::FAULT_STATUS_ABORTED_MAX_ITERATIONS_UNPROPAGATABLE },
		{ "deferred", Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED },
		{ "deferred-timeout", Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_TIMEOUT },
		{ "deferred-max-iterations", Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_MAX_ITERATIONS },
		{ "deferred-unreachable", Fault::TargetedFaultStatus::FAULT_STATUS_DEFERRED_UNREACHABLE },
		{ "equivalent-testable", Fault::TargetedFaultStatus::FAULT_STATUS_EQUIVALENT },
	};
	for (auto& [name, value] : mappings)
	{
		if (status == name)
		{
			return value;
		}
	}

	throw boost::property_tree::ptree_bad_data("Invalid fault status", status);
}

static std::string ConvertPortTypeToString(Circuit::PortType portType)
{
	switch(portType)
	{
	case Circuit::PortType::Input:
		return "input";
	case Circuit::PortType::Output:
		return "output";
	default:
		LOG(FATAL) << "Unknown PortType";
	}
	__builtin_unreachable();
}

static Circuit::PortType ConvertStringToPortType(std::string str)
{
	const std::vector<std::pair<std::string, Circuit::PortType>> mappings =
	{
		{ "input", Circuit::PortType::Input },
		{ "output", Circuit::PortType::Output },
	};

	for (auto& [name, value] : mappings)
	{
		if (str == name)
		{
			return value;
		}
	}
	__builtin_unreachable();
}

static bool CheckOutputLogic(Logic goodOutput, Logic badOutput)
{
	return (goodOutput == Logic::LOGIC_ZERO && badOutput == Logic::LOGIC_ONE)
		|| (badOutput == Logic::LOGIC_ZERO && goodOutput == Logic::LOGIC_ONE);
}

template<typename FaultList, typename... Params>
bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<FaultList>& faultList, Params... params)
{
	using ptree = boost::property_tree::ptree;
	using FaultT = typename FaultList::fault_type;
	using MetaDataT = typename FaultList::metadata_type;

	const auto& circuit { faultList.GetCircuit() };
	const auto& circuitMetaData { circuit.GetMetaData() };
	const auto& mappedCircuit { circuit.GetMappedCircuit() };
	const auto& faults { faultList.GetFaults() };

	ptree faultListType;
	faultListType.put("fault_list_type", GetFaultTypeString<FaultT>());

	ptree exportFaultList;
	size_t faultIndex = 0u;
	for (auto const& [fault, metaData] : faults)
	{
		ptree faultItem;

		// Implement other fault types below
		if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>
			|| std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
		{
			faultItem.put("node_index", fault->GetNode()->GetNodeId());
			faultItem.put("node_name", fault->GetNode()->GetName());
			faultItem.put("signal_name", mappedCircuit.GetDriverForPort(*fault)->GetOutputSignalName());
			faultItem.put("friendly_name", circuitMetaData.GetFriendlyName(*fault));
			faultItem.put("port_type", ConvertPortTypeToString(fault->GetPort().portType));
			faultItem.put("port_number", fault->GetPort().portNumber);
			faultItem.put("type", GetFaultTypeString<FaultT>());
			faultItem.put("subtype", ConvertFaultSubTypeToString(fault->GetType()));
		}
		else if constexpr (std::is_same_v<FaultT, Fault::CellAwareFault>)
		{
			const auto userDefinedFault = fault->GetUserDefinedFault();
			faultItem.put("type", GetFaultTypeString<FaultT>());
			faultItem.put("fault_name", userDefinedFault->GetFaultName());
			faultItem.put("fault_category", userDefinedFault->GetFaultCategory());
			faultItem.put("cell_name", userDefinedFault->GetCellName());
			faultItem.put("cell_group", fault->GetCell()->GetHierarchyName());

			ptree alternativesItem;
			ptree alternativeList;
			for (size_t alternativesIndex = 0; alternativesIndex < userDefinedFault->GetAlternatives().size(); ++alternativesIndex)
			{
				alternativeList.put("index", alternativesIndex);
				alternativeList.put("test_type", userDefinedFault->GetAlternative(alternativesIndex)->GetTestType());
				ptree faultConditionsItem;
				for (const auto& condition : fault->GetAlternatives()[alternativesIndex].conditions)
				{
					const auto& [conditionNode, conditionPort] = condition.nodeAndPort;

					ptree singleFaultConditionItem;
					singleFaultConditionItem.put("node_index", conditionNode->GetNodeId());
					singleFaultConditionItem.put("node_name", conditionNode->GetName());
					singleFaultConditionItem.put("signal_name", mappedCircuit.GetDriverForPort(condition.nodeAndPort)->GetOutputSignalName());
					singleFaultConditionItem.put("friendly_name", circuitMetaData.GetFriendlyName(condition.nodeAndPort));
					singleFaultConditionItem.put("port_type", ConvertPortTypeToString(conditionPort.portType));
					singleFaultConditionItem.put("port_number", conditionPort.portNumber);
					singleFaultConditionItem.put("port_name", condition.portName);
					singleFaultConditionItem.put("port_value", condition.logicConstraints);

					faultConditionsItem.push_back(std::make_pair("", singleFaultConditionItem));
				}
				alternativeList.put_child("fault_conditions", faultConditionsItem);

				ptree faultEffectsItem;
				for (const auto& effect : fault->GetAlternatives()[alternativesIndex].effects)
				{
					const auto& [effectNode, effectPort] = effect.nodeAndPort;

					ptree singleFaultEffectsItem;
					singleFaultEffectsItem.put("node_index", effectNode->GetNodeId());
					singleFaultEffectsItem.put("node_name", effectNode->GetName());
					singleFaultEffectsItem.put("signal_name", mappedCircuit.GetDriverForPort(effect.nodeAndPort)->GetOutputSignalName());
					singleFaultEffectsItem.put("friendly_name", circuitMetaData.GetFriendlyName(effect.nodeAndPort));
					singleFaultEffectsItem.put("port_type", ConvertPortTypeToString(effectPort.portType));
					singleFaultEffectsItem.put("port_number", effectPort.portNumber);
					singleFaultEffectsItem.put("port_name", effect.portName);
					singleFaultEffectsItem.put("faulty_port_value", effect.logicConstraints);

					faultEffectsItem.push_back(std::make_pair("", singleFaultEffectsItem));
				}
				alternativeList.put_child("fault_effects", faultEffectsItem);
				alternativesItem.push_back(std::make_pair("", alternativeList));
			}

			faultItem.put_child("alternatives", alternativesItem);
		}
		else
		{
			static_assert(std::is_same_v<FaultT, Fault::SingleStuckAtFault>, "Unsupported fault type given");
		}

		ptree faultResultItem;
		faultResultItem.put("index", faultIndex++);
		faultResultItem.put_child("fault", faultItem);

		// Implement other metaData types below
		if constexpr (std::is_same_v<MetaDataT, Fault::SingleStuckAtFaultMetaData>
			|| std::is_same_v<MetaDataT, Fault::SingleTransitionDelayFaultMetaData>)
		{
			faultResultItem.put("status", ConvertFaultStatusToString(metaData->faultStatus));
			faultResultItem.put("status_targeted", ConvertTargetedFaultStatusToString(metaData->targetedFaultStatus));

			if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
			{
				ptree detectedByItem;
				if (metaData->detectingNode.node != nullptr)
				{
					detectedByItem.put("node_index", metaData->detectingNode.node->GetNodeId());
					detectedByItem.put("port_type", ConvertPortTypeToString(metaData->detectingNode.port.portType));
					detectedByItem.put("port_number", metaData->detectingNode.port.portNumber);
					detectedByItem.put("node_name", metaData->detectingNode.node->GetName());
					detectedByItem.put("signal_name", mappedCircuit.GetDriverForPort(metaData->detectingNode)->GetOutputSignalName());
					detectedByItem.put("friendly_name", circuitMetaData.GetFriendlyName(metaData->detectingNode));
					detectedByItem.put("timeframe", metaData->detectingTimeframe);
					detectedByItem.put("good_value", static_cast<char>(metaData->detectingOutputGood));
					detectedByItem.put("bad_value", static_cast<char>(metaData->detectingOutputBad));
				}

				ptree testPatternItem;
				testPatternItem.put("pattern_index", metaData->detectingPatternId);
				testPatternItem.put_child("detected_by", detectedByItem);

				faultResultItem.put_child("pattern", testPatternItem);
			}
		}
		else if constexpr (std::is_same_v<MetaDataT, Fault::CellAwareMetaData>)
		{
			faultResultItem.put("status", ConvertFaultStatusToString(metaData->faultStatus));
			faultResultItem.put("status_targeted", ConvertTargetedFaultStatusToString(metaData->targetedFaultStatus));

			if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
			{
				ptree detectedByItem;
				if (metaData->detectingNode.node != nullptr)
				{
					detectedByItem.put("node_index", metaData->detectingNode.node->GetNodeId());
					detectedByItem.put("port_type", ConvertPortTypeToString(metaData->detectingNode.port.portType));
					detectedByItem.put("port_number", metaData->detectingNode.port.portNumber);
					detectedByItem.put("node_name", metaData->detectingNode.node->GetName());
					detectedByItem.put("signal_name", mappedCircuit.GetDriverForPort(metaData->detectingNode)->GetOutputSignalName());
					detectedByItem.put("friendly_name", circuitMetaData.GetFriendlyName(metaData->detectingNode));
					detectedByItem.put("timeframe", metaData->detectingTimeframe);
					detectedByItem.put("good_value", static_cast<char>(metaData->detectingOutputGood));
					detectedByItem.put("bad_value", static_cast<char>(metaData->detectingOutputBad));
				}

				ptree testPatternItem;
				testPatternItem.put("pattern_index", metaData->detectingPatternId);
				testPatternItem.put_child("detected_by", detectedByItem);

				faultResultItem.put_child("pattern", testPatternItem);
			}
		}
		else
		{
			static_assert(std::is_same_v<MetaDataT, Fault::SingleStuckAtFaultMetaData>, "Unsupported fault metaData type given");
		}

		exportFaultList.push_back(std::make_pair("", faultResultItem));
	}

	ptree root;
	root.put_child("meta_data", faultListType);
	root.put_child("faults", exportFaultList);
	root.put_child("circuit", CreateCircuitGuard(circuit));

	try
	{
		boost::property_tree::write_json(output, root);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
		return false;
	}

	return true;
}

template<typename FaultList, typename... Params>
std::optional<FaultListExchangeFormat<FaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit, Params... params)
{
	using ptree = boost::property_tree::ptree;
	using FaultT = typename FaultList::fault_type;
	using MetaDataT = typename FaultList::metadata_type;

	const auto& mappedCircuit = circuit.GetMappedCircuit();
	const auto parameters { std::make_tuple(std::forward<Params>(params)...) };

	try
	{
		ptree root;
		boost::property_tree::read_json(input, root);
		if (!ValidateCircuitGuard(root.get_child("circuit"), circuit))
		{
			return std::nullopt;
		}

		ptree& listMetaData = root.get_child("meta_data");
		std::string faultListTypeString = listMetaData.get_child("fault_list_type").get_value<std::string>();
		if (faultListTypeString != GetFaultTypeString<FaultT>())
		{
			VLOG(1) << "Invalid parser used for file. Expected " << GetFaultTypeString<FaultT>()
				<< " but got " << faultListTypeString;

			throw FaultTypeException(GetFaultTypeString<FaultT>(), faultListTypeString);
		}

		Fault::ConcurrentFaultList<FaultT, MetaDataT> faultList;

		ptree& treeFaults = root.get_child("faults");
		for (auto it = treeFaults.ordered_begin(); it != treeFaults.not_found(); ++it)
		{
			ptree& faultItem = it->second;

			DVLOG(5) << "Importing fault with index " << faultList.size();
			if (faultItem.get_child("index").get_value<size_t>() != faultList.size())
			{
				LOG(ERROR) << "Fault has invalid index";
				return std::nullopt;
			}

			std::shared_ptr<FaultT> fault;
			std::shared_ptr<MetaDataT> metaData;

			// Implement other fault types below
			if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>
				|| std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
			{
				if (faultItem.get_child("fault.type").get_value<std::string>() != GetFaultTypeString<FaultT>())
				{
					LOG(ERROR) << "Invalid parser used for file. Expected " << GetFaultTypeString<FaultT>()
						<< " but got " << faultItem.get_child("fault.type").get_value<std::string>();
					return std::nullopt;
				}

				Circuit::PortType portType = ConvertStringToPortType(faultItem.get_child("fault.port_type").get_value<std::string>());
				size_t nodeId = faultItem.get_child("fault.node_index").get_value<size_t>();
				size_t portNumber = faultItem.get_child("fault.port_number").get_value<size_t>();

				if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>)
				{
					fault = std::make_shared<FaultT>(
						Circuit::MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { portType, portNumber } },
						ConvertStringToFaultSubType<Fault::StuckAtFaultType>(faultItem.get_child("fault.subtype").get_value<std::string>())
					);
				}
				else if constexpr (std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
				{
					fault = std::make_shared<FaultT>(
						Circuit::MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { portType, portNumber } },
						ConvertStringToFaultSubType<Fault::TransitionDelayFaultType>(faultItem.get_child("fault.subtype").get_value<std::string>())
					);
				}

				if (fault->GetNode()->GetNodeId() >= mappedCircuit.GetNumberOfNodes())
				{
					LOG(ERROR) << "Invalid node which does not exist was given for fault";
					return std::nullopt;
				}
				if (fault->GetNode()->GetName() != faultItem.get_child("fault.node_name").get_value<std::string>())
				{
					LOG(ERROR) << "Fault node has invalid name \"" << faultItem.get_child("fault.node_name").get_value<std::string>()
						<< "\", expected \"" << fault->GetNode()->GetName() << "\"";
					return std::nullopt;
				}
				if (fault->GetNode()->GetOutputSignalName() != faultItem.get_child("fault.signal_name").get_value<std::string>())
				{
					LOG(ERROR) << "Fault node has invalid signal name \"" << faultItem.get_child("fault.signal_name").get_value<std::string>()
						<< "\", expected \"" << fault->GetNode()->GetOutputSignalName() << "\"";
					return std::nullopt;
				}

				if (fault->GetPort().portType == Circuit::PortType::Output)
				{
					if (fault->GetPort().portNumber != 0u)
					{
						LOG(ERROR) << "Invalid output port " << fault->GetPort().portNumber << " was selected";
						return std::nullopt;
					}
				}
				else
				{
					if (fault->GetPort().portNumber >= fault->GetNode()->GetNumberOfInputs())
					{
						LOG(ERROR) << "Invalid input port " << fault->GetPort().portNumber << " was selected";
						return std::nullopt;
					}
				}
			}
			else if constexpr(std::is_same_v<FaultT, Fault::CellAwareFault>)
			{
				auto [udfm] = parameters;

				if (faultItem.get_child("fault.type").get_value<std::string>() != GetFaultTypeString<FaultT>())
				{
					LOG(ERROR) << "Invalid parser used for file. Expected " << GetFaultTypeString<FaultT>()
						<< " but got " << faultItem.get_child("fault.type").get_value<std::string>();
					return std::nullopt;
				}

				if (faultItem.get_child("fault.fault_name").get_value<std::string>().empty())
				{
					LOG(ERROR) << "Fault name is empty!";
					return std::nullopt;
				}
				if (faultItem.get_child("fault.fault_category").get_value<std::string>().empty())
				{
					LOG(ERROR) << "Fault category is empty!";
					return std::nullopt;
				}
				if (faultItem.get_child("fault.cell_name").get_value<std::string>().empty())
				{
					LOG(ERROR) << "Cell name is empty!";
					return std::nullopt;
				}
				if (faultItem.get_child("fault.cell_group").get_value<std::string>().empty())
				{
					LOG(ERROR) << "Cell group is empty!";
					return std::nullopt;
				}

				size_t nodeId;
				std::string nodeName;
				std::string signalName;
				Circuit::PortType portType;
				size_t portNumber;
				std::string portName;
				std::vector<LogicConstraint> portConstraints;

				Io::Udfm::UdfmPortMap faultConditionMap;
				Io::Udfm::UdfmPortMap faultEffectMap;
				std::vector<Fault::CellAwareAlternative> faultAlternatives;

				ptree& alternativesItem = faultItem.get_child("fault.alternatives");
				size_t alternativesIndex = 0;
				for (auto altIt = alternativesItem.ordered_begin(); altIt != alternativesItem.not_found(); ++altIt, ++alternativesIndex)
				{
					ptree& alternativeItem = altIt->second;

					if (alternativeItem.get_child("index").get_value<size_t>() != alternativesIndex)
					{
						LOG(ERROR) << "Alternative has invalid index!";
						return std::nullopt;
					}

					faultConditionMap.clear();
					faultAlternatives.push_back( { .conditions = { }, .effects = { } } );
					ptree& faultConditionsItem = alternativeItem.get_child("fault_conditions");
					for (auto fConIt = faultConditionsItem.ordered_begin(); fConIt != faultConditionsItem.not_found(); ++fConIt)
					{
						ptree& faultConditionItem = fConIt->second;

						nodeId = faultConditionItem.get_child("node_index").get_value<size_t>();
						nodeName = faultConditionItem.get_child("node_name").get_value<std::string>();
						signalName = faultConditionItem.get_child("signal_name").get_value<std::string>();
						portType = ConvertStringToPortType(faultConditionItem.get_child("port_type").get_value<std::string>());
						portNumber = faultConditionItem.get_child("port_number").get_value<size_t>();
						portName = faultConditionItem.get_child("port_name").get_value<std::string>();
						portConstraints = GetLogicConstraintsForString(faultConditionItem.get_child("port_value").get_value<std::string>());

						const auto node = mappedCircuit.GetNode(nodeId);
						if (nodeId >= mappedCircuit.GetNumberOfNodes())
						{
							LOG(ERROR) << "Invalid node which does not exist was given for fault";
							return std::nullopt;
						}

						if (nodeName != node->GetName())
						{
							LOG(ERROR) << "Node has invalid name \"" << nodeName << "\", expected \"" << node->GetName() << "\"";
							return std::nullopt;
						}
						if (signalName != node->GetOutputSignalName())
						{
							LOG(ERROR) << "Fault node has invalid signal name \"" << signalName << "\", expected \"" << node->GetOutputSignalName() << "\"";
							return std::nullopt;
						}

						if (portType == Circuit::PortType::Output)
						{
							if (portNumber != 0u)
							{
								LOG(ERROR) << "Found invalid output port with portNumber " << portNumber;
								return std::nullopt;
							}
						}
						else
						{
							if (portNumber >= node->GetNumberOfInputs())
							{
								LOG(ERROR) << "Found input port with portNumber " << portNumber << "is too big";
							}
						}

						if (portName.size() == 0)
						{
							LOG(ERROR) << "Port has empty name!";
							return std::nullopt;
						}

						if (faultConditionMap.find(portName) != faultConditionMap.end())
						{
							LOG(ERROR) << "Port with name \"" << portName << "\" appeared twice in fault_condition!";
							return std::nullopt;
						}
						faultConditionMap[portName] = portConstraints;
						faultAlternatives[alternativesIndex].conditions.push_back( {
							{ mappedCircuit.GetNode(nodeId), { portType, portNumber } },	// NodeAndPort
							portName,
							portConstraints
						} );
					}

					faultEffectMap.clear();
					ptree& faultEffectsItem = alternativeItem.get_child("fault_effects");
					for (auto fEffIt = faultEffectsItem.ordered_begin(); fEffIt != faultEffectsItem.not_found(); ++fEffIt)
					{
						ptree& faultEffectItem = fEffIt->second;

						nodeId = faultEffectItem.get_child("node_index").get_value<size_t>();
						nodeName = faultEffectItem.get_child("node_name").get_value<std::string>();
						signalName = faultEffectItem.get_child("signal_name").get_value<std::string>();
						portType = ConvertStringToPortType(faultEffectItem.get_child("port_type").get_value<std::string>());
						portNumber = faultEffectItem.get_child("port_number").get_value<size_t>();
						portName = faultEffectItem.get_child("port_name").get_value<std::string>();
						portConstraints = GetLogicConstraintsForString(faultEffectItem.get_child("faulty_port_value").get_value<std::string>());

						const auto node = mappedCircuit.GetNode(nodeId);
						if (nodeId >= mappedCircuit.GetNumberOfNodes())
						{
							LOG(ERROR) << "Invalid node which does not exist was given for fault";
							return std::nullopt;
						}

						if (nodeName != node->GetName())
						{
							LOG(ERROR) << "Node has invalid name \"" << nodeName << "\", expected \"" << node->GetName() << "\"";
							return std::nullopt;
						}
						if (signalName != node->GetOutputSignalName())
						{
							LOG(ERROR) << "Fault node has invalid signal name \"" << signalName << "\", expected \"" << node->GetOutputSignalName() << "\"";
							return std::nullopt;
						}

						if (portType == Circuit::PortType::Output)
						{
							if (portNumber != 0u)
							{
								LOG(ERROR) << "Found invalid output port with portNumber " << portNumber;
								return std::nullopt;
							}
						}
						else
						{
							if (portNumber >= node->GetNumberOfInputs())
							{
								LOG(ERROR) << "Found input port with portNumber " << portNumber << "is too big";
							}
						}

						if (portName.size() == 0)
						{
							LOG(ERROR) << "Port has empty name!";
							return std::nullopt;
						}

						if (faultEffectMap.find(portName) != faultEffectMap.end())
						{
							LOG(ERROR) << "Port with name \"" << portName << "\" appeared twice in fault effect!";
							return std::nullopt;
						}

						faultEffectMap[portName] = portConstraints;
						faultAlternatives[alternativesIndex].effects.push_back( {
							{ mappedCircuit.GetNode(nodeId), { portType, portNumber } },	// NodeAndPort
							portName,
							portConstraints
						} );
					}

					std::string cellName = faultItem.get_child("fault.cell_name").get_value<std::string>();
					std::string cellGroup = faultItem.get_child("fault.cell_group").get_value<std::string>();
					std::string faultName = faultItem.get_child("fault.fault_name").get_value<std::string>();
					std::string faultCategory = faultItem.get_child("fault.fault_category").get_value<std::string>();

					if (faultName.size() == 0)
					{
						LOG(ERROR) << "Got empty faultName for cellName " << cellName;
						return std::nullopt;
					}
					if (!udfm.HasCell(cellName))
					{
						LOG(ERROR) << "Was not able to find cellName \"" << cellName << "\" in the UDFM:\n" << to_string(udfm);
						return std::nullopt;
					}
					if (!udfm.HasFault(cellName, faultName))
					{
						LOG(ERROR) << "Was not able to find faultName \"" << faultName << "\" for cellName \"" << cellName
									<< "\" in the UDFM; faults are:\n" << to_string(*udfm.GetCell(cellName));
						return std::nullopt;
					}

					const auto& udfmFault = udfm.GetFault(cellName, faultName);
					if (udfmFault->GetFaultCategory() != faultCategory)
					{
						LOG(ERROR) << "Invalid fault category \"" << faultCategory << "\" in the UDFM:\n" << to_string(udfm);
						return std::nullopt;
					}

					for (size_t testAlternativeIndex = 0u; testAlternativeIndex < udfmFault->GetAlternatives().size(); ++testAlternativeIndex)
					{
						if (udfmFault->GetAlternativeConditions(testAlternativeIndex) == faultConditionMap)
						{
							break;
						}
						if (testAlternativeIndex == udfmFault->GetAlternatives().size() - 1)
						{
							LOG(ERROR) << "Was not able to find conditions: \n" << Io::Udfm::to_string(faultConditionMap)
										<< "\nin any UDFM test alternative of the UserDefinedFault:\n" << to_string(*udfmFault);
							return std::nullopt;
						}
					}

					// Some effects may be missing in the fault list (because of unconnected ports, which are removed)
					// -> Check, if the fault effects in the fault list are a subset of the effects in the UDFM
					for (size_t testAlternativeIndex = 0u; testAlternativeIndex < udfmFault->GetAlternatives().size(); ++testAlternativeIndex)
					{
						if (udfmFault->GetAlternativeEffects(testAlternativeIndex) == faultEffectMap)
						{
							break;
						}
						if (testAlternativeIndex == udfmFault->GetAlternatives().size() - 1)
						{
							LOG(ERROR) << "Was not able to find effects: \n" << Io::Udfm::to_string(faultEffectMap)
										<< "\nin any UDFM test alternative of the UserDefinedFault:\n" << to_string(*udfmFault);
							return std::nullopt;
						}
					}
				}

				auto group = circuit.GetMetaData().GetGroup(faultItem.get_child("fault.cell_group").get_value<std::string>());
				if (group == nullptr)
				{
					LOG(ERROR) << "Could not find cell " << faultItem.get_child("fault.cell_group").get_value<std::string>() << " in circuit!";
					return std::nullopt;
				}

				fault = std::make_shared<Fault::CellAwareFault>(
					udfm.GetFault(
						faultItem.get_child("fault.cell_name").get_value<std::string>(),
						faultItem.get_child("fault.fault_name").get_value<std::string>()
					),
					group,
					faultAlternatives
				);
			}
			else
			{
				static_assert(std::is_same_v<FaultT, Fault::SingleStuckAtFault>, "Unsupported fault type given");
			}

			// Implement other metaData types below
			if constexpr (std::is_same_v<MetaDataT, Fault::SingleStuckAtFaultMetaData>
				|| std::is_same_v<MetaDataT, Fault::SingleTransitionDelayFaultMetaData>)
			{
				metaData = std::make_shared<MetaDataT>();
				metaData->faultStatus = ConvertStringToFaultStatus(faultItem.get_child("status").get_value<std::string>());
				metaData->targetedFaultStatus = ConvertStringToTargetedFaultStatus(faultItem.get_child("status-targeted").get_value<std::string>());

				if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
				{
					metaData->detectingPatternId = faultItem.get_child("pattern.pattern_index").get_value<size_t>();
					if (faultItem.find("pattern.detected_by.node_index") != faultItem.not_found())
					{
						const auto nodeId = faultItem.get_child("pattern.detected_by.node_index").get_value<size_t>();
						const auto portType = ConvertStringToPortType(faultItem.get_child("pattern.detected_by.port_type").get_value<std::string>());
						const auto portNumber = faultItem.get_child("pattern.detected_by.port_number").get_value<size_t>();

						if (nodeId >= mappedCircuit.GetNumberOfNodes())
						{
							LOG(ERROR) << "Detecting node index for fault is not valid";
							return std::nullopt;
						}
						if (mappedCircuit.GetNode(nodeId)->GetName() != faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>())
						{
							LOG(ERROR) << "Fault node has invalid detecting node name \"" << faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>()
								<< "\", expected \"" << mappedCircuit.GetNode(nodeId)->GetName() << "\"";
							return std::nullopt;
						}

						metaData->detectingNode = { mappedCircuit.GetNode(nodeId), { portType, portNumber } };
						metaData->detectingTimeframe = faultItem.get_child("pattern.detected_by.timeframe").get_value<size_t>();
						metaData->detectingOutputGood = static_cast<Logic>(faultItem.get_child("pattern.detected_by.good_value").get_value<char>());
						metaData->detectingOutputBad = static_cast<Logic>(faultItem.get_child("pattern.detected_by.bad_value").get_value<char>());

						if (!CheckOutputLogic(metaData->detectingOutputGood, metaData->detectingOutputBad))
						{
							LOG(ERROR) << "detectingOutputGood and detectingOutputBad show no 01 difference";
							return std::nullopt;
						}
					}
				}
			}
			else if constexpr (std::is_same_v<MetaDataT, Fault::CellAwareMetaData>)
			{
				metaData = std::make_shared<Fault::CellAwareMetaData>();
				metaData->faultStatus = ConvertStringToFaultStatus(faultItem.get_child("status").get_value<std::string>());
				metaData->targetedFaultStatus = ConvertStringToTargetedFaultStatus(faultItem.get_child("status-targeted").get_value<std::string>());

				if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
				{
					metaData->detectingPatternId = faultItem.get_child("pattern.pattern_index").get_value<size_t>();
					if (faultItem.find("pattern.detected_by.node_index") != faultItem.not_found())
					{
						const auto nodeId = faultItem.get_child("pattern.detected_by.node_index").get_value<size_t>();
						const auto portType = ConvertStringToPortType(faultItem.get_child("pattern.detected_by.port_type").get_value<std::string>());
						const auto portNumber = faultItem.get_child("pattern.detected_by.port_number").get_value<size_t>();

						if (nodeId >= mappedCircuit.GetNumberOfNodes())
						{
							LOG(ERROR) << "Detecting node index for fault is not valid";
							return std::nullopt;
						}
						if (mappedCircuit.GetNode(nodeId)->GetName() != faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>())
						{
							LOG(ERROR) << "Fault node has invalid detecting node name \"" << faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>()
								<< "\", expected \"" << mappedCircuit.GetNode(nodeId)->GetName() << "\"";
							return std::nullopt;
						}

						metaData->detectingNode = { mappedCircuit.GetNode(nodeId), { portType, portNumber } };
						metaData->detectingTimeframe = faultItem.get_child("pattern.detected_by.timeframe").get_value<size_t>();
						metaData->detectingOutputGood = static_cast<Logic>(faultItem.get_child("pattern.detected_by.good_value").get_value<char>());
						metaData->detectingOutputBad = static_cast<Logic>(faultItem.get_child("pattern.detected_by.bad_value").get_value<char>());

						if (!CheckOutputLogic(metaData->detectingOutputGood, metaData->detectingOutputBad))
						{
							LOG(ERROR) << "detectingOutputGood and detectingOutputBad show no 01 difference";
							return std::nullopt;
						}
					}
				}
			}
			else
			{
				static_assert(std::is_same_v<MetaDataT, Fault::SingleStuckAtFaultMetaData>, "Unsupported fault metaData type given");
			}

			faultList.emplace_back(std::move(fault), std::move(metaData));
		}

		return std::make_optional<FaultListExchangeFormat<FaultList>>(circuit, faultList);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
	}
	catch (boost::property_tree::ptree_bad_path& exception)
	{
		LOG(ERROR) << "A key does not exist in json file: " << exception.what();
	}
	catch (boost::property_tree::ptree_bad_data& exception)
	{
		LOG(ERROR) << "Invalid data value: " << exception.what();
	}

	return std::nullopt;
}

template std::optional<FaultListExchangeFormat<Fault::SingleStuckAtFaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit);
template bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<Fault::SingleStuckAtFaultList>& faultList);

template std::optional<FaultListExchangeFormat<Fault::SingleTransitionDelayFaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit);
template bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<Fault::SingleTransitionDelayFaultList>& faultList);

template std::optional<FaultListExchangeFormat<Fault::CellAwareFaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit, const Io::Udfm::UdfmModel& udfm);
template bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<Fault::CellAwareFaultList>& faultList);

};
};
