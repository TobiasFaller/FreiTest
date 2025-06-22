#include "Io/SbstDataExporter/SbstDataExporter.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/format.hpp>

#include <utility>

#include "Basic/Overload.hpp"
#include "Tpg/Sbst/SbstMemory.hpp"
#include "Tpg/Sbst/SbstProgram.hpp"

using namespace FreiTest::Tpg::Sbst;

namespace FreiTest
{
namespace Io
{
namespace Sbst
{

using ptree = boost::property_tree::ptree;

static void InstructionsToAssembly(std::ostream& output, const std::vector<Instruction>& instructions)
{
	for (auto& instruction : instructions)
	{
		if (instruction.disassembly.size() > 0u)
		{
			auto hexValue { "0x" + boost::str(boost::format("%1$08x") % instruction.opcode) };
			auto multiInstruction { instruction.disassembly.size() > 1u };

			if (multiInstruction)
			{
				output << "    # " << hexValue << std::endl;
				for (auto& disassembly : instruction.disassembly)
				{
					output << "    " << disassembly << std::endl;
				}
			}
			else
			{
				auto inst { instruction.disassembly[0u] };
				output << "    " << inst
					<< std::string((inst.size() > 24u) ? 1u : (24u - inst.size()), ' ')
					<< "# " << hexValue << std::endl;
			}
		}
		else
		{
			output << "    db " <<
				"0x" << boost::str(boost::format("%1$02x") % ((instruction.opcode >> 0u) & 0xFF)) << ", "
				"0x" << boost::str(boost::format("%1$02x") % ((instruction.opcode >> 8u) & 0xFF)) << ", "
				"0x" << boost::str(boost::format("%1$02x") % ((instruction.opcode >> 16u) & 0xFF)) << ", "
				"0x" << boost::str(boost::format("%1$02x") % ((instruction.opcode >> 24u) & 0xFF)) << std::endl;
		}
	}
}

static void InstructionsToHexadecimal(std::ostream& output, const std::vector<Instruction>& instructions)
{
	for (auto& instruction : instructions)
	{
		output << boost::str(boost::format("%1$08x") % instruction.opcode);

		if (instruction.disassembly.size() > 0u)
		{
			size_t disassemblyIndex = 0;
			for (auto& disassembly : instruction.disassembly)
			{
				if (disassemblyIndex++ != 0u) output << "                ";
				output << " ; " << disassembly << std::endl;
			}
		}
		else
		{
			output << " ; invalid" << std::endl;
		}
	}
}

bool ExportInstructionSequence(std::ostream& output, const SbstInstructionSequence& sequence, size_t sequenceNumber, InstructionSequenceFormat format)
{
	if (format == InstructionSequenceFormat::Assembly)
	{
		output << ".global _sequence_" << sequenceNumber << std::endl;
		output << std::endl;

		output << ".section .text" << std::endl;
		output << "_sequence_" << sequenceNumber << ":" << std::endl;
		InstructionsToAssembly(output, sequence.GetInstructions());
	}
	else if (format == InstructionSequenceFormat::Hexadecimal)
	{
		InstructionsToHexadecimal(output, sequence.GetInstructions());
	}

	return true;
}

bool ExportSbstProgram(std::ostream& output, const SbstProgram& program, const SbstMemory& memory, SbstProgramFormat format)
{
	auto const& vcmTags { program.GetTags() };
	auto const isRiscvE { std::find(vcmTags.begin(), vcmTags.end(), "riscv-base-e") != vcmTags.end() };
	auto const registerCount { isRiscvE ? 16u : 32u };
	auto const nopCount { 5u };

	// Find last used memory block
	std::vector<bool> memoryAllocations(memory.GetNumberOfBlocks(), false);
	for (size_t blockIndex { 0u }; blockIndex < memory.GetNumberOfBlocks(); blockIndex++)
	{
		auto const& bitmap { memory.GetBlock(blockIndex)->GetBitmap() };
		memoryAllocations[blockIndex] = std::any_of(bitmap.begin(), bitmap.end(), [](auto const& value) -> bool {
			return value;
		});
	}
	size_t memoryBlockEnd { memory.GetNumberOfBlocks() };
	while (memoryBlockEnd > 0u && !memoryAllocations[memoryBlockEnd - 1u])
	{
		memoryBlockEnd--;
	}

	if (format == SbstProgramFormat::Assembly)
	{
		// SBST Header
		output << ".global _start" << std::endl;
		output << ".global _sbst_program_start" << std::endl;
		output << ".global _sbst_program_end" << std::endl;
		if (memoryBlockEnd != 0u)
		{
			output << ".global _sbst_data_start" << std::endl;
			output << ".global _sbst_data_end" << std::endl;
			for (size_t blockIndex { 0u }; blockIndex < memoryBlockEnd; blockIndex++)
			{
				output << ".global _sbst_data_block_" << blockIndex << "_start" << std::endl;
				output << ".global _sbst_data_block_" << blockIndex << "_end" << std::endl;
			}
		}
		output << std::endl;

		output << ".section .text" << std::endl;
		output << "_start:" << std::endl;
		for (auto index { 1u }; index < registerCount; index++)
		{
			output << "    add x" << index << ", x0, x0" << std::endl;
		}
		for (auto index { 0u }; index < nopCount; index++)
		{
			output << "    nop" << std::endl;
		}
		output << std::endl;

		// SBST program
		output << "_sbst_program_start:" << std::endl;
		InstructionsToAssembly(output, program.GetInstructions());
		for (auto index { 0u }; index < nopCount; index++)
		{
			output << "    nop" << std::endl;
		}
		output << std::endl;

		// SBST footer
		output << "_sbst_program_end:" << std::endl;
		output << "    nop" << std::endl;
		output << std::endl;

		// SBST data
		for (size_t blockIndex { 0u }; blockIndex < memoryBlockEnd; blockIndex++)
		{
			auto const& block { memory.GetBlock(blockIndex) };
			auto const& bitmap { block->GetBitmap() };
			auto const& memory { block->GetMemory() };

			output << ".section .data" << blockIndex << std::endl;
			if (blockIndex == 0u) output << "_sbst_data_start:" << std::endl;
			output << "_sbst_data_block_" << blockIndex << "_start:" << std::endl;
			output << "# Offset: " << boost::str(boost::format("0x%016x") % block->GetOffset()) << std::endl;
			output << "# Size: " << boost::str(boost::format("0x%016x") % block->GetSize()) << std::endl;
			for (size_t index { 0u }; index < memory.size(); index++)
			{
				output << "    db 0x" << (
						!bitmap[index]
						? "00 # Unused"
						: boost::str(boost::format("%1$02x") % static_cast<unsigned int>(memory[index]))
					) << std::endl;
			}
			output << "_sbst_data_block_" << blockIndex << "_end:" << std::endl;
			if (blockIndex == memoryBlockEnd - 1u) output << "_sbst_data_end:" << std::endl;
		}
	}
	else if (format == SbstProgramFormat::LinkerFile)
	{
		// Linkerfile Header
		output << "OUTPUT_FORMAT(\"elf32-littleriscv\", \"elf32-littleriscv\", \"elf32-littleriscv\")" << std::endl;
		output << "OUTPUT_ARCH(riscv)" << std::endl;
		output << "ENTRY(_start)" << std::endl;
		output << std::endl;

		// Memory definitions
		output << "MEMORY" << std::endl;
		output << "{" << std::endl;
		output << "  program (rxai) : ORIGIN = 0x00000000, LENGTH = 0x10000" << std::endl;
		for (size_t blockIndex { 0u }; blockIndex < memoryBlockEnd; blockIndex++)
		{
			output << "  data" << blockIndex << " (rwai!x) : ORIGIN = 0x"
				<< boost::str(boost::format("%1$08x") % memory.GetBlock(blockIndex)->GetOffset())
				<< ", LENGTH = 0x"
				<< boost::str(boost::format("%1$08x") % memory.GetBlock(blockIndex)->GetSize())
				<< std::endl;
		}
		output << "}" << std::endl;
		output << std::endl;

		// Section definitions
		output << "SECTIONS" << std::endl;
		output << "{" << std::endl;
		output << "  .text :" << std::endl;
		output << "  {" << std::endl;
		output << "    *(.text .text.*)" << std::endl;
		output << "  } > program" << std::endl;
		output << std::endl;
		for (size_t blockIndex { 0u }; blockIndex < memoryBlockEnd; blockIndex++)
		{
			output << "  .data" << blockIndex << " :" << std::endl;
			output << "  {" << std::endl;
			output << "    *(.data" << blockIndex << " .data" << blockIndex << ".*)" << std::endl;
			output << "  } > data" << blockIndex << std::endl;
			output << std::endl;
		}
		output << "}" << std::endl;
	}
	else if (format == SbstProgramFormat::Hexadecimal)
	{
		// SBST program
		output << "; SBST Program" << std::endl;
		InstructionsToHexadecimal(output, program.GetInstructions());
		output << std::endl;

		// SBST data
		output << "; SBST Data" << std::endl;
		for (size_t blockIndex { 0u }; blockIndex < memoryBlockEnd; blockIndex++)
		{
			auto const& block { memory.GetBlock(blockIndex) };
			auto const& bitmap { block->GetBitmap() };
			auto const& memory { block->GetMemory() };

			output << ";   Block " << blockIndex << std::endl;
			output << ";   Offset: " << boost::str(boost::format("0x%016x") % block->GetOffset()) << std::endl;
			output << ";   Size: " << boost::str(boost::format("0x%016x") % block->GetSize()) << std::endl;
			for (size_t index { 0u }; index < memory.size(); index++)
			{
				output << "    " << (
						!bitmap[index]
						? "00 ; Unused"
						: boost::str(boost::format("%02x") % static_cast<unsigned int>(memory[index]))
					) << std::endl;
			}
			output << std::endl;
		}
	}

	return true;
}

bool ExportSbstData(std::ostream& output, const SbstProgram& program, const SbstMemory& memory)
{
	ptree root;

	ptree memoryBlocks;
	for (size_t blockIndex { 0u }; blockIndex < memory.GetNumberOfBlocks(); blockIndex++)
	{
		auto block = memory.GetBlock(blockIndex);
		auto& bitmap = block->GetBitmap();
		auto& memory = block->GetMemory();

		std::string content;
		for (size_t index { 0u }; index < memory.size(); index++)
		{
			content += (!bitmap[index]) ? "XX" : boost::str(boost::format("%02x") % static_cast<unsigned int>(memory[index]));
		}

		ptree memoryBlock;
		memoryBlock.put("offset", boost::str(boost::format("0x%016x") % block->GetOffset()));
		memoryBlock.put("size", boost::str(boost::format("0x%016x") % block->GetSize()));
		memoryBlock.put("memory", content);
		memoryBlocks.push_back(std::make_pair("", memoryBlock));
	}
	root.put_child("memory", memoryBlocks);

	ptree faultStatistics;
	for (size_t faultIndex { 0u }; faultIndex < program.GetNumberOfFaults(); faultIndex++)
	{
		ptree fault;
		fault.put("index", faultIndex);

		auto faultStatistic = program.GetFaultStatistic(faultIndex);
		auto faultPropagation = program.GetFaultPropagation(faultIndex);

		ptree statistics;
		for (auto& [name, value] : faultStatistic) {
			if (value.type() == typeid(std::string))
			{
				statistics.put(name, std::any_cast<std::string>(value));
			}
			else if (value.type() == typeid(size_t))
			{
				statistics.put(name, std::any_cast<size_t>(value));
			}
			else
			{
				LOG(FATAL) << "Unsupported type in statistics: " << name;
			}
		}
		if (faultStatistic.size() == 0) {
			statistics.put("dummy", "placeholder");
		}
		fault.put_child("statistics", statistics);

		ptree propagation;
		std::visit(Basic::overload(
			[&](const NoFaultPropagation& noFaultPropagation) {
				propagation.put("type", "none");
			},
			[&](const ArchitectureRegisterFaultPropagation& faultPropagation) {
				propagation.put("type", "architecture-register");
				propagation.put("timeframe", faultPropagation.timeframe);
				propagation.put("register-normal", boost::str(boost::format("x%d") % faultPropagation.registerNormal));
				propagation.put("register-faulty", boost::str(boost::format("x%d") % faultPropagation.registerFaulty));
				propagation.put("write-enable-normal", faultPropagation.writeEnableNormal);
				propagation.put("write-enable-faulty", faultPropagation.writeEnableFaulty);
				propagation.put("mask-normal", boost::str(boost::format("0x%016x") % faultPropagation.maskNormal));
				propagation.put("mask-faulty", boost::str(boost::format("0x%016x") % faultPropagation.maskFaulty));
				propagation.put("data-normal", boost::str(boost::format("0x%016x") % faultPropagation.dataNormal));
				propagation.put("data-faulty", boost::str(boost::format("0x%016x") % faultPropagation.dataFaulty));
			},
			[&](const ControlAndStatusRegisterFaultPropagation& faultPropagation) {
				propagation.put("type", "csr-register");
				propagation.put("timeframe", faultPropagation.timeframe);
				propagation.put("address-normal", boost::str(boost::format("0x%03x") % faultPropagation.registerNormal));
				propagation.put("address-faulty", boost::str(boost::format("0x%03x") % faultPropagation.registerFaulty));
				propagation.put("write-enable-normal", faultPropagation.writeEnableNormal);
				propagation.put("write-enable-faulty", faultPropagation.writeEnableFaulty);
				propagation.put("mask-normal", boost::str(boost::format("0x%016x") % faultPropagation.maskNormal));
				propagation.put("mask-faulty", boost::str(boost::format("0x%016x") % faultPropagation.maskFaulty));
				propagation.put("data-normal", boost::str(boost::format("0x%016x") % faultPropagation.dataNormal));
				propagation.put("data-faulty", boost::str(boost::format("0x%016x") % faultPropagation.dataFaulty));
			},
			[&](const MemoryFaultPropagation& faultPropagation) {
				propagation.put("type", "memory");
				propagation.put("timeframe", faultPropagation.timeframe);
				propagation.put("address-normal", boost::str(boost::format("0x%016x") % faultPropagation.addressNormal));
				propagation.put("address-faulty", boost::str(boost::format("0x%016x") % faultPropagation.addressFaulty));
				propagation.put("write-enable-normal", faultPropagation.writeEnableNormal);
				propagation.put("write-enable-faulty", faultPropagation.writeEnableFaulty);
				propagation.put("mask-normal", boost::str(boost::format("0x%016x") % faultPropagation.maskNormal));
				propagation.put("mask-faulty", boost::str(boost::format("0x%016x") % faultPropagation.maskFaulty));
				propagation.put("data-normal", boost::str(boost::format("0x%016x") % faultPropagation.dataNormal));
				propagation.put("data-faulty", boost::str(boost::format("0x%016x") % faultPropagation.dataFaulty));
			},
			[&](const ProgramCounterFaultPropagation& faultPropagation) {
				propagation.put("type", "program-counter");
				propagation.put("timeframe", faultPropagation.timeframe);
				propagation.put("write-enable-normal", faultPropagation.writeEnableNormal);
				propagation.put("write-enable-faulty", faultPropagation.writeEnableFaulty);
				propagation.put("mask-normal", boost::str(boost::format("0x%016x") % faultPropagation.maskNormal));
				propagation.put("mask-faulty", boost::str(boost::format("0x%016x") % faultPropagation.maskFaulty));
				propagation.put("data-normal", boost::str(boost::format("0x%016x") % faultPropagation.dataNormal));
				propagation.put("data-faulty", boost::str(boost::format("0x%016x") % faultPropagation.dataFaulty));
			}
		), faultPropagation);
		fault.put_child("propagation", propagation);

		faultStatistics.push_back(std::make_pair("", fault));
	}
	root.put_child("faults", faultStatistics);

	try
	{
		boost::property_tree::write_json(output, root);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
		return false;
	}

	return true;
}

bool ExportSbstTrace(std::ostream& output, const SbstProgram& program, SbstProgramType type)
{
	bool isFlowAltered = false;

	if (type == SbstProgramType::Online)
	{
		for (auto const& propagation : program.GetFaultPropagations())
		{
			std::visit(Basic::overload(
				[](const Tpg::Sbst::NoFaultPropagation& noFaultPropagation) { },
				[&](const Tpg::Sbst::ArchitectureRegisterFaultPropagation& faultPropagation) {
					if (isFlowAltered) return;
					if (faultPropagation.dataFaulty != faultPropagation.dataNormal)
					{
						output << boost::str(boost::format("Fault Effect x%d Timeframe %d: %08x (good), %08x (bad)")
								% faultPropagation.registerNormal
								% faultPropagation.timeframe
								% faultPropagation.dataNormal
								% faultPropagation.dataFaulty)
							<< std::endl;

						if (faultPropagation.timeframe + 1u == program.GetProcessorTestPattern()->GetNumberOfTimeframes())
						{
							output << boost::str(boost::format("Result Register x%d Timeframe %d: %08x (good), %08x (bad)")
									% faultPropagation.registerNormal
									% faultPropagation.timeframe
									% faultPropagation.dataNormal
									% faultPropagation.dataFaulty)
								<< std::endl;
						}
					}
				},
				[](const Tpg::Sbst::ControlAndStatusRegisterFaultPropagation& faultPropagation) { },
				[](const Tpg::Sbst::MemoryFaultPropagation& faultPropagation) { },
				[&](const Tpg::Sbst::ProgramCounterFaultPropagation& faultPropagation) {
					isFlowAltered = true;
					output << boost::str(boost::format("Fault Effect PC Timeframe %d: %08x (good), %08x (bad)")
							% faultPropagation.timeframe
							% faultPropagation.dataNormal
							% faultPropagation.dataFaulty)
						<< std::endl;
				}
			), propagation);
		}
	}
	else if (type == SbstProgramType::Startup)
	{
		for (auto const& propagation : program.GetFaultPropagations())
		{
			std::visit(Basic::overload(
				[](const Tpg::Sbst::NoFaultPropagation& noFaultPropagation) { },
				[](const Tpg::Sbst::ArchitectureRegisterFaultPropagation& faultPropagation) { },
				[](const Tpg::Sbst::ControlAndStatusRegisterFaultPropagation& faultPropagation) { },
				[&](const Tpg::Sbst::MemoryFaultPropagation& faultPropagation) {
					if (isFlowAltered) return;
					if (((faultPropagation.writeEnableNormal != faultPropagation.writeEnableFaulty) && (
							(faultPropagation.writeEnableNormal && faultPropagation.maskNormal)
							|| (faultPropagation.writeEnableFaulty && faultPropagation.maskFaulty)))
						|| ((faultPropagation.writeEnableNormal && faultPropagation.writeEnableFaulty) && (
								(faultPropagation.addressNormal != faultPropagation.addressFaulty)
								|| (faultPropagation.maskNormal != faultPropagation.maskFaulty)
								|| ((faultPropagation.dataNormal & faultPropagation.maskNormal) != (faultPropagation.dataFaulty & faultPropagation.maskFaulty))))
						)
					{
						const auto format_masked_data = [](uint64_t value, uint64_t mask, size_t bytes) -> std::string {
							std::string result;
							for (size_t index = 0; index < bytes; index++) {
								if ((mask >> ((bytes - index - 1u) * 8u)) & 0xFF)
								{
									result += boost::str(boost::format("%02x") % ((value >> ((bytes - index - 1u) * 8u)) & 0xFF));
								}
								else
								{
									result += "--";
								}
							}
							return result;
						};

						output << boost::str(boost::format("Fault Effect Timeframe %d: ") % faultPropagation.timeframe);
						if (faultPropagation.writeEnableNormal)
						{
							output << boost::str(boost::format("write %s to %08x (good), ")
								% format_masked_data(faultPropagation.dataNormal, faultPropagation.maskNormal, 4u)
								% faultPropagation.addressNormal);
						}
						else
						{
							output << "write ---------- to ---------- (good), ";
						}
						if (faultPropagation.writeEnableFaulty)
						{
							output << boost::str(boost::format("write %s to %08x (bad)")
								% format_masked_data(faultPropagation.dataFaulty, faultPropagation.maskFaulty, 4u)
								% faultPropagation.addressFaulty);
						}
						else
						{
							output << "write ---------- to ---------- (bad)";
						}
						output << std::endl;
					}
				},
				[&](const Tpg::Sbst::ProgramCounterFaultPropagation& faultPropagation) {
					isFlowAltered = true;
					output << boost::str(boost::format("PC Invalidated Timeframe %d: %08x (good), %08x (bad)")
							% faultPropagation.timeframe
							% faultPropagation.dataNormal
							% faultPropagation.dataFaulty)
						<< std::endl;
				}
			), propagation);
		}
	}
	return true;
}

};
};
};
