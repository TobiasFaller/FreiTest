#include "Io/VerilogImporter/VerilogConverter.hpp"

#include <unistd.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <optional>
#include <vector>
#include <memory>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Statistic/MemoryStatistic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"
#include "Io/VerilogInstantiator/BuiltinPrimitives.hpp"
#include "Io/VerilogInstantiator/VerilogInstantiator.hpp"
#include "Io/VerilogPreprocessor/VerilogPreprocessor.hpp"
#include "Io/VerilogSpiritParser/VerilogParser.hpp"
#include "Io/VerilogExporter/VerilogExporter.hpp"
#include "Basic/CpuClock.hpp"

using namespace std;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{


VerilogConverter::VerilogConverter(void) = default;
VerilogConverter::~VerilogConverter(void) = default;

std::unique_ptr<CircuitEnvironment> VerilogConverter::LoadCircuit(std::shared_ptr<Settings> settings) const
{
	LOG(INFO) << "Loading verilog sources";

	// -----------------------------------------------------------------------
	// Macro Preprocessor (Handles #includes and #ifdefs)
	// -----------------------------------------------------------------------

	ostringstream preprocessorOutput;
	if (!PreprocessFiles(settings, preprocessorOutput))
	{
		LOG(ERROR) << "Could not preprocess verilog sources";
		return std::unique_ptr<CircuitEnvironment>();
	}

	FreiTest::Statistic::PrintDetailedMemoryUsage();

	// -----------------------------------------------------------------------
	// Parser
	// -----------------------------------------------------------------------

	istringstream preprocessedStream(preprocessorOutput.str());

	auto verilogModules = ParseVerilog(settings, preprocessedStream);
	auto circuit = InstantiateModule(settings, *verilogModules);

	LOG(INFO) << "Instantiated module";
	return circuit;
}

bool VerilogConverter::PreprocessFiles(std::shared_ptr<Settings>& settings, ostream& output) const
{
	const vector<string>& libraryFiles = settings->VerilogLibraryFilenames;
	const vector<string>& circuitFiles = settings->VerilogImportFilenames;

	vector<string> filesToParse;
	std::copy(libraryFiles.cbegin(), libraryFiles.cend(), std::back_inserter(filesToParse));
	std::copy(circuitFiles.cbegin(), circuitFiles.cend(), std::back_inserter(filesToParse));

	std::transform(filesToParse.cbegin(), filesToParse.cend(), filesToParse.begin(),
		[&](const std::string& value) {
			return settings->MapFileName(value, true);
		});

	Verilog::VerilogPreprocessor preprocessor;
	ostringstream preprocessorOutput;
	if (!preprocessor.Transform(filesToParse, preprocessorOutput))
	{
		LOG(ERROR) << "Preprocessor failed to handle input files. Aborting.";
		return false;
	}

	// Free unused resources
	preprocessor.Reset();
	if (settings->VerilogExportPreprocessedFilename != "")
	{
		const string outFilename = settings->MapFileName(
				settings->VerilogExportPreprocessedFilename, false);
		DASSERT(!outFilename.empty()) << "The Verilog preprocessed filename is invalid";

		LOG(INFO) << "Exporting preprocessed verilog source to " << outFilename;
		FileHandle handle(outFilename, false);
		auto& outFile = handle.GetOutStream();
		outFile << preprocessorOutput.str();
	}

	output << preprocessorOutput.str();
	return true;
}

std::unique_ptr<ModuleCollection> VerilogConverter::ParseVerilog(std::shared_ptr<Settings>& settings, std::istream& input) const
{
	Verilog::VerilogParser parser;
	if (!parser.Parse(input))
	{
		LOG(ERROR) << "Could not parse verilog sources";
		return std::unique_ptr<ModuleCollection>();
	}

	if (settings->VerilogExportProcessedFilename != "")
	{
		const string outFilename = settings->MapFileName(
				settings->VerilogExportProcessedFilename, false);
		DASSERT(!outFilename.empty()) << "The Verilog processed filename is invalid";

		LOG(INFO) << "Exporting processed verilog source to " << outFilename;
		FileHandle handle(outFilename, false);
		auto& outFile = handle.GetOutStream();
		Verilog::VerilogExporter exporter(outFile);
		for (auto& module : parser.GetModules())
		{
			exporter.ExportModule(module);
			outFile << std::endl;
		}
	}

	return std::make_unique<ModuleCollection>(parser.GetModules());
}

std::unique_ptr<CircuitEnvironment> VerilogConverter::InstantiateModule(std::shared_ptr<Settings> settings, const ModuleCollection& verilogModules) const
{
	Verilog::Primitives::PrimitiveCollection primitives;
	Verilog::Primitives::DefineConstantPrimitives(primitives);
	Verilog::Primitives::DefinePrimaryPortPrimitives(primitives);
	Verilog::Primitives::DefineBufferPrimitives(primitives);
	Verilog::Primitives::DefineCombinatorialPrimitives(primitives);
	Verilog::Primitives::DefineMultiplexerPrimitives(primitives);
	Verilog::Primitives::DefineTristatePrimitives(primitives);
	Verilog::Primitives::DefineSequentialPrimitives(primitives);

	Verilog::Modules modules(primitives, verilogModules);

	Verilog::VerilogInstantiator instantiator;
	auto instance = instantiator.Instantiate(modules, settings->TopLevelModuleName);
	if (!instance)
	{
		LOG(ERROR) << "Could not create Verilog module instance";
		return std::unique_ptr<CircuitEnvironment>();
	}

	Builder::BuildConfiguration buildConfiguration;
	auto circuit = instantiator.CreateCircuit(modules, *instance, buildConfiguration);
	if (!circuit)
	{
		LOG(ERROR) << "Could not convert instantiated module to circuit representation";
	}

	return circuit;
}

};
};
};
