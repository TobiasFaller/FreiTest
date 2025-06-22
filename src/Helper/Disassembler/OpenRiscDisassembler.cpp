#include "Helper/Disassembler/OpenRiscDisassembler.hpp"

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace OpenRisc
{

Instruction Decode(std::string data)
{
	ASSERT(data.length() == 32) << "The instruction has to have exactly 32 bit";

	std::string result;
	std::string data_31_26 = data.substr(0, 6);
	std::string data_31_21 = data.substr(0, 11);
	std::string data_31_16 = data.substr(0, 16);
	std::string data_20_16 = data.substr(11, 5);
	std::string data_20_0 = data.substr(11, 21);
	std::string data_15_6 = data.substr(16, 10);
	std::string data_15_0 = data.substr(16, 16);
	std::string data_10_0 = data.substr(21, 11);

	if(data_31_26 == "111000" && data_10_0 == "00000000000")
		result = "l.add";
	else if(data_31_26 == "111000" && data_10_0 == "00000000001")
		result = "l.addc";
	else if(data_31_26 == "100111")
		result = "l.addi";
	else if(data_31_26 == "101000")
		result = "l.addic";
	else if(data_31_26 == "111000" && data_10_0 == "00000000011")
		result = "l.and";
	else if(data_31_26 == "101001")
		result = "l.andi";
	else if(data_31_26 == "000100")
		result = "l.bf";
	else if(data_31_26 == "000011")
		result = "l.bnf";
	else if(data_31_26 == "111000" && data_10_0 == "01100001001")
		result = "l.div";
	else if(data_31_26 == "111000" && data_10_0 == "01100001010")
		result = "l.divu";
	else if(data_31_26 == "111000" && data_15_0 == "0000000001001100")
		result = "l.extbs";
	else if(data_31_26 == "111000" && data_15_0 == "0000000011001100")
		result = "l.extbz";
	else if(data_31_26 == "111000" && data_15_0 == "0000000000001100")
		result = "l.exths";
	else if(data_31_26 == "111000" && data_15_0 == "0000000010001100")
		result = "l.exthz";
	else if(data_31_26 == "111000" && data_15_0 == "0000000000001101")
		result = "l.extws";
	else if(data_31_26 == "111000" && data_15_0 == "0000000001001101")
		result = "l.extwz";
	else if(data_31_26 == "111000" && data_15_0 == "0000000000001111")
		result = "l.ff1";
	else if(data_31_26 == "111000" && data_15_0 == "0000000100001111")
		result = "l.fl1";
	else if(data_31_26 == "000000")
		result = "l.j";
	else if(data_31_26 == "000001")
		result = "l.jal";
	else if(data_31_16 == "0100100000000000" && data_10_0 == "00000000000")
		result = "l.jalr";
	else if(data_31_16 == "0100010000000000" && data_10_0 == "00000000000")
		result = "l.jr";
	else if(data_31_26 == "100100")
		result = "l.lbs";
	else if(data_31_26 == "100011")
		result = "l.lbz";
	else if(data_31_26 == "100110")
		result = "l.lhs";
	else if(data_31_26 == "100101")
		result = "l.lhz";
	else if(data_31_26 == "100010")
		result = "l.lws";
	else if(data_31_26 == "100001")
		result = "l.lwz";
	else if(data_31_21 == "11000100000" && data_10_0 == "00000000001")
		result = "l.mac";
	else if(data_31_21 == "01001100000")
		result = "l.maci";
	else if(data_31_26 == "000110" && data_20_0 == "000010000000000000000")
		result = "l.macrc";
	else if(data_31_26 == "101101")
		result = "l.mfspr";
	else if(data_31_26 == "000110" && data_20_16 == "00000")
		result = "l.movhi";
	else if(data_31_21 == "11000100000" && data_10_0 == "00000000010")
		result = "l.msb";
	else if(data_31_26 == "110000")
		result = "l.mtspr";
	else if(data_31_26 == "111000" && data_10_0 == "01100000110")
		result = "l.mul";
	else if(data_31_26 == "101100")
		result = "l.muli";
	else if(data_31_16 == "0001010100000000")
		result = "l.nop";
	else if(data_31_26 == "111000" && data_10_0 == "00000000100")
		result = "l.or";
	else if(data_31_26 == "101010")
		result = "l.ori";
	else if(data == "00100100000000000000000000000000")
		result = "l.rfe";
	else if(data_31_26 == "111000" && data_10_0 == "00011001000")
		result = "l.ror";
	else if(data_31_26 == "101110" && data_15_6 == "0000000011")
		result = "l.rori";
	else if(data_31_26 == "110110")
		result = "l.sb";
	else if(data_31_21 == "11100100000" && data_10_0 == "00000000000")
		result = "l.sfeq";
	else if(data_31_21 == "11100101011" && data_10_0 == "00000000000")
		result = "l.sfges";
	else if(data_31_21 == "11100100011" && data_10_0 == "00000000000")
		result = "l.sfgeu";
	else if(data_31_21 == "11100101010" && data_10_0 == "00000000000")
		result = "l.sfgts";
	else if(data_31_21 == "11100100010" && data_10_0 == "00000000000")
		result = "l.sfgtu";
	else if(data_31_21 == "11100100101" && data_10_0 == "00000000000")
		result = "l.sfleu";
	else if(data_31_21 == "11100101100" && data_10_0 == "00000000000")
		result = "l.sflts";
	else if(data_31_21 == "11100100100" && data_10_0 == "00000000000")
		result = "l.sfltu";
	else if(data_31_21 == "11100100001" && data_10_0 == "00000000000")
		result = "l.sfne";
	else if(data_31_26 == "110111")
		result = "l.sh";
	else if(data_31_26 == "111000" && data_10_0 == "00000001000")
		result = "l.sll";
	else if(data_31_26 == "101110" && data_15_6 == "0000000000")
		result = "l.slli";
	else if(data_31_26 == "111000" && data_10_0 == "00010001000")
		result = "l.sra";
	else if(data_31_26 == "101110" && data_15_6 == "0000000010")
		result = "l.srai";
	else if(data_31_26 == "111000" && data_10_0 == "00001001000")
		result = "l.srl";
	else if(data_31_26 == "101110" && data_15_6 == "0000000001")
		result = "l.srli";
	else if(data_31_26 == "111000" && data_10_0 == "00000000010")
		result = "l.sub";
	else if(data_31_26 == "110101")
		result = "l.sw";
	else if(data_31_16 == "0010000000000000")
		result = "l.sys";
	else if(data_31_16 == "0010000100000000")
		result = "l.trap";
	else if(data_31_26 == "111000" && data_10_0 == "00000000101")
		result = "l.xor";
	else if(data_31_26 == "101011")
		result = "l.xori";
	else if(data == "00010100010000010000000000000000")
		result = "OR1200 NOP";
	else
		result = "UNKNOWN INSTRUCTION";

	return { result };
}

};
};
