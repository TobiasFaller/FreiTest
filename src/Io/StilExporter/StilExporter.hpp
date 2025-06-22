#pragma once

#include <iostream>

#include "Basic/Pattern/TestPatternList.hpp"
#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Io
{

enum class StilPatternType {
	FullScan,
	Sequential
};

void ExportStilPatterns(const Circuit::CircuitEnvironment& circuit, const Pattern::TestPatternList& patterns, StilPatternType type, std::ostream& output);

};
};
