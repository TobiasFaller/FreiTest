#pragma once

#include "Io/VcdExporter/VcdModel.hpp"

#include "Basic/Logic.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Simulation/CircuitSimulationResult.hpp"

#include <string>
#include <ctime>
#include <utility>
#include <map>
#include <set>

namespace FreiTest
{
namespace Io
{
namespace Vcd
{

class VcdModelBuilder
{
public:
	VcdModelBuilder();
	virtual ~VcdModelBuilder();

	void Initialize(const Circuit::CircuitEnvironment& circuit);
	VcdModel BuildVcdModel(const Simulation::SimulationResult& simulationResult,
		const Circuit::CircuitEnvironment& circuit, std::string version, int timelapse = 1, std::time_t time = 0) const;

private:
	std::string CreateReference();
	std::string GetOrCreateReference(std::size_t connectionId);
	std::shared_ptr<Scope> CreateScope(const Circuit::CircuitEnvironment& circuit, const Circuit::GroupMetaData* group = nullptr);

	void BuildVcdHeader(VcdModel& model, std::string version, int timelapse, std::time_t time = 0) const;
	void BuildVcdData(VcdModel& model, const Simulation::SimulationResult& sim, const Circuit::CircuitEnvironment& circuit) const;

	VcdModel model;
	Timeframe defaultTimeframe;
	std::vector<int> nextReference;
	std::map<std::string, std::shared_ptr<Scope>> scopes;
	std::map<std::size_t, std::string> references;
	std::map<std::size_t, std::pair<std::string, std::size_t>> connectionIdToBusRefAndIndex;

};

};
};
};
