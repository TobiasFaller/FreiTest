#pragma once

#include <cstdint>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/FuzzingBase.hpp"

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
	bool SetSetting(std::string key, std::string value) override;

private:
	enum class ScanChainType {
		LaunchOnShift, // aka. skewed-load
		// TODO: LaunchOnCapture, // aka. broadside, double-capture
		EnhancedScan, // aka. enhanced-scan, hold-scan
	};

	void GeneratePatternForFault(size_t seed, size_t faultIndex);

	ScanChainType scanChainType;
};

};
};
};
