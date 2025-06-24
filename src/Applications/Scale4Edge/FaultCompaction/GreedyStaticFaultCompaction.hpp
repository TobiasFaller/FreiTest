#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Udfm/UdfmMixin.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

template <typename FaultModel>
class GreedyCompactionData
{
};

template<>
class GreedyCompactionData<Fault::SingleStuckAtFaultModel> {
public:
	GreedyCompactionData(std::string configPrefix);
	virtual ~GreedyCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<>
class GreedyCompactionData<Fault::SingleTransitionDelayFaultModel> {
public:
	GreedyCompactionData(std::string configPrefix);
	virtual ~GreedyCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<>
class GreedyCompactionData<Fault::CellAwareFaultModel>:
	public Mixin::UdfmMixin
{
public:
	GreedyCompactionData(std::string configPrefix);
	virtual ~GreedyCompactionData(void);

	bool SetSetting(std::string key, std::string value);
	void Init(void);
	void Run(void);

};

template<typename FaultModel, typename FaultList>
class GreedyStaticFaultCompaction:
	public virtual BaseApplication,
	public GreedyCompactionData<FaultModel>
{
public:
    GreedyStaticFaultCompaction(void);
	virtual ~GreedyStaticFaultCompaction(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

protected:
	enum class CompactionOrder { CompactAscendingId, CompactDescendingId };

private:
	void CompactPatternList(std::vector<std::shared_ptr<Pattern::TestPattern>>& patternList, const FaultList& faultList);

	CompactionOrder _compactionOrder;

};

};
};
};
