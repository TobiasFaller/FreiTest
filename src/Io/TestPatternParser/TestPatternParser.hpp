#pragma once

#include <iostream>
#include <optional>

#include "Circuit/CircuitEnvironment.hpp"
#include "Io/TestPatternParser/TestPatternExchangeFormat.hpp"

namespace FreiTest
{
namespace Io
{

bool ExportPatterns(std::ostream& output, const TestPatternExchangeFormat& patterns);
std::optional<TestPatternExchangeFormat> ImportPatterns(std::istream& input, const Circuit::CircuitEnvironment& circuit);

};
};
