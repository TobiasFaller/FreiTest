#include "Circuit/DriverFinder.hpp"

#include <regex>

namespace FreiTest
{
namespace Circuit
{

std::vector<const Circuit::MappedNode*> GetDrivers(const std::string& reference, const CircuitEnvironment& circuit, const DriverSearchConfig& config)
{
	auto get_driver_for_connection = [&](const auto* connection) -> const MappedNode* {
		if (connection != nullptr)
		{
			auto drivers = connection->GetMappedSources();
			ASSERT(drivers.size() <= 1u) << "There is more than one driver for the connection";
			return (drivers.size() == 1u) ? drivers[0u].node : nullptr;
		}

		return nullptr;
	};

	std::regex rangeRegex("^(.+) \\[(\\d+):(\\d+)\\]$");
	std::regex indexRegex("^(.+) \\[(\\d+)\\]$");
	std::smatch match;

	if (std::regex_match(reference.cbegin(), reference.cend(), match, rangeRegex))
	{
		const size_t index1 = atoi(match.str(2).c_str());
		const size_t index2 = atoi(match.str(3).c_str());

		const size_t start = std::min(index1, index2);
		const size_t end = std::max(index1, index2);

		if (const auto* port = circuit.GetMetaData().GetPort(match.str(1)); config.searchPorts && port != nullptr)
		{
			auto& size = port->GetSize();
			ASSERT(index1 >= size.GetMin() && index1 <= size.GetMax())
				<< "The port " << match.str(1) << " does not have the index " << index1
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";
			ASSERT(index2 >= size.GetMin() && index2 <= size.GetMax())
				<< "The port " << match.str(1) << " does not have the index " << index2
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";

			std::vector<const Circuit::MappedNode*> result;
			for (size_t index = start; index <= end; ++index)
			{
				auto const& driver = result.emplace_back(get_driver_for_connection(port->GetConnectionForIndex(index)));
				LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
					<< "Index " << index << " of port " << port->GetHierarchyName() << " is not driven!";
			}
			if (index1 > index2) std::reverse(result.begin(), result.end());
			return result;
		}
		else if (const auto* wire = circuit.GetMetaData().GetWire(match.str(1)); config.searchWires && wire != nullptr)
		{
			auto& size = wire->GetSize();
			ASSERT(index1 >= size.GetMin() && index1 <= size.GetMax())
				<< "The wire " << match.str(1) << " does not have the index " << index1
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";
			ASSERT(index2 >= size.GetMin() && index2 <= size.GetMax())
				<< "The wire " << match.str(1) << " does not have the index " << index2
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";

			std::vector<const Circuit::MappedNode*> result;
			for (size_t index = start; index <= end; ++index)
			{
				auto const& driver = result.emplace_back(get_driver_for_connection(wire->GetConnectionForIndex(index)));
				LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
					<< "Index " << index << " of wire " << wire->GetHierarchyName() << " is not driven!";
			}
			if (index1 > index2) std::reverse(result.begin(), result.end());
			return result;
		}
	}
	else if (std::regex_match(reference.cbegin(), reference.cend(), match, indexRegex))
	{
		const size_t index = atoi(match.str(2).c_str());

		if (const auto* port = circuit.GetMetaData().GetPort(match.str(1)); config.searchPorts && port != nullptr)
		{
			auto& size = port->GetSize();
			ASSERT(index >= size.GetMin() && index <= size.GetMax())
				<< "The port " << match.str(1) << " does not have the index " << index
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";
			auto driver = get_driver_for_connection(port->GetConnectionForIndex(index));
			LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
				<< "Index " << index << " of port " << port->GetHierarchyName() << " is not driven!";
			return std::vector<const Circuit::MappedNode*> { driver };
		}
		else if (const auto* wire = circuit.GetMetaData().GetWire(match.str(1)); config.searchWires && wire != nullptr)
		{
			auto& size = wire->GetSize();
			ASSERT(index >= size.GetMin() && index <= size.GetMax())
				<< "The wire " << match.str(1) << " does not have the index " << index
				<< " (only " << size.GetMin() << " to " << size.GetMax() << " allowed)";
			auto driver = get_driver_for_connection(wire->GetConnectionForIndex(index));
			LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
				<< "Index " << index << " of wire " << wire->GetHierarchyName() << " is not driven!";
			return std::vector<const Circuit::MappedNode*> { driver };
		}
	}
	else if (const auto* port = circuit.GetMetaData().GetPort(reference); config.searchPorts && port != nullptr)
	{
		std::vector<const Circuit::MappedNode*> result;
		for (size_t index = port->GetSize().GetMin(); index <= port->GetSize().GetMax(); ++index)
		{
			auto const& driver = result.emplace_back(get_driver_for_connection(port->GetConnectionForIndex(index)));
			LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
				<< "Index " << index << " of port " << port->GetHierarchyName() << " is not driven!";
		}
		if (port->GetSize().GetTop() > port->GetSize().GetBottom()) std::reverse(result.begin(), result.end());
		return result;
	}
	else if (const auto* wire = circuit.GetMetaData().GetWire(reference); config.searchWires && wire != nullptr)
	{
		std::vector<const Circuit::MappedNode*> result;
		for (size_t index = wire->GetSize().GetMin(); index <= wire->GetSize().GetMax(); ++index)
		{
			auto const& driver = result.emplace_back(get_driver_for_connection(wire->GetConnectionForIndex(index)));
			LOG_IF(driver == nullptr && !config.allowUnconnected, FATAL)
				<< "Index " << index << " of wire " << wire->GetHierarchyName() << " is not driven!";
		}
		if (wire->GetSize().GetTop() > wire->GetSize().GetBottom()) std::reverse(result.begin(), result.end());
		return result;
	}

	if (!config.allowNotFound)
	{
		Logging::Panic("The wire or port " + reference + " was not found!");
	}

	return { };
};

};
};
