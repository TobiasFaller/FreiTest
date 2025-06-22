#include "Helper/ValuePrinter.hpp"

#include <iomanip>
#include <sstream>
#include <vector>
#include <string>
#include <limits>

#include "Basic/Overload.hpp"
#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Helper
{

// See https://en.wikipedia.org/wiki/Box-drawing_character
static const std::string HORIZONTAL = "\u2550";
static const std::string VERTICAL = "\u2551";
static const std::string TOP_LEFT = "\u2554";
static const std::string TOP_RIGHT = "\u2557";
static const std::string BOT_LEFT = "\u255A";
static const std::string BOT_RIGHT = "\u255D";
static const std::string VERTICAL_RIGHT = "\u2560";
static const std::string VERTICAL_LEFT = "\u2563";
static const std::string UPDOWN = "\u2566";
static const std::string DOWNUP = "\u2569";
static const std::string CROSS = "\u256C";

std::ostream& operator<<(std::ostream& stream, const ValuePrinter::ValuesPrinter& instance)
{
	const auto print_value = [](std::ostream& stream, auto& config, auto& watcherValue, size_t columnWidth) -> std::ostream& {
		const auto get_value = [&](void) -> std::string {
			auto values = watcherValue.values;
			switch (config.format)
			{
				case ValuePrinter::ValueFormat::Binary:
				{
					std::string result;
					result.reserve(values.size());
					for (auto &value : values)
					{
						result += to_string(value);
					}
					std::reverse(result.begin(), result.end());
					return result;
				}

				case ValuePrinter::ValueFormat::Decimal:
				{
					size_t result = 0u;
					for (size_t i = 0u; i < values.size(); ++i)
					{
						if (values[i] == Basic::Logic::LOGIC_ONE)
						{
							result |= (1u << i);
						}
					}
					return std::to_string(result);
				}

				case ValuePrinter::ValueFormat::Hexadecimal:
				{
					std::string result;
					for (size_t i = 0u; i < values.size(); i += 4u)
					{
						size_t value = 0u;
						bool undefined = false;
						bool dontCare = false;
						bool invalid = true;

						for (size_t j = i; j < (i + 4u) && j < values.size(); ++j)
						{
							switch (values[j])
							{
								case Basic::Logic::LOGIC_INVALID:
									break;
								case Basic::Logic::LOGIC_ZERO:
									invalid = false;
									break;
								case Basic::Logic::LOGIC_ONE:
									invalid = false;
									value |= (1u << (j % 4u));
									break;
								case Basic::Logic::LOGIC_DONT_CARE:
									invalid = false;
									dontCare = true;
									break;
								case Basic::Logic::LOGIC_UNKNOWN:
									invalid = false;
									undefined = true;
									break;
							}
						}

						if (invalid)
						{
							result += '-';
						}
						else if (undefined)
						{
							result += 'U';
						}
						else if (dontCare)
						{
							result += 'X';
						}
						else
						{
							result += "0123456789ABCDEF"[value];
						}
					}

					std::reverse(result.begin(), result.end());
					return result;
				}

				default:
					__builtin_unreachable();
			}
		};
		const auto get_prefix = [&](void) -> std::string {
			if (config.prefix == ValuePrinter::ValuePrefix::No)
			{
				return "";
			}

			switch (config.format)
			{
				case ValuePrinter::ValueFormat::Binary:
					return "0b";
				case ValuePrinter::ValueFormat::Decimal:
					return "0d";
				case ValuePrinter::ValueFormat::Hexadecimal:
					return "0x";
				default:
					__builtin_unreachable();
			}
		};

		const auto prefix = get_prefix();
		const auto value = get_value();
		const auto textLength { value.size() + prefix.size() };

		if (textLength >= columnWidth)
		{
			return stream << prefix << value;
		}

		switch (config.alignment)
		{
			case ValuePrinter::ValueAlignment::Left:
				return stream
					<< std::left << prefix
					<< std::left << std::setw(columnWidth - prefix.size()) << value;
			case ValuePrinter::ValueAlignment::Center:
				return stream
					<< std::right << std::setw((columnWidth - textLength) / 2 + prefix.size()) << prefix
					<< std::left << std::setw(columnWidth - (columnWidth - textLength) / 2 - prefix.size()) << value;
			case ValuePrinter::ValueAlignment::Right:
				return stream
					<< std::right << std::setw(columnWidth - value.size()) << prefix
					<< std::left << value;
			case ValuePrinter::ValueAlignment::Internal:
				return stream
					<< std::left << std::setw(columnWidth - value.size()) << prefix
					<< std::left << value;
			default:
				__builtin_unreachable();
		}
	};
	const auto repeat = [](const std::string& value, size_t count) -> std::string {
		std::string result;
		result.reserve(value.size() * count);
		for (size_t index { 0u }; index < count; ++index)
		{
			result += value;
		}
		return result;
	};

	auto& printer = instance.printer;
	auto& values = instance.values;
	auto& configurations = printer.configurations;

	const auto get_function = [&](const std::string& name) -> const ValuePrinter::CustomFunction& {
		if (auto it = std::find_if(printer.customFunctions.begin(), printer.customFunctions.end(),
				[&name](auto& function) { return function.name == name; });
			it != printer.customFunctions.end())
		{
			return *it;
		}

		Logging::Panic("Custom function " + name + " has not been found");
	};

	std::vector<size_t> configToValueIndex;
	for (size_t configIndex { 0u }; configIndex < configurations.size(); ++configIndex)
	{
		configToValueIndex.push_back(std::visit(Basic::overload(
			[&](const ValuePrinter::WatcherConfig& config) -> size_t {
				for (size_t valueIndex { 0u }; valueIndex < values.size(); ++valueIndex)
				{
					if (config.watcher == values[valueIndex].name)
					{
						return valueIndex;
					}
				}

				Logging::Panic("Could not find watcher " + config.watcher + " in list of watched values");
			},
			[&](const ValuePrinter::SpaceConfig& config) -> size_t {
				return std::numeric_limits<size_t>::max();
			},
			[&](const ValuePrinter::DivideConfig& config) -> size_t {
				return std::numeric_limits<size_t>::max();
			},
			[&](const ValuePrinter::CustomConfig& config) -> size_t {
				return std::numeric_limits<size_t>::max();
			},
			[&](const ValuePrinter::TitleConfig& config) -> size_t {
				return std::numeric_limits<size_t>::max();
			}
		), *configurations[configIndex]));
	}

	size_t timeframes { 0u };
	for (size_t configIndex { 0u }; configIndex < configurations.size(); ++configIndex)
	{
		std::visit(Basic::overload(
			[&](const ValuePrinter::WatcherConfig& config) -> void {
				timeframes = std::max(timeframes, values[configToValueIndex[configIndex]].timeframes.size());
			},
			[&](const ValuePrinter::SpaceConfig& config) -> void { },
			[&](const ValuePrinter::DivideConfig& config) -> void { },
			[&](const ValuePrinter::CustomConfig& config) -> void {
				timeframes = std::max(timeframes,  get_function(config.function).timeframes());
			},
			[&](const ValuePrinter::TitleConfig& config) -> void { }
		), *configurations[configIndex]);
	}

	size_t nameColumnWidth { printer.title.size() };
	std::vector<size_t> columnWidths(timeframes, 0u);
	for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
	{
		columnWidths[timeframe] = std::max(columnWidths[timeframe], printer.timeframeTitles(timeframe).size());
	}
	for (size_t configIndex { 0u }; configIndex < configurations.size(); ++configIndex)
	{
		std::visit(Basic::overload(
			[&](const ValuePrinter::WatcherConfig& config) -> void {
				nameColumnWidth = std::max(nameColumnWidth, config.name.size());
				for (size_t timeframe { 0u }; timeframe < values[configToValueIndex[configIndex]].timeframes.size(); ++timeframe)
				{
					std::stringstream ss;
					print_value(ss, config, values[configToValueIndex[configIndex]].timeframes[timeframe], 0u);
					columnWidths[timeframe] = std::max(columnWidths[timeframe], static_cast<size_t>(ss.tellp()));
				}
			},
			[&](const ValuePrinter::SpaceConfig& config) -> void { },
			[&](const ValuePrinter::DivideConfig& config) -> void { },
			[&](const ValuePrinter::CustomConfig& config) -> void {
				nameColumnWidth = std::max(nameColumnWidth, config.name.size());
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					columnWidths[timeframe] = std::max(columnWidths[timeframe], get_function(config.function).generator(timeframe).size());
				}
			},
			[&](const ValuePrinter::TitleConfig& config) -> void {
				nameColumnWidth = std::max(nameColumnWidth, config.name.size());
			}
		), *configurations[configIndex]);
	}

	if (printer.equalColumnWidths)
	{
		size_t maxWidth { *std::max_element(columnWidths.begin(), columnWidths.end()) };
		std::for_each(columnWidths.begin(), columnWidths.end(), [maxWidth](auto& width) { width = maxWidth; });
	}

	// First row of the title bar
	stream << std::left << TOP_LEFT << repeat(HORIZONTAL, nameColumnWidth + 2u) << UPDOWN;
	for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
	{
		if (timeframe > 0u) stream << UPDOWN;
		stream << repeat(HORIZONTAL, columnWidths[timeframe] + 2u);
	}
	stream << TOP_RIGHT << std::endl;

	// Titles of each timeframe
	stream << VERTICAL << ' ' << std::setw(nameColumnWidth) << printer.title << ' ' << VERTICAL;
	for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
	{
		stream << ' ' << std::setw(columnWidths[timeframe]) << printer.timeframeTitles(timeframe) << ' ' << VERTICAL;
	}
	stream << std::endl;

	// Last row of the title bar
	stream << VERTICAL_RIGHT << repeat(HORIZONTAL, nameColumnWidth + 2u) << CROSS;
	for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
	{
		if (timeframe > 0u) stream << CROSS;
		stream << repeat(HORIZONTAL, columnWidths[timeframe] + 2u);
	}
	stream << VERTICAL_LEFT << std::endl;

	// Content of the table, row by row
	for (size_t configIndex { 0u }; configIndex < configurations.size(); ++configIndex)
	{
		std::visit(Basic::overload(
			[&](const ValuePrinter::WatcherConfig& config) -> void {
				stream << VERTICAL;
				stream << std::left << ' ' << std::setw(nameColumnWidth) << config.name << ' ';
				stream << VERTICAL;
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					stream << std::left << ' ';
					if (timeframe < values[configToValueIndex[configIndex]].timeframes.size())
						print_value(stream, config, values[configToValueIndex[configIndex]].timeframes[timeframe], columnWidths[timeframe]);
					else
						stream << std::setw(columnWidths[timeframe]) << "";
					stream << std::left << ' ' << VERTICAL;
				}
				stream << std::endl;
			},
			[&](const ValuePrinter::SpaceConfig& config) -> void {
				stream << VERTICAL << std::left << std::string(nameColumnWidth + 2u, ' ') << VERTICAL;
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					stream << std::left << std::string(columnWidths[timeframe] + 2u, ' ') << VERTICAL;
				}
				stream << std::endl;
			},
			[&](const ValuePrinter::DivideConfig& config) -> void {
				const auto line = [&](size_t length) -> std::string {
					std::string result;
					result.reserve(2 * length);
					for (size_t index { 0u }; index < length; ++index)
					{
						switch (config.type)
						{
							case ValuePrinter::DivideType::BoldSolid:
								result += HORIZONTAL;
								break;
							case ValuePrinter::DivideType::BoldDashed:
								result += (index % 2) ? HORIZONTAL : " ";
								break;
							case ValuePrinter::DivideType::LightSolid:
								result += '-';
								break;
							case ValuePrinter::DivideType::LightDashed:
								result += (index % 2) ? '-' : ' ';
								break;
						}
					}
					return result;
				};
				stream << VERTICAL_RIGHT << line(nameColumnWidth + 2u) << CROSS;
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					if (timeframe > 0u) stream << CROSS;
					stream << line(columnWidths[timeframe] + 2u);
				}
				stream << VERTICAL_LEFT << std::endl;
			},
			[&](const ValuePrinter::CustomConfig& config) -> void {
				stream << VERTICAL << std::left << ' ' << std::setw(nameColumnWidth) << config.name << ' ' << VERTICAL;
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					stream << std::left << ' ' << std::setw(columnWidths[timeframe]) << get_function(config.function).generator(timeframe) << ' ' << VERTICAL;
				}
				stream << std::endl;
			},
			[&](const ValuePrinter::TitleConfig& config) -> void {
				stream << VERTICAL << std::left << ' ' << std::setw(nameColumnWidth) << config.name << ' ' << VERTICAL;
				for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
				{
					stream << std::left << std::string(columnWidths[timeframe] + 2u, ' ') << VERTICAL;
				}
				stream << std::endl;
			}
		), *configurations[configIndex]);
	}

	// Last row of the table
	stream << BOT_LEFT << repeat(HORIZONTAL, nameColumnWidth + 2u) << DOWNUP;
	for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
	{
		if (timeframe > 0u) stream << DOWNUP;
		stream << repeat(HORIZONTAL, columnWidths[timeframe] + 2u);
	}
	stream << BOT_RIGHT << std::endl;

	return stream;
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddBinaryWatcher(std::string name, std::string watcher, ValueAlignment alignment, ValuePrefix prefix)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(WatcherConfig { name, watcher, ValueFormat::Binary, alignment, prefix }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddDecimalWatcher(std::string name, std::string watcher, ValueAlignment alignment, ValuePrefix prefix)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(WatcherConfig { name, watcher, ValueFormat::Decimal, alignment, prefix }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddHexadecimalWatcher(std::string name, std::string watcher, ValueAlignment alignment, ValuePrefix prefix)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(WatcherConfig { name, watcher, ValueFormat::Hexadecimal, alignment, prefix }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddSpacer(void)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(SpaceConfig { }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddDivider(DivideType type)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(DivideConfig { type }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddCustom(std::string name, std::string function)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(CustomConfig { name, function }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddTitle(std::string name)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(TitleConfig { name }));
}

std::shared_ptr<ValuePrinter::PrintConfig> ValuePrinter::AddConfig(PrintConfig config)
{
	return configurations.emplace_back(std::make_shared<ValuePrinter::PrintConfig>(config));
}

ValuePrinter::ValuePrinter(void) = default;
ValuePrinter::~ValuePrinter(void) = default;

void ValuePrinter::SetTitle(std::string title)
{
	this->title = title;
}

const std::string& ValuePrinter::GetTitle(void) const
{
	return title;
}


void ValuePrinter::SetTimeframeTitles(std::function<std::string(size_t)> titles)
{
	this->timeframeTitles = titles;
}

const std::function<std::string(size_t)>& ValuePrinter::GetTimeframeTitles(void) const
{
	return timeframeTitles;
}

void ValuePrinter::SetEqualColumnWidths(bool equal)
{
	this->equalColumnWidths = equal;
}

const bool& ValuePrinter::IsEqualColumnWidths(void) const
{
	return equalColumnWidths;
}

ValuePrinter::ValuesPrinter ValuePrinter::ToPrintable(const std::vector<ValueWatcher::WatcherValues>& values) const
{
	return ValuesPrinter(*this, values);
}

void ValuePrinter::AddCustomFunction(std::string name, std::function<size_t(void)> timeframes, std::function<std::string(size_t)> generator)
{
	customFunctions.emplace_back(name, timeframes, generator);
}

ValuePrinter::WatcherConfig::WatcherConfig(std::string name, std::string watcher, ValueFormat format, ValueAlignment alignment, ValuePrefix prefix):
	name(name),
	watcher(watcher),
	format(format),
	alignment(alignment),
	prefix(prefix)
{
}
ValuePrinter::WatcherConfig::~WatcherConfig(void) = default;

ValuePrinter::SpaceConfig::SpaceConfig(void) = default;
ValuePrinter::SpaceConfig::~SpaceConfig(void) = default;

ValuePrinter::DivideConfig::DivideConfig(DivideType type):
	type(type)
{
}
ValuePrinter::DivideConfig::~DivideConfig(void) = default;

ValuePrinter::CustomConfig::CustomConfig(std::string name, std::string function):
	name(name),
	function(function)
{
}
ValuePrinter::CustomConfig::~CustomConfig(void) = default;

ValuePrinter::TitleConfig::TitleConfig(std::string name):
	name(name)
{
}
ValuePrinter::TitleConfig::~TitleConfig(void) = default;

ValuePrinter::ValuesPrinter::ValuesPrinter(const ValuePrinter& printer, const std::vector<ValueWatcher::WatcherValues>& values):
	printer(printer),
	values(values)
{
}


ValuePrinter::CustomFunction::CustomFunction(std::string name, std::function<size_t(void)> timeframes, std::function<std::string(size_t)> generator):
	name(name),
	timeframes(timeframes),
	generator(generator)
{
}

};
};


