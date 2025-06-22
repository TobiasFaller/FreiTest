#include "Simulation/CircuitSimulationResult.hpp"

#include "Circuit/MappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Simulation
{

SimulationTimeframe::SimulationTimeframe(size_t nodes):
	values(nodes, Logic::LOGIC_DONT_CARE)
{
}

SimulationTimeframe::SimulationTimeframe(size_t nodes, Logic value):
	values(nodes, value)
{
}

SimulationTimeframe::~SimulationTimeframe(void) = default;

size_t SimulationTimeframe::size(void)
{
	return values.size();
}

size_t SimulationTimeframe::size(void) const
{
	return values.size();
}

Logic& SimulationTimeframe::operator[](size_t index)
{
	return values[index];
}

const Logic& SimulationTimeframe::operator[](size_t index) const
{
	return values[index];
}

size_t SimulationTimeframe::GetNumberOfValues(void)
{
	return values.size();
}

size_t SimulationTimeframe::GetNumberOfValues(void) const
{
	return values.size();
}

std::vector<Logic>& SimulationTimeframe::GetValues(void)
{
	return values;
}

const std::vector<Logic>& SimulationTimeframe::GetValues(void) const
{
	return values;
}

SimulationTimeframe::iterator SimulationTimeframe::begin(void)
{
	return values.begin();
}

SimulationTimeframe::iterator SimulationTimeframe::end(void)
{
	return values.end();
}

SimulationTimeframe::const_iterator SimulationTimeframe::begin(void) const
{
	return values.begin();
}

SimulationTimeframe::const_iterator SimulationTimeframe::end(void) const
{
	return values.end();
}

SimulationTimeframe::const_iterator SimulationTimeframe::cbegin(void) const
{
	return values.cbegin();
}

SimulationTimeframe::const_iterator SimulationTimeframe::cend(void) const
{
	return values.cend();
}

SimulationResult::SimulationResult(size_t timeframes, size_t nodes):
	timeframes(timeframes, SimulationTimeframe(nodes))
{
}

SimulationResult::SimulationResult(size_t timeframes, size_t nodes, Logic value):
	timeframes(timeframes, SimulationTimeframe(nodes, value))
{
}

SimulationResult::~SimulationResult(void) = default;

size_t SimulationResult::size(void)
{
	return timeframes.size();
}

size_t SimulationResult::size(void) const
{
	return timeframes.size();
}

SimulationTimeframe& SimulationResult::operator[](size_t index)
{
	return timeframes[index];
}

const SimulationTimeframe& SimulationResult::operator[](size_t index) const
{
	return timeframes[index];
}

SimulationResult::iterator SimulationResult::begin(void)
{
	return timeframes.begin();
}

SimulationResult::iterator SimulationResult::end(void)
{
	return timeframes.end();
}

SimulationResult::const_iterator SimulationResult::begin(void) const
{
	return timeframes.begin();
}

SimulationResult::const_iterator SimulationResult::end(void) const
{
	return timeframes.end();
}

SimulationResult::const_iterator SimulationResult::cbegin(void) const
{
	return timeframes.cbegin();
}

SimulationResult::const_iterator SimulationResult::cend(void) const
{
	return timeframes.cend();
}

Logic& SimulationResult::GetOutputLogic(const MappedNode* node, size_t timeframe)
{
	return timeframes[timeframe][node->GetNodeId()];
}

const Logic& SimulationResult::GetOutputLogic(const MappedNode* node, size_t timeframe) const
{
	return timeframes[timeframe][node->GetNodeId()];
}

Logic& SimulationResult::GetInputLogic(const MappedNode* node, size_t input, size_t timeframe)
{
	return timeframes[timeframe][node->GetInput(input)->GetNodeId()];
}

const Logic& SimulationResult::GetInputLogic(const MappedNode* node, size_t input, size_t timeframe) const
{
	return timeframes[timeframe][node->GetInput(input)->GetNodeId()];
}

size_t SimulationResult::GetNumberOfTimeframes(void)
{
	return timeframes.size();
}

size_t SimulationResult::GetNumberOfTimeframes(void) const
{
	return timeframes.size();
}

std::vector<SimulationTimeframe>& SimulationResult::GetTimeframes(void)
{
	return timeframes;
}

const std::vector<SimulationTimeframe>& SimulationResult::GetTimeframes(void) const
{
	return timeframes;
}

void SimulationResult::ReplaceWith(const SimulationResult& other)
{
	for (size_t timeframeId = 0u; timeframeId < timeframes.size(); ++timeframeId)
	{
		auto const& otherData = other.timeframes[timeframeId].values;
		timeframes[timeframeId].values.assign(otherData.begin(), otherData.end());
	}
}

void SimulationResult::Fill(Logic value)
{
	for (size_t timeframeId = 0u; timeframeId < timeframes.size(); ++timeframeId)
	{
		auto& timeframe = timeframes[timeframeId].values;
		std::fill(timeframe.begin(), timeframe.end(), value);
	}
}

std::string to_string(const SimulationResult& simulationResult)
{
	std::string result;

	for (const SimulationTimeframe& timeframe : simulationResult)
	{
		if (result != "")
		{
			result += " -> ";
		}

		result += to_string(timeframe);
	}

	return result;
}

std::string to_string(const SimulationTimeframe& timeframe)
{
	std::string result;
	for (const Logic& value : timeframe)
	{
		result += to_string(value);
	}
	return result;
}


std::string to_string(const SimulationResult& simulationResult, const MappedCircuit& circuit)
{
	std::string result;

	size_t timeframeId = 0u;
	for (const SimulationTimeframe& timeframe : simulationResult)
	{
		result += "Timeframe " + std::to_string(timeframeId) + "\n";
		result += to_string(timeframe, circuit);
		timeframeId++;
	}

	return result;
}

std::string to_string(const SimulationTimeframe& timeframe, const MappedCircuit& circuit)
{
	std::string result;

	for (const auto [nodeId, node] : circuit.EnumerateNodes())
	{
		result += std::to_string(nodeId) + " " + node->GetName() + ": " + to_string(timeframe[nodeId]) + "\n";
	}
	return result;
}

std::string to_string(const SimulationResult& goodResult, const SimulationResult& badResult, const Circuit::MappedCircuit& circuit)
{
	ASSERT(goodResult.size() == badResult.size());
	std::string result;

	for (size_t timeframeId = 0u; timeframeId < goodResult.size(); ++timeframeId)
	{
		result += "Timeframe " + std::to_string(timeframeId) + "\n";
		result += to_string(goodResult[timeframeId], badResult[timeframeId], circuit);
	}

	return result;
}

std::string to_string(const SimulationTimeframe& goodTimeframe, const SimulationTimeframe& badTimeframe, const Circuit::MappedCircuit& circuit)
{
	std::string result;

	for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
	{
		result += "NodeId: " + std::to_string(nodeId) + ", " + circuit.GetNode(nodeId)->GetName() + ": "
				+ to_string(goodTimeframe[nodeId]) + "/" + to_string(badTimeframe[nodeId]);

		if (goodTimeframe[nodeId] != badTimeframe[nodeId])
		{
			result += " <-----------";
			if (goodTimeframe[nodeId] != Logic::LOGIC_INVALID && goodTimeframe[nodeId] != badTimeframe[nodeId])
			{
				result += " !!!!!";
			}
		}
		result += "\n";
	}
	return result;
}

std::string to_string(const SimulationResult& atpgGoodResult, const SimulationResult& atpgBadResult,
						const SimulationResult& simGoodResult, const SimulationResult& simBadResult, const Circuit::MappedCircuit& circuit)
{
	ASSERT(atpgGoodResult.size() == atpgBadResult.size());
	ASSERT(atpgGoodResult.size() == simGoodResult.size());
	ASSERT(simGoodResult.size() == simBadResult.size());

	std::string result;

	for (size_t timeframeId = 0u; timeframeId < atpgGoodResult.size(); ++timeframeId)
	{
		result += "\nTimeframe " + std::to_string(timeframeId) + "\n";
		for (size_t nodeId = 0u; nodeId < circuit.GetNumberOfNodes(); ++nodeId)
		{
			result += "NodeId: " + std::to_string(nodeId) + ", " + circuit.GetNode(nodeId)->GetName() + ": ATPG= "
					+ to_string(atpgGoodResult[timeframeId][nodeId]) + "/" + to_string(atpgBadResult[timeframeId][nodeId])
					+ " vs " + to_string(simGoodResult[timeframeId][nodeId]) + "/" + to_string(simBadResult[timeframeId][nodeId]) + " =SIM";

			if (atpgGoodResult[timeframeId][nodeId] != simGoodResult[timeframeId][nodeId]
				|| atpgBadResult[timeframeId][nodeId] != simBadResult[timeframeId][nodeId])
			{
				result += " <-----------";
				if ((atpgGoodResult[timeframeId][nodeId] != Logic::LOGIC_INVALID && atpgGoodResult[timeframeId][nodeId] != simGoodResult[timeframeId][nodeId])
					|| (atpgBadResult[timeframeId][nodeId] != Logic::LOGIC_INVALID && atpgBadResult[timeframeId][nodeId] != simBadResult[timeframeId][nodeId]))
				{
					result += " !!!!!";
				}
			}
			result += "\n";
		}
	}

	return result;
}

std::string to_debug(const SimulationResult& goodResult, const SimulationResult& badResult, const Circuit::MappedCircuit& circuit)
{
	ASSERT(goodResult.size() == badResult.size());
	std::string result;

	for (const auto [nodeId, node] : circuit.EnumerateNodes())
	{
		result += "Node " + std::to_string(nodeId) + "(" + node->GetName() + ", " + to_string(node->GetCellType()) + "):\n";
		std::string good;
		std::string bad;
		for (size_t timeframeId = 0u; timeframeId < goodResult.size(); ++timeframeId)
		{
			good += to_string(goodResult[timeframeId][nodeId]);
			bad += to_string(badResult[timeframeId][nodeId]);
		}
		result += "good: " + good + "\nbad:  " + bad + "\n";
	}

	return result;
}

};
};
