#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Pattern/TestPatternList.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class SequentialAtpgBase:
	public virtual AtpgBase<FaultModel, FaultList>
{
public:
	SequentialAtpgBase(std::string configPrefix);
	virtual ~SequentialAtpgBase(void);

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;

protected:
	enum class CombinationalTestabilityCheck { Enabled, Disabled };
	enum class StartingState { Unconstrained, AllZeroes, AllOnes, AllDontCares, AllUnknowns, Custom };

	bool SimulateTestPattern(size_t faultIndex, const Pattern::TestPattern& pattern);

	CombinationalTestabilityCheck settingsCombinationalTestabilityCheck;
	size_t settingsMaximumTimeframes;
	StartingState settingsStartingState;
	std::string settingsCustomStartingState;
	std::vector<Basic::Logic> startingState;

private:
	std::string configPrefix;
};

};
};
};
