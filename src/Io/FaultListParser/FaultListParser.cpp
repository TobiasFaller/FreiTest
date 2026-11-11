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

static Fault::TargetedFaultStatus ConvertStringToTargetedFaultStatus(const std::string& status)
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
		throw boost::property_tree::ptree_bad_data("Unknown port type", portType);
	}
}

static Circuit::PortType ConvertStringToPortType(const std::string& str)
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

	throw boost::property_tree::ptree_bad_data("Invalid port type", str);
}

static bool CheckOutputLogic(Logic goodOutput, Logic badOutput)
{
	return (goodOutput == Logic::LOGIC_ZERO && badOutput == Logic::LOGIC_ONE)
		|| (badOutput == Logic::LOGIC_ZERO && goodOutput == Logic::LOGIC_ONE);
}

template<typename FaultList, typename... Params>
bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<FaultList>& faultList, const Params&... params)
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
		faultItem.put("type", GetFaultTypeString<FaultT>());

		// Implement other fault types below
		if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>
			|| std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
		{
			faultItem.put("subtype", ConvertFaultSubTypeToString(fault->GetType()));
			faultItem.put("node_index", fault->GetNode()->GetNodeId());
			faultItem.put("node_name", fault->GetNode()->GetName());
			faultItem.put("signal_name", mappedCircuit.GetDriverForPort(*fault)->GetOutputSignalName());
			faultItem.put("friendly_name", circuitMetaData.GetFriendlyName(*fault));
			faultItem.put("port_type", ConvertPortTypeToString(fault->GetPort().portType));
			faultItem.put("port_number", fault->GetPort().portNumber);
		}
		else if constexpr (std::is_same_v<FaultT, Fault::CellAwareFault>)
		{
			faultItem.put("fault_name", fault->GetName());
			faultItem.put("cell_name", fault->GetCellName());
			faultItem.put("cell_type", fault->GetCellType());
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
			|| std::is_same_v<MetaDataT, Fault::SingleTransitionDelayFaultMetaData>
			|| std::is_same_v<MetaDataT, Fault::CellAwareMetaData>)
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
std::optional<FaultListExchangeFormat<FaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit, const Params&... params)
{
	using ptree = boost::property_tree::ptree;
	using FaultT = typename FaultList::fault_type;
	using MetaDataT = typename FaultList::metadata_type;

	const auto& mappedCircuit = circuit.GetMappedCircuit();
	const auto parameters { std::make_tuple(std::forward<const Params&>(params)...) };

	try
	{
		ptree root;
		boost::property_tree::read_json(input, root);
		if (!ValidateCircuitGuard(root.get_child("circuit"), circuit))
		{
			return std::nullopt;
		}

		ptree& listMetaData = root.get_child("meta_data");
		auto const faultListType = listMetaData.get_child("fault_list_type").get_value<std::string>();
		if (faultListType != GetFaultTypeString<FaultT>())
		{
			VLOG(1) << "Invalid parser used for file. Expected "
				<< GetFaultTypeString<FaultT>() << " but got " << faultListType;
			throw FaultTypeException(GetFaultTypeString<FaultT>(), faultListType);
		}

		Fault::ConcurrentFaultList<FaultT, MetaDataT> faultList;
		ptree& treeFaults = root.get_child("faults");
		for (auto it = treeFaults.ordered_begin(); it != treeFaults.not_found(); ++it)
		{
			ptree& faultItem = it->second;

			DVLOG(5) << "Importing fault with index " << faultList.size();
			auto const index = faultItem.get_child("index").get_value<size_t>();
			auto const faultType = faultItem.get_child("fault.type").get_value<std::string>();
			if (index != faultList.size())
			{
				LOG(ERROR) << "Fault has invalid index of " << index;
				return std::nullopt;
			}
			if (faultType != GetFaultTypeString<FaultT>())
			{
				LOG(ERROR) << "Invalid parser used for file. Expected "
					<< GetFaultTypeString<FaultT>() << " but got " << faultType;
				return std::nullopt;
			}

			std::shared_ptr<FaultT> fault;
			std::shared_ptr<MetaDataT> metaData;

			// Implement other fault types below
			if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>
				|| std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
			{
				auto const nodeId = faultItem.get_child("fault.node_index").get_value<size_t>();
				auto const portNumber = faultItem.get_child("fault.port_number").get_value<size_t>();
				auto const nodeName = faultItem.get_child("fault.node_name").get_value<std::string>();
				auto const signalName = faultItem.get_child("fault.signal_name").get_value<std::string>();
				auto const portTypeRaw = faultItem.get_child("fault.port_type").get_value<std::string>();
				if (nodeName.empty())
				{
					LOG(ERROR) << "Node name is empty!";
					return std::nullopt;
				}
				if (signalName.empty())
				{
					LOG(ERROR) << "Signal name is empty!";
					return std::nullopt;
				}
				if (portTypeRaw.empty())
				{
					LOG(ERROR) << "Port type is empty!";
					return std::nullopt;
				}

				if (nodeId >= mappedCircuit.GetNumberOfNodes())
				{
					LOG(ERROR) << "Invalid node which does not exist was given for fault";
					return std::nullopt;
				}

				auto const* node = mappedCircuit.GetNode(nodeId);
				auto const portType = ConvertStringToPortType(portTypeRaw);
				if (portType == Circuit::PortType::Input)
				{
					if (portNumber >= node->GetNumberOfInputs())
					{
						LOG(ERROR) << "Invalid input port " << portNumber << " was selected";
						return std::nullopt;
					}
				}
				else if (portType == Circuit::PortType::Output)
				{
					if (portNumber != 0u)
					{
						LOG(ERROR) << "Invalid output port " << portNumber << " was selected";
						return std::nullopt;
					}
				}
				else
				{
					LOG(ERROR) << "Invalid port type " << to_string(portType) << " was selected";
					return std::nullopt;
				}

				if (node->GetName() != nodeName)
				{
					LOG(ERROR) << "Fault node has invalid name \"" << nodeName
						<< "\", expected \"" << node->GetName() << "\"";
					return std::nullopt;
				}
				if (node->GetOutputSignalName() != signalName)
				{
					LOG(ERROR) << "Fault node has invalid signal name \"" << signalName
						<< "\", expected \"" << node->GetOutputSignalName() << "\"";
					return std::nullopt;
				}

				if constexpr (std::is_same_v<FaultT, Fault::SingleStuckAtFault>)
				{
					fault = std::make_shared<FaultT>(
						Circuit::MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { portType, portNumber } },
						ConvertStringToFaultSubType<Fault::StuckAtFaultType>(
							faultItem.get_child("fault.subtype").get_value<std::string>())
					);
				}
				else if constexpr (std::is_same_v<FaultT, Fault::SingleTransitionDelayFault>)
				{
					fault = std::make_shared<FaultT>(
						Circuit::MappedCircuit::NodeAndPort { mappedCircuit.GetNode(nodeId), { portType, portNumber } },
						ConvertStringToFaultSubType<Fault::TransitionDelayFaultType>(
							faultItem.get_child("fault.subtype").get_value<std::string>())
					);
				}
			}
			else if constexpr(std::is_same_v<FaultT, Fault::CellAwareFault>)
			{
				auto [specification] = parameters;

				auto const faultName = faultItem.get_child("fault.fault_name").get_value<std::string>();
				auto const cellName = faultItem.get_child("fault.cell_name").get_value<std::string>();
				auto const cellType = faultItem.get_child("fault.cell_type").get_value<std::string>();
				if (faultName.empty())
				{
					LOG(ERROR) << "Fault name is empty!";
					return std::nullopt;
				}
				if (cellName.empty())
				{
					LOG(ERROR) << "Cell name is empty!";
					return std::nullopt;
				}
				if (cellType.empty())
				{
					LOG(ERROR) << "Cell type is empty!";
					return std::nullopt;
				}

				if (!specification.HasCell(cellType))
				{
					LOG(ERROR) << "Was not able to find cell type \"" << cellType << "\" in the specification:\n"
						<< to_string(specification);
					return std::nullopt;
				}
				if (!specification.HasFault(cellType, faultName))
				{
					LOG(ERROR) << "Was not able to find fault \"" << faultName << "\" for cell type \"" << cellType
								<< "\" in the specification; faults are:\n"
								<< to_string(specification.GetCell(cellType));
					return std::nullopt;
				}

				auto group = circuit.GetMetaData().GetGroup(cellName);
				if (group == nullptr)
				{
					LOG(ERROR) << "Could not find cell " << cellName << " in circuit!";
					return std::nullopt;
				}

				auto faultSpecification { specification.GetFault(cellType, faultName) };
				auto cellAwareFault { Fault::CreateCellAwareFaultForCell(faultSpecification, group) };
				if (!cellAwareFault)
				{
					LOG(ERROR) << "Could not construct cell-aware fault " << faultName << " for cell " << cellName
						<< "(" << cellType << ") in circuit!";
					return std::nullopt;
				}
				fault = std::make_shared<Fault::CellAwareFault>(*cellAwareFault);
			}
			else
			{
				static_assert(std::is_same_v<FaultT, Fault::SingleStuckAtFault>, "Unsupported fault type given");
			}

			// Implement other metaData types below
			if constexpr (std::is_same_v<MetaDataT, Fault::SingleStuckAtFaultMetaData>
				|| std::is_same_v<MetaDataT, Fault::SingleTransitionDelayFaultMetaData>
				|| std::is_same_v<MetaDataT, Fault::CellAwareMetaData>)
			{

				auto const faultStatus = faultItem.get_child("status").get_value<std::string>();
				auto const targetedStatus = faultItem.get_child("status-targeted").get_value<std::string>();
				if (faultStatus.empty())
				{
					LOG(ERROR) << "Fault status is empty!";
					return std::nullopt;
				}
				if (targetedStatus.empty())
				{
					LOG(ERROR) << "Targeted fault status is empty!";
					return std::nullopt;
				}

				metaData = std::make_shared<MetaDataT>();
				metaData->faultStatus = ConvertStringToFaultStatus(faultItem.get_child("status").get_value<std::string>());
				metaData->targetedFaultStatus = ConvertStringToTargetedFaultStatus(
					faultItem.get_child("status-targeted").get_value<std::string>());

				if (metaData->faultStatus == Fault::FaultStatus::FAULT_STATUS_DETECTED)
				{
					metaData->detectingPatternId = faultItem.get_child("pattern.pattern_index").get_value<size_t>();
					if (faultItem.find("pattern.detected_by.node_index") != faultItem.not_found())
					{
						const auto nodeId = faultItem.get_child("pattern.detected_by.node_index").get_value<size_t>();
						const auto portType = ConvertStringToPortType(
							faultItem.get_child("pattern.detected_by.port_type").get_value<std::string>());
						const auto portNumber = faultItem.get_child("pattern.detected_by.port_number").get_value<size_t>();

						if (nodeId >= mappedCircuit.GetNumberOfNodes())
						{
							LOG(ERROR) << "Detecting node index for fault is not valid";
							return std::nullopt;
						}
						if (mappedCircuit.GetNode(nodeId)->GetName()
							!= faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>())
						{
							LOG(ERROR) << "Fault node has invalid detecting node name \""
								<< faultItem.get_child("pattern.detected_by.node_name").get_value<std::string>()
								<< "\", expected \"" << mappedCircuit.GetNode(nodeId)->GetName() << "\"";
							return std::nullopt;
						}

						metaData->detectingNode = { mappedCircuit.GetNode(nodeId), { portType, portNumber } };
						metaData->detectingTimeframe = faultItem.get_child("pattern.detected_by.timeframe").get_value<size_t>();
						metaData->detectingOutputGood = static_cast<Logic>(
							faultItem.get_child("pattern.detected_by.good_value").get_value<char>());
						metaData->detectingOutputBad = static_cast<Logic>(
							faultItem.get_child("pattern.detected_by.bad_value").get_value<char>());

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

template std::optional<FaultListExchangeFormat<Fault::CellAwareFaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit, const Fault::CellAwareModelSpecification& specification);
template bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<Fault::CellAwareFaultList>& faultList);

};
};
