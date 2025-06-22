#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/StatePatcher.hpp"

namespace FreiTest
{
namespace Io
{

class StatePatcherImporter
{
public:
	StatePatcherImporter(void);
	virtual ~StatePatcherImporter(void);

	bool LoadPatchers(std::istream& stream);
	void ClearPatchers(void);

	std::vector<std::shared_ptr<Helper::StatePatcher::Patcher>> ApplyPatchers(Helper::StatePatcher& patcher, const Circuit::CircuitEnvironment& circuit, const std::vector<std::string>& tags, const std::string& target) const;

private:
	struct PatcherEntry
	{
		PatcherEntry(std::string name, std::vector<std::string> registers, std::vector<Basic::Logic> values, std::vector<std::string> tags, std::vector<std::string> targets);

		std::string name;
		std::vector<std::string> registers;
		std::vector<Basic::Logic> values;
		std::vector<std::string> tags;
		std::vector<std::string> targets;
	};

	std::vector<PatcherEntry> patchers = {};

};

};
};
