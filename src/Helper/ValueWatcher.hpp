#pragma once

#include <string>
#include <memory>
#include <vector>

#include "Basic/Logic.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Simulation/CircuitSimulationResult.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Helper
{

class ValueWatcher
{
public:
	class Watcher
	{
	public:
		Watcher(std::string name, std::vector<const Circuit::MappedNode*> nodes, std::vector<bool> set, std::vector<bool> clear, std::vector<bool> invert, std::vector<bool> mask);

		std::string name;
		std::vector<const Circuit::MappedNode*> nodes;
		std::vector<bool> set;
		std::vector<bool> clear;
		std::vector<bool> invert;
		std::vector<bool> mask;

		void SetName(std::string name);
		void SetNodes(std::vector<const Circuit::MappedNode*> nodes);

		void SetBit(size_t index, bool set = true);
		void ClearBit(size_t index, bool clear = true);
		void MaskBit(size_t index, bool mask = true);
		void InvertBit(size_t index, bool invert = true);

		void SetBits(size_t index, size_t count, bool set = true);
		void ClearBits(size_t index, size_t count, bool clear = true);
		void MaskBits(size_t index, size_t count, bool mask = true);
		void InvertBits(size_t index, size_t count, bool invert = true);
	};

	struct WatcherValue
	{
		std::string name;
		std::vector<Basic::Logic> values;

		explicit operator size_t(void) const;
	};

	struct WatcherValues
	{
		std::string name;
		std::vector<WatcherValue> timeframes;
	};

	ValueWatcher(void);
	virtual ~ValueWatcher(void);

	std::shared_ptr<Watcher> AddWatcher(const std::string& name, const std::vector<const Circuit::MappedNode*>& references);
	std::shared_ptr<Watcher> GetWatcher(const std::string& name) const;
	bool HasWatcher(const std::string& name) const;
	void RemoveWatcher(const std::string& name);

	std::vector<std::shared_ptr<Watcher>>& GetWatchers(void);
	const std::vector<std::shared_ptr<Watcher>>& GetWatchers(void) const;

	template<typename PinData, typename Tag>
	std::vector<WatcherValues> GetWatcherValues(Tpg::GeneratorContext<PinData>& context, std::string prefix = "") const;
	std::vector<WatcherValues> GetWatcherValues(const Simulation::SimulationResult& simulation, std::string prefix = "") const;

	template<typename PinData, typename Tag>
	std::vector<WatcherValue> GetWatcherValues(Tpg::GeneratorContext<PinData>& context, size_t timeframe, std::string prefix = "") const;
	std::vector<WatcherValue> GetWatcherValues(const Simulation::SimulationResult& simulation, size_t timeframe, std::string prefix = "") const;

	template<typename PinData, typename Tag>
	WatcherValue GetWatcherValue(Tpg::GeneratorContext<PinData>& context, const Watcher& watcher, size_t timeframe, std::string prefix = "") const;
	WatcherValue GetWatcherValue(const Simulation::SimulationResult& simulation, const Watcher& watcher, size_t timeframe, std::string prefix = "") const;

private:
	std::vector<std::shared_ptr<Watcher>> watchers;

};

std::string to_string(const ValueWatcher::WatcherValue& value);

};
};
