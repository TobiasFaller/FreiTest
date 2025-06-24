#pragma once

#include <string>
#include <map>
#include <vector>

namespace FreiTest
{
namespace Io
{
namespace FaultCoverage
{

struct FaultInformation
{
	std::string location;
	std::string type;

	bool operator<(const FaultInformation& other) const
	{
		return (location < other.location)
			|| (location == other.location && type < other.type);
	}
};

struct Coverage
{
	Coverage() = default;
	virtual ~Coverage() = default;

	std::vector<std::vector<FaultInformation>> pattern;
	std::map<FaultInformation, std::vector<size_t>> fault;
	std::vector<std::vector<std::map<std::string, std::string>>> input;

};

};
};
};
