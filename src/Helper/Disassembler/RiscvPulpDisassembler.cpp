#include "Helper/Disassembler/RiscvPulpDisassembler.hpp"
#include "Helper/Disassembler/RiscvDisassembler.hpp"
#include "Basic/Logging.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>

namespace FreiTest
{
namespace Riscv
{

#pragma GCC push_options
#pragma GCC optimize ("O0")

std::vector<std::string> DecodePulp(const std::vector<uint8_t>& stream)
{
	std::string inst;

	if (stream.size() < 4)
	{
		return Riscv::Decode(stream);
	}

	for (auto& bytes : stream)
	{
		for (size_t bit = 0; bit < 8; bit++)
		{
			inst = std::to_string((bytes >> bit) & 1) + inst;
		}
	}

	if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0110011"))
	{
		auto ls3 = inst.substr(2,5);
		auto ls2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extract ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(ls2, nullptr, 2)) + ", " + std::to_string(std::stoi(ls3, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0110011"))
	{
		auto ls3 = inst.substr(2,5);
		auto ls2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extractu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(ls2, nullptr, 2)) + ", " + std::to_string(std::stoi(ls3, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0110011"))
	{
		auto ls3 = inst.substr(2,5);
		auto ls2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_insert ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(ls2, nullptr, 2)) + ", " + std::to_string(std::stoi(ls3, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "0110011"))
	{
		auto ls3 = inst.substr(2,5);
		auto ls2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_bclr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(ls2, nullptr, 2)) + ", " + std::to_string(std::stoi(ls3, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "0110011"))
	{
		auto ls3 = inst.substr(2,5);
		auto ls2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_bset ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(ls2, nullptr, 2)) + ", " + std::to_string(std::stoi(ls3, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extractr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extractur ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_insertr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_bclrr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_bsetr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_ror ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_ff1 ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_fl1 ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_clb ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_cnt ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000001000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_abs ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_slet ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sletu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_min ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_minu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_max ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_maxu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_exths ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_exthz ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extbs ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000100000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_extbz ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0110011"))
	{
		auto uimm = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_clip ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0110011"))
	{
		auto uimm = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_clipu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_clipr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "0110011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_clipur ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addrn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addurn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subrn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_suburn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "01") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addunr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addrnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_addurnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subunr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_subrnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_suburnr ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(12, 5) == "00000") && (inst.substr(17, 3) == "000") && (inst.substr(20, 4) == "0000") && (inst.substr(25, 7) == "1111011"))
	{
		auto uimmL = inst.substr(0,12);
		auto L = inst.substr(24,1);
		return { std::string("lp_starti ") + std::to_string(std::stoi(uimmL, nullptr, 2)) + ", " + std::to_string(std::stoi(L, nullptr, 2)) };
	}
	else if ((inst.substr(12, 5) == "00000") && (inst.substr(17, 3) == "001") && (inst.substr(20, 4) == "0000") && (inst.substr(25, 7) == "1111011"))
	{
		auto uimmL = inst.substr(0,12);
		auto L = inst.substr(24,1);
		return { std::string("lp_endi ") + std::to_string(std::stoi(uimmL, nullptr, 2)) + ", " + std::to_string(std::stoi(L, nullptr, 2)) };
	}
	else if ((inst.substr(12, 5) == "00000") && (inst.substr(17, 3) == "011") && (inst.substr(20, 4) == "0000") && (inst.substr(25, 7) == "1111011"))
	{
		auto uimmL = inst.substr(0,12);
		auto L = inst.substr(24,1);
		return { std::string("lp_counti ") + std::to_string(std::stoi(uimmL, nullptr, 2)) + ", " + std::to_string(std::stoi(L, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "000000000000") && (inst.substr(17, 3) == "010") && (inst.substr(20, 4) == "0000") && (inst.substr(25, 7) == "1111011"))
	{
		auto rs1 = inst.substr(12,5);
		auto L = inst.substr(24,1);
		return { std::string("lp_count ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(L, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "100") && (inst.substr(20, 4) == "0000") && (inst.substr(25, 7) == "1111011"))
	{
		auto uimmL = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto L = inst.substr(24,1);
		return { std::string("lp_setup ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(uimmL, nullptr, 2)) + ", " + std::to_string(std::stoi(L, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "101") && (inst.substr(20, 4) == "0000") && (inst.substr(24, 1) == "0") && (inst.substr(25, 7) == "1111011"))
	{
		auto uimmL = inst.substr(0,12);
		auto uimmS = inst.substr(12,5);
		return { std::string("lp_setupi ") + std::to_string(std::stoi(uimmL, nullptr, 2)) + ", " + std::to_string(std::stoi(uimmS, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "1100011"))
	{
		auto imm1 = inst.substr(20,4)+inst.substr(1,6)+inst.substr(24,1)+inst.substr(0,1);
		auto imm2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		return { std::string("p_beqimm ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm2, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "011") && (inst.substr(25, 7) == "1100011"))
	{
		auto imm1 = inst.substr(20,4)+inst.substr(1,6)+inst.substr(24,1)+inst.substr(0,1);
		auto imm2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		return { std::string("p_bneimm ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm2, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0001011"))
	{
		auto imm = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lb_i_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "0001011"))
	{
		auto imm = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lbu_i_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0001011"))
	{
		auto imm = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lh_i_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "0001011"))
	{
		auto imm = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lhu_i_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0001011"))
	{
		auto imm = inst.substr(0,12);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lw_i_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0001011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lb_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0001011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lbu_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0001011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lh_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0001011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lhu_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0001011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lw_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0000011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lb_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0000011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lbu_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0000011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lh_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0000011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lhu_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0000011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_lw_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1011011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulhhu ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "01") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulhhun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulurn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "01") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_mulhhurn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_macsn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_machhsn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "10") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_macsrn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "11") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_machhsrn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_macun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "01") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_machhun ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "00") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_macurn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 2) == "01") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1011011"))
	{
		auto uimm = inst.substr(2,5);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_machhurn ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(uimm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "0101011"))
	{
		auto imm = inst.substr(20,5)+inst.substr(0,7);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		return { std::string("p_sb_i_inc ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "0101011"))
	{
		auto imm = inst.substr(20,5)+inst.substr(0,7);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		return { std::string("p_sh_i_inc ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(17, 3) == "010") && (inst.substr(25, 7) == "0101011"))
	{
		auto imm = inst.substr(20,5)+inst.substr(0,7);
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		return { std::string("p_sw_i_inc ") + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0101011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sb_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0101011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sh_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0101011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sw_r_inc ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0100011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sb_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0100011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sh_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "0100011"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("p_sw_r ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000001") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000001") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpeq_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000110") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000110") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000011") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000110") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000110") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000011") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpne_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000101") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000101") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgt_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001110") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001110") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000111") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001110") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001110") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000111") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpge_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010010") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001001") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001001") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmplt_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010110") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010110") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001011") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010110") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010110") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001011") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmple_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011010") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001101") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001101") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgtu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011110") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011110") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001111") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011110") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011110") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001111") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpgeu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100010") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100010") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010001") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100010") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100010") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010001") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpltu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100110") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100110") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010011") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100110") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100110") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010011") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_cmpleu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 1) == "0") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00000") && (inst.substr(5, 1) == "0") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_add_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00001") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00001") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00001") && (inst.substr(5, 1) == "0") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 5) == "00001") && (inst.substr(5, 2) == "00") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0000100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sub_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000100") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avg_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0001100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "000110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_avgu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_min_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0010100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_minu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001100") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_max_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0011100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "001110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_maxu_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_srl_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0100100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sra_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010100") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sll_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0101100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "010110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_or_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_xor_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "0110100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_and_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "011100000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_abs_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 12) == "011100000000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_abs_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_extract_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "011110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_extract_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100100") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_extractu_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_extractu_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101100") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_insert_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_insert_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotup_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1000100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotusp_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1001100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1001100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1001100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1001100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "100110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_dotsp_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010000") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010000") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotup_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101010") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1010100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotusp_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1011100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1011100") && (inst.substr(17, 3) == "100") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_sc_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101110") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1011100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1011100") && (inst.substr(17, 3) == "101") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_sc_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "101110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_sdotsp_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shuffle_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "110000") && (inst.substr(17, 3) == "110") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shuffle_sci_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shuffle_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "110000") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shufflei0_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "111010") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shufflei1_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "111100") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shufflei2_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 6) == "111110") && (inst.substr(17, 3) == "111") && (inst.substr(25, 7) == "1010111"))
	{
		auto imm = inst.substr(6,6);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shufflei3_sci_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + std::to_string(std::stoi(imm, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100100") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shuffle2_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1100100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_shuffle2_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1101000") && (inst.substr(17, 3) == "000") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_pack_h ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1101100") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_packhi_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}
	else if ((inst.substr(0, 7) == "1110000") && (inst.substr(17, 3) == "001") && (inst.substr(25, 7) == "1010111"))
	{
		auto rs2 = inst.substr(7,5);
		auto rs1 = inst.substr(12,5);
		auto rd = inst.substr(20,5);
		return { std::string("pv_packlo_b ") + "x" + std::to_string(std::stoi(rd, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs1, nullptr, 2)) + ", " + "x" + std::to_string(std::stoi(rs2, nullptr, 2)) };
	}



	return FreiTest::Riscv::Decode(stream);

}

#pragma GCC pop_options

};
};
