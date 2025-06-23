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
class LfsrAtpg:
	public AtpgBase<FaultModel, FaultList>
{
public:
	LfsrAtpg(void);
	virtual ~LfsrAtpg(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

private:
	void GeneratePattern(size_t lfsrId);

	size_t _maxNumLfsrs;
	size_t _maxTimeFrames;

};

};
};
};
