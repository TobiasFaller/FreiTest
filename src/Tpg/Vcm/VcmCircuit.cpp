#include "Tpg/Vcm/VcmCircuit.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>
#include <boost/algorithm/string.hpp>

#include <regex>
#include <string>
#include <type_traits>
#include <vector>

#include "Helper/StringHelper.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/DriverFinder.hpp"

using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

template<typename PortType>
static std::tuple<PortType, std::string, size_t> GetVcmPort(const MappedNode* vcmNode, const CircuitEnvironment& vcmCircuit)
{
	auto const& metaData { vcmCircuit.GetMetaData() };
	auto const& mappedCircuit { vcmCircuit.GetMappedCircuit() };

	std::string allowedSuffixes;
	if constexpr (std::is_same_v<PortType, VcmInput>)
	{
		allowedSuffixes = "[gbd]|gv|bv|param|init|const0|const1|constx|constu|sfree|dfree|bcntr|ucntr|tf";
	}
	else if constexpr (std::is_same_v<PortType, VcmOutput>)
	{
		allowedSuffixes = "[ict]|result";
	}

	auto vcmNodeName = metaData.GetGroup(vcmNode)->GetParent()->GetHierarchyName();
	ASSERT(vcmNodeName.find("/") != std::string::npos) << "VCM port is missing circuit name";
	vcmNodeName = vcmNodeName.substr(vcmNodeName.find("/") + 1u);

	std::smatch match;
	std::regex suffixRegex { "^(.+)_(" + allowedSuffixes + ") \\[(\\d+)\\]$", std::regex::icase };
	ASSERT(std::regex_match(vcmNodeName.cbegin(), vcmNodeName.cend(), match, suffixRegex))
		<< "Vcm port \"" << vcmNodeName << "\" is invalid";

	auto rawName = match.str(1);
	auto rawType = boost::to_lower_copy(match.str(2));
	auto rawIndex = match.str(3);

	ASSERT(rawName != "") << "Empty VCM port name for port " << vcmNodeName;
	ASSERT(rawType != "") << "Empty VCM port type for port " << vcmNodeName;
	ASSERT(rawIndex != "") << "Empty VCM port index for port " << vcmNodeName;

	if constexpr (std::is_same_v<PortType, VcmInput>)
	{
		return {
			VcmInput { vcmNode, vcmNodeName, GetVcmInputType(rawType), std::stoul(rawIndex), mappedCircuit.GetPrimaryInputNumber(vcmNode) },
			rawName, std::stoul(rawIndex)
		};
	}
	else if constexpr (std::is_same_v<PortType, VcmOutput>)
	{
		return {
			VcmOutput { vcmNode, vcmNodeName, GetVcmOutputType(rawType), std::stoul(rawIndex), mappedCircuit.GetPrimaryOutputNumber(vcmNode) },
			rawName, std::stoul(rawIndex)
		};
	}
}

VcmInput GetVcmInput(const CircuitEnvironment& vcmCircuit, const MappedNode* vcmNode, const CircuitEnvironment& targetCircuit)
{
	auto [vcmPort, targetName, targetIndex] = GetVcmPort<VcmInput>(vcmNode, vcmCircuit);
	switch (vcmPort.vcmPortType)
	{
	case VcmInputType::Good:
	case VcmInputType::Bad:
	case VcmInputType::Difference:
	case VcmInputType::GoodValid:
	case VcmInputType::BadValid:
	{
		auto driverName = targetCircuit.GetName() + "/" + [targetName = targetName, targetIndex = targetIndex]() -> std::string {
			if (auto index = targetName.find("{index}"); index != std::string::npos)
			{
				// Index position has been specified -> replace placeholder instead of appending.
				return StringHelper::ReplaceString("{index}", std::to_string(targetIndex), targetName);
			}
			else
			{
				// Index is simply appended to name to make a one to one mapping of VCM index to target index.
				return boost::str(boost::format("%s [%d]") % targetName % targetIndex);
			}
		}();

		auto const drivers = Circuit::GetDrivers(driverName, targetCircuit);
		ASSERT(drivers.size() == 1u) << "Found VCM target " << targetName << " that maps to bus. Some indexing and splitting went wrong.";
		ASSERT(drivers[0u] != nullptr) << "Found VCM target " << targetName << " that has no driver.";
		return vcmPort.ConvertToNode(drivers[0], targetName);
	}

	case VcmInputType::Parameter:
		return vcmPort.ConvertToParameter(targetName, targetIndex);

	case VcmInputType::ConstantZero:
	case VcmInputType::ConstantOne:
	case VcmInputType::ConstantDontCare:
	case VcmInputType::ConstantUnknown:
	case VcmInputType::FreeDynamic:
	case VcmInputType::FreeStatic:
	case VcmInputType::CounterInitial:
	case VcmInputType::CounterUnary:
	case VcmInputType::CounterBinary:
	case VcmInputType::CounterTimeframe:
		return vcmPort;

	default:
		Logging::Panic("Not implemented");
	}
}

VcmOutput GetVcmOutput(const CircuitEnvironment& vcmCircuit, const MappedNode* vcmNode)
{
	auto [vcmPort, targetName, targetIndex] = GetVcmPort<VcmOutput>(vcmNode, vcmCircuit);
	switch (vcmPort.vcmPortType)
	{
	case VcmOutputType::InitialConstraint:
	case VcmOutputType::TransitionConstraint:
	case VcmOutputType::TargetConstraint:
		return vcmPort;

	case VcmOutputType::Result:
		return vcmPort.ConvertToResult(targetName, targetIndex);

	default:
		Logging::Panic("Not implemented");
	}

}

};
};
};
