#pragma once

#include <filesystem>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"

class Settings
{
public:
	template<typename T>
	static bool ParseEnum(const std::string& value, T& output, const std::vector<std::pair<std::string, T>>& options)
	{
		for (auto const& [option, enumValue] : options)
		{
			if (value == option)
			{
				output = enumValue;
				return true;
			}
		}

		return false;
	}
	static bool ParseSizet(const std::string& value, size_t& output)
	{
		try
		{
			output = std::stoul(value);
			return true;
		}
		catch (std::invalid_argument& exception)
		{
			return false;
		}
		catch (std::out_of_range& exception)
		{
			return false;
		}
	}
	static bool IsOption(const std::string& key, const std::string& option, const std::string& prefix)
	{
		return (key == (prefix.empty() ? option : (prefix + "/" + option)));
	}

	struct Option
	{
		Option(std::string name, std::string value, bool optional, bool overwrite):
			name(name),
			value(value),
			optional(optional),
			overwrite(overwrite)
		{
		}

		std::string name;
		std::string value;
		bool optional;
		bool overwrite;
	};

	Settings(void);
	virtual ~Settings(void);

	const std::vector<Option>& GetHistory(void) const;
	const std::vector<Option>& GetApplicationSettings(void) const;
	const std::vector<std::string>& GetParameters(void) const;

	static void SetInstance(std::shared_ptr<Settings> settings);
	static std::shared_ptr<Settings> GetInstance(void);

	bool ParseCommandLineSetting(const std::vector<std::pair<std::string, std::string>>& arguments);
	void SetSetting(std::string key, std::string value);
	std::string MapFileName(std::string rawFilename, bool forReading=false);

	enum class CircuitSourceType
	{
		None = 1,
		Verilog
	};

	std::string Application;
	std::map<std::string, std::string> Defines;
	std::vector<std::string> Parameters;
	std::string DataImportDirectory;
	std::string DataExportDirectory;
	std::string StatisticsExportFilename;

	SolverProxy::Sat::SatSolver SatSolver;
	SolverProxy::Bmc::BmcSolver BmcSolver;
	std::string DynamicIpasirLibraryLocation;

	std::string CircuitName;
	CircuitSourceType CircuitSourceType;
	std::vector<std::string> CircuitBaseDirectories;
	std::vector<std::string> VerilogImportFilenames;
	std::vector<std::string> VerilogLibraryFilenames;
	std::string VerilogExportPreprocessedFilename;
	std::string VerilogExportProcessedFilename;
	std::string TopLevelModuleName;

private:
	struct LoadContext
	{
		LoadContext(std::filesystem::path path):
			workingPath(path)
		{
		}

		std::filesystem::path workingPath;
	};

	bool LoadFromFile(LoadContext context, std::filesystem::path path);
	bool Include(LoadContext context, std::filesystem::path path);
	void Define(LoadContext context, std::string key, std::string value);
	void Undefine(LoadContext context, std::string key);
	void AddParameter(LoadContext context, std::string value);
	bool SetSetting(LoadContext context, std::string key, std::string value, bool optional, bool overwrite);
	std::string ReplacePlaceholder(LoadContext context, std::string value);

	std::vector<Option> _history;
	std::vector<Option> _applicationSettings;

	static std::shared_ptr<Settings> instance;
};
