#include <bitset>
#include <sstream>
#include <algorithm>
#include <regex>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include "Helper/Disassembler/RiscvDsl2Disassembler.hpp"
#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Riscv
{

RiscvDsl2Disassembler::RiscvDsl2Disassembler(std::vector<std::string> filenames):
	instructions(),
	sources(filenames)
{
	ParseJsonSources();
}

std::string RiscvDsl2Disassembler::Decode(size_t value, Endianess endianess)
{
	std::string disassembledInstruction;
	value = ( endianess == Endianess::Little ) ? __builtin_bswap32(value) : value;
	std::bitset<32> zeroFilledValue(value);
	std::string zeroFilledValueStr = zeroFilledValue.to_string();
	auto foreignOpcode = zeroFilledValueStr.substr(zeroFilledValue.size() - standardOpcodeSize, standardOpcodeSize);
	std::vector<Instruction> matchingOpcodes;

	for ( auto& parsedInstruction : instructions)
	{
		if (parsedInstruction.opcode == foreignOpcode)
		{
			matchingOpcodes.push_back(parsedInstruction);
		}
	}

	// Match(es) found. Map func bits and compose the instruction
	if (matchingOpcodes.size())
	{
		for (auto& match : matchingOpcodes)
		{
			if (std::all_of(
				match.subOpcodes.begin(),
				match.subOpcodes.end(),
				[&zeroFilledValueStr](SubOpcode matchSegment)
				{
					return (zeroFilledValueStr.substr(matchSegment.lo, matchSegment.hi - matchSegment.lo) == matchSegment.bits);
				}))
			{
				disassembledInstruction = ComposeInstruction(match, zeroFilledValueStr);
				break;
			}
		}
	}
	// No match(es) mean either that a its a 'C' instruction(s) (1 or 2) or an invalid instruction
	else
	{
		std::vector<std::string> foreignCompressedInstructions = {
			zeroFilledValueStr.substr(0u, 16u),
			zeroFilledValueStr.substr(16u,16u)
		};

		for ( auto& instr : foreignCompressedInstructions )
		{
			for ( auto& parsedInstruction : instructions)
			{
				if (parsedInstruction.opcode == instr.substr(instr.length() - compressedOpcodeSize, compressedOpcodeSize))
				{
					matchingOpcodes.push_back(parsedInstruction);
				}
			}
			for (auto& match : matchingOpcodes)
			{
				if (std::all_of(
					match.subOpcodes.begin(),
					match.subOpcodes.end(),
					[&zeroFilledValueStr](SubOpcode matchSegment)
					{
						return (zeroFilledValueStr.substr(matchSegment.lo, matchSegment.hi - matchSegment.lo) == matchSegment.bits);
					}))
				{
					LOG(WARNING) << "Match found! " << match.name;
					disassembledInstruction += ComposeInstruction(match, zeroFilledValueStr) + "\n";
					break;
				}
			}
		}
		ASSERT(disassembledInstruction != "") << "Unable to disassemble instruction 0x" << std::hex << value;
	}

	return disassembledInstruction;
}

std::string RiscvDsl2Disassembler::ComposeInstruction(Instruction match, std::string foreignValue)
{
	auto syntax  = match.syntax;
	// REMARK: The following UGLY clean-ups are based on the way DSL2 instruction syntax is written.
	// Remove instruction name from syntax
	syntax.replace(syntax.find(match.name), match.name.length(), "");
	boost::trim(syntax);
	// remove 'rm' from 'F'/'D' extensions
	boost::replace_all(syntax, "rm,", "");
	boost::trim(syntax);
	// remove '(fm, rs1, rd)' from FENCEs
	syntax = std::regex_replace(syntax, std::regex("\\(fm[^)]+\\)"), "");
	boost::trim(syntax);
	// tokenize the string
	std::vector<std::string> operands;
	boost::split(operands, syntax, boost::is_any_of(","));

	for (auto& operand : operands)
	{
		boost::trim(operand);

		bool isCompressedRegister = operand.find("+") != std::string::npos;
		if (isCompressedRegister)
		{
			operand = std::regex_replace(operand, std::regex("[8\\+|\\+8]"), "");
		}
		bool isFloatingPointRegister = operand.find("f ") != std::string::npos;
		if (isFloatingPointRegister)
		{
			operand = std::regex_replace(operand, std::regex("f\\s?"), "");
		}

		std::string reconstructedBinaryValue {""};
		size_t reconstructedValue;

		ASSERT(match.operands.find(operand) != match.operands.end())
			<< "Operand " << operand
			<< " not found in instruction " <<  match.name << " operands map";

		for (auto& bitSegment : match.operands[operand])
		{
			reconstructedBinaryValue += foreignValue.substr(bitSegment.lo, bitSegment.hi - bitSegment.lo);
		}
		reconstructedValue = std::stoul(reconstructedBinaryValue, nullptr, 2);

		// All types of immediates & shift amounts converted to hexadecimal values
		if (operand.find("imm") != std::string::npos || operand.find("shamt") != std::string::npos)
		{
			std::stringstream ss;
			ss << std::hex << reconstructedValue;
			operand = "0x" + ss.str();
		}
		// All types of source and destination registers. If needed, extend accordingly
		else if (operand.find("rs1") != std::string::npos
		|| operand.find("rs2") != std::string::npos
		|| operand.find("rd") != std::string::npos
		|| operand.find("sp") != std::string::npos)
		{
			ASSERT(!isCompressedRegister || (0u <= reconstructedValue && reconstructedValue <= 7u))
			<< "Compressed register in instruction " << match.name << " (0x" << std::hex << std::to_string(std::stoul(foreignValue,nullptr,2))
			<< ") is invalid (out of range [0,7]).";

			operand = (isFloatingPointRegister)
			? "f" + std::to_string(reconstructedValue)
			: "x" + ((isCompressedRegister)
				? std::to_string(8u + reconstructedValue)
				: std::to_string(reconstructedValue));
		}
		// CSRs
		else if (operand == "csr")
		{
			try
			{
				operand = csrs.at(reconstructedValue);
			}
			catch (const std::out_of_range& error)
			{
				LOG(WARNING) << "Unknown (non-standard) CSR: 0x" << std::hex << reconstructedValue;
				std::stringstream ss;
				ss << std::hex << reconstructedValue;
				operand = "0x" + ss.str();
			}
		}
		// FENCE
		else if (operand == "succ" || operand == "pred")
		{
			try
			{
				operand = fenceOps.at(reconstructedValue);
			}
			catch (const std::out_of_range& error)
			{
				LOG(FATAL) << "Unknown FENCE succ/pred operand: " << reconstructedBinaryValue;
			}
		}
	}

	boost::replace_all(match.name, "_", ".");
	LOG(INFO) << match.name + " " + boost::algorithm::join(operands, ", ");
	return match.name + " " + boost::algorithm::join(operands, ", ");

}

void RiscvDsl2Disassembler::ParseJsonSources(void)
{
	for ( auto& jsonSource : sources )
	{
		boost::property_tree::ptree root;
		boost::property_tree::read_json(jsonSource, root);

		for (auto& entry : root)
		{
			Instruction rvInstruction;
			rvInstruction.subOpcodes = std::vector<SubOpcode>();

			boost::property_tree::ptree instructionName = entry.second.get_child("name");
			boost::property_tree::ptree instructionEncoding = entry.second.get_child("encoding");
			boost::property_tree::ptree instructionSyntax = entry.second.get_child("syntax");

			rvInstruction.name = instructionName.get_value<std::string>();
			rvInstruction.syntax = instructionSyntax.get_value<std::string>();

			size_t bitIndex = 0u;
			for (auto it = instructionEncoding.begin(); it != instructionEncoding.end(); it ++)
			{
				std::string segmentType = it->second.get_child("type").get_value<std::string>();
				size_t segmentSize = it->second.get_child("bits").get_value<int>();

				if (it == --instructionEncoding.end())
				{
					rvInstruction.opcode = it->second.get_child("pattern").get_value<std::string>();
					break;
				}

				if (segmentType == "operand")
				{
					std::string operandName = it->second.get_child("name").get_value<std::string>();
					BitSegment segment { .lo = bitIndex, .hi = bitIndex + segmentSize};
					if (!rvInstruction.operands.count(operandName))
					{
						rvInstruction.operands[operandName] = std::vector<BitSegment>();
						rvInstruction.operands[operandName].push_back(segment);
					}
					else
					{
						rvInstruction.operands[operandName].push_back(segment);
					}
				}
				else
				{
					std::string pattern = it->second.get_child("pattern").get_value<std::string>();
					rvInstruction.subOpcodes.push_back(SubOpcode {.lo = bitIndex, .hi = bitIndex + segmentSize, .bits = pattern });
				}

				bitIndex += segmentSize;
			}
			instructions.push_back(rvInstruction);
		}
	}
	Decode(0x3723FF00u, Endianess::Little);
	Decode(0x00ff2337u); // lui x6, 0xff2
	Decode(0x30539373u); // csrrw x6, mtvec, x7
	Decode(0xd0039373u); // csrrw x6, 0xd00, x7 Unknown CSR
	Decode(0x0210000fu); // fence r, w
	Decode(0x0480000fu); // fence o, i
	Decode(0xd00170d3u); // fcvt.s.w f1, x2
	Decode(0x84958495u); // c.srai x9, 0x5 c.srai x9, 0x5
}

std::string to_string(BitSegment& segment)
{
	return "BitSegment(lo=" + std::to_string(segment.lo) + ", hi=" + std::to_string(segment.hi) + ")";
}

std::string to_string(SubOpcode& subOpcode)
{
	return "SubOpcode(lo=" + std::to_string(subOpcode.lo) + ", hi=" + std::to_string(subOpcode.hi) + ", code=" + subOpcode.bits + ")";
}

};
};
