#pragma once

#include <cstdint>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/FuzzingBase.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class SatFullScanFuzzing:
	public FuzzingBase<FaultModel, FaultList>
{
public:
	SatFullScanFuzzing(void);
	virtual ~SatFullScanFuzzing(void);

	void Init(void) override;
	void Run(void) override;

private:
	void GeneratePatternForFault(size_t seed, size_t faultIndex);

};

};
};
};
