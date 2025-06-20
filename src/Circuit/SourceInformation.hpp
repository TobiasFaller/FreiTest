#pragma once

#include <any>
#include <string>
#include <map>
#include <optional>
#include <stdexcept>

static const std::string NO_SOURCE_FILE = "None";
static const std::string NO_SOURCE_LOCATION = "None";
static const std::string NO_SOURCE_NAME = "Unknown";
static const std::string NO_SOURCE_TYPE = "Unknown";

struct SourceInformation
{
	SourceInformation(void);
	SourceInformation(std::string sourceFile, std::string sourceLocation, std::string sourceName, std::string sourceType);
	virtual ~SourceInformation(void);

	template<typename T>
	void AddProperty(std::string name, T value)
	{
		properties.emplace(name, value);
	}

	void ApplyProperties(const std::map<std::string, std::any>& other)
	{
		for (auto const& [key, value] : other)
		{
			properties.emplace(key, value);
		}
	}

	template<typename T>
	bool HasProperty(std::string name) const
	{
		return GetProperty<T>(name).has_value();
	}

	template<typename T>
	std::optional<T> GetProperty(std::string name) const
	{
		if (auto it = properties.find(name); it != properties.end() && it->second.has_value())
		{
			try
			{
				return std::any_cast<T>(it->second);
			}
			catch (const std::bad_any_cast& e)
			{
				throw std::runtime_error("The property could not be cast to the given type!");
			}
		}

		return std::nullopt;
	}

	std::string sourceFile;
	std::string sourceLocation;

	std::string sourceName;
	std::string sourceType;

	std::map<std::string, std::any> properties;

};

std::string to_string(const SourceInformation& sourceInfo);
