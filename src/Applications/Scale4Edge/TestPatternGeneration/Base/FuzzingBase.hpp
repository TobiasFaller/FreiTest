#pragma once

#include <cstdint>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel, typename FaultList>
class FuzzingBase:
	public virtual AtpgBase<FaultModel, FaultList>
{
public:
	FuzzingBase(std::string configPrefix);
	virtual ~FuzzingBase(void);

	void Init(void) override;
	void Run(void) override;

protected:
	void GenerateCircuit(size_t seed);
	void GenerateFaultModel(size_t seed);
	void GenerateFaultList(size_t seed);

	size_t configSeed = 0;
	size_t configNumCircuits = 1000;
	size_t configNumSimulations = 100;
	size_t configNumInputsMin = 3, configNumInputsMax = 20;
	size_t configNumOutputsMin = 1, configNumOutputsMax = 10;
	size_t configNumCellsMin = 5, configNumCellsMax = 20;
	size_t configNumCellInputsMin = 2, configNumCellInputsMax = 5;

	// Cell-aware state-machine fault model
	size_t configNumFaultsMin = 1, configNumFaultsMax = 3;
	size_t configNumBranchesMin = 1, configNumBranchesMax = 5;
	size_t configNumDontCare = 4;

	// Quick preset for development
	// size_t configSeed = 1;
	// size_t configNumCircuits = 1000;
	// size_t configNumSimulations = 100;
	// size_t configNumInputsMin = 2, configNumInputsMax = 2;
	// size_t configNumOutputsMin = 1, configNumOutputsMax = 1;
	// size_t configNumCellsMin = 1, configNumCellsMax = 1;
	// size_t configNumCellInputsMin = 3, configNumCellInputsMax = 3;
	// size_t configNumFaultsMin = 1, configNumFaultsMax = 1;
	// size_t configNumBranchesMin = 1, configNumBranchesMax = 3;
	// size_t configNumDontCare = 8;

	const std::string& GetCircuitName(void) const { return this->circuit->GetName(); }
	size_t GetNumberOfInputs(void) const { return inputs.size(); }
	size_t GetNumberOfOutputs(void) const { return outputs.size(); }
	size_t GetNumberOfCells(void) const { return cells.size(); }

private:
	struct Cell {
		std::string name;
		std::vector<std::string> inputs;
		std::vector<std::string> outputs;
		std::vector<std::string> logic;
		size_t flipFlopCount;
	};
	std::vector<std::string> inputs;
	std::vector<std::string> outputs;
	std::vector<Cell> cells;
	std::string configPrefix;

};

};
};
};
