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
class FaultListExport:
	public AtpgBase<FaultModel, FaultList>
{
public:
	FaultListExport(void);
	virtual ~FaultListExport(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

};

};
};
};
