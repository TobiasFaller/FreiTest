#pragma once

#include <cstdint>
#include <vector>
#include <string>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"

namespace FreiTest
{
namespace Simulation
{

class SimulationResult;

class SimulationTimeframe
{
public:
	using iterator = std::vector<Basic::Logic>::iterator;
	using const_iterator = std::vector<Basic::Logic>::const_iterator;

	SimulationTimeframe(size_t nodes);
	SimulationTimeframe(size_t nodes, Basic::Logic value);
	virtual ~SimulationTimeframe(void);

	size_t size(void);
	size_t size(void) const;

	Basic::Logic& operator[](size_t index);
	const Basic::Logic& operator[](size_t index) const;

	size_t GetNumberOfValues(void);
	size_t GetNumberOfValues(void) const;
	std::vector<Basic::Logic>& GetValues(void);
	const std::vector<Basic::Logic>& GetValues(void) const;

	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;

	const_iterator cbegin(void) const;
	const_iterator cend(void) const;

private:
	std::vector<Basic::Logic> values;

	friend SimulationResult;
};

class SimulationResult
{
public:
	using iterator = std::vector<SimulationTimeframe>::iterator;
	using const_iterator = std::vector<SimulationTimeframe>::const_iterator;

	SimulationResult(size_t timeframes, size_t nodes);
	SimulationResult(size_t timeframes, size_t nodes, Basic::Logic value);
	virtual ~SimulationResult(void);

	size_t size(void);
	size_t size(void) const;

	SimulationTimeframe& operator[](size_t index);
	const SimulationTimeframe& operator[](size_t index) const;

	size_t GetNumberOfTimeframes(void);
	size_t GetNumberOfTimeframes(void) const;
	std::vector<SimulationTimeframe>& GetTimeframes(void);
	const std::vector<SimulationTimeframe>& GetTimeframes(void) const;

	Basic::Logic& GetOutputLogic(const Circuit::MappedNode* node, size_t timeframe);
	Basic::Logic& GetInputLogic(const Circuit::MappedNode* node, size_t input, size_t timeframe);
	const Basic::Logic& GetOutputLogic(const Circuit::MappedNode* node, size_t timeframe) const;
	const Basic::Logic& GetInputLogic(const Circuit::MappedNode* node, size_t input, size_t timeframe) const;

	void ReplaceWith(const SimulationResult& other);
	void Fill(Basic::Logic value);

	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;

	const_iterator cbegin(void) const;
	const_iterator cend(void) const;

private:
	std::vector<SimulationTimeframe> timeframes;
};

std::string to_string(const SimulationResult& result);
std::string to_string(const SimulationTimeframe& timeframe);

std::string to_string(const SimulationResult& result, const Circuit::MappedCircuit& circuit);
std::string to_string(const SimulationTimeframe& timeframe, const Circuit::MappedCircuit& circuit);

std::string to_string(const SimulationResult& goodResult, const SimulationResult& badResult, const Circuit::MappedCircuit& circuit);
std::string to_string(const SimulationTimeframe& goodTimeframe, const SimulationTimeframe& badTimeframe, const Circuit::MappedCircuit& circuit);

std::string to_string(const SimulationResult& atpgGoodResult, const SimulationResult& atpgBadResult,
						const SimulationResult& simGoodResult, const SimulationResult& simBadResult, const Circuit::MappedCircuit& circuit);

std::string to_debug(const SimulationResult& goodResult, const SimulationResult& badResult, const Circuit::MappedCircuit& circuit);

};
};
