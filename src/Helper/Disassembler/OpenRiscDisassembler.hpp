#pragma once

#include <string>

namespace FreiTest
{
namespace OpenRisc
{

struct Instruction
{
	std::string name;
};

// Data has to start with the most significant bit, i.e. bit 31 has to be at data[0]
// Bit
// 31     25    20	 15    10    5
// 000000 00000 00000 00000 00000 000000
// Position
// 0      6     11    16    21    26
Instruction Decode(std::string data);

};
};
