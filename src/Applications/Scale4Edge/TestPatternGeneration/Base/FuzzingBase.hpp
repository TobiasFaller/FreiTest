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
	enum class UdfmType { FullScan, Functional };
	void GenerateCircuit(size_t seed);
	void GenerateFaultModel(size_t seed, UdfmType udfmType);
	void GenerateFaultList(size_t seed);

	size_t configSeed = 0;
	size_t configNumCircuits = 1000;
	size_t configNumSimulations = 10000;
	size_t configNumInputsMin = 10, configNumInputsMax = 20;
	size_t configNumOutputsMin = 10, configNumOutputsMax = 20;
	size_t configNumCellsMin = 1, configNumCellsMax = 20;
	size_t configNumCellInputsMin = 2, configNumCellInputsMax = 5;
	size_t configNumUdfmFaultsMin = 1, configNumUdfmFaultsMax = 5;
	size_t configNumUdfmAltsMin = 1, configNumUdfmAltsMax = 5;
	size_t configNumUdfmDontCare = 4;

	// Quick preset for development
	// size_t configSeed = 1;
	// size_t configNumCircuits = 1000;
	// size_t configNumSimulations = 100;
	// size_t configNumInputsMin = 2, configNumInputsMax = 2;
	// size_t configNumOutputsMin = 1, configNumOutputsMax = 1;
	// size_t configNumCellsMin = 1, configNumCellsMax = 1;
	// size_t configNumCellInputsMin = 3, configNumCellInputsMax = 3;
	// size_t configNumUdfmFaultsMin = 1, configNumUdfmFaultsMax = 1;
	// size_t configNumUdfmAltsMin = 1, configNumUdfmAltsMax = 3;
	// size_t configNumUdfmDontCare = 8;

private:
	struct Cell {
		std::string name;
		std::vector<std::string> inputs;
		std::vector<std::string> outputs;
		std::vector<std::string> logic;
		size_t flipFlopCount;
	};
	std::vector<Cell> cells;
	std::string configPrefix;

};

};
};
};
