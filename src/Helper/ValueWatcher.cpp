#include "Helper/ValueWatcher.hpp"

#include <algorithm>
#include <regex>

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/DriverFinder.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Helper
{

ValueWatcher::ValueWatcher(void):
	watchers()
{
}

ValueWatcher::~ValueWatcher(void) = default;

std::shared_ptr<ValueWatcher::Watcher> ValueWatcher::AddWatcher(const std::string& name, const std::vector<const Circuit::MappedNode*>& references)
{
	return watchers.emplace_back(std::make_shared<Watcher>(
		name,
		references,
		std::vector<bool>(references.size(), false),
		std::vector<bool>(references.size(), false),
		std::vector<bool>(references.size(), false),
		std::vector<bool>(references.size(), false)
	));
}

std::shared_ptr<ValueWatcher::Watcher> ValueWatcher::GetWatcher(const std::string& name) const
{
	if (auto it = std::find_if(watchers.begin(), watchers.end(), [&name](auto& watcher) { return watcher->name == name; }); it != watchers.end())
	{
		return *it;
	}

	Logging::Panic("Could not find watcher " + name);
}

bool ValueWatcher::HasWatcher(const std::string& name) const
{
	return std::find_if(watchers.begin(), watchers.end(), [&name](auto& watcher) { return watcher->name == name; }) != watchers.end();
}

void ValueWatcher::RemoveWatcher(const std::string& name)
{
	if (auto it = std::find_if(watchers.begin(), watchers.end(), [&name](auto& watcher) { return watcher->name == name; }); it != watchers.end())
	{
		watchers.erase(it);
	}
}

std::vector<std::shared_ptr<ValueWatcher::Watcher>>& ValueWatcher::GetWatchers(void)
{
	return watchers;
}

const std::vector<std::shared_ptr<ValueWatcher::Watcher>>& ValueWatcher::GetWatchers(void) const
{
	return watchers;
}

template<typename PinData, typename Tag>
ValueWatcher::WatcherValue ValueWatcher::GetWatcherValue(Tpg::GeneratorContext<PinData>& context, const Watcher& watcher, size_t timeframeId, std::string prefix) const
{
	const auto& encoder = context.GetEncoder();

	if (auto bmcSolver = std::dynamic_pointer_cast<SolverProxy::Bmc::BmcSolverProxy>(context.GetSolver()); bmcSolver)
	{
		bmcSolver->SetTargetTimeframe(timeframeId);
	}

	const auto& timeframe = context.GetTimeframe(timeframeId);

	std::vector<Logic> watcherValue;
	for (auto& node : watcher.nodes)
	{
		if (node == nullptr)
		{
			watcherValue.push_back(Logic::LOGIC_INVALID);
			continue;
		}

		const auto container = timeframe.template GetContainer<Tag>(
			context, node->GetNodeId(), Circuit::PortType::Output, 0u);
		if (!container.IsSet())
		{
			watcherValue.push_back(Logic::LOGIC_INVALID);
			continue;
		}

		watcherValue.push_back(encoder.GetSolvedLogicValue(container));
	}

	for (size_t bitIndex = 0u; bitIndex < watcher.nodes.size(); ++bitIndex)
	{
		auto& value = watcherValue[bitIndex];
		if (watcher.set[bitIndex]) value = Logic::LOGIC_ONE;
		if (watcher.clear[bitIndex]) value = Logic::LOGIC_ZERO;
		if (watcher.invert[bitIndex]) value = InvertLogicValue(value);
		if (watcher.mask[bitIndex]) value = Logic::LOGIC_INVALID;
	}

	return { prefix + watcher.name, watcherValue };
}

ValueWatcher::WatcherValue ValueWatcher::GetWatcherValue(const Simulation::SimulationResult& simulation, const Watcher& watcher, size_t timeframeId, std::string prefix) const
{
	std::vector<Logic> watcherValue;
	for (auto& node : watcher.nodes)
	{
		if (node == nullptr)
		{
			watcherValue.push_back(Logic::LOGIC_INVALID);
			continue;
		}

		watcherValue.push_back(simulation[timeframeId][node->GetNodeId()]);
	}

	for (size_t bitIndex = 0u; bitIndex < watcher.nodes.size(); ++bitIndex)
	{
		auto& value = watcherValue[bitIndex];
		if (watcher.set[bitIndex]) value = Logic::LOGIC_ONE;
		if (watcher.clear[bitIndex]) value = Logic::LOGIC_ZERO;
		if (watcher.invert[bitIndex]) value = InvertLogicValue(value);
		if (watcher.mask[bitIndex]) value = Logic::LOGIC_INVALID;
	}

	return { prefix + watcher.name, watcherValue };
}

template<typename PinData, typename Tag>
std::vector<ValueWatcher::WatcherValues> ValueWatcher::GetWatcherValues(Tpg::GeneratorContext<PinData>& context, std::string prefix) const
{
	const auto timeframes = context.GetNumberOfTimeframesForTarget(Tpg::GenerationTarget::PatternExtraction);

	std::vector<WatcherValues> result;
	result.reserve(watchers.size());

	for (auto& watcher : watchers)
	{
		std::vector<WatcherValue> values;
		values.reserve(timeframes);

		for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
		{
			values.push_back(GetWatcherValue<PinData, Tag>(context, *watcher, timeframe, prefix));
		}

		result.push_back(WatcherValues { prefix + watcher->name, values });
	}

	return result;
}

std::vector<ValueWatcher::WatcherValues> ValueWatcher::GetWatcherValues(const Simulation::SimulationResult& simulation, std::string prefix) const
{
	std::vector<WatcherValues> result;
	result.reserve(watchers.size());

	for (auto& watcher : watchers)
	{
		const auto timeframes = simulation.size();
		std::vector<WatcherValue> values;
		values.reserve(timeframes);

		for (size_t timeframe { 0u }; timeframe < timeframes; ++timeframe)
		{
			values.push_back(GetWatcherValue(simulation, *watcher, timeframe, prefix));
		}

		result.push_back(WatcherValues { prefix + watcher->name, values });
	}

	return result;
}

template<typename PinData, typename Tag>
std::vector<ValueWatcher::WatcherValue> ValueWatcher::GetWatcherValues(Tpg::GeneratorContext<PinData>& context, size_t timeframeId, std::string prefix) const
{
	std::vector<ValueWatcher::WatcherValue> watcherValues;
	watcherValues.reserve(watchers.size());

	for (auto& watcher : watchers)
	{
		watcherValues.push_back(GetWatcherValue<PinData, Tag>(context, *watcher, timeframeId, prefix));
	}

	return watcherValues;
}

std::vector<ValueWatcher::WatcherValue> ValueWatcher::GetWatcherValues(const Simulation::SimulationResult& simulation, size_t timeframeId, std::string prefix) const
{
	std::vector<ValueWatcher::WatcherValue> watcherValues;
	watcherValues.reserve(watchers.size());

	for (auto& watcher : watchers)
	{
		watcherValues.push_back(GetWatcherValue(simulation, *watcher, timeframeId, prefix));
	}

	return watcherValues;
}

ValueWatcher::Watcher::Watcher(std::string name, std::vector<const Circuit::MappedNode*> nodes, std::vector<bool> set, std::vector<bool> clear, std::vector<bool> invert, std::vector<bool> mask):
	name(name),
	nodes(nodes),
	set(set),
	clear(clear),
	invert(invert),
	mask(mask)
{
}

void ValueWatcher::Watcher::SetBit(size_t index, bool set)
{
	this->set[index] = set;
}

void ValueWatcher::Watcher::ClearBit(size_t index, bool clear)
{
	this->clear[index] = clear;
}

void ValueWatcher::Watcher::MaskBit(size_t index, bool mask)
{
	this->mask[index] = mask;
}

void ValueWatcher::Watcher::InvertBit(size_t index, bool invert)
{
	this->invert[index] = invert;
}

void ValueWatcher::Watcher::SetBits(size_t index, size_t count, bool set)
{
	for (size_t i = 0u; i < count; ++i)
	{
		this->set[index + i] = set;
	}
}

void ValueWatcher::Watcher::ClearBits(size_t index, size_t count, bool clear)
{
	for (size_t i = 0u; i < count; ++i)
	{
		this->clear[index + i] = clear;
	}
}

void ValueWatcher::Watcher::MaskBits(size_t index, size_t count, bool mask)
{
	for (size_t i = 0u; i < count; ++i)
	{
		this->mask[index + i] = mask;
	}
}

void ValueWatcher::Watcher::InvertBits(size_t index, size_t count, bool invert)
{
	for (size_t i = 0u; i < count; ++i)
	{
		this->invert[index + i] = invert;
	}
}

ValueWatcher::WatcherValue::operator std::size_t() const
{
	size_t result = 0u;
	for (size_t i = 0u; i < values.size(); ++i)
	{
		if (values[i] == Basic::Logic::LOGIC_ONE)
		{
			result |= (1u << i);
		}
	}
	return result;
}

std::string to_string(const ValueWatcher::WatcherValue& value)
{
	std::string result;
	for (size_t i = 0u; i < value.values.size(); ++i)
	{
		result += to_string(value.values[i]);
	}
	return result;
}

#define FOR_TAG(PINDATA, TAG) \
	template std::vector<ValueWatcher::WatcherValues> ValueWatcher::GetWatcherValues<PINDATA, TAG>(Tpg::GeneratorContext<PINDATA>& context, std::string prefix) const; \
	template std::vector<ValueWatcher::WatcherValue> ValueWatcher::GetWatcherValues<PINDATA, TAG>(Tpg::GeneratorContext<PINDATA>& context, size_t timeframeId, std::string prefix) const; \
	template ValueWatcher::WatcherValue ValueWatcher::GetWatcherValue<PINDATA, TAG>(Tpg::GeneratorContext<PINDATA>& context, const ValueWatcher::Watcher& watcher, size_t timeframeId, std::string prefix) const;
INSTANTIATE_FOR_ALL_TAGS

};
};
