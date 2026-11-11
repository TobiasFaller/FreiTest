#include "Applications/PoliTO/MultiPointStressStimuliGeneration.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <utility>

#include "Basic/CpuClock.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/Disassembler/RiscvDisassembler.hpp"
#include "Helper/Disassembler/RiscvDsl2Disassembler.hpp"
#include "Io/VcdExporter/VcdExporter.hpp"
#include "Io/VcdExporter/VcdModelBuilder.hpp"
#include "Io/ValueWatcherImporter/ValueWatcherImporter.hpp"
#include "Io/ValuePrinterImporter/ValuePrinterImporter.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/TestPatternExtractor.hpp"
#include "Tpg/LogicGenerator/SimulationResultExtractor.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Utility/UnaryCounterEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Vcm/VcmInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/SequentialConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/Constraint/MultiTransitionConstraintEncoder.hpp"
#include "Helper/LuaInvoker.hpp"

using namespace SolverProxy;
using namespace FreiTest::Tpg;

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

MultiPointStressStimuliGeneration::MultiPointStressStimuliGeneration():
	Mixin::VcmMixin("PoliTO/MultiPointStressStimuliGeneration"),
	Mixin::InitialStateMixin("PoliTO/MultiPointStressStimuliGeneration"),
	netSampleSource(NetSampleSource::None),
	netNamesPath(),
	netSampleCount(50u),
	netSampleSize(2u),
	netSamplesPath(),
	targetedModule(),
	moduleWatcherPath(),
	modulePrinterPath(),
	vcmWatcherPath(),
	vcmPrinterPath(),
	moduleWatcherImporter(std::make_unique<Io::ValueWatcherImporter>()),
	modulePrinterImporter(std::make_unique<Io::ValuePrinterImporter>()),
	vcmWatcherImporter(std::make_unique<Io::ValueWatcherImporter>()),
	vcmPrinterImporter(std::make_unique<Io::ValuePrinterImporter>()),
	mutex(),
	exportRiscvAssembly(MultiPointStressStimuliGeneration::ExportRiscvStressPrograms::Disabled),
	programPrologue(""),
	programEpilogue(""),
	stressTargets(),
	stressStatistics()
{
	initStateEnable = ApplyInitialState::Enabled;
	vcmEnable = VcmEnable::Enabled;
}

MultiPointStressStimuliGeneration::~MultiPointStressStimuliGeneration() = default;

bool MultiPointStressStimuliGeneration::SetSetting(std::string key, std::string value)
{
	if (key == "PoliTO/MultiPointStressStimuliGeneration/NetSampleSource")
	{
		return Settings::ParseEnum(value, netSampleSource, {
			{ "FreiTest", NetSampleSource::FreiTest },
			{ "File", NetSampleSource::File },
		});
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/NetNamesPath")
	{
		netNamesPath = value;
		return true;
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/NetSampleCount")
	{
		return Settings::ParseSizet(value, netSampleCount);
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/NetSampleSize")
	{
		return Settings::ParseSizet(value, netSampleSize);
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/NetSamplesPath")
	{
		netSamplesPath = value;
		return true;
	}

	if (key == "PoliTO/MultiPointStressStimuliGeneration/ModuleWatcher")
	{
		moduleWatcherPath = value;
		return true;
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/ModulePrinter")
	{
		modulePrinterPath = value;
		return true;
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/VcmWatcher")
	{
		vcmWatcherPath = value;
		return true;
	}
	if (key == "PoliTO/MultiPointStressStimuliGeneration/VcmPrinter")
	{
		vcmPrinterPath = value;
		return true;
	}

	if (key == "PoliTO/MultiPointStressStimuliGeneration/PrologueEpilogue")
	{
		ASSERT(exportRiscvAssembly == MultiPointStressStimuliGeneration::ExportRiscvStressPrograms::Enabled ) <<
		"Cannot specify Prologue/Epilogue file without enabling the \"PoliTO/MultiPointStressStimuliGeneration/ExportRiscvStressPrograms\" option";

		FileHandle prologueEpilogue(value, true);

		auto& prologueEpilogueStream = prologueEpilogue.GetStream();
		ASSERT(prologueEpilogueStream.good()) << "Could not open prologue-epilogue file";

		// REMARK: No sanity checks on the input file. We assume that the file has the format
		// PROLOGUE_BEGIN ... PROLOGUE_END ... EPILOGUE_BEGIN ... EPILOGUE_END
		bool inPrologue = false;
		bool inEpilogue = false;
		while (!prologueEpilogueStream.eof() && prologueEpilogueStream.good())
		{
			std::string line;
			std::getline(prologueEpilogueStream, line);
			boost::trim(line);

			if (line.find("PROLOGUE_BEGIN") != std::string::npos)
			{
				inPrologue = true;
				continue;
			}
			if (line.find("PROLOGUE_END") != std::string::npos)
			{
				inPrologue = false;
				continue;
			}
			if (line.find("EPILOGUE_BEGIN") != std::string::npos)
			{
				inEpilogue = true;
				continue;
			}
			if (line.find("EPILOGUE_END") != std::string::npos)
			{
				inEpilogue = false;
				continue;
			}

			if (inPrologue)
			{
				programPrologue += line + "\n";
			}
			else if (inEpilogue)
			{
				programEpilogue += line + "\n";
			}

		}

		return true;
	}

	if (key == "PoliTO/MultiPointStressStimuliGeneration/ExportRiscvStressPrograms")
	{
		return Settings::ParseEnum(value, exportRiscvAssembly, {
			{"Enabled", MultiPointStressStimuliGeneration::ExportRiscvStressPrograms::Enabled },
			{"Disabled", MultiPointStressStimuliGeneration::ExportRiscvStressPrograms::Disabled }
		});
	}

	if (key == "PoliTO/MultiPointStressStimuliGeneration/DisassemblerSource")
	{
		disassemblerSourceCode = "";

		FileHandle luaSource(value, true);
		auto& sourceStream = luaSource.GetStream();
		ASSERT(sourceStream.good()) << "Could not open Lua disassembler source code file";
		while (!sourceStream.eof() && sourceStream.good())
		{
			std::string line;
			boost::trim(line);
			std::getline(sourceStream, line);
			disassemblerSourceCode += line + '\n';
		}

		return true;
	}

	if (key == "PoliTO/MultiPointStressStimuliGeneration/DisassemblerJsonSource")
	{
		disassemblerSources.push_back(value);
		return true;
	}

	return Mixin::VcmMixin::SetSetting(key, value) || Mixin::InitialStateMixin::SetSetting(key,value);
}

void MultiPointStressStimuliGeneration::Init(void)
{
	Mixin::InitialStateMixin::Init();
	Mixin::VcmMixin::Init();

	LOG_IF(moduleWatcherPath.empty(), WARNING) << "Not loading any watchers for module";
	if (!moduleWatcherPath.empty())
	{
		FileHandle moduleWatcherHandle { moduleWatcherPath , true };
		if (!moduleWatcherImporter->LoadWatchers(moduleWatcherHandle.GetStream(), "module/", this->circuit->GetName() + "/"))
		{
			LOG(FATAL) << "Could not load module value watchers from file " << moduleWatcherPath;
		}
	}

	LOG_IF(modulePrinterPath.empty(), WARNING) << "Not loading any printers for module";
	if (!modulePrinterPath.empty())
	{
		FileHandle modulePrinterHandle { modulePrinterPath , true };
		if (!modulePrinterImporter->LoadPrinters(modulePrinterHandle.GetStream(), "module/"))
		{
			LOG(FATAL) << "Could not load module value printers from file " << modulePrinterPath;
		}
	}

	LOG_IF(vcmWatcherPath.empty(), WARNING) << "Not loading any watchers for vcm";
	if (!vcmWatcherPath.empty())
	{
		FileHandle vcmWatcherHandle { vcmWatcherPath , true };
		if (!vcmWatcherImporter->LoadWatchers(vcmWatcherHandle.GetStream(), "vcm/", vcmTopLevelModuleName + "/"))
		{
			LOG(FATAL) << "Could not load module value watchers from file " << vcmWatcherPath;
		}
	}

	LOG_IF(vcmPrinterPath.empty(), WARNING) << "Not loading any printers for vcm";
	if (!vcmPrinterPath.empty())
	{
		FileHandle vcmPrinterHandle { vcmPrinterPath , true };
		if (!vcmPrinterImporter->LoadPrinters(vcmPrinterHandle.GetStream(), "vcm/"))
		{
			LOG(FATAL) << "Could not load module value printers from file " << vcmPrinterPath;
		}
	}

	// TESTING PURPOSES
	//auto RVDisass = FreiTest::Riscv::RiscvDsl2Disassembler({
	// "/home/broxigar/workspace/freitest/scripts/code_generation/generate_vcm/json-dbt-rise/rv32i.json",
	// "/home/broxigar/workspace/freitest/scripts/code_generation/generate_vcm/json-dbt-rise/rvzicsr.json",
	// "/home/broxigar/workspace/freitest/scripts/code_generation/generate_vcm/json-dbt-rise/rv32f.json",
	// "/home/broxigar/workspace/freitest/scripts/code_generation/generate_vcm/json-dbt-rise/rv32c.json"});
	//exit(EXIT_SUCCESS);
}

void MultiPointStressStimuliGeneration::Run(void)
{
	Mixin::InitialStateMixin::Run();
	Mixin::VcmMixin::Run();

	InitializeStressTargets();

	PrintStatistics();
	Logging::SetFaultLimits(0u, stressTargets.size());
	Parallel::SetThreads(Parallel::Arena::PatternGeneration, 0u);
	Parallel::ExecuteParallel(0u, stressTargets.size(), Parallel::Arena::PatternGeneration, Parallel::Order::Parallel, [&](size_t index) {
		Logging::SetCurrentFault(index);
		GenerateStressPattern(index);
		PrintStatistics();
	});
	Logging::ClearCurrentFault();
	PrintStatistics();

	ExportStressResults();
	ExportStatistics();
}

Basic::ApplicationStatistics MultiPointStressStimuliGeneration::GetStatistics(void)
{
	return std::move(statistics);
}

void MultiPointStressStimuliGeneration::InitializeStressTargets(void)
{
	const auto& metaData = this->circuit->GetMetaData();

	const auto get_driver = [&](auto& name) -> const Circuit::MappedNode* {
		auto const* wire = metaData.GetWire(name);
		ASSERT(wire != nullptr) << "Could not find wire '" << name << "'";
		ASSERT(wire->GetSize().GetSize() == 1u) << "Wire '" << name << "' has " << wire->GetSize().GetSize() << " elements instead of 1";

		auto const& sources = wire->GetConnections()[0u]->GetMappedSources();
		ASSERT(sources.size() == 1u) << "Wire '" << name << "' has " << sources.size() << " drivers instead of 1";
		return sources[0u].node;
	};
	const auto get_stress_target = [&](const std::vector<const Circuit::MappedNode*>& netDrivers) -> StressTarget {
		std::string targetName;
		std::vector<std::string> netNames;
		for (auto const& netDriver : netDrivers)
		{
			auto name = metaData.GetFriendlyName({ netDriver, { Circuit::PortType::Output, 0u }});
			// Remove the top level name from the string
			if (auto index = name.find("/"); index != std::string::npos) name = name.substr(index + 1u);

			netNames.push_back(name);
			if (!targetName.empty()) targetName += ", ";
			targetName += name;
		}

		return { targetName, netNames, netDrivers };
	};

	switch (netSampleSource)
	{
		case NetSampleSource::FreiTest:
		{
			// Read the net file and interpret as
			ASSERT(!netNamesPath.empty()) << "Please provide a file with circuit nets to stress";
			FileHandle netNameHandle { netNamesPath, true };
			auto& netNameStream = netNameHandle.GetStream();
			ASSERT(netNameStream.good()) << "Could not open circuit nets file";

			// Find the nets and determine the drivers
			std::vector<const Circuit::MappedNode*> netDrivers;
			while (!netNameStream.eof() && netNameStream.good())
			{
				std::string line;
				std::getline(netNameStream, line);
				boost::trim(line);

				if (line.empty() || line[0u] == '#')
				{
					continue;
				}

				netDrivers.push_back(get_driver(line));
			}

			// Sample the nets and prepare the target list
			std::random_device randomDevice {};
			std::mt19937 randomSource { randomDevice() };
			for (size_t sample { 0u }; sample < netSampleCount; sample++)
			{
				std::vector<const Circuit::MappedNode*> stressedNets;
				std::sample(netDrivers.begin(), netDrivers.end(), std::back_inserter(stressedNets), netSampleSize, randomSource);
				stressTargets.emplace_back(get_stress_target(netDrivers), {});
				stressStatistics.Add(StressStatus::Unknown);
			}
			break;
		}

		case NetSampleSource::File:
		{
			// Read the net file and interpret as tuples of nets
			ASSERT(!netSamplesPath.empty()) << "Please provide a file with stress samples";
			FileHandle netSamplesHandle { netSamplesPath, true };
			auto& netSamplesStream = netSamplesHandle.GetStream();
			ASSERT(netSamplesStream.good()) << "Could not open circuit nets file";

			while (!netSamplesStream.eof() && netSamplesStream.good())
			{
				std::string line;
				std::getline(netSamplesStream, line);
				boost::trim(line);

				if (line.empty() || line[0u] == '#')
				{
					continue;
				}

				std::vector<std::string> stressedNetNames;
				boost::split(stressedNetNames, line, boost::is_any_of(";"));

				std::vector<const Circuit::MappedNode*> stressedNets;
				std::transform(stressedNetNames.begin(), stressedNetNames.end(), std::back_inserter(stressedNets), [&](auto& name) {
					boost::trim(name);
					return get_driver(name);
				});
				stressTargets.emplace_back(get_stress_target(stressedNets),	StressMetaData(StressStatus::Unknown, std::vector<size_t>(stressedNets.size(), 0)));
				stressStatistics.Add(StressStatus::Unknown);
			}
			break;
		}

		default:
			Logging::Panic("Sample source not implemented");
	}
}

std::vector<std::vector<size_t>> MultiPointStressStimuliGeneration::GenerateSwitchCountProbeOrder(size_t targetIndex)
{
	auto [target, metaData] = stressTargets[targetIndex];

	const auto netCount = target->GetNets().size();
	const auto permutations = std::pow(3u, netCount);
	std::vector<std::vector<size_t>> probeOrder;
	probeOrder.reserve(permutations);

	// Generate all permutations of transition counts
	for (size_t index { 1u }; index < permutations; index++)
	{
		std::vector<size_t> switchCount;
		switchCount.reserve(netCount);

		size_t value { index };
		for (size_t net { 0u }; net < netCount; net++)
		{
			switchCount.push_back(value % 3u);
			value /= 3u;
		}

		probeOrder.push_back(switchCount);
	}

	// Sort according to their transition counts
	std::sort(probeOrder.begin(), probeOrder.end(), [](auto& vec1, auto& vec2) -> bool {
		auto vec1Transition = std::accumulate(vec1.begin(), vec1.end(), 0u);
		auto vec2Transition = std::accumulate(vec2.begin(), vec2.end(), 0u);
		return vec1Transition > vec2Transition;
	});

	return probeOrder;
}

void MultiPointStressStimuliGeneration::GenerateStressPattern(size_t targetIndex)
{
	const auto convert_order_to_string = [](auto& order) -> std::string {
		std::string result;
		for (auto& value : order)
		{
			if (!result.empty()) result += ", ";
			result += std::to_string(value);
		}
		return result;
	};

	using LogicContainer = Tpg::LogicContainer01X;
	using PinData = Tpg::PinDataG<LogicContainer>;
	using VcmPinData = Tpg::PinDataG<LogicContainer>;

	// Check for already detected faults
	auto [target, metaData] = stressTargets[targetIndex];
	const auto nets = target->GetNets();
	const auto netCount = nets.size();

	VLOG(3) << "Generating stress pattern for nets " << target->GetName();
	const auto probeOrder = GenerateSwitchCountProbeOrder(targetIndex);

	for (auto& transitionCounts : probeOrder)
	{
		VLOG(6) << "Generating stress pattern for probe order " << convert_order_to_string(transitionCounts);

		std::shared_ptr<Bmc::BmcSolverProxy> bmcSolver = Bmc::BmcSolverProxy::CreateBmcSolver(Bmc::BmcSolver::PROD_NCIP);
		ASSERT(bmcSolver) << "Could not initialize BMC-Solver!";

		bmcSolver->SetSolverTimeout(1000u);
		bmcSolver->SetSolverDebug(false);
		bmcSolver->SetSolverSilent(true);
		bmcSolver->SetMaximumDepth(10u + 1u);

		auto logicGenerator = std::make_shared<Tpg::LogicGenerator<PinData>>(bmcSolver, this->circuit);
		auto vcmLogicGenerator = std::make_shared<Tpg::LogicGenerator<VcmPinData>>(bmcSolver, this->vcmCircuit);

		logicGenerator->EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();

		// The BMC based approach requires an unary counter to be encoded to detect the current active timeframe.
		logicGenerator->EmplaceModule<Tpg::UnaryCounterEncoder<PinData>>();

		// Common circuit logic encoding with sequential support.
		logicGenerator->EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		logicGenerator->EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		logicGenerator->EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
		logicGenerator->EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
		logicGenerator->EmplaceModule<Tpg::SequentialConstraintEncoder<PinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(GetInitialState()), Tpg::ConstrainedSequentials::Inputs);

		std::vector<Tpg::TransitionTarget> transitionTargets;
		transitionTargets.reserve(netCount);
		for (size_t netIndex { 0u }; netIndex < netCount; netIndex++)
		{
			transitionTargets.push_back({ { nets[netIndex], { Circuit::PortType::Output, 0u } }, transitionCounts[netIndex] });
		}
		logicGenerator->EmplaceModule<Tpg::MultiTransitionConstraintEncoder<PinData>>(transitionTargets);

		Tpg::Vcm::VcmContext vcmContext { "stress_generation", "Stress Generation" };
		vcmContext.AddTags(vcmTags);
		vcmContext.AddVcmParameters(vcmParameters);
		if (auto it = vcmConfigurations.find(vcmConfiguration); it != vcmConfigurations.end())
		{
			vcmContext.AddTags(it->second.GetTags());
			vcmContext.AddVcmParameters(it->second.GetParameters());
		}

		// Encode the whole circuit with good encoding only as there is no fault model for the VCM circuit
		vcmLogicGenerator->EmplaceModule<Tpg::WholeCircuitTagger<VcmPinData, GoodTag>>();
		vcmLogicGenerator->EmplaceModule<Tpg::UnaryCounterEncoder<VcmPinData>>();
		vcmLogicGenerator->EmplaceModule<Tpg::VcmInputLogicEncoder<VcmPinData, PinData>>(logicGenerator, vcmInputs, vcmContext.GetVcmParameters());
		vcmLogicGenerator->EmplaceModule<Tpg::DefaultLogicEncoder<VcmPinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->EmplaceModule<Tpg::SequentialInputLogicEncoder<VcmPinData, GoodTag>>(Basic::LogicConstraint::ONLY_LOGIC_01X);
		vcmLogicGenerator->EmplaceModule<Tpg::SequentialOutputLogicEncoder<VcmPinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
		vcmLogicGenerator->EmplaceModule<Tpg::SequentialConnectionEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->EmplaceModule<Tpg::OutputLogicEncoder<VcmPinData, GoodTag>>();
		vcmLogicGenerator->EmplaceModule<Tpg::SequentialConstraintEncoder<VcmPinData, GoodTag>>(Tpg::MakeConstantSequentialInitializer(Basic::Logic::LOGIC_ZERO), Tpg::ConstrainedSequentials::Inputs);
		vcmLogicGenerator->EmplaceModule<Tpg::VcmOutputLogicEncoder<VcmPinData>>(vcmOutputs);

		CpuClock cnfGenerationTimer;
		CpuClock bmcSolverTimer;

		cnfGenerationTimer.SetTimeReference();
		logicGenerator->GetContext().SetNumberOfTimeframes(10u + 1u);
		vcmLogicGenerator->GetContext().SetNumberOfTimeframes(10u + 1u);
		ASSERT(logicGenerator->GenerateCircuitLogic() && vcmLogicGenerator->GenerateCircuitLogic()) << "Invalid circuit tagging";
		cnfGenerationTimer.Stop();

		bmcSolverTimer.SetTimeReference();
		bmcSolver->Solve();
		bmcSolverTimer.Stop();

		switch (bmcSolver->GetLastResult())
		{
			case Bmc::BmcResult::Reachable:
			{
				Helper::ValueWatcher moduleWatcher { };
				Helper::ValueWatcher vcmWatcher { };
				Helper::ValueWatcher netWatcher { };
				moduleWatcherImporter->ApplyWatchers(moduleWatcher, *this->circuit, vcmTags, targetedModule);
				vcmWatcherImporter->ApplyWatchers(vcmWatcher, *this->vcmCircuit, vcmTags, targetedModule);
				for (size_t netIndex { 0u }; netIndex < netCount; netIndex++)
				{
					netWatcher.AddWatcher(boost::str(boost::format("nets/net_%d") % netIndex), { nets[netIndex] });
				}

				auto moduleWatches { moduleWatcher.GetWatcherValues<PinData, GoodTag>(logicGenerator->GetContext()) };
				auto vcmWatches { vcmWatcher.GetWatcherValues<PinData, GoodTag>(vcmLogicGenerator->GetContext()) };
				auto netWatches { netWatcher.GetWatcherValues<PinData, GoodTag>(logicGenerator->GetContext()) };

				std::vector<Helper::ValueWatcher::WatcherValues> watchedValues;
				watchedValues.reserve(moduleWatches.size() + vcmWatches.size() + netWatches.size());
				watchedValues.insert(watchedValues.end(), moduleWatches.begin(), moduleWatches.end());
				watchedValues.insert(watchedValues.end(), vcmWatches.begin(), vcmWatches.end());
				watchedValues.insert(watchedValues.end(), netWatches.begin(), netWatches.end());

				Helper::ValuePrinter printer { };
				printer.SetTitle("Results");
				printer.SetTimeframeTitles(+[](size_t timeframe) { return "Timeframe #" + std::to_string(timeframe); });
				printer.SetEqualColumnWidths(true);
				modulePrinterImporter->ApplyPrinters(printer, vcmTags, targetedModule);
				printer.AddDivider(Helper::ValuePrinter::DivideType::BoldSolid);
				// vcmPrinterImporter->ApplyPrinters(printer, vcmTags, targetedModule);
				// printer.AddDivider(Helper::ValuePrinter::DivideType::BoldSolid);
				for (size_t netIndex { 0u }; netIndex < netCount; netIndex++)
				{
					printer.AddBinaryWatcher(
						boost::str(boost::format("Net %d") % (netIndex + 1u)),
						boost::str(boost::format("nets/net_%d") % netIndex),
						Helper::ValuePrinter::ValueAlignment::Right,
						Helper::ValuePrinter::ValuePrefix::No
					);
				}

				FileHandle traceHandle(boost::str(boost::format("[DataExportDirectory]/StressTarget_%d.txt") % targetIndex), false);
				auto& traceOut = traceHandle.GetOutStream();
				traceOut << "Nets:" << std::endl;
				for (size_t netIndex { 0u }; netIndex < netCount; netIndex++)
				{
					traceOut << "    Net " << std::to_string(netIndex + 1u) << ": "
						<< target->GetNetNames()[netIndex] << ", "
						<< transitionCounts[netIndex] << " Transitions" << std::endl;
				}
				traceOut << std::endl;
				traceOut << printer.ToPrintable(watchedValues);

				auto modelSimulationResult { Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(logicGenerator->GetContext()) };
				auto vcmSimulationResult { Tpg::Extractor::ExtractSimulationResult<PinData, GoodTag>(vcmLogicGenerator->GetContext()) };

				Io::Vcd::VcdModelBuilder modelVcdBuilder;
				Io::Vcd::VcdModelBuilder vcmVcdBuilder;
				modelVcdBuilder.Initialize(*this->circuit);
				vcmVcdBuilder.Initialize(*this->vcmCircuit);
				Io::Vcd::ExportVcd(modelVcdBuilder.BuildVcdModel(modelSimulationResult, *this->circuit, "", 1), boost::str(boost::format("[DataExportDirectory]/StressTarget_%d.vcd") % targetIndex));
				Io::Vcd::ExportVcd(vcmVcdBuilder.BuildVcdModel(vcmSimulationResult, *this->vcmCircuit, "", 1), boost::str(boost::format("[DataExportDirectory]/StressTarget_%d_Vcm.vcd") % targetIndex));

				if (exportRiscvAssembly == ExportRiscvStressPrograms::Enabled)
				{
					//REMARK: The values disassembled stem from the BUS input to the processor. Watcher name should be "fetch_instruction_data"
					FileHandle testProgramHandle(boost::str(boost::format("[DataExportDirectory]/StressTarget_%d.S") % targetIndex), false);
					auto& testProgramOut = testProgramHandle.GetOutStream();
					testProgramOut << programPrologue << "\n";

					const Helper::ValueWatcher::WatcherValues* fetchedInstruction = nullptr;
					for ( auto& watcher : watchedValues)
					{
						if (watcher.name == "module/fetch_instruction_data")
						{
							fetchedInstruction = &watcher;
						}
					}

					FreiTest::Helper::Lua luaInvoker;
					luaInvoker.SetGlobal("NOTEST", "1");
					luaInvoker.LoadCode("package.path = package.path .. \";./libraries/other/lua-json/?.lua;./libraries/other/lua-luaunit/?.lua;\"");
					luaInvoker.LoadCode(disassemblerSourceCode);
					luaInvoker.LoadCode("sources_filenames = {}");
					for ( auto& source : disassemblerSources)
					{
						luaInvoker.LoadCode("table.insert(sources_filenames, \"" + source + "\")");
					}
					luaInvoker.LoadCode("sources = parse_json_sources(sources_filenames)");
					luaInvoker.LoadCode("function decode_riscv(value, endianess) return decode(sources, value, endianess) end");
					std::string instruction;
					for (size_t timeframeId {0u}; timeframeId < modelSimulationResult.GetNumberOfTimeframes(); timeframeId ++)
					{
						auto bitValue = static_cast<size_t>(fetchedInstruction->timeframes[timeframeId]);
						auto instructions = luaInvoker.Invoke<std::string, uint32_t, std::string>("decode_riscv", bitValue, "big");

						if (instructions.has_value())
						{
							LOG(INFO) << "Disassembled instruction: " << instructions.value();
							testProgramOut << instructions.value() << std::endl;
						}
						else
						{
							testProgramOut << "// UKNOWN instruction";
						}
					}
					testProgramOut << programEpilogue;
				}

				std::scoped_lock lock { mutex };
				stressStatistics.Update(metaData->GetStatus(), StressStatus::Done);
				metaData->SetStatus(StressStatus::Done);
				metaData->SetNetTransitions(transitionCounts);
				return;
			}

			case Bmc::BmcResult::Unreachable:
			case Bmc::BmcResult::MaxIterationsReached:
				continue;

			default:
			case Bmc::BmcResult::Timeout:
			{
				std::scoped_lock lock { mutex };
				stressStatistics.Update(metaData->GetStatus(), StressStatus::Timeout);
				metaData->SetStatus(StressStatus::Timeout);
				metaData->SetNetTransitions(std::vector<size_t>(target->GetNets().size(), 0u));
				return;
			}
		}
	}

	std::scoped_lock lock { mutex };
	stressStatistics.Update(metaData->GetStatus(), StressStatus::Done);
	metaData->SetStatus(StressStatus::Done);
	metaData->SetNetTransitions(std::vector<size_t>(target->GetNets().size(), 0u));
}

void MultiPointStressStimuliGeneration::ExportStressResults(void)
{
	using ptree = boost::property_tree::ptree;

	ptree targets;
	for (const auto& [target, metaData] : stressTargets)
	{
		ptree nets;
		for (size_t index { 0u }; index < target->GetNets().size(); index++)
		{
			ptree net;
			net.add("name", target->GetNetNames()[index]);
			net.add("driver_node", target->GetNets()[index]->GetNodeId());
			net.add("driver_name", target->GetNets()[index]->GetName());
			net.add("signal_name", target->GetNets()[index]->GetOutputSignalName());
			net.add("transitions", metaData->GetNetTransitions()[index]);
			nets.push_back(std::make_pair("", net));
		}

		ptree item;
		item.add("name", target->GetName());
		item.add("status", to_string(metaData->GetStatus()));
		item.add("metric", std::accumulate(metaData->GetNetTransitions().begin(), metaData->GetNetTransitions().end(), 0u));
		item.add_child("nets", nets);
		targets.push_back(std::make_pair("", item));
	}

	ptree root;
	root.add_child("targets", targets);
	try
	{
		FileHandle exportHandle("[DataExportDirectory]/stress.json", false);
		boost::property_tree::write_json(exportHandle.GetOutStream(), root);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(FATAL) << "Could not write json data: " << exception.what();
	}
}

void MultiPointStressStimuliGeneration::PrintStatistics(void) const
{
	std::scoped_lock lock { mutex };
	const size_t done = stressStatistics.Get(StressStatus::Done);
	const size_t timeout = stressStatistics.Get(StressStatus::Timeout);
	const size_t unknown = stressStatistics.Get(StressStatus::Unknown);
	const size_t total = stressTargets.size();

	const float completedPercent = static_cast<float>(done + timeout) / static_cast<float>(total) * 100.0f;

	std::map<size_t, size_t> stressEfficiencies;

	for (const auto& [target, metaData] : stressTargets)
	{
		size_t totalTransitions = 0;

		for (size_t index { 0u }; index < target->GetNets().size(); index++)
		{
			totalTransitions += metaData->GetNetTransitions()[index];
		}

		stressEfficiencies[totalTransitions] += 1;
	}

	for (size_t index = 0u; index < std::pow(2u, std::get<0>(stressTargets[0])->GetNets().size())+1; index++)
	{
		auto const& pairs = stressEfficiencies[index];
		LOG(INFO) << "Stress Efficiency " << index << ": " << pairs << "/"  << total << " (" << boost::format("%.2f") % (pairs/(float)total * 100.0f) << "%)";
	}

	LOG(INFO) << "Done: " << done << ", Timeouts: " << timeout << ", Unknown: " << unknown << " (" << (boost::format("%.2f") % completedPercent) << " %)";
	std::cout << "\x1b]2;" << "Done: " << done << ", Timeouts: " << timeout << ", Unknown: " << unknown << " (" << (boost::format("%.2f") % completedPercent) << " %)" << "\x07";
}

void MultiPointStressStimuliGeneration::ExportStatistics(void)
{
	for (auto const [target, metaData] : stressTargets)
	{
		LOG(INFO) << target->GetName() << " has reached " << std::accumulate(metaData->GetNetTransitions().begin(), metaData->GetNetTransitions().end(), 0u) << " transitions";
	}
}

StressTarget::StressTarget(std::string name, std::vector<std::string> netNames, std::vector<const Circuit::MappedNode*> nets):
	name(name),
	netNames(netNames),
	nets(nets)
{
}
StressTarget::~StressTarget(void) = default;

const std::string& StressTarget::GetName(void) const
{
	return name;
}

const std::vector<std::string>& StressTarget::GetNetNames(void) const
{
	return netNames;
}

const std::vector<const Circuit::MappedNode*>& StressTarget::GetNets(void) const
{
	return nets;
}

std::string to_string(const StressStatus& status)
{
	switch (status)
	{
		case StressStatus::Unknown: return "Unknown";
		case StressStatus::Done: return "Done";
		case StressStatus::Timeout: return "Timeout";
		default: __builtin_unreachable();
	}
}

StressMetaData::StressMetaData(StressStatus status, std::vector<size_t> netTransitions):
	status(status),
	netTransitions(netTransitions)
{
}

StressMetaData::StressMetaData(void):
	status(StressStatus::Unknown),
	netTransitions()
{
}
StressMetaData::~StressMetaData(void) = default;

void StressMetaData::SetStatus(StressStatus status)
{
	this->status = status;
}

const StressStatus& StressMetaData::GetStatus(void) const
{
	return status;
}

void StressMetaData::SetNetTransitions(std::vector<size_t> netTransitions)
{
	this->netTransitions = netTransitions;
}

const std::vector<size_t>& StressMetaData::GetNetTransitions(void) const
{
	return netTransitions;
}

}
}
}
