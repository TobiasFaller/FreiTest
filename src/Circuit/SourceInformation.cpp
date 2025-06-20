#include "Circuit/SourceInformation.hpp"

SourceInformation::SourceInformation(void):
	SourceInformation(NO_SOURCE_FILE, NO_SOURCE_LOCATION, NO_SOURCE_NAME, NO_SOURCE_TYPE)
{
}

SourceInformation::SourceInformation(std::string sourceFile, std::string sourceLocation, std::string sourceName, std::string sourceType):
	sourceFile(sourceFile),
	sourceLocation(sourceLocation),
	sourceName(sourceName),
	sourceType(sourceType)
{
}

SourceInformation::~SourceInformation(void) = default;

std::string to_string(const SourceInformation& info)
{
	std::string result;

	result += "file: ";
	result += info.sourceFile;

	result += ", location: ";
	result += info.sourceLocation;

	result += ", name: ";
	result += info.sourceName;

	result += ", type: ";
	result += info.sourceType;

	for (const auto& [key, value] : info.properties)
	{
		result += ", ";
		result += key;
		result += ": ";

		if (value.type() == typeid(std::string))
		{
			result += std::any_cast<std::string>(value);
		}
		else if (value.type() == typeid(size_t))
		{
			result += std::to_string(std::any_cast<size_t>(value));
		}
	}

	return result;
}
