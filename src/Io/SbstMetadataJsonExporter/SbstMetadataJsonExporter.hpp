#pragma once

#include <iostream>

#include "Tpg/Sbst/SbstProgram.hpp"
#include "Tpg/Sbst/SbstMemory.hpp"
#include "Helper/ValueWatcher.hpp"

namespace FreiTest
{
namespace Io
{
namespace Sbst
{

bool ExportMetadata(std::ostream& output, const std::vector<FreiTest::Helper::ValueWatcher::WatcherValues>& processorwatches, const std::vector<FreiTest::Helper::ValueWatcher::WatcherValues>& vcmresult);

};
};
};
