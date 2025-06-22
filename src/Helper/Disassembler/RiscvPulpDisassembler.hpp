#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstdint>

namespace FreiTest
{
namespace Riscv
{

std::vector<std::string> DecodePulp(const std::vector<uint8_t>& stream);

};
};
