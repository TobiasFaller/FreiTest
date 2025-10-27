#include "Applications/PoliTO/MacishSafeFaultAnalysis.hpp"

#include <cmath>
#include <memory>
#include <tuple>
#include <optional>
#include <random>

#include <boost/format.hpp>
#include <boost/utility.hpp>
#include <mpfr.h>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/ValuePrinter.hpp"
#include "Helper/ValueWatcher.hpp"
#include "Circuit/DriverFinder.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"
#include "Io/FaultListParser/FaultListParser.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"
#include "Tpg/LogicGenerator/Tagger/DChain/DChainCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/StuckAtFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/TransitionDelayFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Fault/CellAwareFaultCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/Constraint/PortConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/NetSensitizationConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/DChainBaseEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainEncoder.hpp"
#include "Tpg/LogicGenerator/DChain/BackwardDChainConnectionEncoder.hpp"
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
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/MaxSat/MaxSatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Tpg/Sbst/SbstContext.hpp"
#include "Tpg/Sbst/SbstProgram.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"

using namespace SolverProxy;
using namespace FreiTest::Basic;
using namespace FreiTest::Fault;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

struct LimitResult
{
	enum class Category { Sat, Unsat, Timeout };
	friend std::string to_string(const Category& category);

	uint64_t exact;
	uint64_t macish;
	uint64_t difference;
	double relative;
	double absolute;
	std::optional<Pattern::TestPattern> pattern;
	std::vector<Helper::ValueWatcher::WatcherValues> watches;
	std::vector<LogicConstraint> constraints;
	uint64_t maxDifference;
	double maxAbsolute;
	Category category;

	LimitResult(Category category):
		exact(0u),
		macish(0u),
		difference(0),
		relative(0.0),
		absolute(0.0),
		pattern(std::nullopt),
		watches(),
		constraints(),
		maxDifference(0),
		maxAbsolute(0.0),
		category(category)
	{}

	LimitResult(uint64_t exact, uint64_t macish, uint64_t difference, double relative, double absolute,
		std::optional<Pattern::TestPattern> pattern,
		std::vector<Helper::ValueWatcher::WatcherValues> watches,
		std::vector<LogicConstraint> constraints,
		uint64_t maxDifference, double maxAbsolute, Category category
	):
		exact(exact),
		macish(macish),
		difference(difference),
		relative(relative),
		absolute(absolute),
		pattern(pattern),
		watches(watches),
		constraints(constraints),
		maxDifference(maxDifference),
		maxAbsolute(maxAbsolute),
		category(category)
	{}
};

struct LimitsResult
{
	LimitResult limitPos;
	LimitResult limitNeg;
};

struct StatsResult
{
	mpfr_t errorRate;
	mpfr_t errorMean;
	mpfr_t errorMeanNorm;
	mpfr_t errorMax;
	mpfr_t errorMaxNorm;
	mpfr_t errorOverflow;
	mpfr_t errorUnderflow;

	StatsResult():
		errorRate(),
		errorMean(),
		errorMeanNorm(),
		errorMax(),
		errorMaxNorm(),
		errorOverflow(),
		errorUnderflow()
	{
		mpfr_init2(errorRate, 1024); mpfr_set_ui(errorRate, 0ul, MPFR_RNDN);
		mpfr_init2(errorMean, 1024); mpfr_set_ui(errorMean, 0ul, MPFR_RNDN);
		mpfr_init2(errorMeanNorm, 1024); mpfr_set_ui(errorMeanNorm, 0ul, MPFR_RNDN);
		mpfr_init2(errorMax, 1024); mpfr_set_ui(errorMax, 0ul, MPFR_RNDN);
		mpfr_init2(errorMaxNorm, 1024); mpfr_set_ui(errorMaxNorm, 0ul, MPFR_RNDN);
		mpfr_init2(errorOverflow, 1024); mpfr_set_ui(errorOverflow, 0ul, MPFR_RNDN);
		mpfr_init2(errorUnderflow, 1024); mpfr_set_ui(errorUnderflow, 0ul, MPFR_RNDN);
	}
};

std::string to_string(const LimitResult::Category& category) {
	switch (category) {
		case LimitResult::Category::Sat: return "Sat";
		case LimitResult::Category::Unsat: return "Unsat";
		case LimitResult::Category::Timeout: return "Timeout";
	}
	__builtin_unreachable();
}

static const std::string SETTINGS_PREFIX = "PoliTO/MacishSafeFaultAnalysis";

template<typename FaultModel, typename PinData> struct AtpgConfig { };
template<typename PinData> struct AtpgConfig<Fault::SingleStuckAtFaultModel, PinData> {
	using FaultGenerator = Tpg::StuckAtFaultEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitization = Tpg::StuckAtSensitizationConstraintEncoder<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultSensitizationTimeframe = Tpg::StuckAtSensitizationTimeframe;
	using FaultTagger = Tpg::StuckAtFaultCircuitTagger<PinData, Fault::SingleStuckAtFaultModel>;
	using FaultTaggerMode = Tpg::StuckAtFaultTaggingMode;
};
template<typename PinData> struct AtpgConfig<Fault::SingleTransitionDelayFaultModel, PinData> {
	using FaultGenerator = Tpg::TransitionDelayFaultEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitization = Tpg::TransitionSensitizationConstraintEncoder<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultSensitizationTimeframe = Tpg::TransitionSensitizationTimeframe;
	using FaultTagger = Tpg::TransitionDelayFaultCircuitTagger<PinData, Fault::SingleTransitionDelayFaultModel>;
	using FaultTaggerMode = Tpg::TransitionDelayFaultTaggingMode;
};
template<typename PinData> struct AtpgConfig<Fault::CellAwareFaultModel, PinData> {
	using FaultGenerator = Tpg::CellAwareFaultEncoder<PinData>;
	using FaultSensitization = Tpg::CellAwareSensitizationConstraintEncoder<PinData>;
	using FaultSensitizationTimeframe = Tpg::CellAwareSensitizationTimeframe;
	using FaultTagger = Tpg::CellAwareFaultCircuitTagger<PinData>;
	using FaultTaggerMode = Tpg::CellAwareFaultTaggingMode;
};

static size_t GetRequiredTimeframeCount(const Fault::SingleStuckAtFaultModel& faultModel) { return 1u; }
static size_t GetRequiredTimeframeCount(const Fault::SingleTransitionDelayFaultModel& faultModel) { return 2u; }
static size_t GetRequiredTimeframeCount(const Fault::CellAwareFaultModel& faultModel) { return faultModel.GetFault()->GetTimeframeSpread(); }

template<typename FaultModel, typename FaultList>
MacishSafeFaultAnalysis<FaultModel, FaultList>::MacishSafeFaultAnalysis():
	Mixin::StatisticsMixin(SETTINGS_PREFIX),
	Mixin::FaultStatisticsMixin<FaultList>(SETTINGS_PREFIX),
	Mixin::SimulationStatisticsMixin(SETTINGS_PREFIX),
	Mixin::SolverStatisticsMixin(SETTINGS_PREFIX),
	Mixin::VcdExportMixin<FaultList>(SETTINGS_PREFIX),
	Mixin::VcmMixin(SETTINGS_PREFIX),
	Scale4Edge::AtpgBase<FaultModel, FaultList>(SETTINGS_PREFIX),
	target(Target::Multiplier),
	bits(Bits::Bits8),
	complete(Complete::Complete),
	trace(Trace::Off),
	incompleteMinDigits(0u),
	samples(100000u)
{
	this->vcmEnable = VcmEnable::Enabled;
}

template<typename FaultModel, typename FaultList>
MacishSafeFaultAnalysis<FaultModel, FaultList>::~MacishSafeFaultAnalysis() = default;

template<typename FaultModel, typename FaultList>
bool MacishSafeFaultAnalysis<FaultModel, FaultList>::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "Target", SETTINGS_PREFIX))
	{
		return Settings::ParseEnum(value, target, {
			{ "Macish", Target::Macish },
			{ "Multiplier", Target::Multiplier },
		});
	}
	if (Settings::IsOption(key, "Bits", SETTINGS_PREFIX))
	{
		return Settings::ParseEnum(value, bits, {
			{ "8", Bits::Bits8 },
			{ "16", Bits::Bits16 },
			{ "32", Bits::Bits32 },
		});
	}
	if (Settings::IsOption(key, "Complete", SETTINGS_PREFIX))
	{
		return Settings::ParseEnum(value, complete, {
			{ "Incomplete", Complete::Incomplete },
			{ "Complete", Complete::Complete },
		});
	}
	if (Settings::IsOption(key, "Trace", SETTINGS_PREFIX))
	{
		return Settings::ParseEnum(value, trace, {
			{ "On", Trace::On },
			{ "Off", Trace::Off },
		});
	}
	if (Settings::IsOption(key, "MinDigits", SETTINGS_PREFIX))
	{
		return Settings::ParseSizet(value, incompleteMinDigits);
	}
	if (Settings::IsOption(key, "Samples", SETTINGS_PREFIX))
	{
		return Settings::ParseSizet(value, samples);
	}

	return Mixin::SimulationStatisticsMixin::SetSetting(key, value)
		|| Mixin::SolverStatisticsMixin::SetSetting(key, value)
		|| Mixin::VcmMixin::SetSetting(key, value)
		|| Scale4Edge::AtpgBase<FaultModel, FaultList>::SetSetting(key, value);
}

template<typename FaultModel, typename FaultList>
void MacishSafeFaultAnalysis<FaultModel, FaultList>::Init()
{
	Mixin::SimulationStatisticsMixin::Init();
	Mixin::SolverStatisticsMixin::Init();
	Mixin::VcmMixin::Init();
	Scale4Edge::AtpgBase<FaultModel, FaultList>::Init();
}

template<typename FaultModel, typename FaultList>
void MacishSafeFaultAnalysis<FaultModel, FaultList>::Run()
{
	if (this->vcmEnable != VcmMixin::VcmEnable::Enabled)
	{
		LOG(FATAL) << "VCM has to be enabled";
	}

	Scale4Edge::AtpgBase<FaultModel, FaultList>::Run();
	Scale4Edge::AtpgBase<FaultModel, FaultList>::GenerateFaultList();

	auto const mpfr_to_string = [](auto const& value) -> std::string {
		mpfr_exp_t exponent;
		auto* str { mpfr_get_str(nullptr, &exponent, 10, 10, value, MPFR_RNDN) };
		auto const isNegative { mpfr_sgn(value) < 0 };
		auto const result { boost::str(boost::format("%s0.%se%ld")
			% (isNegative ? "-" : "") % (isNegative ? str + 1 : str) % static_cast<long>(exponent)) };
		mpfr_free_str(str);
		return result;
	};
	auto find_limits = [&](size_t faultIndex) -> LimitsResult {
		// If testing for fault-free use the last fault as dummy
		bool faultFree = (faultIndex == std::numeric_limits<size_t>::max());
		faultIndex = std::min(faultIndex, this->faultList.size() - 1u);

		auto const& [fault, metaData] = this->faultList[faultIndex];
		Logging::SetCurrentFault(faultIndex);

		using PinData = Tpg::PinDataGBD<LogicContainer01>;
		using VcmPinData = Tpg::PinDataG<LogicContainer01>;

		using FaultGenerator = typename AtpgConfig<FaultModel, PinData>::FaultGenerator;
		//using FaultSensitization = typename AtpgConfig<FaultModel, PinData>::FaultSensitization;
		//using FaultSensitizationTimeframe = typename AtpgConfig<FaultModel, PinData>::FaultSensitizationTimeframe;
		using FaultTagger = typename AtpgConfig<FaultModel, PinData>::FaultTagger;
		using FaultTaggerMode = typename AtpgConfig<FaultModel, PinData>::FaultTaggerMode;

		std::shared_ptr<Sat::SatSolverProxy> solver = Sat::SatSolverProxy::CreateSatSolver(Settings::GetInstance()->SatSolver);
		ASSERT (solver) << "Could not initialize SAT-Solver!";
		solver->SetSolverTimeout(600);

		// The logic generator combines all colorization, encoding and constraint modules into one generator.
		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(solver, this->circuit);
		auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>>(solver, this->vcmCircuit);
		logicGenerator->GetContext().SetIncremental(false);
		vcmLogicGenerator->GetContext().SetIncremental(false);

		auto faultModel = std::make_shared<FaultModel>(fault);
		logicGenerator->template EmplaceModule<FaultGenerator>(faultModel);

		// The good value of all secondary outputs is required -> encode everything with good encoding.
		if (this->vcmEnable == VcmMixin::VcmEnable::Enabled)
		{
			// The whole circuit is marked with good encoding and the fault location and it's output cone is marked with bad encoding.
			logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
		}
		auto faultColorizer = logicGenerator->template EmplaceModule<FaultTagger>(faultModel);
		faultColorizer->SetTaggingMode(FaultTaggerMode::Functional);
		if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainCircuitTagger<PinData>>();

		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, BadTag>>(LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, BadTag>>(MakeUnclockedSetResetFlipFlopModel());
		//logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
		//logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, BadTag>>();
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
		logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, BadTag>>();
		if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::DChainBaseEncoder<PinData>>();
		if constexpr (has_diff_encoding_v<PinData>) logicGenerator->template EmplaceModule<Tpg::BackwardDChainEncoder<PinData>>();

		logicGenerator->template EmplaceModule<Tpg::PortConstraintEncoder<PinData, LDiffTag>>(Tpg::MakeConstantPortConstraint(Logic::LOGIC_ZERO), Tpg::ConstrainedPorts::Inputs);
		logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
		logicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, BadTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);

		// Not required since the maximization target is on the last timeframe that would have a fault activation.
		//if (!faultFree) {
		//	logicGenerator->template EmplaceModule<FaultSensitization>(faultModel, FaultSensitizationTimeframe::Any);
		//	logicGenerator->template EmplaceModule<Tpg::FaultPropagationConstraintEncoder<PinData>>(Tpg::FaultPropagationTarget::PrimaryOutputsOnly, Tpg::FaultPropagationTimeframe::Last);
		//}

		// Encode the whole circuit with good encoding only as there is no fault model for the VCM circuit
		vcmLogicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(logicGenerator, this->vcmInputs);
		vcmLogicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, GoodTag>>(MakeUnclockedSetResetFlipFlopModel());
		//vcmLogicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->template EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(this->vcmOutputs);
		vcmLogicGenerator->template EmplaceModule<Tpg::SequentialConstraintEncoder<VcmPinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
		vcmLogicGenerator->template EmplaceModule<Tpg::UnaryCounterEncoder<VcmPinData>>();

		std::unique_ptr<FileHandle> traceHandle;
		std::ostream* traceStream;
		if (trace == Trace::On)
		{
			auto name { faultFree ? "[DataExportDirectory]/traces/fault_free.txt"
				: boost::str(boost::format("[DataExportDirectory]/traces/fault_%d.txt") % faultIndex) };
			traceHandle = std::make_unique<FileHandle>(Settings::GetInstance()->MapFileName(name), false);
			traceStream = &traceHandle->GetOutStream();
			if (faultFree)
			{
				*traceStream << "Fault Free" << std::endl;
			}
			else
			{
				*traceStream << "Fault " << to_string(*fault) << std::endl;
			}
		}

		auto timeframes { GetRequiredTimeframeCount(*faultModel) + ((target == Target::Macish) ? 2u : 0u) };
		logicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
		if (this->vcmEnable == VcmMixin::VcmEnable::Enabled) vcmLogicGenerator->GetContext().SetNumberOfTimeframes(timeframes);
		if (!logicGenerator->GenerateCircuitLogic()
			|| ((this->vcmEnable == VcmMixin::VcmEnable::Enabled) && !vcmLogicGenerator->GenerateCircuitLogic()))
		{
			if (trace == Trace::On)
			{
				*traceStream << "Unpropagateable" << std::endl;
			}
			return {
				.limitPos = { LimitResult::Category::Unsat },
				.limitNeg = { LimitResult::Category::Unsat },
			};
		}

		auto& context { logicGenerator->GetContext() };
		auto& vcmContext { vcmLogicGenerator->GetContext() };
		auto& encoder { context.GetEncoder() };

		auto const get_values = [&encoder](auto const& container) -> std::vector<Logic> {
			std::vector<Logic> result;
			for (auto& c : container)
			{
				result.push_back(encoder.GetSolvedLogicValue(c));
			}
			return result;
		};
		auto const logic_to_number = [&](auto const& values) -> uint64_t {
			bool negative = (values.back() == Logic::LOGIC_ONE);
			uint64_t result = 0ull;
			for (size_t i { 0u }; i < values.size() - 1u; i++)
			{
				if (values[i] == Logic::LOGIC_ONE)
				{
					result |= (1ull << i);
				}
			}
			return negative ? (((~result) + 1ull) & ((1ull << (values.size() - 1ull)) - 1ull)) : result;
		};
		auto const constraint_to_logic = [](auto const& constraints) {
			std::vector<Logic> result;
			for (auto& element : constraints)
			{
				result.push_back(GetLogicForConstraint(element));
			}
			return result;
		};
		auto const replace_dontcare = [](auto const& constraints) {
			std::vector<LogicConstraint> result;
			for (auto& element : constraints)
			{
				result.push_back((element == LogicConstraint::NO_CONSTRAINT)
					? LogicConstraint::ONLY_LOGIC_ONE
					: element);
			}
			return result;
		};
		auto const evaluate = [&](auto& exact, auto& macish, auto& difference, auto& constraints, bool positive) -> LimitResult {
			std::optional<Pattern::TestPattern> resultPattern { };
			std::vector<Helper::ValueWatcher::WatcherValues> resultWatches { };
			LimitResult::Category resultCategory { LimitResult::Category::Timeout };
			uint64_t resultExact { 0u };
			uint64_t resultMacish { 0u };
			uint64_t resultDifference { 0u };

			for (size_t index { 0u }; index <= 2*+bits + (positive ? 0u : 1u); index++)
			{
				if (index < 2*+bits)
				{
					// Set highest bit to assume the value is included in terms.
					constraints[2*+bits - 1u - index] = positive
						? LogicConstraint::ONLY_LOGIC_ONE
						: LogicConstraint::ONLY_LOGIC_ZERO;
				}

solve_again:
				solver->ClearAssumptions();
				for (size_t i { 0u }; i < constraints.size(); i++)
				{
					solver->AddAssumption(encoder.EncodeLogicValueConstraintDetector(difference[i], constraints[i]).l0);
				}

				CpuClock solveTimer;
				solveTimer.SetTimeReference();
				auto status { solver->Solve() };
				switch (status)
				{
					case Sat::SatResult::SAT:
					{
						if (trace == Trace::On)
						{
							*traceStream << "Assuming " << to_string(constraints) << " -> SAT   ("
								<< std::fixed << std::setprecision(3) << solveTimer.RunTimeSinceReference() << " s)" << std::endl;
						}

						resultCategory = LimitResult::Category::Sat;
						resultPattern = Tpg::Extractor::ExtractTestPattern<PinData, GoodTag>(
							context, Pattern::InputCapture::PrimaryAndSecondaryInputs);

						auto circuitSimulationGood { Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(context) };
						auto circuitSimulationBad { Tpg::Extractor::ExtractSimulationResult<PinData, BadTag>(context) };
						auto vcmSimulation { Tpg::Extractor::ExtractSimulationResult<VcmPinData, GoodTag>(vcmContext) };

						Helper::ValueWatcher circuitWatcherGood { };
						Helper::ValueWatcher circuitWatcherBad { };
						Helper::ValueWatcher vcmWatcher { };
						for (auto& watcher : this->circuitWatcherImporter->ApplyWatchers(circuitWatcherGood, *this->circuit, { }, ""))
						{
							watcher->name = watcher->name + "_good";
						}
						for (auto& watcher : this->circuitWatcherImporter->ApplyWatchers(circuitWatcherBad, *this->circuit, { }, ""))
						{
							watcher->name = watcher->name + "_bad";
						}
						this->vcmWatcherImporter->ApplyWatchers(vcmWatcher, *this->vcmCircuit, { }, "");
		
						std::vector<Helper::ValueWatcher::WatcherValues> watches;
						auto circuitWatchesGood { circuitWatcherGood.GetWatcherValues(circuitSimulationGood) };
						auto circuitWatchesBad { circuitWatcherBad.GetWatcherValues(circuitSimulationBad) };
						auto vcmWatches { vcmWatcher.GetWatcherValues(vcmSimulation) };
						watches.insert(watches.end(), std::make_move_iterator(circuitWatchesGood.begin()), std::make_move_iterator(circuitWatchesGood.end()));
						watches.insert(watches.end(), std::make_move_iterator(circuitWatchesBad.begin()), std::make_move_iterator(circuitWatchesBad.end()));
						watches.insert(watches.end(), std::make_move_iterator(vcmWatches.begin()), std::make_move_iterator(vcmWatches.end()));
						resultWatches = watches;

						Helper::ValuePrinter printer { };
						printer.AddTitle("Circuit Good");
						for (auto& printer : this->circuitPrinterImporter->ApplyPrinters(printer, { }, ""))
						{
							if (auto* watcherPrinter = std::get_if<Helper::ValuePrinter::WatcherConfig>(&(*printer)); watcherPrinter)
							{
								watcherPrinter->watcher = watcherPrinter->watcher + "_good";
							}
						}
						printer.AddDivider(Helper::ValuePrinter::DivideType::LightSolid);
						printer.AddTitle("Circuit Bad");
						for (auto& printer : this->circuitPrinterImporter->ApplyPrinters(printer, { }, ""))
						{
							if (auto* watcherPrinter = std::get_if<Helper::ValuePrinter::WatcherConfig>(&(*printer)); watcherPrinter)
							{
								watcherPrinter->watcher = watcherPrinter->watcher + "_bad";
							}
						}
						printer.AddDivider(Helper::ValuePrinter::DivideType::LightSolid);
						printer.AddTitle("VCM");
						this->vcmPrinterImporter->ApplyPrinters(printer, { }, "");

						resultExact = logic_to_number(get_values(exact));
						resultMacish = logic_to_number(get_values(macish));
						resultDifference = logic_to_number(get_values(difference));

						if ((index >= 2*+bits - 1u)
							|| ((complete == Complete::Incomplete) && ((index + 1u) >= incompleteMinDigits)))
						{
							if (trace == Trace::On)
							{
								if (complete == Complete::Incomplete)
								{
									*traceStream << "Incomplete result is " << replace_dontcare(constraints) << std::endl;
								}
								*traceStream << printer.ToPrintable(watches) << std::endl;
							}
							goto end_loop;
						}
						continue;
					}

					case Sat::SatResult::UNSAT:
						if (trace == Trace::On)
						{
							*traceStream << "Assuming " << to_string(constraints) << " -> UNSAT ("
								<< std::fixed << std::setprecision(3) << solveTimer.RunTimeSinceReference() << " s)" << std::endl;
						}

						if (index == 2*+bits + (positive ? 0u : 1u))
						{
							//Logging::Panic("Can't find model for unconstrained problem. Something went really wrong.");
							resultCategory = LimitResult::Category::Unsat;
							resultExact = 0ull;
							resultMacish = 0ull;
							resultDifference = 0ull;
							goto end_loop;
						}
						else if (index == 2*+bits)
						{
							// Switch to signed 0 and make sure a model can be found.
							constraints = std::vector<LogicConstraint>(2*+bits + 1u, LogicConstraint::ONLY_LOGIC_ZERO);
						}
						else if (constraints[2*+bits - 1u - index] != (positive ? LogicConstraint::ONLY_LOGIC_ZERO : LogicConstraint::ONLY_LOGIC_ONE))
						{
							// Flip highest bit to assume the value is not included in terms.
							constraints[2*+bits - 1u - index] = positive
								? LogicConstraint::ONLY_LOGIC_ZERO
								: LogicConstraint::ONLY_LOGIC_ONE;

							if ((complete == Complete::Incomplete) && (incompleteMinDigits > 0u) && ((index + 1u) >= incompleteMinDigits))
							{
								goto solve_again;
							}
						}
						else
						{
							resultCategory = LimitResult::Category::Unsat;
							resultExact = 0ull;
							resultMacish = 0ull;
							resultDifference = 0ull;
							goto end_loop;
						}
						continue;

					case Sat::SatResult::UNKNOWN:
					default:
						if (trace == Trace::On)
						{
							*traceStream  << "Assuming " << to_string(constraints) << " -> UNKNOWN ("
								<< std::fixed << std::setprecision(3) << solveTimer.RunTimeSinceReference() << " s)" << std::endl;
						}
						return LimitResult(LimitResult::Category::Timeout);
				}
			}

end_loop:
			{
				auto const maxValue { std::pow(2.0, 2*+bits) - 1.0 };
				ASSERT(std::isfinite(maxValue));

				auto resultRelative { std::abs(static_cast<double>(resultMacish) - static_cast<double>(resultExact)) / std::max(1.0, static_cast<double>(resultExact)) };
				auto resultAbsolute { std::abs(static_cast<double>(resultMacish) - static_cast<double>(resultExact)) / maxValue };
				auto resultConstraints { (complete == Complete::Incomplete) ? replace_dontcare(constraints) : constraints };
				auto resultMaxDifference { logic_to_number(constraint_to_logic(resultConstraints)) };
				auto resultMaxAbsolute { std::abs(static_cast<double>(resultMaxDifference)) / maxValue };
				ASSERT(std::isfinite(resultRelative));
				ASSERT(std::isfinite(resultAbsolute));
				ASSERT(std::isfinite(resultMaxAbsolute));

				if (trace == Trace::On)
				{
					*traceStream << "Exact:         " << resultExact      << std::endl;
					*traceStream << "Macish:        " << resultMacish     << std::endl;
					*traceStream << "Difference:    " << resultDifference << std::endl;
					*traceStream << "Relative:      " << resultRelative   << std::endl;
					*traceStream << "Absolute:      " << resultAbsolute   << std::endl;
					*traceStream << "Pattern:       " << (resultPattern.has_value() ? to_string(*resultPattern) : "None") << std::endl;
					*traceStream << "Constraints:   " << to_string(resultConstraints) << std::endl;
					*traceStream << "MaxDifference: " << resultMaxDifference << std::endl;
					*traceStream << "MaxAbsolute:   " << resultMaxAbsolute << std::endl;
					*traceStream << "Watches:" << std::endl;

					for (auto& [name, timeframes] : resultWatches)
					{
						*traceStream << "- " << name << ": ";
						size_t fIndex { 0u };
						for (auto& timeframe : timeframes)
						{
							if (fIndex++ != 0u) *traceStream << " -> ";
							*traceStream << to_string(timeframe);
						}
						*traceStream << std::endl;
					}

					*traceStream << std::endl;
				}

				return LimitResult(
					resultExact,
					resultMacish,
					resultDifference,
					resultRelative,
					resultAbsolute,
					resultPattern,
					resultWatches,
					resultConstraints,
					resultMaxDifference,
					resultMaxAbsolute,
					resultCategory
				);
			}
		};

		const auto get_container = [&](const Circuit::MappedNode* node, Circuit::PortType portType, size_t index) {
			auto& context { vcmLogicGenerator->GetContext() };
			auto& timeframe { context.GetTimeframe(logicGenerator->GetContext().GetNumberOfTimeframesForTarget(Tpg::GenerationTarget::CircuitEncoding) - 1u) };
			return timeframe.template GetContainer<GoodTag>(context, node->GetNodeId(), portType, index);
		};
		const auto get_containers = [&](const auto& port) {
			std::vector<get_pin_data_container_t<VcmPinData, GoodTag>> result;
			for (auto& node : Circuit::GetDrivers(port, *this->vcmCircuit))
			{
				result.push_back(get_container(node, Circuit::PortType::Output, 0u));
			}
			// We need the ports in big endian order
			std::reverse(result.begin(), result.end());
			return result;
		};

		// Constraints and ports are used in big endian order here
		std::vector<LogicConstraint> constraintsBadPos(2*+bits + 1u, LogicConstraint::NO_CONSTRAINT);
		std::vector<LogicConstraint> constraintsBadNeg(2*+bits + 1u, LogicConstraint::NO_CONSTRAINT);
		std::vector<LogicConstraint> constraintsGoodPos(2*+bits + 1u, LogicConstraint::NO_CONSTRAINT);
		std::vector<LogicConstraint> constraintsGoodNeg(2*+bits + 1u, LogicConstraint::NO_CONSTRAINT);
		constraintsBadPos[2*+bits] = LogicConstraint::ONLY_LOGIC_ZERO; // Initialize sign bit
		constraintsBadNeg[2*+bits] = LogicConstraint::ONLY_LOGIC_ZERO;  // ONLY_LOGIC_ONE if using signed values
		constraintsGoodPos[2*+bits] = LogicConstraint::ONLY_LOGIC_ZERO;
		constraintsGoodNeg[2*+bits] = LogicConstraint::ONLY_LOGIC_ZERO;  // ONLY_LOGIC_ONE if using signed values

		auto vcmName { boost::str(boost::format((target == Target::Macish) ? "MACISH%d_VCM/MAC_" : "MUL%d_VCM/MUL_") % +bits) };
		auto resultExact            { get_containers(vcmName + "OUT_exact_result") };
		auto resultMacishFaulty     { get_containers(vcmName + "OUT_faulty_result") };
		auto resultMacishFaultFree  { get_containers(vcmName + "OUT_faultfree_result") };
		auto differencePosFaulty    { get_containers(vcmName + "DIFFP_faulty_result") };
		auto differenceNegFaulty    { get_containers(vcmName + "DIFFN_faulty_result") };
		auto differencePosFaultFree { get_containers(vcmName + "DIFFP_faultfree_result") };
		auto differenceNegFaultFree { get_containers(vcmName + "DIFFN_faultfree_result") };
		ASSERT(resultExact.size()            == 2*+bits + 1u);
		ASSERT(resultMacishFaulty.size()     == 2*+bits + 1u);
		ASSERT(resultMacishFaultFree.size()  == 2*+bits + 1u);
		ASSERT(differencePosFaulty.size()    == 2*+bits + 1u);
		ASSERT(differenceNegFaulty.size()    == 2*+bits + 1u);
		ASSERT(differencePosFaultFree.size() == 2*+bits + 1u);
		ASSERT(differenceNegFaultFree.size() == 2*+bits + 1u);

		if (trace == Trace::On) { *traceStream << "Positive" << std::endl; }
		auto limitPos = faultFree
			? evaluate(resultExact, resultMacishFaultFree, differencePosFaultFree, constraintsGoodPos, true)
			: evaluate(resultExact, resultMacishFaulty,    differencePosFaulty,    constraintsBadPos,  true);

		if (trace == Trace::On) { *traceStream << "Negative" << std::endl; }
		auto limitNeg = faultFree
			? evaluate(resultExact, resultMacishFaultFree, differenceNegFaultFree, constraintsGoodNeg, true)  // ...GoodPos, false if using signed values
			: evaluate(resultExact, resultMacishFaulty,    differenceNegFaulty,    constraintsBadNeg,  true); // ...BadPos, false if using signed values

		return LimitsResult {
			.limitPos = limitPos,
			.limitNeg = limitNeg,
		};
	};

	// 8 bit, 16 bit result -> 24 bit precision
	// 16 bit, 32 bit result -> 48 bit precision
	// 32 bit, 64 bit result -> 96 bit precision
	const mpfr_prec_t precision { static_cast<mpfr_prec_t>(+bits) * static_cast<mpfr_prec_t>(3u) };
	mpfr_t constantOne; mpfr_init2(constantOne, precision);
	mpfr_t numInputBits; mpfr_init2(numInputBits, precision);
	mpfr_t numOutputBits; mpfr_init2(numOutputBits, precision);
	mpfr_t minInputValue; mpfr_init2(minInputValue, precision);
	mpfr_t maxInputValue; mpfr_init2(maxInputValue, precision);
	mpfr_t maxOutputValue; mpfr_init2(maxOutputValue, precision);
	mpfr_t maxOutputValueLower; mpfr_init2(maxOutputValueLower, precision);
	mpfr_t maxOutputValueUpper; mpfr_init2(maxOutputValueUpper, precision);
	mpfr_set_ui(numInputBits, +bits, MPFR_RNDN);
	mpfr_set_ui(numOutputBits, 2u*+bits, MPFR_RNDN);
	mpfr_set_ui(constantOne, 1u, MPFR_RNDN);
	mpfr_set_ui(minInputValue, 0u, MPFR_RNDN);
	mpfr_exp2m1(maxInputValue, numInputBits, MPFR_RNDN);
	mpfr_exp2m1(maxOutputValue, numOutputBits, MPFR_RNDN);
	mpfr_mul_d(maxOutputValueLower, maxOutputValue, 0.2, MPFR_RNDN);
	mpfr_mul_d(maxOutputValueUpper, maxOutputValue, 0.8, MPFR_RNDN);

	// Bits Mu    Sigma
	// 4    8     1.5
	// 8    128   25.5
	// 16   32768 6553.5
	mpfr_t normal_mu; mpfr_init2(normal_mu, precision);
	mpfr_t normal_sigma; mpfr_init2(normal_sigma, precision);
	mpfr_set(normal_mu, maxInputValue, MPFR_RNDN);
	mpfr_add_ui(normal_mu, normal_mu, 1u, MPFR_RNDN);
	mpfr_div_ui(normal_mu, normal_mu, 2u, MPFR_RNDN);
	mpfr_set(normal_sigma, maxInputValue, MPFR_RNDN);
	mpfr_add_ui(normal_sigma, normal_sigma, 1u, MPFR_RNDN);
	mpfr_div_ui(normal_sigma, normal_sigma, 10u, MPFR_RNDN);

	const auto simulate_stats = [&](size_t faultIndex) -> StatsResult {
		// If testing for fault-free use the last fault as dummy
		bool faultFree = (faultIndex == std::numeric_limits<size_t>::max());
		faultIndex = std::min(faultIndex, this->faultList.size() - 1u);

		auto const& [fault, metaData] = this->faultList[faultIndex];
		Logging::SetCurrentFault(faultIndex);

		const auto get_containers = [&](const auto& port) -> std::vector<const Circuit::MappedNode *> {
			std::vector<const Circuit::MappedNode *> result;
			for (auto& node : Circuit::GetDrivers(port, *this->circuit))
			{
				result.push_back(node);
			}
			// We need the ports in big endian order
			std::reverse(result.begin(), result.end());
			return result;
		};

		auto portA { get_containers(this->circuit->GetName() + "/A") };
		auto portB { get_containers(this->circuit->GetName() + "/B") };
		auto portO { get_containers(this->circuit->GetName() + "/O") };
		assert(portA.size() == +bits);
		assert(portB.size() == +bits);
		assert(portO.size() == 2*+bits);

		gmp_randstate_t mpfr_random;
		gmp_randinit_mt(mpfr_random);
		gmp_randseed_ui(mpfr_random, 0u);

		auto const& mappedCircuit { this->circuit->GetMappedCircuit() };
		auto const mpfr_set_input = [&](auto& pattern, auto const& port, size_t timeframe, const mpfr_t& value) -> void {
			mpfr_t shifter; mpfr_init2(shifter, precision);
			mpfr_t bit; mpfr_init2(bit, 2u);
			mpfr_set(shifter, value, MPFR_RNDN);
			unsigned long computed { 0u };
			for (size_t index { 0u }; index < port.size(); index++)
			{
				mpfr_fmod_ui(bit, shifter, 2u, MPFR_RNDZ);
				mpfr_div_ui(shifter, shifter, 2u, MPFR_RNDZ);
				auto const setBit { mpfr_greaterequal_p(bit, constantOne) };
				if (setBit) {
					computed |= 1ul << index;
				}
				pattern.SetPrimaryInput(timeframe, mappedCircuit.GetPrimaryInputNumber(port[index]),
					setBit ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO);
			}
			mpfr_clear(shifter);
			mpfr_clear(bit);
			ASSERT(mpfr_get_ui(value, MPFR_RNDN) == computed);
		};
		auto const mpfr_get_output = [&](auto& result, auto const& port, size_t timeframe, mpfr_t& value) -> void {
			mpfr_t bit; mpfr_init2(bit, precision);
			mpfr_set_ui(value, 0u, MPFR_RNDN);
			unsigned long computed { 0u };
			for (size_t index { 0u }; index < port.size(); index++)
			{
				if (result.GetOutputLogic(port[index], timeframe) == Logic::LOGIC_ONE)
				{
					computed |= 1ul << index;
					mpfr_set_ui(bit, index, MPFR_RNDN);
					mpfr_exp2(bit, bit, MPFR_RNDN);
					mpfr_add(value, value, bit, MPFR_RNDN);
				}
			}
			mpfr_clear(bit);

			ASSERT(mpfr_get_ui(value, MPFR_RNDN) == computed);
		};

		Simulation::SimulationConfig simulationConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		Simulation::SimulationResult simulationResult { GetRequiredTimeframeCount(FaultModel { fault }) + ((target == Target::Macish) ? 2u : 0u), mappedCircuit.GetNumberOfNodes() };
		StatsResult statsResult { };
		for (size_t index { 0u }; index < samples; index++)
		{
			std::vector<mpfr_t> valuesA(simulationResult.GetNumberOfTimeframes());
			std::vector<mpfr_t> valuesB(simulationResult.GetNumberOfTimeframes());
			for (auto& value : valuesA)
			{
				mpfr_init2(value, precision);
			}
			for (auto& value : valuesB)
			{
				mpfr_init2(value, precision);
			}

			Pattern::TestPattern pattern(simulationResult.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs());
			for (size_t timeframe { 0u }; timeframe < simulationResult.GetNumberOfTimeframes(); timeframe++)
			{
				mpfr_nrandom(valuesA[timeframe], mpfr_random, MPFR_RNDN);
				mpfr_mul(valuesA[timeframe], valuesA[timeframe], normal_sigma, MPFR_RNDN);
				mpfr_add(valuesA[timeframe], valuesA[timeframe], normal_mu, MPFR_RNDN);
				mpfr_max(valuesA[timeframe], valuesA[timeframe], minInputValue, MPFR_RNDN);
				mpfr_min(valuesA[timeframe], valuesA[timeframe], maxInputValue, MPFR_RNDN);
				mpfr_trunc(valuesA[timeframe], valuesA[timeframe]);
				mpfr_set_input(pattern, portA, timeframe, valuesA[timeframe]);

				mpfr_nrandom(valuesB[timeframe], mpfr_random, MPFR_RNDN);
				mpfr_mul(valuesB[timeframe], valuesB[timeframe], normal_sigma, MPFR_RNDN);
				mpfr_add(valuesB[timeframe], valuesB[timeframe], normal_mu, MPFR_RNDN);
				mpfr_max(valuesB[timeframe], valuesB[timeframe], minInputValue, MPFR_RNDN);
				mpfr_min(valuesB[timeframe], valuesB[timeframe], maxInputValue, MPFR_RNDN);
				mpfr_trunc(valuesB[timeframe], valuesB[timeframe]);
				mpfr_set_input(pattern, portB, timeframe, valuesB[timeframe]);
			}

			if (faultFree)
			{
				Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, pattern, { }, simulationResult, simulationConfig);
			}
			else
			{
				Simulation::SimulateTestPatternEventDriven<FaultModel>(mappedCircuit, pattern, { fault }, simulationResult, simulationConfig);
			}

			auto const timeframe { simulationResult.GetNumberOfTimeframes() - 1u };

			mpfr_t valueO; mpfr_init2(valueO, precision);
			mpfr_t expectedO; mpfr_init2(expectedO, precision);
			mpfr_get_output(simulationResult, portO, timeframe, valueO);
			mpfr_mul(expectedO, valuesA[timeframe], valuesB[timeframe], MPFR_RNDN);

			if (!mpfr_equal_p(valueO, expectedO))
			{
				mpfr_t difference; mpfr_init2(difference, precision);
				mpfr_t differenceAbs; mpfr_init2(differenceAbs, precision);
				mpfr_sub(difference, valueO, expectedO, MPFR_RNDN);
				mpfr_abs(differenceAbs, difference, MPFR_RNDN);

				mpfr_t expectedOrOne; mpfr_init2(expectedOrOne, precision);
				mpfr_max(expectedOrOne, expectedO, constantOne, MPFR_RNDN);

				mpfr_t errorMean; mpfr_init2(errorMean, precision);
				mpfr_t errorMeanNorm; mpfr_init2(errorMeanNorm, precision);
				mpfr_t errorMax; mpfr_init2(errorMax, precision);
				mpfr_t errorMaxNorm; mpfr_init2(errorMaxNorm, precision);
				mpfr_div(errorMean, difference, expectedOrOne, MPFR_RNDN);
				mpfr_div(errorMeanNorm, difference, maxOutputValue, MPFR_RNDN);
				mpfr_div(errorMax, differenceAbs, expectedOrOne, MPFR_RNDN);
				mpfr_div(errorMaxNorm, differenceAbs, maxOutputValue, MPFR_RNDN);

      			mpfr_add_ui(statsResult.errorRate, statsResult.errorRate, 1u, MPFR_RNDN);
				mpfr_add(statsResult.errorMean, statsResult.errorMean, errorMean, MPFR_RNDN);
				mpfr_add(statsResult.errorMeanNorm, statsResult.errorMeanNorm, errorMeanNorm, MPFR_RNDN);
				mpfr_max(statsResult.errorMax, statsResult.errorMax, errorMax, MPFR_RNDN);
				mpfr_max(statsResult.errorMaxNorm, statsResult.errorMaxNorm, errorMaxNorm, MPFR_RNDN);

				if (mpfr_greaterequal_p(expectedO, maxOutputValueUpper) && mpfr_lessequal_p(valueO, maxOutputValueLower)) {
      				mpfr_add_ui(statsResult.errorOverflow, statsResult.errorOverflow, 1u, MPFR_RNDN);
				}
				if (mpfr_lessequal_p(expectedO, maxOutputValueLower) && mpfr_greaterequal_p(valueO, maxOutputValueUpper)) {
      				mpfr_add_ui(statsResult.errorUnderflow, statsResult.errorUnderflow, 1u, MPFR_RNDN);
				}

				mpfr_clear(difference);
				mpfr_clear(differenceAbs);
				mpfr_clear(expectedOrOne);
				mpfr_clear(errorMean);
				mpfr_clear(errorMeanNorm);
				mpfr_clear(errorMax);
				mpfr_clear(errorMaxNorm);
			}

			mpfr_clear(valueO);
			mpfr_clear(expectedO);

			for (auto& value : valuesA)
			{
				mpfr_clear(value);
			}
			for (auto& value : valuesB)
			{
				mpfr_clear(value);
			}
		}
		gmp_randclear(mpfr_random);

		mpfr_div_ui(statsResult.errorRate, statsResult.errorRate, samples, MPFR_RNDN);
		mpfr_div_ui(statsResult.errorMean, statsResult.errorMean, samples, MPFR_RNDN);
		mpfr_div_ui(statsResult.errorMeanNorm, statsResult.errorMeanNorm, samples, MPFR_RNDN);

		return statsResult;
	};

	// Limit computation for maximum and minimum differences
	LOG(INFO) << "Analyzing limits";
	std::atomic<size_t> progressLimits { 0u };
	LimitsResult limitsFaultFree { { LimitResult::Category::Timeout }, { LimitResult::Category::Timeout } };
	std::vector<LimitsResult> limitsFaulty(this->faultList.size(), LimitsResult { { LimitResult::Category::Timeout }, { LimitResult::Category::Timeout } });
	Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd + 1u, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
		if (index < this->faultList.size())
		{
			limitsFaulty[index] = find_limits(index);
		}
		else
		{
			limitsFaultFree = find_limits(std::numeric_limits<size_t>::max());
		}
		auto progress = progressLimits.fetch_add(1u, std::memory_order_acq_rel) + 1u;
		LOG(INFO) << "Progress limits " << progress << "/" << (this->faultList.size() + 1u);
	});
	Logging::ClearCurrentFault();

	// Fault simulation to get average errors
	LOG(INFO) << "Simulating normal samples";
	std::atomic<size_t> progressStats { 0u };
	StatsResult statsFaultFree { };
	std::vector<StatsResult> statsFaulty(this->faultList.size(), StatsResult());
	Parallel::ExecuteParallel(this->faultListBegin, this->faultListEnd + 1u, Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
		if (index < this->faultList.size())
		{
			statsFaulty[index] = simulate_stats(index);
		}
		else
		{
			statsFaultFree = simulate_stats(std::numeric_limits<size_t>::max());
		}
		auto progress = progressStats.fetch_add(1u, std::memory_order_acq_rel) + 1u;
		LOG(INFO) << "Progress simulation " << progress << "/" << (this->faultList.size() + 1u);
	});
	Logging::ClearCurrentFault();

	auto const limits_to_ptree = [&](auto const& limit) {
		boost::property_tree::ptree element;
		element.put("category", to_string(limit.category));
		if (limit.category == LimitResult::Category::Sat) {
			element.put("exact", limit.exact);
			element.put("macish", limit.macish);
			element.put("difference", limit.difference);
			element.put("relative", limit.relative);
			element.put("absolute", limit.absolute);
			element.put("pattern", to_string(*limit.pattern));
			element.put("constraints", to_string(limit.constraints));
			element.put("max-difference", limit.maxDifference);
			element.put("max-absolute", limit.maxAbsolute);

			boost::property_tree::ptree watches;
			for (auto& [name, timeframes] : limit.watches) {
				boost::property_tree::ptree ptimeframes;
				for (auto& timeframe : timeframes) {
					boost::property_tree::ptree ptimeframe;
					ptimeframe.put("", to_string(timeframe));
					ptimeframes.push_back(std::make_pair("", ptimeframe));
				}
				watches.put_child(name, ptimeframes);
			}
			element.put_child("watches", watches);
		}
		return element;
	};
	auto const stats_to_ptree = [&](auto const& stats) {
		boost::property_tree::ptree element;
		element.put("error-rate", mpfr_to_string(stats.errorRate));
		element.put("error-mean", mpfr_to_string(stats.errorMean));
		element.put("error-mean-norm", mpfr_to_string(stats.errorMeanNorm));
		element.put("error-max", mpfr_to_string(stats.errorMax));
		element.put("error-max-norm", mpfr_to_string(stats.errorMaxNorm));
		element.put("error-overflow", mpfr_to_string(stats.errorOverflow));
		element.put("error-underflow", mpfr_to_string(stats.errorUnderflow));
		return element;
	};

	boost::property_tree::ptree root;
	root.put("faults", this->faultList.size());
	root.put("bits-input", mpfr_to_string(numInputBits));
	root.put("bits-output", mpfr_to_string(numOutputBits));
	root.put("max-input", mpfr_to_string(maxInputValue));
	root.put("max-output", mpfr_to_string(maxOutputValue));
	root.put("normal-mu", mpfr_to_string(normal_mu));
	root.put("normal-sigma", mpfr_to_string(normal_sigma));

	boost::property_tree::ptree faultFree;
	faultFree.put_child("limits-positive", limits_to_ptree(limitsFaultFree.limitPos));
	faultFree.put_child("limits-negative", limits_to_ptree(limitsFaultFree.limitNeg));
	faultFree.put_child("stats", stats_to_ptree(statsFaultFree));
	root.add_child("fault-free", faultFree);

	for (size_t index { 0u }; index < this->faultList.size(); index++)
	{
		boost::property_tree::ptree element;
		element.put("fault", to_string(*std::get<0>(this->faultList[index])));
		element.put_child("limits-positive", limits_to_ptree(limitsFaulty[index].limitPos));
		element.put_child("limits-negative", limits_to_ptree(limitsFaulty[index].limitNeg));
		element.put_child("stats", stats_to_ptree(statsFaulty[index]));
		root.add_child(std::to_string(index), element);
	}

	mpfr_clear(constantOne);
	mpfr_clear(numInputBits);
	mpfr_clear(numOutputBits);
	mpfr_clear(minInputValue);
	mpfr_clear(maxInputValue);
	mpfr_clear(maxOutputValue);
	mpfr_clear(maxOutputValueLower);
	mpfr_clear(maxOutputValueUpper);
	mpfr_clear(normal_mu);
	mpfr_clear(normal_sigma);
	mpfr_free_cache();

	FileHandle resultHandle { Settings::GetInstance()->MapFileName("[DataExportDirectory]/results.json"), false};
	boost::property_tree::jsonc_parser::write_jsonc(resultHandle.GetOutStream(), root);

	Scale4Edge::AtpgBase<FaultModel, FaultList>::ExportStatistics();
	Scale4Edge::AtpgBase<FaultModel, FaultList>::ExportTestPatterns(Pattern::InputCapture::PrimaryAndSecondaryInputs);
	Scale4Edge::AtpgBase<FaultModel, FaultList>::ExportFaultList();
}

template class MacishSafeFaultAnalysis<Fault::SingleStuckAtFaultModel, Fault::SingleStuckAtFaultList>;
template class MacishSafeFaultAnalysis<Fault::SingleTransitionDelayFaultModel, Fault::SingleTransitionDelayFaultList>;
template class MacishSafeFaultAnalysis<Fault::CellAwareFaultModel, Fault::CellAwareFaultList>;

};
};
};
