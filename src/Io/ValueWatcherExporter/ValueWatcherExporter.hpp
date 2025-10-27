#pragma once

#include <iostream>
#include <vector>

#include "Helper/ValueWatcher.hpp"

namespace FreiTest
{
namespace Io
{

bool ExportWatchers(std::ostream& output, const std::vector<Helper::ValueWatcher::WatcherValues>& watches);

}
}
