#include "Applications/Scale4Edge/FaultListExport/FaultListExport.hpp"

#include <boost/format.hpp>

#include <cstdint>
#include <execution>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/CpuClock.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Models/SingleTransitionDelayFaultModel.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Iterator/IntegerIterator.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/Sat/Glucose421ParallelSolverProxy.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/FaultPropagationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/StuckAtSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionDelayFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/TransitionSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareFaultEncoder.hpp"
#include "Tpg/LogicGenerator/FaultLogic/CellAwareSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/Vcm/VcmContext.hpp"
#include "Io/ValueWatcherExporter/ValueWatcherExporter.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace FreiTest::Application::Mixin;

namespace FreiTest
{
namespace Application
{
namespace Scale4Edge
{

static const std::string SCALE4EDGE_FAULT_CONFIG = "Scale4Edge/FaultListExport";

template <typename FaultModel, typename FaultList>
FaultListExport<FaultModel, FaultList>::FaultListExport(void):
	StatisticsMixin(SCALE4EDGE_FAULT_CONFIG),
	FaultStatisticsMixin<FaultList>(SCALE4EDGE_FAULT_CONFIG),
	SimulationStatisticsMixin(SCALE4EDGE_FAULT_CONFIG),
	SolverStatisticsMixin(SCALE4EDGE_FAULT_CONFIG),
	VcdExportMixin<FaultList>(SCALE4EDGE_FAULT_CONFIG),
	VcmMixin(SCALE4EDGE_FAULT_CONFIG),
	AtpgBase<FaultModel, FaultList>(SCALE4EDGE_FAULT_CONFIG)
{
}

template <typename FaultModel, typename FaultList>
FaultListExport<FaultModel, FaultList>::~FaultListExport(void) = default;

template <typename FaultModel, typename FaultList>
void FaultListExport<FaultModel, FaultList>::Init(void)
{
	AtpgBase<FaultModel, FaultList>::Init();
}

template <typename FaultModel, typename FaultList>
void FaultListExport<FaultModel, FaultList>::Run(void)
{
	AtpgBase<FaultModel, FaultList>::Run();

	VLOG(6) << to_debug(this->circuit->GetUnmappedCircuit(), VLOG_VERBOSE(9));
	VLOG(6) << to_debug(this->circuit->GetMappedCircuit(), VLOG_VERBOSE(9));

	AtpgBase<FaultModel, FaultList>::GenerateFaultList();
	VLOG(6) << to_debug(this->faultList, this->circuit->GetMappedCircuit());

	AtpgBase<FaultModel, FaultList>::ExportStatistics();
	AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template <typename FaultModel, typename FaultList>
bool FaultListExport<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	return AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template <typename FaultModel, typename FaultList>
Basic::ApplicationStatistics FaultListExport<FaultModel, FaultList>::GetStatistics(void)
{
	return AtpgBase<FaultModel, FaultList>::GetStatistics();
}

template class FaultListExport<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class FaultListExport<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class FaultListExport<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
