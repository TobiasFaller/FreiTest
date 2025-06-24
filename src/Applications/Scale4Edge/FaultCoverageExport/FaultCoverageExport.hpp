#pragma once

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Io/FaultCoverage/Coverage.hpp"

#include <string>
#include <vector>
#include <tuple>

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template<typename FaultModel, typename FaultList>
class FaultCoverageExport:
	public virtual AtpgBase<FaultModel, FaultList>
{
public:
	FaultCoverageExport(void);
	virtual ~FaultCoverageExport(void);

	void Run(void) override;

	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

private:
	std::string faultCoverageFileName;
	size_t exportThreadLimit;
	size_t simulationThreadLimit;
	Basic::ApplicationStatistics statistics;

	std::mutex parallelMutex;

	void GenerateFaultList(void);
	std::vector<bool> Simulate(const Pattern::TestPattern& pattern, Pattern::InputCapture capture, const FaultList& faultList);
	Io::FaultCoverage::FaultInformation GetFaultInformation(const FaultModel& fault);

};

};
};
};
