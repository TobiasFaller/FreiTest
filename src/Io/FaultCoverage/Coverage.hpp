#pragma once


#include "Basic/Fault/FaultMetaData.hpp"

#include <iostream>
#include <cstdlib>
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
	std::string pin;
	std::string stuckAtFault;

	FaultInformation(std::string pin, std::string stuckAtFault)
		:pin(pin), stuckAtFault(stuckAtFault)
	{
	}
};

struct Coverage
{
	// The index of the vector is the number of the pattern.
	// For each pattern will be stored which Pin and which
	// fault will be detected
	Coverage():
		pattern(),
		input(),
		stuckAtOne(),
		stuckAtZero(),
		stuckAtUnknown(),
		stuckAtX(),
		faultOne(),
		faultZero()
	{
	}


	std::vector<std::vector<FaultInformation>> pattern;
	std::vector<std::vector<std::map<std::string, std::string>>> input;

	// For each pin will be storred which pattern can detect
	// a stuckAtOne or stuckAtZero fault.
	std::map<std::string, std::vector<size_t>> stuckAtOne;
	std::map<std::string, std::vector<size_t>> stuckAtZero;
	std::map<std::string, std::vector<size_t>> stuckAtUnknown;
	std::map<std::string, std::vector<size_t>> stuckAtX;

	// The fault status for every fault will be stored within ths array:
	std::map<std::string, FreiTest::Fault::FaultStatus> faultOne;
	std::map<std::string, FreiTest::Fault::FaultStatus> faultZero;
};

};
};
};
