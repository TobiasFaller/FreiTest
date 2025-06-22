#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"

#include "Io/JsoncParser/JsonCParser.hpp"

namespace FreiTest
{
namespace Io
{

ValuePrinterImporter::ValuePrinterImporter(void) = default;
ValuePrinterImporter::~ValuePrinterImporter(void) = default;

bool ValuePrinterImporter::LoadPrinters(std::istream& stream, std::string watcherPrefix)
{
	boost::property_tree::ptree root;
	try
	{
		boost::property_tree::read_jsonc(stream, root);

		for (auto& [_key, child] : root)
		{
			auto typeIt = child.find("type");
			auto tagsIt = child.find("tags");
			auto targetsIt = child.find("targets");
			if (typeIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"type\" in printer list";
				return false;
			}
			if (tagsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"tags\" in printer list";
				return false;
			}
			if (targetsIt == child.not_found())
			{
				LOG(ERROR) << "Found entry without \"targets\" in printer list";
				return false;
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

			auto type = typeIt->second.get_value<std::string>();
			if (type == "title")
			{
				auto nameIt = child.find("name");
				if (nameIt == child.not_found())
				{
					LOG(ERROR) << "Found title entry without name in printer list";
					return false;
				}

				auto name = nameIt->second.get_value<std::string>();
				printers.emplace_back(Helper::ValuePrinter::TitleConfig { name }, tags, targets);
				continue;
			}
			if (type == "spacer")
			{
				printers.emplace_back(Helper::ValuePrinter::SpaceConfig { }, tags, targets);
				continue;
			}
			if (type == "divider")
			{
				auto styleIt = child.find("style");
				if (styleIt == child.not_found())
				{
					LOG(ERROR) << "Found divider entry without style in printer list";
					return false;
				}

				auto styleString = styleIt->second.get_value<std::string>();
				Helper::ValuePrinter::DivideType style;
				if (styleString == "bold-solid")
				{
					style = Helper::ValuePrinter::DivideType::BoldSolid;
				}
				else if (styleString == "bold-dashed")
				{
					style = Helper::ValuePrinter::DivideType::BoldDashed;
				}
				else if (styleString == "light-solid")
				{
					style = Helper::ValuePrinter::DivideType::LightSolid;
				}
				else if (styleString == "light-dashed")
				{
					style = Helper::ValuePrinter::DivideType::LightDashed;
				}
				else
				{
					LOG(ERROR) << "Found divider entry with invalid style \"" << styleString << "\"";
					return false;
				}

				printers.emplace_back(Helper::ValuePrinter::DivideConfig { style }, tags, targets);
				continue;
			}
			if (type == "watcher")
			{
				auto nameIt = child.find("name");
				auto watcherIt = child.find("watcher");
				auto formatIt = child.find("format");
				auto alignmentIt = child.find("alignment");
				auto prefixIt = child.find("prefix");
				if (nameIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without name in printer list";
					return false;
				}
				if (watcherIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without watcher in printer list";
					return false;
				}
				if (formatIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without format in printer list";
					return false;
				}
				if (alignmentIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without alignment in printer list";
					return false;
				}
				if (prefixIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without prefix in printer list";
					return false;
				}

				auto name = nameIt->second.get_value<std::string>();
				auto watcher = watcherIt->second.get_value<std::string>();
				auto formatString = formatIt->second.get_value<std::string>();
				auto alignmentString = alignmentIt->second.get_value<std::string>();
				auto prefixString = prefixIt->second.get_value<std::string>();

				Helper::ValuePrinter::ValueFormat format;
				Helper::ValuePrinter::ValueAlignment alignment;
				Helper::ValuePrinter::ValuePrefix prefix;
				if (formatString == "binary")
				{
					format = Helper::ValuePrinter::ValueFormat::Binary;
				}
				else if (formatString == "decimal")
				{
					format = Helper::ValuePrinter::ValueFormat::Decimal;
				}
				else if (formatString == "hexadecimal")
				{
					format = Helper::ValuePrinter::ValueFormat::Hexadecimal;
				}
				else
				{
					LOG(ERROR) << "Found divider entry with invalid format \"" << formatString << "\"";
					return false;
				}

				if (alignmentString == "left")
				{
					alignment = Helper::ValuePrinter::ValueAlignment::Left;
				}
				else if (alignmentString == "center")
				{
					alignment = Helper::ValuePrinter::ValueAlignment::Center;
				}
				else if (alignmentString == "right")
				{
					alignment = Helper::ValuePrinter::ValueAlignment::Right;
				}
				else if (alignmentString == "internal")
				{
					alignment = Helper::ValuePrinter::ValueAlignment::Internal;
				}
				else
				{
					LOG(ERROR) << "Found divider entry with invalid alignment \"" << alignmentString << "\"";
					return false;
				}

				if (prefixString == "yes")
				{
					prefix = Helper::ValuePrinter::ValuePrefix::Yes;
				}
				else if (prefixString == "no")
				{
					prefix = Helper::ValuePrinter::ValuePrefix::No;
				}
				else
				{
					LOG(ERROR) << "Found divider entry with invalid prefix \"" << prefixString << "\"";
					return false;
				}

				printers.emplace_back(Helper::ValuePrinter::WatcherConfig { name, watcherPrefix + watcher, format, alignment, prefix }, tags, targets);
				continue;
			}
			if (type == "function")
			{
				auto nameIt = child.find("name");
				auto functionIt = child.find("function");
				if (nameIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without name in printer list";
					return false;
				}
				if (functionIt == child.not_found())
				{
					LOG(ERROR) << "Found watcher entry without function in printer list";
					return false;
				}

				auto name = nameIt->second.get_value<std::string>();
				auto function = functionIt->second.get_value<std::string>();
				printers.emplace_back(Helper::ValuePrinter::CustomConfig { name, function }, tags, targets);
				continue;
			}

			LOG(ERROR) << "Invalid printer entry of type \"" << type << "\"";
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

std::vector<std::shared_ptr<Helper::ValuePrinter::PrintConfig>> ValuePrinterImporter::ApplyPrinters(Helper::ValuePrinter& valuePrinter, const std::vector<std::string>& tags, const std::string& target) const
{
	std::vector<std::shared_ptr<Helper::ValuePrinter::PrintConfig>> result;
	for (auto& printer : printers)
	{
		// Filter entries that do not apply to the circuit or the target
		if (std::find(printer.tags.begin(), printer.tags.end(), "*") == printer.tags.end())
		{
			for (const auto& tag : tags)
			{
				if (std::find(printer.tags.begin(), printer.tags.end(), tag) != printer.tags.end())
				{
					goto valid_tag_found;
				}
			}

			// No tag did match -> skip
			continue;
		}

	valid_tag_found:
		if (std::find(printer.targets.begin(), printer.targets.end(), "*") == printer.targets.end())
		{
			if (std::find(printer.targets.begin(), printer.targets.end(), target) == printer.targets.end())
			{
				continue;
			}
		}

		result.emplace_back(valuePrinter.AddConfig(printer.config));
	}
	return result;
}

ValuePrinterImporter::PrinterEntry::PrinterEntry(Helper::ValuePrinter::PrintConfig config, std::vector<std::string> tags, std::vector<std::string> targets):
	config(config),
	tags(tags),
	targets(targets)
{
}

};
};
