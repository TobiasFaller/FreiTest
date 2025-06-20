#include "ApplicationStatistics.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

#include "Io/JsoncParser/JsonCParser.hpp"

namespace FreiTest
{
namespace Basic
{

using ptree = boost::property_tree::ptree;

static void PrintHumanReadableToStreamImpl(std::ostream& out, ptree& tree, size_t level);

ApplicationStatistics::ApplicationStatistics(void):
    statistics(std::make_unique<ptree>())
{
}
ApplicationStatistics::ApplicationStatistics(ApplicationStatistics&& other) = default;
ApplicationStatistics::~ApplicationStatistics(void) = default;

template<typename T> void ApplicationStatistics::Add(const std::string& key, T value, std::string unit, std::string description)
{
    ptree entry;

    entry.put("Value", value);
    entry.put("Unit", unit);
    entry.put("Description", description);

    statistics->add_child(key, entry);
}

template<>
void ApplicationStatistics::Add<const char*>(const std::string& key, const char* value, std::string unit, std::string description)
{
	Add(key, std::string(value), unit, description);
}

void ApplicationStatistics::Add(const std::string& key, ptree value)
{
    statistics->add_child(key, value);
}

void ApplicationStatistics::PrintHumanReadableToStream(std::ostream& out)
{
    PrintHumanReadableToStreamImpl(out, *statistics, 0u);
}

void ApplicationStatistics::PrintJsonToStream(std::ostream& out)
{
	boost::property_tree::jsonc_parser::write_jsonc(out, *statistics, true);
}

void ApplicationStatistics::Merge(const ApplicationStatistics& other)
{
	if (!other.statistics) return;
	for (auto [key, value] : (*other.statistics))
	{
		statistics->add_child(key, value);
	}
}

void ApplicationStatistics::Merge(const std::string& key, const ApplicationStatistics& other)
{
	if (!other.statistics) return;
	if (auto it = statistics->find(key); it != statistics->not_found())
	{
		for (auto& otherIt : *(other.statistics))
		{
			it->second.add_child(otherIt.first, otherIt.second);
		}
	}
	else
	{
		statistics->add_child(key, *(other.statistics));
	}
}

static void PrintHumanReadableToStreamImpl(std::ostream& out, ptree& tree, size_t level)
{
    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
		auto& [key, value] = *it;

        // Statistic data leaf
        if (auto valueIt = value.find("Value"), unitIt = value.find("Unit"), descIt = value.find("Description");
            valueIt != value.not_found() && unitIt != value.not_found() && descIt != value.not_found())
        {
            out << std::string(4u * level, ' ') << it->first << ": " << descIt->second.data() << std::endl;
            out << std::string(4u * (level + 1u), ' ') << valueIt->second.data() << ' ' << unitIt->second.data() << std::endl;
            out << std::endl;
            continue;
        }

		// Setting data leaf
		if (auto settingIt = value.find("setting"), valueIt = value.find("value");
			settingIt != value.not_found() && valueIt != value.not_found())
		{
			out << std::string(4u * level, ' ') << settingIt->second.data() << ": " << valueIt->second.data() << std::endl;
            continue;
		}

        out << std::string(4u * level, ' ') << key << ":" << std::endl;
        PrintHumanReadableToStreamImpl(out, value, level + 1u);
    }
}

template void ApplicationStatistics::Add<uint64_t>(const std::string& key, uint64_t value, std::string unit, std::string description);
template void ApplicationStatistics::Add<int64_t>(const std::string& key, int64_t value, std::string unit, std::string description);
template void ApplicationStatistics::Add<uint32_t>(const std::string& key, uint32_t value, std::string unit, std::string description);
template void ApplicationStatistics::Add<int32_t>(const std::string& key, int32_t value, std::string unit, std::string description);
template void ApplicationStatistics::Add<double>(const std::string& key, double value, std::string unit, std::string description);
template void ApplicationStatistics::Add<float>(const std::string& key, float value, std::string unit, std::string description);
template void ApplicationStatistics::Add<std::string>(const std::string& key, std::string value, std::string unit, std::string description);

};
};
