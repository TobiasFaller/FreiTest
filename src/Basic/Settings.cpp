#include "Basic/Settings.hpp"

#include <boost/property_tree/ptree.hpp>

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>

#include "Basic/Logging.hpp"
#include "Helper/StringHelper.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"

using namespace std;
using namespace SolverProxy;

std::shared_ptr<Settings> Settings::instance = std::shared_ptr<Settings>();

Settings::Settings(void):
	Application("None"),
	DataImportDirectory("./output"),
	DataExportDirectory("./output"),
	StatisticsExportFilename("[DataExportDirectory]/statistics.json"),
	SatSolver(SolverProxy::Sat::SatSolver::PROD_SAT_MINISAT),
	BmcSolver(SolverProxy::Bmc::BmcSolver::PROD_NCIP),
	CircuitName("UnnamedCircuit"),
	CircuitSourceType(Settings::CircuitSourceType::None),
	CircuitBaseDirectories({"."}),
	VerilogImportFilenames({}),
	VerilogLibraryFilenames({}),
	VerilogExportPreprocessedFilename(""),
	VerilogExportProcessedFilename(""),
	TopLevelModuleName("LAST_DEFINED")
{
}

Settings::~Settings(void) = default;

// TODO: Remove me
void Settings::SetInstance(std::shared_ptr<Settings> settings)
{
	Settings::instance = settings;
}

// TODO: Remove me
std::shared_ptr<Settings> Settings::GetInstance(void)
{
	ASSERT(Settings::instance) << "No global settings instance is set!";
	return Settings::instance;
}

bool Settings::LoadFromFile(LoadContext context, std::filesystem::path path)
{
	LOG(INFO) << "Loading settings file: " << path;

	std::ifstream infile(path, ios::in);
	if (!infile.good())
	{
		LOG(ERROR) << "Opening settings file " << path << " failed.";
		return false;
	}

	try
	{
		using ptree = boost::property_tree::ptree;

		ptree root;
		boost::property_tree::read_jsonc(infile, root);

		for (auto [key, child] : root)
		{
			if (auto appIt = child.find("application"); appIt != child.not_found())
			{
				this->Application = appIt->second.get_value<std::string>();
				continue;
			}

			if (auto defIt = child.find("define"); defIt != child.not_found())
			{
				std::string name = defIt->second.get_value<std::string>();
				if (auto valueIt = child.find("value"); valueIt != child.not_found())
				{
					std::string value = valueIt->second.get_value<std::string>();
					Define(context, name, value);
				}
				else
				{
					Define(context, name, "TRUE");
				}
				continue;
			}

			if (auto undefIt = child.find("undefine"); undefIt != child.not_found())
			{
				std::string name = undefIt->second.get_value<std::string>();
				Undefine(context, name);
				continue;
			}

			if (auto settingIt = child.find("setting"); settingIt != child.not_found())
			{
				std::string name = settingIt->second.get_value<std::string>();
				bool optional = child.get_child_optional("optional")
					.flat_map([](auto value) { return value.template get_value_optional<bool>(); })
					.value_or(false);
				bool overwrite = child.get_child_optional("overwrite")
					.flat_map([](auto value) { return value.template get_value_optional<bool>(); })
					.value_or(false);

				if (auto valueIt = child.find("value"); valueIt != child.not_found())
				{
					std::string value = valueIt->second.get_value<std::string>();
					if(!SetSetting(context, name, value, optional, overwrite) && !optional)
					{
						LOG(ERROR) << "Unknown key / value combination: " << key << " " << value;
						return false;
					}
				}
				else
				{
					LOG(ERROR) << "Found setting " << name << " without value!";
					return false;
				}
				continue;
			}

			if (auto includeIt = child.find("include"); includeIt != child.not_found())
			{
				std::string value = includeIt->second.get_value<std::string>();
				std::filesystem::path path = (context.workingPath / value).lexically_normal();
				LOG(INFO) << "\tLoading include file " << path;

				LoadContext newContext { path.parent_path() };
				if(!Include(newContext, path))
				{
					return false;
				}
				continue;
			}
		}
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
		return false;
	}
	catch (boost::property_tree::ptree_bad_path& exception)
	{
		LOG(ERROR) << "A key does not exist in json file: " << exception.what();
		return false;
	}
	catch (boost::property_tree::ptree_bad_data& exception)
	{
		LOG(ERROR) << "Invalid data value: " << exception.what();
		return false;
	}

	return true;
}

bool Settings::ParseCommandLineSetting(const vector<pair<string, string>>& arguments)
{
	LoadContext context { "." };
	for (auto [key, value] : arguments)
	{
		std::string normalizedKey = key;

		if (StringHelper::StartsWith("--", key))
		{
			normalizedKey = key.substr(2);
		}
		else if (StringHelper::StartsWith("-", key))
		{
			normalizedKey = key.substr(1);
		}

		if (!SetSetting(context, normalizedKey, value, false, false))
		{
			LOG(ERROR) << "Unknown key / value combination: " << normalizedKey << " " << value;
			return false;
		}
	}
	return true;
}

bool Settings::Include(LoadContext context, std::filesystem::path path)
{
	return LoadFromFile(context, path);
}

void Settings::Define(LoadContext context, std::string key, std::string value)
{
	_history.emplace_back("Define", std::string(key) + "=" + value, false, false);
	Defines[key] = value;
}

void Settings::Undefine(LoadContext context, std::string key)
{
	_history.emplace_back("Undefine", key, false, false);
	Defines.erase(key);
}

void Settings::AddParameter(LoadContext context, std::string value)
{
	_history.emplace_back("Parameter", value, false, false);
	Parameters.push_back(value);
}

bool Settings::SetSetting(LoadContext context, string key, string value, bool optional, bool overwrite)
{
	if (key == "Settings")
	{
		std::filesystem::path path = (context.workingPath / value).lexically_normal();
		LOG(INFO) << "\tLoading settings file " << path;

		LoadContext newContext { path.parent_path() };
		return Include(newContext, path);
	}
	else if (key == "Define")
	{
		const size_t separator = value.find('=');
		if (separator == string::npos)
		{
			LOG(INFO) << "\tDefining " << value;
			Define(context, value, value);
		}
		else
		{
			const string key = value.substr(0, separator);
			const string define = value.substr(separator + 1);
			LOG(INFO) << "\tDefining " << key << " to " << define;
			Define(context, key, define);
		}
		return true;
	}
	else if (key == "Undefine")
	{
		LOG(INFO) << "\tUndefining " << value;
		Undefine(context, value);
		return true;
	}
	else if (key == "Include")
	{
		std::filesystem::path path = (context.workingPath / value).lexically_normal();
		LOG(INFO) << "\tLoading include file " << path;

		LoadContext newContext { path.parent_path() };
		return Include(newContext, path);
	}
	else if (key == "Parameter")
	{
		AddParameter(context, value);
		return true;
	}

	_history.emplace_back(key, value, optional, overwrite);

	LOG(INFO) << "\tSetting configuration option " << key << " to \"" << value << "\"";

	// General configuration
	if (key == "Application")
	{
		this->Application = value;
	}
	else if (key == "DataImportDirectory")
	{
		this->DataImportDirectory = value;
	}
	else if (key == "DataExportDirectory")
	{
		this->DataExportDirectory = value;
	}
	else if (key == "StatisticsExportFilename")
	{
		this->StatisticsExportFilename = value;
	}

	// Solver configuration
	else if (key == "SatSolver")
	{
		const vector<pair<string, SolverProxy::Sat::SatSolver>> solverList =
		{
			{ "PROD_SAT_SINGLE_GLUCOSE_421", SolverProxy::Sat::SatSolver::PROD_SAT_SINGLE_GLUCOSE_421 },
			{ "PROD_SAT_PARALLEL_GLUCOSE_421", SolverProxy::Sat::SatSolver::PROD_SAT_PARALLEL_GLUCOSE_421 },
			{ "PROD_SAT_CADICAL", SolverProxy::Sat::SatSolver::PROD_SAT_CADICAL },
			{ "PROD_SAT_MINISAT", SolverProxy::Sat::SatSolver::PROD_SAT_MINISAT },
			{ "PROD_SAT_CRYPTO_MINISAT", SolverProxy::Sat::SatSolver::PROD_SAT_CRYPTO_MINISAT },
			{ "PROD_SAT_IPASIR_DYNAMIC", SolverProxy::Sat::SatSolver::PROD_SAT_IPASIR_DYNAMIC },
			{ "PROD_SAT_IPASIR_STATIC", SolverProxy::Sat::SatSolver::PROD_SAT_IPASIR_STATIC },
			{ "PROD_MAX_SAT_PACOSE", SolverProxy::Sat::SatSolver::PROD_MAX_SAT_PACOSE },
			{ "PROD_MAX_SAT_INC_BMO_COMPLETE", SolverProxy::Sat::SatSolver::PROD_MAX_SAT_INC_BMO_COMPLETE },
			{ "EXPORT_SAT_DIMACS", SolverProxy::Sat::SatSolver::EXPORT_SAT_DIMACS },
			{ "EXPORT_MAX_SAT_DIMACS", SolverProxy::Sat::SatSolver::EXPORT_MAX_SAT_DIMACS }
		};
		for (auto [name, solver] : solverList)
		{
			if (name == value)
			{
				SatSolver = solver;
				return true;
			}
		}
		LOG(FATAL) << "Unknown SAT solver: " << value;
	}
	else if (key == "BmcSolver")
	{
		const std::vector<std::pair<std::string, SolverProxy::Bmc::BmcSolver>> solverList = {
			{ "PROD_NCIP",  SolverProxy::Bmc::BmcSolver::PROD_NCIP },
			{ "EXPORT_CIP", SolverProxy::Bmc::BmcSolver::EXPORT_CIP },
		};
		for (auto [name, solver] : solverList)
		{
			if (name == value)
			{
				BmcSolver = solver;
				return true;
			}
		}
		LOG(FATAL) << "Unknown BMC solver: " << value;
	}

	else if (key == "IpasirSatSolverLibrary")
	{
		DynamicIpasirLibraryLocation = value;
	}

	// Circuit configuration
	else if (key == "CircuitName")
	{
		this->CircuitName = value;
	}
	else if (key == "CircuitSourceType")
	{
		if (value == "None")
		{
			this->CircuitSourceType = Settings::CircuitSourceType::None;
		}
		else if (value == "Verilog")
		{
			this->CircuitSourceType = Settings::CircuitSourceType::Verilog;
		}
		else
		{
			return false;
		}
	}

	// Verilog configuration
	else if (key == "CircuitBaseDirectory")
	{
		if (value == "")
			this->CircuitBaseDirectories.clear();
		else
			this->CircuitBaseDirectories.push_back(value);
	}
	else if (key == "VerilogImportFilename")
	{
		if (value == "")
			this->VerilogImportFilenames.clear();
		else
			this->VerilogImportFilenames.push_back(value);
	}
	else if (key == "VerilogLibraryFilename")
	{
		if (value == "")
			this->VerilogLibraryFilenames.clear();
		else
			this->VerilogLibraryFilenames.push_back(value);
	}
	else if (key == "VerilogExportPreprocessedFilename")
	{
		this->VerilogExportPreprocessedFilename = value;
	}
	else if (key == "VerilogExportProcessedFilename")
	{
		this->VerilogExportProcessedFilename = value;
	}
	else if (key == "TopLevelModuleName")
	{
		this->TopLevelModuleName = value;
	}
	else
	{
		_applicationSettings.emplace_back(key, value, optional, overwrite);
	}

	return true;
}

string Settings::ReplacePlaceholder(LoadContext context, string value)
{
	std::string oldValue;
	do {
		oldValue = value;
		for (auto iter = Defines.cbegin(); iter != Defines.cend(); ++iter)
		{
			value = StringHelper::ReplaceString("[" + iter->first + "]", iter->second, value);
		}

		value = StringHelper::ReplaceString("[DataExportDirectory]", this->DataExportDirectory, value);
		value = StringHelper::ReplaceString("[DataImportDirectory]", this->DataImportDirectory, value);

		value = StringHelper::ReplaceString("[Circuit]", this->CircuitName, value);
		value = StringHelper::ReplaceString("[CircuitName]", this->CircuitName, value);
	} while(value != oldValue);

	return value;
}

string Settings::MapFileName(string rawFilename, bool forReading)
{
	LoadContext context { "" };

	std::filesystem::path path = ReplacePlaceholder(context, rawFilename);
	path = path.lexically_normal();

	if (forReading)
	{
		if (std::filesystem::exists(context.workingPath / path))
		{
			return (context.workingPath / path).lexically_normal();
		}

		// Search if file exists using easy load system.
		// Therefore search in all base paths.
		for(string basePath : CircuitBaseDirectories)
		{
			std::filesystem::path extendedBasePath = ReplacePlaceholder(context, basePath) / path;
			if (std::filesystem::exists(extendedBasePath))
			{
				return extendedBasePath.lexically_normal();
			}
		}

		LOG(ERROR) << "Failed to map " << rawFilename << " (" << path << ") to a valid file!";
		LOG(INFO) << "\tTried filename " << context.workingPath / path << " (not found).";

		for(string basePath : CircuitBaseDirectories)
		{
			std::filesystem::path extendedBasePath = ReplacePlaceholder(context, basePath) / path;
			LOG(INFO) << "\tTried filename " << extendedBasePath << " (not found).";
		}
	}

	return path;
}

const std::vector<Settings::Option>& Settings::GetHistory(void) const
{
	return _history;
}

const std::vector<Settings::Option>& Settings::GetApplicationSettings(void) const
{
	return _applicationSettings;
}
