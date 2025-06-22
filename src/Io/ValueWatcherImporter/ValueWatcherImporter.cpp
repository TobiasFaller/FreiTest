#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"

#include <regex>
#include <iostream>

#include "Io/JsoncParser/JsonCParser.hpp"
#include "Circuit/DriverFinder.hpp"

namespace FreiTest
{
namespace Io
{

ValueWatcherImporter::ValueWatcherImporter(void) = default;
ValueWatcherImporter::~ValueWatcherImporter(void) = default;

bool ValueWatcherImporter::LoadWatchers(std::istream& stream, std::string watcherPrefix, std::string signalPrefix)
{
	boost::property_tree::ptree root;
	try
	{
		boost::property_tree::read_jsonc(stream, root);

		for (auto& [_key, child] : root)
		{
			auto nameIt = child.find("name");
			auto endianessIt = child.find("endianess");
			auto signalsIt = child.find("signals");
			auto setBitsIt = child.find("set");
			auto clearBitsIt = child.find("clear");
			auto invertBitsIt = child.find("invert");
			auto maskBitsIt = child.find("mask");
			auto tagsIt = child.find("tags");
			auto targetsIt = child.find("targets");

			if (nameIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"name\" in watcher list";
				return false;
			}
			if (endianessIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"endianess\" in watcher list";
				return false;
			}
			if (signalsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"signals\" in watcher list";
				return false;
			}
			if (tagsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"tags\" in watcher list";
				return false;
			}
			if (targetsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"targets\" in watcher list";
				return false;
			}

			std::string name = nameIt->second.get_value<std::string>();
			std::string endianessValue = endianessIt->second.get_value<std::string>();
			Endianess endianess;
			if (endianessValue == "little")
			{
				endianess = Endianess::Little;
			}
			else if (endianessValue == "big")
			{
				endianess = Endianess::Big;
			}
			else
			{
				LOG(ERROR) << "Found entry with invalid endianess \"" << endianessValue << "\".";
				return false;
			}

			WatcherEntry entry { watcherPrefix + name, endianess, {}, {}, {}, {}, {}, {}, {} };
			for (auto& [_key, signal] : signalsIt->second)
			{
				auto value { signal.get_value<std::string>() };
				entry.signals.push_back((value == "0" || value == "1" || value == "X")
					? value : (signalPrefix + value));
			}

			if (setBitsIt != child.not_found())
			{
				std::string setBitsValue = setBitsIt->second.get_value<std::string>();
				for (const char value : setBitsValue)
				{
					ASSERT(value == '0' || value == '1' || value == ' ') << "Found invalid set mask character: \"" << value << "\"";
					if (value == '0' || value == '1')
					{
						entry.setBits.push_back(value == '1');
					}
				}
			}

			if (clearBitsIt != child.not_found())
			{
				std::string clearBitsValue = clearBitsIt->second.get_value<std::string>();
				for (const char value : clearBitsValue)
				{
					ASSERT(value == '0' || value == '1' || value == ' ') << "Found invalid clear mask character: \"" << value << "\"";
					if (value == '0' || value == '1')
					{
						entry.clearBits.push_back(value == '1');
					}
				}
			}

			if (invertBitsIt != child.not_found())
			{
				std::string invertBitsValue = invertBitsIt->second.get_value<std::string>();
				for (const char value : invertBitsValue)
				{
					ASSERT(value == '0' || value == '1' || value == ' ') << "Found invalid invert mask character: \"" << value << "\"";
					if (value == '0' || value == '1')
					{
						entry.invertBits.push_back(value == '1');
					}
				}
			}

			if (maskBitsIt != child.not_found())
			{
				std::string maskBitsValue = maskBitsIt->second.get_value<std::string>();
				for (const char value : maskBitsValue)
				{
					ASSERT(value == '0' || value == '1' || value == ' ') << "Found invalid mask character: \"" << value << "\"";
					if (value == '0' || value == '1')
					{
						entry.maskBits.push_back(value == '1');
					}
				}
			}

			for (auto& [_key, tag] : tagsIt->second)
			{
				entry.tags.push_back(tag.get_value<std::string>());
			}
			for (auto& [_key, target] : targetsIt->second)
			{
				entry.targets.push_back(target.get_value<std::string>());
			}

			watcherEntries.emplace_back(entry);
		}

		return true;
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
		return false;
	}
	catch (boost::property_tree::ptree_bad_path& exception)
	{
		LOG(ERROR) << "A key does not exist in json file: " << exception.what();
		return false;
	}
	catch (boost::property_tree::ptree_bad_data& exception)
	{
		LOG(ERROR) << "Invalid data value: " << exception.what();
		return false;
	}

	Logging::Panic("Unreachable code has been reached");
}

void ValueWatcherImporter::ClearWatchers(void)
{
	watcherEntries.clear();
}

std::vector<std::shared_ptr<Helper::ValueWatcher::Watcher>> ValueWatcherImporter::ApplyWatchers(Helper::ValueWatcher& valueWatcher, const Circuit::CircuitEnvironment& circuit, const std::vector<std::string>& tags, const std::string& target) const
{
	std::vector<std::shared_ptr<Helper::ValueWatcher::Watcher>> watchers;

	for (auto& watcherEntry : watcherEntries)
	{
		// Filter entries that do not apply to the circuit or the target
		if (std::find(watcherEntry.tags.begin(), watcherEntry.tags.end(), "*") == watcherEntry.tags.end())
		{
			for (const auto& tag : tags)
			{
				if (std::find(watcherEntry.tags.begin(), watcherEntry.tags.end(), tag) != watcherEntry.tags.end())
				{
					goto valid_tag_found;
				}
			}

			// No tag did match -> skip
			continue;
		}

	valid_tag_found:
		if (std::find(watcherEntry.targets.begin(), watcherEntry.targets.end(), "*") == watcherEntry.targets.end())
		{
			if (std::find(watcherEntry.targets.begin(), watcherEntry.targets.end(), target) == watcherEntry.targets.end())
			{
				continue;
			}
		}

		std::vector<size_t> toClear;
		std::vector<size_t> toSet;

		std::vector<const Circuit::MappedNode*> signals;
		for (auto& reference : watcherEntry.signals)
		{
			if (reference == "0")
			{
				toClear.push_back(signals.size());
				signals.push_back(nullptr);
			}
			else if (reference == "1")
			{
				toSet.push_back(signals.size());
				signals.push_back(nullptr);
			}
			else if (reference == "X")
			{
				signals.push_back(nullptr);
			}
			else
			{
				std::regex range_regex { "\\{(\\d+):(\\d+)\\}" };
				std::smatch match;
				if (std::regex_search(reference, match, range_regex))
				{
					ssize_t start { std::stol(match[1]) };
					ssize_t end { std::stol(match[2]) };
					for (ssize_t i { start }; (start <= end) ? (i <= end) : (i >= end); (start <= end) ? i++ : i--)
					{
						auto name {
							reference.substr(0, match.position(0))
							+ std::to_string(i)
							+ reference.substr(match.position(0) + match.length(0))
						};
						auto drivers { Circuit::GetDrivers(name, circuit) };
						signals.insert(signals.end(), drivers.begin(), drivers.end());
					}
				}
				else
				{
					auto drivers { Circuit::GetDrivers(reference, circuit) };
					signals.insert(signals.end(), drivers.begin(), drivers.end());
				}
			}
		}

		auto& newWatcher = watchers.emplace_back(valueWatcher.AddWatcher(watcherEntry.name, signals));
		for (size_t index = 0u; index < watcherEntry.setBits.size() && index < newWatcher->set.size(); index++)
		{
			newWatcher->set[index] = watcherEntry.setBits[index];
		}
		for (size_t index = 0u; index < watcherEntry.clearBits.size() && index < newWatcher->clear.size(); index++)
		{
			newWatcher->clear[index] = watcherEntry.clearBits[index];
		}
		for (size_t index = 0u; index < watcherEntry.invertBits.size() && index < newWatcher->invert.size(); index++)
		{
			newWatcher->invert[index] = watcherEntry.invertBits[index];
		}
		for (size_t index = 0u; index < watcherEntry.maskBits.size() && index < newWatcher->mask.size(); index++)
		{
			newWatcher->mask[index] = watcherEntry.maskBits[index];
		}

		for (auto& index : toClear)
		{
			if (index < newWatcher->clear.size() && !newWatcher->set[index])
			{
				newWatcher->clear[index] = true;
			}
		}
		for (auto& index : toSet)
		{
			if (index < newWatcher->set.size() && !newWatcher->clear[index])
			{
				newWatcher->set[index] = true;
			}
		}

		// Value watcher always stores signals in big endian order
		if (watcherEntry.endianess == Endianess::Little)
		{
			std::reverse(newWatcher->nodes.begin(), newWatcher->nodes.end());
			std::reverse(newWatcher->set.begin(), newWatcher->set.end());
			std::reverse(newWatcher->clear.begin(), newWatcher->clear.end());
			std::reverse(newWatcher->invert.begin(), newWatcher->invert.end());
			std::reverse(newWatcher->mask.begin(), newWatcher->mask.end());
		}

		VLOG(3) << newWatcher->name << " has " << newWatcher->nodes.size() << " bits";
		for (auto &node : newWatcher->nodes)
		{
			VLOG(6) << "    " << ((node != nullptr) ? node->GetOutputSignalName() : "<empty>");
		}
	}

	return watchers;
}

};
};
