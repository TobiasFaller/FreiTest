#include "Helper/Disassembler/RiscvDisassembler.hpp"

#include <capstone/capstone.h>

#include <cstring>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Riscv
{

std::vector<std::string> Decode(const std::vector<uint8_t>& stream)
{
	const uint8_t* opcodes = &stream.front();
	const size_t size = stream.size();

	csh capstoneHandle;
	if (auto capstoneResult = cs_open(CS_ARCH_RISCV, CS_MODE_RISCV32, &capstoneHandle); capstoneResult != CS_ERR_OK)
	{
		Logging::Panic(std::string("Could not initialize capstone library: ") + cs_strerror(capstoneResult));
	}

	cs_insn *instructions;
	size_t instructionCount = cs_disasm(capstoneHandle, opcodes, size, 0x0000000000000000ull, 0u, &instructions);
	if (instructionCount == 0u)
	{
		cs_close(&capstoneHandle);
		return { };
	}

	std::vector<std::string> disassembledInstructions;
	for (size_t instruction = 0u; instruction < instructionCount; ++instruction)
	{
		disassembledInstructions.emplace_back(std::string(instructions[instruction].mnemonic) + " " + instructions[instruction].op_str);
	}

	cs_free(instructions, instructionCount);
	cs_close(&capstoneHandle);

	return disassembledInstructions;
}

};
};
