#include "Applications/Scale4Edge/FaultCompaction/GreedyStaticFaultCompaction.hpp"

#include <boost/format.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Pattern/Capture.hpp"
#include "Basic/Pattern/TestPatternList.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Settings.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Io/TestPatternParser/TestPatternParser.hpp"

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

GreedyCompactionData<Fault::SingleStuckAtFaultModel>::GreedyCompactionData(std::string configPrefix)
{
}
GreedyCompactionData<Fault::SingleStuckAtFaultModel>::~GreedyCompactionData(void) = default;

bool GreedyCompactionData<Fault::SingleStuckAtFaultModel>::SetSetting(std::string key, std::string value)
{
	return false;
}

void GreedyCompactionData<Fault::SingleStuckAtFaultModel>::Init(void)
{
}

void GreedyCompactionData<Fault::SingleStuckAtFaultModel>::Run(void)
{
}

GreedyCompactionData<Fault::SingleTransitionDelayFaultModel>::GreedyCompactionData(std::string configPrefix)
{
}
GreedyCompactionData<Fault::SingleTransitionDelayFaultModel>::~GreedyCompactionData(void) = default;

bool GreedyCompactionData<Fault::SingleTransitionDelayFaultModel>::SetSetting(std::string key, std::string value)
{
	return false;
}

void GreedyCompactionData<Fault::SingleTransitionDelayFaultModel>::Init(void)
{
}

void GreedyCompactionData<Fault::SingleTransitionDelayFaultModel>::Run(void)
{
}

GreedyCompactionData<Fault::CellAwareFaultModel>::GreedyCompactionData(std::string configPrefix):
	Mixin::UdfmMixin(configPrefix)
{
}
GreedyCompactionData<Fault::CellAwareFaultModel>::~GreedyCompactionData(void) = default;

bool GreedyCompactionData<Fault::CellAwareFaultModel>::SetSetting(std::string key, std::string value)
{
	return UdfmMixin::SetSetting(key, value);
}

void GreedyCompactionData<Fault::CellAwareFaultModel>::Init(void)
{
	UdfmMixin::Init();
}

void GreedyCompactionData<Fault::CellAwareFaultModel>::Run(void)
{
	UdfmMixin::Run();
}

template <typename FaultModel, typename FaultList>
GreedyStaticFaultCompaction<FaultModel, FaultList>::GreedyStaticFaultCompaction(void):
	BaseApplication(),
	GreedyCompactionData<FaultModel>("Scale4Edge/FaultCompaction"),
	_compactionOrder(CompactionOrder::CompactAscendingId)
{
}

template <typename FaultModel, typename FaultList>
GreedyStaticFaultCompaction<FaultModel, FaultList>::~GreedyStaticFaultCompaction(void) = default;

template <typename FaultModel, typename FaultList>
bool GreedyStaticFaultCompaction<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (key == "Scale4Edge/FaultCompaction/CompactionOrder")
	{
		return Settings::ParseEnum(value, _compactionOrder, {
			{ "Ascending", CompactionOrder::CompactAscendingId },
			{ "Descending", CompactionOrder::CompactDescendingId },
		});
	}

	return false;
}

template <typename FaultModel, typename FaultList>
void GreedyStaticFaultCompaction<FaultModel, FaultList>::Init(void)
{
}

template <typename FaultModel, typename FaultList>
void GreedyStaticFaultCompaction<FaultModel, FaultList>::Run(void)
{
	auto copy_pattern_list = [&](auto& patternsInput, auto& patternsOutput, auto& faultList) {
		switch (_compactionOrder)
		{
		case CompactionOrder::CompactAscendingId:
			std::copy(patternsInput.begin(), patternsInput.end(), std::back_inserter(patternsOutput));
			break;

		case CompactionOrder::CompactDescendingId:
			std::copy(patternsInput.rbegin(), patternsInput.rend(), std::back_inserter(patternsOutput));

			// Patterns have been reversed -> reverse also numbering of detecting patterns too
			for (auto const& [fault, metaData] : faultList)
			{
				if (metaData->detectingPatternId != std::numeric_limits<size_t>::max())
				{
					metaData->detectingPatternId = patternsInput.size() - metaData->detectingPatternId - 1u;
				}
			}
			break;

		default:
			Logging::Panic("Not implemented");
		}
	};

	FileHandle importPatternHandle("[DataImportDirectory]/patterns.json", true);
	auto patternResult = Io::ImportPatterns(importPatternHandle.GetStream(), *this->circuit);
	if (!patternResult) LOG(FATAL) << "Test patterns could not be read";

	FileHandle importMetaDataHandle("[DataImportDirectory]/faults.json", true);
	auto faultResult = [&]() -> std::optional<Io::FaultListExchangeFormat<FaultList>> {
		if constexpr (std::is_same_v<FaultModel, Fault::SingleStuckAtFaultModel>
			|| std::is_same_v<FaultModel, Fault::SingleTransitionDelayFaultModel>)
		{
			return Io::ImportFaults<FaultList>(importMetaDataHandle.GetStream(), *this->circuit);
		}
		else if constexpr (std::is_same_v<FaultModel, Fault::CellAwareFaultModel>)
		{
			ASSERT(this->GetUdfm()) << "No User-Defined Fault Model (UDFM) was loaded";
			return Io::ImportFaults<FaultList>(importMetaDataHandle.GetStream(), *this->circuit, *this->GetUdfm());
		}
		else
		{
			Logging::Panic("Unknown fault model");
		}
	}();
	if (!faultResult) LOG(FATAL) << "Fault data could not be read";

	auto importedPatterns { patternResult->GetTestPatterns() };
	auto inputCapture { patternResult->GetInputCapture() };

	LOG(INFO) << "Imported " << importedPatterns.size() << " patterns";
	for (size_t patternIndex { 0u }; patternIndex < importedPatterns.size(); ++patternIndex)
	{
		LOG(INFO) << "Imported Pattern " << patternIndex << ": " << to_string(*importedPatterns[patternIndex]);
	}

	auto const& faultList { faultResult->GetFaults() };
	std::vector<std::shared_ptr<Pattern::TestPattern>> patternList;
	copy_pattern_list(importedPatterns, patternList, faultList);
	CompactPatternList(patternList, faultList);

	Pattern::TestPatternList exportedPatterns;
	copy_pattern_list(patternList, exportedPatterns, faultList);

	LOG(INFO) << "Exporting " << importedPatterns.size() << " patterns";
	for (size_t patternIndex { 0u }; patternIndex < exportedPatterns.size(); ++patternIndex)
	{
		LOG(INFO) << "Exporting Pattern " << patternIndex << ": " << to_string(*exportedPatterns[patternIndex]);
	}

	FileHandle exportPatternHandle("[DataExportDirectory]/patterns.json", false);
	Io::ExportPatterns(exportPatternHandle.GetOutStream(), { *this->circuit, exportedPatterns, inputCapture });

	FileHandle exportMetaDataHandle("[DataExportDirectory]/faults.json", false);
	Io::FaultListExchangeFormat<FaultList> faultData { *this->circuit, faultList };
	Io::ExportFaults(exportMetaDataHandle.GetOutStream(), faultData);
}

template <typename FaultModel, typename FaultList>
void GreedyStaticFaultCompaction<FaultModel, FaultList>::CompactPatternList(std::vector<std::shared_ptr<Pattern::TestPattern>>& patternList, const FaultList& faultList)
{
	for (size_t i = 0; i < patternList.size()-1; i++)
	{
		for (size_t j = i+1; j < patternList.size(); j++)
		{
			if (patternList[i]->IsCompatible(*patternList[j]) || patternList[j]->IsCompatible(*patternList[i]))
			{
				if (patternList[i]->GetNumberOfTimeframes() < patternList[j]->GetNumberOfTimeframes())
				{
					VLOG(3) << "Merged " << i << " into " << j << " (storing to id " <<  i << ")";
					patternList[j]->Compact(*patternList[i]);
					patternList[i] = patternList[j];
				}
				else
				{
					VLOG(3) << "Merged " << j << " into " << i << " (storing to id " <<  i << ")";
					patternList[i]->Compact(*patternList[j]);
				}

				for (auto [fault, metadata] : faultList)
				{
					if (metadata->detectingPatternId == j) metadata->detectingPatternId = i;
					if (metadata->detectingPatternId > j && metadata->detectingPatternId != std::numeric_limits<size_t>::max()) metadata->detectingPatternId -= 1;
				}

				patternList.erase(patternList.begin() + j);
			}
		}
	}
}

template class GreedyStaticFaultCompaction<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class GreedyStaticFaultCompaction<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class GreedyStaticFaultCompaction<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
