#pragma once

#include <cstdint>
#include <string>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/FuzzingBase.hpp"
#include "Applications/Scale4Edge/TestPatternGeneration/Base/SequentialAtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class BmcSequentialFuzzing:
	public FuzzingBase<FaultModel, FaultList>,
	public SequentialAtpgBase<FaultModel, FaultList>
{
public:
	BmcSequentialFuzzing(void);
	virtual ~BmcSequentialFuzzing(void);

	void Init(void) override;
	void Run(void) override;

protected:
	void GeneratePatternForFault(size_t seed, size_t faultIndex);

};

};
};
};
