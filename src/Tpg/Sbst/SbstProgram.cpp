#include "Tpg/Sbst/SbstProgram.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>

#include "Basic/Overload.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

Instruction::Instruction(size_t instructionId):
    instructionId(instructionId),
    dataAccessIds(),
    opcode(0x00000000u),
    disassembly({})
{
}

Instruction::~Instruction(void) = default;

std::string to_string(const Instruction& instruction)
{
	std::string result;
	for (auto& instruction : instruction.disassembly)
	{
		if (!result.empty()) result += ", ";
		result += instruction;
	}
	return result;
}

DataAccess::DataAccess(size_t dataAccessId):
    dataAccessId(dataAccessId),
    instructionIds(),
    type(DataAccessType::Read),
    address(0x0000000000000000u),
    mask(0x0000000000000000u),
    value(0x0000000000000000u)
{
}

DataAccess::~DataAccess(void) = default;

std::string to_string(const DataAccess& dataAccess)
{
	return boost::str(boost::format("%s %s, Address: %08x, Mask: %08x, Value: %08x")
		% (dataAccess.bus == DataBusType::Data ? "Data" : "Instruction")
		% (dataAccess.type == DataAccessType::Read ? "Read" : "Write")
		% dataAccess.address
		% dataAccess.mask
		% dataAccess.value);
}

std::string to_string(const FaultPropagation& faultPropagation)
{
	return std::visit(Basic::overload(
		[](const NoFaultPropagation& noFaultPropagation) -> std::string {
			return "No propagation";
		},
		[](const ArchitectureRegisterFaultPropagation& faultPropagation) -> std::string {
			return boost::str(boost::format("Architecture Register [Timeframe %d, Write: %s (good) %s (bad), Mask %08x (good) vs %08x (bad), Register: %d (good), %d (bad), Data: %08x (good) %08x (bad)]")
				% faultPropagation.timeframe
				% (faultPropagation.writeEnableNormal ? "yes" : "no")
				% (faultPropagation.writeEnableFaulty ? "yes" : "no")
				% faultPropagation.maskNormal
				% faultPropagation.maskFaulty
				% faultPropagation.registerNormal
				% faultPropagation.registerFaulty
				% faultPropagation.dataNormal
				% faultPropagation.dataFaulty
			);
		},
		[](const ControlAndStatusRegisterFaultPropagation& faultPropagation) -> std::string {
			return boost::str(boost::format("Control / Status Register [Timeframe %d, Write: %s (good) %s (bad), Mask %08x (good) vs %08x (bad), Register: %d (good), %d (bad), Data: %08x (good) %08x (bad)]")
				% faultPropagation.timeframe
				% (faultPropagation.writeEnableNormal ? "yes" : "no")
				% (faultPropagation.writeEnableFaulty ? "yes" : "no")
				% faultPropagation.maskNormal
				% faultPropagation.maskFaulty
				% faultPropagation.registerNormal
				% faultPropagation.registerFaulty
				% faultPropagation.dataNormal
				% faultPropagation.dataFaulty
			);
		},
		[](const MemoryFaultPropagation& faultPropagation) -> std::string {
			return boost::str(boost::format("Memory [Timeframe %d, Write: %s (good) %s (bad), Mask %08x (good) vs %08x (bad), Address: %08x (good), %08x (bad), Data: %08x (good) %08x (bad)]")
				% faultPropagation.timeframe
				% (faultPropagation.writeEnableNormal ? "yes" : "no")
				% (faultPropagation.writeEnableFaulty ? "yes" : "no")
				% faultPropagation.maskNormal
				% faultPropagation.maskFaulty
				% faultPropagation.addressNormal
				% faultPropagation.addressFaulty
				% faultPropagation.dataNormal
				% faultPropagation.dataFaulty
			);
		},
		[](const ProgramCounterFaultPropagation& faultPropagation) -> std::string {
			return boost::str(boost::format("Program Counter [Timeframe %d, Write: %s (good) %s (bad), Mask %08x (good) vs %08x (bad), Data: %08x (good), %08x (bad)]")
				% faultPropagation.timeframe
				% (faultPropagation.writeEnableNormal ? "yes" : "no")
				% (faultPropagation.writeEnableFaulty ? "yes" : "no")
				% faultPropagation.maskNormal
				% faultPropagation.maskFaulty
				% faultPropagation.dataNormal
				% faultPropagation.dataFaulty
			);
		}
	), faultPropagation);
}

SbstInstructionSequence::SbstInstructionSequence(void):
	instructions(),
	dataAccesses(),
	faultPropagations(),
	properties(),
	processorPattern(),
	processorStartState(),
	processorEndState(),
	vcmPattern(),
	vcmStartState(),
	vcmEndState(),
	vcmParameters(),
	leadInTimeframes(0u),
	leadOutTimeframes(0u)
{
}

SbstInstructionSequence::~SbstInstructionSequence(void) = default;

void SbstInstructionSequence::SetInstructions(std::vector<Instruction> instructions)
{
    this->instructions = instructions;
}

void SbstInstructionSequence::SetDataAccesses(std::vector<DataAccess> dataAccesses)
{
    this->dataAccesses = dataAccesses;
}

void SbstInstructionSequence::SetFaultPropagations(std::vector<FaultPropagation> faultPropagations)
{
	this->faultPropagations = faultPropagations;
}

void SbstInstructionSequence::SetProcessorTestPattern(std::shared_ptr<Pattern::TestPattern> pattern)
{
    this->processorPattern = pattern;
}

void SbstInstructionSequence::SetProcessorStartState(std::vector<Basic::Logic> state)
{
    this->processorStartState = state;
}

void SbstInstructionSequence::SetProcessorEndState(std::vector<Basic::Logic> state)
{
    this->processorEndState = state;
}

void SbstInstructionSequence::SetVcmTestPattern(std::shared_ptr<Pattern::TestPattern> pattern)
{
    this->vcmPattern = pattern;
}

void SbstInstructionSequence::SetVcmStartState(std::vector<Basic::Logic> state)
{
    this->vcmStartState = state;
}

void SbstInstructionSequence::SetVcmEndState(std::vector<Basic::Logic> state)
{
    this->vcmEndState = state;
}

void SbstInstructionSequence::SetVcmParameters(std::map<std::string, Vcm::VcmParameter> parameters)
{
	this->vcmParameters = parameters;
}

void SbstInstructionSequence::SetLeadInTimeframes(size_t timeframes)
{
    this->leadInTimeframes = timeframes;
}

void SbstInstructionSequence::SetLeadOutTimeframes(size_t timeframes)
{
    this->leadOutTimeframes = timeframes;
}

const std::vector<Instruction>& SbstInstructionSequence::GetInstructions(void) const
{
    return instructions;
}

const std::vector<DataAccess>& SbstInstructionSequence::GetDataAccesses(void) const
{
    return dataAccesses;
}

const std::vector<FaultPropagation>& SbstInstructionSequence::GetFaultPropagations(void) const
{
	return faultPropagations;
}

const std::shared_ptr<Pattern::TestPattern>& SbstInstructionSequence::GetProcessorTestPattern(void) const
{
    return processorPattern;
}

const std::vector<Basic::Logic>& SbstInstructionSequence::GetProcessorStartState(void) const
{
    return processorStartState;
}

const std::vector<Basic::Logic>& SbstInstructionSequence::GetProcessorEndState(void) const
{
    return processorEndState;
}

const std::shared_ptr<Pattern::TestPattern>& SbstInstructionSequence::GetVcmTestPattern(void) const
{
    return vcmPattern;
}

const std::vector<Basic::Logic>& SbstInstructionSequence::GetVcmStartState(void) const
{
    return vcmStartState;
}

const std::vector<Basic::Logic>& SbstInstructionSequence::GetVcmEndState(void) const
{
    return vcmEndState;
}

const std::map<std::string, Vcm::VcmParameter>& SbstInstructionSequence::GetVcmParameters(void) const
{
	return vcmParameters;
}

const size_t& SbstInstructionSequence::GetLeadInTimeframes(void) const
{
    return leadInTimeframes;
}

const size_t& SbstInstructionSequence::GetLeadOutTimeframes(void) const
{
    return leadOutTimeframes;
}


SbstProgram SbstInstructionSequence::ConvertToSbst(void) const
{
	SbstProgram program;

	program.SetInstructions(GetInstructions());
	program.SetDataAccesses(GetDataAccesses());
	program.SetFaultPropagations(GetFaultPropagations());
	program.SetProcessorTestPattern(GetProcessorTestPattern());
	program.SetProcessorStartState(GetProcessorStartState());
	program.SetProcessorEndState(GetProcessorEndState());
	program.SetVcmTestPattern(GetVcmTestPattern());
	program.SetVcmStartState(GetVcmStartState());
	program.SetVcmEndState(GetVcmEndState());

	return program;
}

std::string to_string(const SbstInstructionSequence& instructionSequence)
{
	std::string result;
    result += "Instruction Sequence";
    result += ": instructions=";
    result += std::to_string(instructionSequence.instructions.size());

    result += ", dataAccesses=";
    result += std::to_string(instructionSequence.dataAccesses.size());

    result += ", faultPropagations=";
    result += std::to_string(instructionSequence.faultPropagations.size());

	for (const auto& [key, value] : instructionSequence.properties)
	{
        result += ", ";
		result += key;
		result += "=";

		if (value.type() == typeid(std::string))
		{
            result += "\"";
			result += std::any_cast<std::string>(value);
            result += "\"";
		}
		else if (value.type() == typeid(size_t))
		{
			result += std::to_string(std::any_cast<size_t>(value));
		}
	}

	return result;
}

std::string to_instruction_string(const SbstInstructionSequence& instructionSequence)
{
	std::string result;

	for (auto& instruction : instructionSequence.GetInstructions())
	{
		if (result != "") result += " -> ";

		if (instruction.disassembly.size() > 0u)
		{
			size_t disassemblyIndex = 0;
			for (auto& disassembly : instruction.disassembly)
			{
				if (disassemblyIndex++ != 0u) result += "; ";
				result += disassembly;
			}
		}
		else
		{
			result += "invalid";
		}

		result += boost::str(boost::format(" (0x%1$08x)") % instruction.opcode);
	}

	return result;
}

SbstProgram::SbstProgram(void) = default;
SbstProgram::~SbstProgram(void) = default;

void SbstProgram::SetTags(std::vector<std::string> tags)
{
	this->tags = tags;
}

void SbstProgram::AddTags(const std::vector<std::string>& tags)
{
	for (auto const& tag : tags)
	{
		this->tags.push_back(tag);
	}
}

void SbstProgram::ClearTags(void)
{
	tags.clear();
}

const std::vector<std::string>& SbstProgram::GetTags(void) const
{
	return tags;
}

size_t SbstProgram::AddInstructionSequence(SbstInstructionSequence instructionSequence)
{
	this->instructionSequences.emplace_back(instructionSequence);
	return this->instructionSequences.size() - 1u;
}

const std::vector<SbstInstructionSequence>& SbstProgram::GetInstructionSequences(void) const
{
	return this->instructionSequences;
}

void SbstProgram::SetInstructions(std::vector<Instruction> instructions)
{
    this->instructions = instructions;
}

void SbstProgram::SetDataAccesses(std::vector<DataAccess> dataAccesses)
{
    this->dataAccesses = dataAccesses;
}

void SbstProgram::SetFaultPropagations(std::vector<FaultPropagation> propagations)
{
	this->faultPropagations = propagations;
}

void SbstProgram::SetNumberOfFaults(size_t faults)
{
	this->faultPropagations.resize(faults);
	this->faultStatistics.resize(faults);
}

size_t SbstProgram::GetNumberOfFaults(void) const {
	return this->faultPropagations.size();
}

void SbstProgram::SetFaultPropagation(size_t faultIndex, const FaultPropagation& faultPropagation)
{
	ASSERT(faultIndex < this->faultPropagations.size());
	this->faultPropagations[faultIndex] = faultPropagation;
}

void SbstProgram::SetFaultStatistic(size_t faultIndex, const std::map<std::string, std::any>& faultStatistic)
{
	ASSERT(faultIndex < this->faultStatistics.size());
	this->faultStatistics[faultIndex] = faultStatistic;
}

void SbstProgram::SetProcessorTestPattern(std::shared_ptr<Pattern::TestPattern> pattern)
{
    this->processorPattern = pattern;
}

void SbstProgram::SetProcessorStartState(std::vector<Basic::Logic> state)
{
    this->processorStartState = state;
}

void SbstProgram::SetProcessorEndState(std::vector<Basic::Logic> state)
{
    this->processorEndState = state;
}

void SbstProgram::SetVcmTestPattern(std::shared_ptr<Pattern::TestPattern> pattern)
{
    this->vcmPattern = pattern;
}

void SbstProgram::SetVcmStartState(std::vector<Basic::Logic> state)
{
    this->vcmStartState = state;
}

void SbstProgram::SetVcmEndState(std::vector<Basic::Logic> state)
{
    this->vcmEndState = state;
}

const std::vector<Instruction>& SbstProgram::GetInstructions(void) const
{
    return instructions;
}

const std::vector<DataAccess>& SbstProgram::GetDataAccesses(void) const
{
    return dataAccesses;
}

const std::vector<FaultPropagation>& SbstProgram::GetFaultPropagations(void) const
{
	return faultPropagations;
}

const FaultPropagation& SbstProgram::GetFaultPropagation(size_t faultIndex) const
{
	return faultPropagations[faultIndex];
}

const std::vector<std::map<std::string, std::any>>& SbstProgram::GetFaultStatistics(void) const
{
	return faultStatistics;
}

const std::map<std::string, std::any>& SbstProgram::GetFaultStatistic(size_t faultIndex) const
{
	return faultStatistics[faultIndex];
}

const std::shared_ptr<Pattern::TestPattern>& SbstProgram::GetProcessorTestPattern(void) const
{
    return processorPattern;
}

const std::vector<Basic::Logic>& SbstProgram::GetProcessorStartState(void) const
{
    return processorStartState;
}

const std::vector<Basic::Logic>& SbstProgram::GetProcessorEndState(void) const
{
    return processorEndState;
}

const std::shared_ptr<Pattern::TestPattern>& SbstProgram::GetVcmTestPattern(void) const
{
    return vcmPattern;
}

const std::vector<Basic::Logic>& SbstProgram::GetVcmStartState(void) const
{
    return vcmStartState;
}

const std::vector<Basic::Logic>& SbstProgram::GetVcmEndState(void) const
{
    return vcmEndState;
}

};
};
};
