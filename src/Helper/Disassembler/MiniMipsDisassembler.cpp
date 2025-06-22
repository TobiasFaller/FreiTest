#include "Helper/Disassembler/MiniMipsDisassembler.hpp"

#include <sstream>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace MiniMips
{

static size_t GetIntFromBinary(std::string data)
{
	size_t result = 0;
	size_t bitValue = 1;

	for(int i = (data.length() - 1); i >= 0; i--)
	{
		if(data[i] == '1')
			result += bitValue;

		bitValue *= 2;
	}

	return result;
}

static std::string GetStringFromBinary(std::string data)
{
	size_t resultInt = GetIntFromBinary(data);
	std::stringstream ss;
	ss << resultInt;

	return ss.str();
}

Instruction Decode(std::string data)
{
	ASSERT(data.length() == 32) << "Instruction size must be exactly 32 bit";

	std::string result;
	std::string data_31_26 = data.substr(0, 6);
	std::string data_25_21 = data.substr(6, 5);
	std::string data_20_16 = data.substr(11, 5);
	std::string data_15_11 = data.substr(16, 5);
	std::string data_15_0  = data.substr(16, 16);
	std::string data_10_0  = data.substr(21, 11);
	std::string data_10_6  = data.substr(21, 5);
	std::string data_5_0   = data.substr(26, 6);

	size_t instructionNumber = ILLEGAL_INSTRUCTION;

	if(data == "00000000000000000000000000000000") {
		result = "NOP (SLL)";
		instructionNumber = 48;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100000") {
		result = "ADD \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " + $" + GetStringFromBinary(data_20_16);
		instructionNumber = 0;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100001") {
		result = "ADDU \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " + $" + GetStringFromBinary(data_20_16);
		instructionNumber = 1;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100100") {
		result = "AND \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " AND $" + GetStringFromBinary(data_20_16);
		instructionNumber = 2;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100111") {
		result = "NOR \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " NOR $" + GetStringFromBinary(data_20_16);
		instructionNumber = 3;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100101") {
		result = "OR \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " OR $" + GetStringFromBinary(data_20_16);
		instructionNumber = 4;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000000100") {
		result = "SLLV \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " << " + GetStringFromBinary(data_25_21);
		instructionNumber = 5;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000101010") {
		result = "SLT \t $" + GetStringFromBinary(data_15_11) + " = ($" + GetStringFromBinary(data_25_21) + " < $" + GetStringFromBinary(data_20_16) + ")";
		instructionNumber = 6;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000101011") {
		result = "SLTU \t $" + GetStringFromBinary(data_15_11) + " = ($" + GetStringFromBinary(data_25_21) + " < $" + GetStringFromBinary(data_20_16) + ")";
		instructionNumber = 7;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000000111") {
		result = "SRAV \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " >> $" + GetStringFromBinary(data_25_21) + " arithmetic";
		instructionNumber = 8;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000000110") {
		result = "SRLV \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " >> $" + GetStringFromBinary(data_25_21) + " logical";
		instructionNumber = 9;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100010") {
		result = "SUB \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " - $" + GetStringFromBinary(data_20_16);
		instructionNumber = 10;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100011") {
		result = "SUBU \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " - $" + GetStringFromBinary(data_20_16);
		instructionNumber = 11;
	}
	else if(data_31_26 == "000000" && data_10_0 == "00000100110") {
		result = "XOR \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_25_21) + " XOR $" + GetStringFromBinary(data_20_16);
		instructionNumber = 12;
	}
	else if(data_31_26 == "000001" && data_20_16 == "00001") {
		result = "BGEZ \t if $" + GetStringFromBinary(data_25_21) + " >= 0 then branch";
		instructionNumber = 13;
	}
	else if(data_31_26 == "000001" && data_20_16 == "10001") {
		result = "BGEZAL \t if $" + GetStringFromBinary(data_25_21) + " >= 0 then procedure_call";
		instructionNumber = 14;
	}
	else if(data_31_26 == "000001" && data_20_16 == "00000") {
		result = "BLTZ \t if $" + GetStringFromBinary(data_25_21) + " < 0 then branch";
		instructionNumber = 15;
	}
	else if(data_31_26 == "000001" && data_20_16 == "10000") {
		result = "BLTZAL \t if $" + GetStringFromBinary(data_25_21) + " < 0 then procedure_call";
		instructionNumber = 16;
	}
	else if(data_31_26 == "000111" && data_20_16 == "00000") {
		result = "BGTZ \t if $" + GetStringFromBinary(data_25_21) + " > 0 then branch";
		instructionNumber = 17;
	}
	else if(data_31_26 == "000110" && data_20_16 == "00000") {
		result = "BLEZ \t if $" + GetStringFromBinary(data_25_21) + " <= 0 then branch";
		instructionNumber = 18;
	}
	else if(data_31_26 == "001000") {
		result = "ADDI \t $" + GetStringFromBinary(data_20_16) + " = $" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0);
		instructionNumber = 19;
	}
	else if(data_31_26 == "001001") {
		result = "ADDIU \t $" + GetStringFromBinary(data_20_16) + " = $" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0);
		instructionNumber = 20;
	}
	else if(data_31_26 == "001100") {
		result = "ANDI \t $" + GetStringFromBinary(data_20_16) + " = $" + GetStringFromBinary(data_25_21) + " AND " + GetStringFromBinary(data_15_0);
		instructionNumber = 21;
	}
	else if(data_31_26 == "001101") {
		result = "ORI \t $" + GetStringFromBinary(data_20_16) + " = $" + GetStringFromBinary(data_25_21) + " OR " + GetStringFromBinary(data_15_0);
		instructionNumber = 22;
	}
	else if(data_31_26 == "001110") {
		result = "XORI \t $" + GetStringFromBinary(data_20_16) + " = $" + GetStringFromBinary(data_25_21) + " XOR " + GetStringFromBinary(data_15_0);
		instructionNumber = 23;
	}
	else if(data_31_26 == "001010") {
		result = "SLTI \t $" + GetStringFromBinary(data_20_16) + " = ($" + GetStringFromBinary(data_25_21) + " < " + GetStringFromBinary(data_15_0) + ")";
		instructionNumber = 24;
	}
	else if(data_31_26 == "001011") {
		result = "SLTIU \t $" + GetStringFromBinary(data_20_16) + " = ($" + GetStringFromBinary(data_25_21) + " < " + GetStringFromBinary(data_15_0) + ")";
		instructionNumber = 25;
	}
	else if(data_31_26 == "000100") {
		result = "BEQ \t if $" + GetStringFromBinary(data_25_21) + " = $" + GetStringFromBinary(data_20_16) + " then branch";
		instructionNumber = 26;
	}
	else if(data_31_26 == "000101") {
		result = "BNE \t if $" + GetStringFromBinary(data_25_21) + " != $" + GetStringFromBinary(data_20_16) + " then branch";
		instructionNumber = 27;
	}
	else if(data_31_26 == "000010") {
		result = "J";
		instructionNumber = 28;
	}
	else if(data_31_26 == "000011") {
		result = "JAL";
		instructionNumber = 29;
	}
	else if(data_31_26 == "000000" && data_5_0 == "001101") {
		result = "BREAK";
		instructionNumber = 30;
	}
	else if(data_31_26 == "000000" && data_5_0 == "001100") {
		result = "SYSCALL";
		instructionNumber = 31;
	}
	else if(data_31_26 == "010000" && data_25_21 == "00001") {
		result = "COP0";
		instructionNumber = 32;
	}
	else if(data_31_26 == "000000" && data_20_16 == "00000" && data_10_0 == "00000001001") {
		result = "JALR \t $" + GetStringFromBinary(data_15_11) + " = return_adr, PC = $" + GetStringFromBinary(data_25_21);
		instructionNumber = 33;
	}
	else if(data_31_26 == "000000" && data_20_16 == "00000" && data_15_11 == "00000" && data_10_0 == "00000001000") {
		result = "JR \t PC = $" + GetStringFromBinary(data_25_21);
		instructionNumber = 34;
	}
	else if(data_31_26 == "001111" && data_25_21 == "00000") {
		result = "LUI \t $" + GetStringFromBinary(data_20_16) + " = " + GetStringFromBinary(data_15_0) + " || 0^16";
		instructionNumber = 35;
	}
	else if(data_31_26 == "100011") {
		result = "LW \t $" + GetStringFromBinary(data_20_16) + " = mem[" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0) + "]";
		instructionNumber = 36;
	}
	else if(data_31_26 == "101011") {
		result = "SW \t mem[" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0) + "] = $" + GetStringFromBinary(data_20_16);
		instructionNumber = 37;
	}
	else if(data_31_26 == "111000") {
		result = "SWC0 \t mem[" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0) + "] = cs" + GetStringFromBinary(data_20_16);
		instructionNumber = 38;
	}
	else if(data_31_26 == "110000") {
		result = "LWC0 \t cs" + GetStringFromBinary(data_20_16) + " = mem[" + GetStringFromBinary(data_25_21) + " + " + GetStringFromBinary(data_15_0) + "]";
		instructionNumber = 39;
	}
	else if(data_31_26 == "010000" && data_25_21 == "00000" && data_10_0 == "00000000000") {
		result = "MFC0 \t $" + GetStringFromBinary(data_20_16) + " = cs" + GetStringFromBinary(data_15_11);
		instructionNumber = 40;
	}
	else if(data_31_26 == "010000" && data_25_21 == "00100" && data_10_0 == "00000000000") {
		result = "MTC0 \t cs" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16);
		instructionNumber = 41;
	}
	else if(data_31_26 == "000000" && data_25_21 == "00000" && data_20_16 == "00000" && data_10_0 == "00000010000") {
		result = "MFHI \t $" + GetStringFromBinary(data_15_11) + " = HI";
		instructionNumber = 42;
	}
	else if(data_31_26 == "000000" && data_25_21 == "00000" && data_20_16 == "00000" && data_10_0 == "00000010010") {
		result = "MFLO \t $" + GetStringFromBinary(data_15_11) + " = LO";
		instructionNumber = 43;
	}
	else if(data_31_26 == "000000" && data_20_16 == "00000" && data_15_11 == "00000" && data_10_0 == "00000010001") {
		result = "MTHI \t HI = $" + GetStringFromBinary(data_25_21);
		instructionNumber = 44;
	}
	else if(data_31_26 == "000000" && data_20_16 == "00000" && data_15_11 == "00000" && data_10_0 == "00000010011") {
		result = "MTLO \t LO = $" + GetStringFromBinary(data_25_21);
		instructionNumber = 45;
	}
	else if(data_31_26 == "000000" && data_15_11 == "00000" && data_10_0 == "00000011000") {
		result = "MULT \t (HI, LO) = $" + GetStringFromBinary(data_25_21) + " * $" + GetStringFromBinary(data_20_16);
		instructionNumber = 46;
	}
	else if(data_31_26 == "000000" && data_15_11 == "00000" && data_10_0 == "00000011001") {
		result = "MULTU \t (HI, LO) = $" + GetStringFromBinary(data_25_21) + " * $" + GetStringFromBinary(data_20_16);
		instructionNumber = 47;
	}
	else if(data_31_26 == "000000" && data_25_21 == "00000" && data_5_0 == "000000") {
		result = "SLL \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " << " + GetStringFromBinary(data_10_6);
		instructionNumber = 48;
	}
	else if(data_31_26 == "000000" && data_25_21 == "00000" && data_5_0 == "000011") {
		result = "SRA \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " >> " + GetStringFromBinary(data_10_6) + " arithmetic";
		instructionNumber = 49;
	}
	else if(data_31_26 == "000000" && data_25_21 == "00000" && data_5_0 == "000010") {
		result = "SRL \t $" + GetStringFromBinary(data_15_11) + " = $" + GetStringFromBinary(data_20_16) + " >> " + GetStringFromBinary(data_10_6) + " logical";
		instructionNumber = 50;
	}
	else
	{
		return { "UNKNOWN INSTRUCTION", ILLEGAL_INSTRUCTION };
	}

	return { result, instructionNumber };
}

size_t GetNumberOfInstructions(void)
{
	return 50;
}

std::string GetInstruction(size_t number)
{
	ASSERT(number < 51) << "There is no instruction with that number";

	std::string instruction;

	if(number == 0)
		instruction = "000000XXXXXXXXXXXXXXX00000100000"; //ADD
	else if(number == 1)
		instruction = "000000XXXXXXXXXXXXXXX00000100001"; //ADDU
	else if(number == 2)
		instruction = "000000XXXXXXXXXXXXXXX00000100100"; //AND
	else if(number == 3)
		instruction = "000000XXXXXXXXXXXXXXX00000100111"; //NOR
	else if(number == 4)
		instruction = "000000XXXXXXXXXXXXXXX00000100101"; //OR
	else if(number == 5)
		instruction = "000000XXXXXXXXXXXXXXX00000000100"; //SLLV
	else if(number == 6)
		instruction = "000000XXXXXXXXXXXXXXX00000101010"; //SLT
	else if(number == 7)
		instruction = "000000XXXXXXXXXXXXXXX00000101011"; //SLTU
	else if(number == 8)
		instruction = "000000XXXXXXXXXXXXXXX00000000111"; //SRAV
	else if(number == 9)
		instruction = "000000XXXXXXXXXXXXXXX00000000110"; //SRLV
	else if(number == 10)
		instruction = "000000XXXXXXXXXXXXXXX00000100010"; //SUB
	else if(number == 11)
		instruction = "000000XXXXXXXXXXXXXXX00000100011"; //SUBU
	else if(number == 12)
		instruction = "000000XXXXXXXXXXXXXXX00000100110"; //XOR
	else if(number == 13)
		instruction = "000001XXXXX00001XXXXXXXXXXXXXXXX"; //BGEZ
	else if(number == 14)
		instruction = "000001XXXXX10001XXXXXXXXXXXXXXXX"; //BGEZAL
	else if(number == 15)
		instruction = "000001XXXXX00000XXXXXXXXXXXXXXXX"; //BLTZ
	else if(number == 16)
		instruction = "000001XXXXX10000XXXXXXXXXXXXXXXX"; //BLTZAL
	else if(number == 17)
		instruction = "000111XXXXX00000XXXXXXXXXXXXXXXX"; //BGTZ
	else if(number == 18)
		instruction = "000110XXXXX00000XXXXXXXXXXXXXXXX"; //BLEZ
	else if(number == 19)
		instruction = "001000XXXXXXXXXXXXXXXXXXXXXXXXXX"; //ADDI
	else if(number == 20)
		instruction = "001001XXXXXXXXXXXXXXXXXXXXXXXXXX"; //ADDIU
	else if(number == 21)
		instruction = "001100XXXXXXXXXXXXXXXXXXXXXXXXXX"; //ANDI
	else if(number == 22)
		instruction = "001101XXXXXXXXXXXXXXXXXXXXXXXXXX"; //ORI
	else if(number == 23)
		instruction = "001110XXXXXXXXXXXXXXXXXXXXXXXXXX"; //XORI
	else if(number == 24)
		instruction = "001010XXXXXXXXXXXXXXXXXXXXXXXXXX"; //SLTI
	else if(number == 25)
		instruction = "001011XXXXXXXXXXXXXXXXXXXXXXXXXX"; //SLTIU
	else if(number == 26)
		instruction = "000100XXXXXXXXXXXXXXXXXXXXXXXXXX"; //BEQ
	else if(number == 27)
		instruction = "000101XXXXXXXXXXXXXXXXXXXXXXXXXX"; //BNE
	else if(number == 28)
		instruction = "000010XXXXXXXXXXXXXXXXXXXXXXXXXX"; //J
	else if(number == 29)
		instruction = "000011XXXXXXXXXXXXXXXXXXXXXXXXXX"; //JAL
	else if(number == 30)
		instruction = "000000XXXXXXXXXXXXXXXXXXXX001101"; //BREAK
	else if(number == 31)
		instruction = "000000XXXXXXXXXXXXXXXXXXXX001100"; //SYSCALL
	else if(number == 32)
		instruction = "01000000001XXXXXXXXXXXXXXXXXXXXX"; //COP0
	else if(number == 33)
		instruction = "000000XXXXX00000XXXXX00000001001"; //JALR
	else if(number == 34)
		instruction = "000000XXXXX000000000000000001000"; //JR
	else if(number == 35)
		instruction = "00111100000XXXXXXXXXXXXXXXXXXXXX"; //LUI
	else if(number == 36)
		instruction = "100011XXXXXXXXXXXXXXXXXXXXXXXXXX"; //LW
	else if(number == 37)
		instruction = "101011XXXXXXXXXXXXXXXXXXXXXXXXXX"; //SW
	else if(number == 38)
		instruction = "111000XXXXXXXXXXXXXXXXXXXXXXXXXX"; //SWC0
	else if(number == 39)
		instruction = "110000XXXXXXXXXXXXXXXXXXXXXXXXXX"; //LWC0
	else if(number == 40)
		instruction = "01000000000XXXXXXXXXX00000000000"; //MFC0
	else if(number == 41)
		instruction = "01000000100XXXXXXXXXX00000000000"; //MTC0
	else if(number == 42)
		instruction = "0000000000000000XXXXX00000010000"; //MFHI
	else if(number == 43)
		instruction = "0000000000000000XXXXX00000010010"; //MFLO
	else if(number == 44)
		instruction = "000000XXXXX000000000000000010001"; //MTHI
	else if(number == 45)
		instruction = "000000XXXXX000000000000000010011"; //MTLO
	else if(number == 46)
		instruction = "000000XXXXXXXXXX0000000000011000"; //MULT
	else if(number == 47)
		instruction = "000000XXXXXXXXXX0000000000011001"; //MULTU
	else if(number == 48)
		instruction = "00000000000XXXXXXXXXXXXXXX000000"; //SLL
	else if(number == 49)
		instruction = "00000000000XXXXXXXXXXXXXXX000011"; //SRA
	else if(number == 50)
		instruction = "00000000000XXXXXXXXXXXXXXX000010"; //SRL

	return instruction;
}

};
};
