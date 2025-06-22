#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Helper/ValueWatcher.hpp"

namespace FreiTest
{
namespace Io
{

class ValueWatcherImporter
{
public:
	ValueWatcherImporter(void);
	virtual ~ValueWatcherImporter(void);

	bool LoadWatchers(std::istream& stream, std::string watcherPrefix = "", std::string signalPrefix = "");
	void ClearWatchers(void);

	std::vector<std::shared_ptr<Helper::ValueWatcher::Watcher>> ApplyWatchers(Helper::ValueWatcher& watcher, const Circuit::CircuitEnvironment& circuit, const std::vector<std::string>& tags, const std::string& target) const;

private:
	enum class Endianess { Little, Big };

	struct WatcherEntry
	{
		std::string name;
		Endianess endianess;
		std::vector<std::string> signals;
		std::vector<bool> setBits;
		std::vector<bool> clearBits;
		std::vector<bool> invertBits;
		std::vector<bool> maskBits;
		std::vector<std::string> tags;
		std::vector<std::string> targets;
	};

	std::vector<WatcherEntry> watcherEntries;

};

};
};
