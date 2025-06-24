#pragma once

#include <iostream>
#include <optional>

#include "Circuit/CircuitEnvironment.hpp"
#include "Io/FaultListParser/FaultListExchangeFormat.hpp"

namespace FreiTest
{
namespace Io
{

enum class FaultType {
	SingleStuckAtFault,
	SingleTransitionDelayFault,
	CellAwareFault,
	UnknownFault
};

class FaultTypeException
{
public:
	FaultTypeException(std::string expectedFault, std::string detectedFault);
	~FaultTypeException(void);

	const FaultType& GetExpectedFaultType(void) const;
	const FaultType& GetDetectedFaultType(void) const;
private:
	FaultType ConvertStringToFaultType(std::string type) const;

	FaultType expectedFault;
	FaultType detectedFault;
};

template<typename FaultList, typename... Params>
bool ExportFaults(std::ostream& output, const FaultListExchangeFormat<FaultList>& faultList, const Params&... params);

template<typename FaultList, typename... Params>
std::optional<FaultListExchangeFormat<FaultList>> ImportFaults(std::istream& input, const Circuit::CircuitEnvironment& circuit, const Params&... params);

};
};
