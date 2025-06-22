#include "Io/StatePatcherImporter/StatePatcherImporter.hpp"

#include "Circuit/DriverFinder.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Io
{

StatePatcherImporter::PatcherEntry::PatcherEntry(std::string name, std::vector<std::string> registers, std::vector<Logic> values, std::vector<std::string> tags, std::vector<std::string> targets):
	name(name),
	registers(registers),
	values(values),
	tags(tags),
	targets(targets)
{
}

StatePatcherImporter::StatePatcherImporter(void) = default;
StatePatcherImporter::~StatePatcherImporter(void) = default;

bool StatePatcherImporter::LoadPatchers(std::istream& stream)
{
	boost::property_tree::ptree root;
	try
	{
		boost::property_tree::read_jsonc(stream, root);

		for (auto& [_key, child] : root)
		{
			auto nameIt = child.find("name");
			auto registersIt = child.find("registers");
			auto valuesIt = child.find("values");
			auto tagsIt = child.find("tags");
			auto targetsIt = child.find("targets");

			if (nameIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"name\" in patcher list";
				return false;
			}
			if (registersIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"registers\" in patcher list";
				return false;
			}
			if (valuesIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"values\" in patcher list";
				return false;
			}
			if (tagsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"tags\" in patcher list";
				return false;
			}
			if (targetsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"targets\" in patcher list";
				return false;
			}

			std::string name = nameIt->second.get_value<std::string>();
			std::string valuesString = valuesIt->second.get_value<std::string>();
			std::vector<Logic> values;
			for (auto& character : valuesString)
			{
				if (character == ' ')
				{
					continue;
				}

				values.push_back(GetLogicForCharacter(character));
			}

			std::vector<std::string> registers;
			for (auto& [_key, reg] : registersIt->second)
			{
				registers.push_back(reg.get_value<std::string>());
			}

			std::vector<std::string> tags;
			for (auto& [_key, tag] : tagsIt->second)
			{
				tags.push_back(tag.get_value<std::string>());
			}

			std::vector<std::string> targets;
			for (auto& [_key, target] : targetsIt->second)
			{
				targets.push_back(target.get_value<std::string>());
			}

			patchers.emplace_back(name, registers, values, tags, targets);
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

void StatePatcherImporter::ClearPatchers(void)
{
	patchers.clear();
}

std::vector<std::shared_ptr<Helper::StatePatcher::Patcher>> StatePatcherImporter::ApplyPatchers(Helper::StatePatcher& statePatcher, const Circuit::CircuitEnvironment& circuit, const std::vector<std::string>& tags, const std::string& target) const
{
	const auto& mappedCircuit = circuit.GetMappedCircuit();

	std::vector<std::shared_ptr<Helper::StatePatcher::Patcher>> result;
	for (const auto& patcher : patchers)
	{
		// Filter entries that do not apply to the circuit or the target
		if (std::find(patcher.tags.begin(), patcher.tags.end(), "*") == patcher.tags.end())
		{
			for (const auto& tag : tags)
			{
				if (std::find(patcher.tags.begin(), patcher.tags.end(), tag) != patcher.tags.end())
				{
					goto valid_tag_found;
				}
			}

			// No tag did match -> skip
			continue;
		}

	valid_tag_found:
		if (std::find(patcher.targets.begin(), patcher.targets.end(), "*") == patcher.targets.end())
		{
			if (std::find(patcher.targets.begin(), patcher.targets.end(), target) == patcher.targets.end())
			{
				continue;
			}
		}

		std::vector<size_t> flipFlops;
		for (const auto& reg : patcher.registers)
		{
			for (const auto& driver : Circuit::GetDrivers(circuit.GetName() + "/" + reg, circuit))
			{
				ASSERT(driver->GetCellCategory() == CellCategory::MAIN_BUF) << "Expected output buffer of flip-flop / latch";
				ASSERT(driver->GetCellType() == CellType::BUF) << "Expected output buffer of flip-flop / latch";

				const auto& flipFlopGate = driver->GetInput(0u);
				ASSERT(flipFlopGate->GetCellCategory() == CellCategory::MAIN_IN) << "Expected secondary input of flip-flop / latch";
				ASSERT(flipFlopGate->GetCellType() == CellType::S_IN) << "Expected secondary input of flip-flop / latch";
				ASSERT(mappedCircuit.IsSecondaryInput(flipFlopGate)) << "The patcher " << patcher.name << " references port / wire that is not a register.";

				flipFlops.push_back(mappedCircuit.GetSecondaryInputNumber(flipFlopGate));
			}
		}

		ASSERT(flipFlops.size() == patcher.values.size()) << "Values and signals of patcher " << patcher.name << " does not match.";

		std::vector<Helper::StatePatcher::PatchValue> values;
		for (size_t index { 0u }; index < flipFlops.size(); ++index)
		{
			values.emplace_back(flipFlops[index], patcher.values[index]);
		}

		result.emplace_back(statePatcher.AddPatcher(patcher.name, values));
	}
	return result;
}

};
};
