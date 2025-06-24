#pragma once

#include <string>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Udfm/UdfmMixin.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Basic/Pattern/TestPatternList.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel>
class SatCompactionData
{
};

template<>
class SatCompactionData<Fault::SingleStuckAtFaultModel> {
public:
	SatCompactionData(std::string configPrefix);
	virtual ~SatCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<>
class SatCompactionData<Fault::SingleTransitionDelayFaultModel> {
public:
	SatCompactionData(std::string configPrefix);
	virtual ~SatCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<>
class SatCompactionData<Fault::CellAwareFaultModel>:
	public Mixin::UdfmMixin
{
public:
	SatCompactionData(std::string configPrefix);
	virtual ~SatCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<typename FaultModel, typename FaultList>
class SatStaticFaultCompaction:
	public virtual BaseApplication,
	public SatCompactionData<FaultModel>
{
public:
    SatStaticFaultCompaction(void);
	virtual ~SatStaticFaultCompaction(void);

	void Init(void) override;
	void Run(void) override;

protected:
	enum class SatStaticFaultCompactionMode { FullScan, Sequential };

private:
	Pattern::TestPatternList CompactPatternList(Pattern::TestPatternList& patternList, const FaultList& faultList, Pattern::InputCapture capture);
	void RunFaultSimulation(std::shared_ptr<Pattern::TestPattern> pattern, const FaultList& faultList, std::vector<bool>& faultCoverage, Pattern::OutputCapture capture);

};

};
};
};
