#pragma once

#include "Basic/Pattern/Capture.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Extractor
{

template<typename PinData, typename Tag>
Pattern::TestPattern ExtractTestPattern(GeneratorContext<PinData>& pinData, Pattern::InputCapture capture);

};
};
};
