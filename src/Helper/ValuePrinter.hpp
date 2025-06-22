#pragma once

#include <string>
#include <ostream>
#include <memory>
#include <variant>
#include <functional>

#include "Basic/Logic.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/ValueWatcher.hpp"
#include "Simulation/CircuitSimulationResult.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Helper
{

class ValuePrinter
{
public:
	enum class ValueFormat { Binary, Decimal, Hexadecimal };
	enum class ValueAlignment { Left, Center, Right, Internal };
	enum class ValuePrefix { Yes, No };

	struct WatcherConfig
	{
		WatcherConfig(std::string name, std::string watcher, ValueFormat format, ValueAlignment alignment, ValuePrefix prefix);
		virtual ~WatcherConfig(void);

		std::string name;
		std::string watcher;
		ValueFormat format;
		ValueAlignment alignment;
		ValuePrefix prefix;
	};

	struct SpaceConfig
	{
		SpaceConfig(void);
		virtual ~SpaceConfig(void);
	};

	enum class DivideType { BoldSolid, BoldDashed, LightSolid, LightDashed };

	struct DivideConfig
	{
		DivideConfig(DivideType type);
		virtual ~DivideConfig(void);

		DivideType type;
	};

	struct CustomConfig
	{
		CustomConfig(std::string name, std::string function);
		virtual ~CustomConfig(void);

		std::string name;
		std::string function;
	};

	struct TitleConfig
	{
		TitleConfig(std::string name);
		virtual ~TitleConfig(void);

		std::string name;
	};

	using PrintConfig = std::variant<WatcherConfig, SpaceConfig, DivideConfig, CustomConfig, TitleConfig>;

	class ValuesPrinter
	{
		ValuesPrinter(const ValuePrinter& printer, const std::vector<ValueWatcher::WatcherValues>& values);

		const ValuePrinter& printer;
		const std::vector<ValueWatcher::WatcherValues>& values;

		friend ValuePrinter;
		friend std::ostream& operator<<(std::ostream& stream, const ValuesPrinter& printedValue);
	};
	friend std::ostream& operator<<(std::ostream& stream, const ValuesPrinter& printedValue);

	struct CustomFunction
	{
		CustomFunction(std::string name, std::function<size_t(void)> timeframes, std::function<std::string(size_t)> generator);

		std::string name;
		std::function<size_t(void)> timeframes;
		std::function<std::string(size_t)> generator;
	};

	ValuePrinter(void);
	virtual ~ValuePrinter(void);

	void SetTitle(std::string title);
	const std::string& GetTitle(void) const;

	void SetTimeframeTitles(std::function<std::string(size_t)> titles);
	const std::function<std::string(size_t)>& GetTimeframeTitles(void) const;

	void SetEqualColumnWidths(bool equal);
	const bool& IsEqualColumnWidths(void) const;

	std::shared_ptr<PrintConfig> AddBinaryWatcher(std::string name, std::string watcher, ValueAlignment alignment = ValueAlignment::Right, ValuePrefix prefix = ValuePrefix::Yes);
	std::shared_ptr<PrintConfig> AddDecimalWatcher(std::string name, std::string watcher, ValueAlignment alignment = ValueAlignment::Right, ValuePrefix prefix = ValuePrefix::Yes);
	std::shared_ptr<PrintConfig> AddHexadecimalWatcher(std::string name, std::string watcher, ValueAlignment alignment = ValueAlignment::Right, ValuePrefix prefix = ValuePrefix::Yes);
	std::shared_ptr<PrintConfig> AddSpacer(void);
	std::shared_ptr<PrintConfig> AddDivider(DivideType type = DivideType::BoldSolid);
	std::shared_ptr<PrintConfig> AddCustom(std::string name, std::string function);
	std::shared_ptr<PrintConfig> AddTitle(std::string name);
	std::shared_ptr<PrintConfig> AddConfig(PrintConfig config);

	void AddCustomFunction(std::string name, std::function<size_t(void)> timeframes, std::function<std::string(size_t)> generator);

	template<typename PinData, typename Generator, typename... Watchers>
	void AddGeneratedFunction(std::string name, Tpg::GeneratorContext<PinData>& context, const ValueWatcher& watcher, Generator generator, Watchers... watchers)
	{
		std::function<size_t(void)> timeframeFunction =  [](void) -> size_t { return 0u; };
		std::function<std::string(size_t)> generatorFunction = [&watcher, &context, generator, watchers...](size_t timeframe) -> std::string {
			return generator(watcher.GetWatcherValue<PinData, Tpg::GoodTag>(context, *watcher.GetWatcher(watchers), timeframe)...);
		};
		AddCustomFunction(name, timeframeFunction, generatorFunction);
	};
	template<typename Generator, typename... Watchers>
	void AddGeneratedFunction(std::string name, Simulation::SimulationResult& simulation, const ValueWatcher& watcher, Generator generator, Watchers... watchers)
	{
		std::function<size_t(void)> timeframeFunction =  [](void) -> size_t { return 0u; };
		std::function<std::string(size_t)> generatorFunction = [&watcher, &simulation, generator, watchers...](size_t timeframe) -> std::string {
			return generator(watcher.GetWatcherValue(simulation, *watcher.GetWatcher(watchers), timeframe)...);
		};
		AddCustomFunction(name, timeframeFunction, generatorFunction);
	};

	ValuesPrinter ToPrintable(const std::vector<ValueWatcher::WatcherValues>& values) const;

private:
	std::vector<std::shared_ptr<PrintConfig>> configurations = {};
	std::vector<CustomFunction> customFunctions = {};

	std::string title = "OBSERVED WIRES & PORT NAMES";
	std::function<std::string(size_t)> timeframeTitles = +[](size_t timeframe){ return "TF #" + std::to_string(timeframe); };
	bool equalColumnWidths = true;

};

};
};
