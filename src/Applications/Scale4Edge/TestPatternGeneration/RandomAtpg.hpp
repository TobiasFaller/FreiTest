#pragma once

#include <cstdint>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class RandomAtpg:
	public AtpgBase<FaultModel, FaultList>
{
public:
	RandomAtpg(void);
	virtual ~RandomAtpg(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

private:
	Basic::Logic IntToLogic(int value);
	void GeneratePattern(size_t iteration);
	size_t _numberOfPatterns;
	size_t _numberOfTimeframes;
	bool _injectDontCares;
};

};
};
};
