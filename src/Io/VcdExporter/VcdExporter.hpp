#pragma once

#include "Io/VcdExporter/VcdModel.hpp"

#include <iostream>
#include <string>

namespace FreiTest
{
namespace Io
{
namespace Vcd
{

bool ExportVcd(const VcdModel& model, const std::string& path);

};
};
};
