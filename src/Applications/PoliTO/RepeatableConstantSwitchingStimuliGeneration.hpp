#include <regex>
#include <fstream>
#include <cstdio>
#include <iomanip>
#include <random>
#include <memory>
#include <variant>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Vcm/VcmMixin.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Fault/Faults/SingleStuckAtFault.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/ApplicationStatistics.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "SolverProxy/MaxSat/IncBmoSolverProxy.hpp"
#include "SolverProxy/MaxSat/PacoseSolverProxy.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"
#include "Tpg/Vcm/VcmCircuit.hpp"


namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

struct MaximizationPhase
{
	size_t firstTimeframeOfStateA;
	size_t firstTimeframeOfStateB;
	size_t firstTimeframeOfStateC;
};

class RepeatableConstantSwitchingStimuliGeneration:
	public virtual Mixin::VcmMixin
{
public:
	RepeatableConstantSwitchingStimuliGeneration();
	virtual ~RepeatableConstantSwitchingStimuliGeneration(void);

	void Run() override;
	void Init() override;
	bool SetSetting(std::string key, std::string value) override;
	Basic::ApplicationStatistics GetStatistics(void) override;

private:

	enum class StressTargetCells { IncludeCellNets, ExcludeCellNets };
	enum class StressTargetSubmodules { IncludeSubmodules, ExcludeSubmodules };
	enum class RiscvCompliant { Yes, No };
	enum class LiteralDChainOptimization { Yes, No };

	struct StressTarget
	{
		Circuit::MappedCircuit::NodeAndPort target;
		size_t clauseWeight;
	};

	Basic::ApplicationStatistics statistics;

    size_t timeframes;
    std::string stressTarget;
	StressTargetCells stressTargetCells;
	StressTargetSubmodules stressTargetSubmodules;

	MaximizationPhase states;
	LiteralDChainOptimization dChainOptimization;
	RiscvCompliant disassemble;

	std::variant<SolverProxy::Sat::IncBmoSolverProxy::Algorithm, SolverProxy::Sat::PacoseEncoding> solverSetting;

	std::string moduleWatcherPath;
	std::string modulePrinterPath;
	std::string vcmWatcherPath;
	std::string vcmPrinterPath;
	std::unique_ptr<Io::ValueWatcherImporter> moduleWatcherImporter;
	std::unique_ptr<Io::ValuePrinterImporter> modulePrinterImporter;
	std::unique_ptr<Io::ValueWatcherImporter> vcmWatcherImporter;
	std::unique_ptr<Io::ValuePrinterImporter> vcmPrinterImporter;

	std::vector<StressTarget> GetStressTargets(void) const;

};



};
};
};
