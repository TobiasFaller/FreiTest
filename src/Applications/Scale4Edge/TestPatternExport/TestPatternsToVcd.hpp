#pragma once

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class TestPatternsToVcd:
	public AtpgBase<FaultModel, FaultList>
{
public:
	TestPatternsToVcd(void);
	virtual ~TestPatternsToVcd(void);

	void Init(void) override;
	void Run(void) override;

	bool SetSetting(std::string key, std::string value) override;

private:
	size_t ExportThreadLimit;

};

};
};
};
