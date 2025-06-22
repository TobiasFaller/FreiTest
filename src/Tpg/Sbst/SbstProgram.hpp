#pragma once

#include <any>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <variant>
#include <mutex>

#include "Basic/Logic.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

enum class DataBusType { Instruction, Data };
enum class DataAccessType { Read, Write };

struct Instruction
{
    Instruction(size_t instructionId);
    virtual ~Instruction(void);

    size_t instructionId;
    std::vector<size_t> dataAccessIds;

    uint32_t opcode;
    std::vector<std::string> disassembly;
};

struct DataAccess
{
    DataAccess(size_t dataAccessId);
    virtual ~DataAccess(void);

    size_t dataAccessId;
    std::vector<size_t> instructionIds;

    DataBusType bus;
    DataAccessType type;
    uint64_t address;
    uint64_t mask;
    uint64_t value;
};

struct __attribute__ ((packed)) NoFaultPropagation { };

struct __attribute__ ((packed)) ArchitectureRegisterFaultPropagation {
	size_t timeframe;
	bool writeEnableNormal;
	bool writeEnableFaulty;
	uint64_t registerNormal;
	uint64_t registerFaulty;
	uint64_t maskNormal;
	uint64_t maskFaulty;
	uint64_t dataNormal;
	uint64_t dataFaulty;
};

struct __attribute__ ((packed)) ControlAndStatusRegisterFaultPropagation {
	size_t timeframe;
	bool writeEnableNormal;
	bool writeEnableFaulty;
	uint64_t registerNormal;
	uint64_t registerFaulty;
	uint64_t maskNormal;
	uint64_t maskFaulty;
	uint64_t dataNormal;
	uint64_t dataFaulty;
};

struct __attribute__ ((packed)) MemoryFaultPropagation {
	size_t timeframe;
	bool writeEnableNormal;
	bool writeEnableFaulty;
	uint64_t addressNormal;
	uint64_t addressFaulty;
	uint64_t maskNormal;
	uint64_t maskFaulty;
	uint64_t dataNormal;
	uint64_t dataFaulty;
};

struct __attribute__ ((packed)) ProgramCounterFaultPropagation {
	size_t timeframe;
	bool writeEnableNormal;
	bool writeEnableFaulty;
	uint64_t maskNormal;
	uint64_t maskFaulty;
	uint64_t dataNormal;
	uint64_t dataFaulty;
};

using FaultPropagation = std::variant<
	NoFaultPropagation,
	ArchitectureRegisterFaultPropagation,
	ControlAndStatusRegisterFaultPropagation,
	MemoryFaultPropagation,
	ProgramCounterFaultPropagation
>;

std::string to_string(const Instruction& instruction);
std::string to_string(const DataAccess& dataAccess);
std::string to_string(const FaultPropagation& faultPropagation);

class SbstProgram;

class SbstInstructionSequence
{
public:
    SbstInstructionSequence(void);
    virtual ~SbstInstructionSequence(void);

    void SetInstructions(std::vector<Instruction> instructions);
    void SetDataAccesses(std::vector<DataAccess> accesses);
	void SetFaultPropagations(std::vector<FaultPropagation> faultPropagations);
    void SetProcessorTestPattern(std::shared_ptr<Pattern::TestPattern> pattern);
    void SetProcessorStartState(std::vector<Basic::Logic> state);
    void SetProcessorEndState(std::vector<Basic::Logic> state);
    void SetVcmTestPattern(std::shared_ptr<Pattern::TestPattern> pattern);
    void SetVcmStartState(std::vector<Basic::Logic> state);
    void SetVcmEndState(std::vector<Basic::Logic> state);
	void SetVcmParameters(std::map<std::string, Vcm::VcmParameter> parameters);
	void SetLeadInTimeframes(size_t timeframes);
	void SetLeadOutTimeframes(size_t timeframes);

    const std::vector<Instruction>& GetInstructions(void) const;
    const std::vector<DataAccess>& GetDataAccesses(void) const;
	const std::vector<FaultPropagation>& GetFaultPropagations(void) const;
    const std::shared_ptr<Pattern::TestPattern>& GetProcessorTestPattern(void) const;
    const std::vector<Basic::Logic>& GetProcessorStartState(void) const;
    const std::vector<Basic::Logic>& GetProcessorEndState(void) const;
    const std::shared_ptr<Pattern::TestPattern>& GetVcmTestPattern(void) const;
    const std::vector<Basic::Logic>& GetVcmStartState(void) const;
    const std::vector<Basic::Logic>& GetVcmEndState(void) const;
	const std::map<std::string, Vcm::VcmParameter>& GetVcmParameters(void) const;
	const size_t& GetLeadInTimeframes(void) const;
	const size_t& GetLeadOutTimeframes(void) const;

	SbstProgram ConvertToSbst(void) const;

	template<typename T>
	void AddProperty(std::string name, T value)
	{
		properties.emplace(name, value);
	}

	void ApplyProperties(const std::map<std::string, std::any>& other)
	{
		for (auto const& [key, value] : other)
		{
			properties.emplace(key, value);
		}
	}

	template<typename T>
	bool HasProperty(const std::string& name) const
	{
		return GetProperty<T>(name).has_value();
	}

	template<typename T>
	std::optional<T> GetProperty(const std::string& name) const
	{
		if (auto it = properties.find(name); it != properties.end() && it->second.has_value())
		{
			try
			{
				return std::any_cast<T>(it->second);
			}
			catch (const std::bad_any_cast& e)
			{
				throw std::runtime_error("The property could not be cast to the given type!");
			}
		}

		return std::nullopt;
	}

    friend std::string to_string(const SbstInstructionSequence& instructionSequence);
	friend std::string to_instruction_string(const SbstInstructionSequence& instructionSequence);

private:
    std::vector<Instruction> instructions;
    std::vector<DataAccess> dataAccesses;
	std::vector<FaultPropagation> faultPropagations;
	std::map<std::string, std::any> properties;

    std::shared_ptr<Pattern::TestPattern> processorPattern;
    std::vector<Basic::Logic> processorStartState;
    std::vector<Basic::Logic> processorEndState;

    std::shared_ptr<Pattern::TestPattern> vcmPattern;
    std::vector<Basic::Logic> vcmStartState;
    std::vector<Basic::Logic> vcmEndState;
	std::map<std::string, Vcm::VcmParameter> vcmParameters;

	size_t leadInTimeframes;
	size_t leadOutTimeframes;

};

std::string to_string(const SbstInstructionSequence& instructionSequence);
std::string to_instruction_string(const SbstInstructionSequence& instructionSequence);

class SbstProgram
{
public:
    SbstProgram(void);
    virtual ~SbstProgram(void);

	void SetTags(std::vector<std::string> tags);
	void AddTags(const std::vector<std::string>& tags);
	void ClearTags(void);
	const std::vector<std::string>& GetTags(void) const;

    size_t AddInstructionSequence(SbstInstructionSequence instructionSequence);
    const std::vector<SbstInstructionSequence>& GetInstructionSequences(void) const;

    void SetInstructions(std::vector<Instruction> instructions);
    void SetDataAccesses(std::vector<DataAccess> accesses);
	void SetFaultPropagations(std::vector<FaultPropagation> propagations);
	void SetNumberOfFaults(size_t faults);
	size_t GetNumberOfFaults(void) const;
	void SetFaultPropagation(size_t faultIndex, const FaultPropagation& faultPropagation);
	void SetFaultStatistic(size_t faultIndex, const std::map<std::string, std::any>& faultStatistic);
    void SetProcessorTestPattern(std::shared_ptr<Pattern::TestPattern> pattern);
    void SetProcessorStartState(std::vector<Basic::Logic> state);
    void SetProcessorEndState(std::vector<Basic::Logic> state);
    void SetVcmTestPattern(std::shared_ptr<Pattern::TestPattern> pattern);
    void SetVcmStartState(std::vector<Basic::Logic> state);
    void SetVcmEndState(std::vector<Basic::Logic> state);

    const std::vector<Instruction>& GetInstructions(void) const;
    const std::vector<DataAccess>& GetDataAccesses(void) const;
	const std::vector<FaultPropagation>& GetFaultPropagations(void) const;
	const FaultPropagation& GetFaultPropagation(size_t faultIndex) const;
	const std::vector<std::map<std::string, std::any>>& GetFaultStatistics(void) const;
	const std::map<std::string, std::any>& GetFaultStatistic(size_t faultIndex) const;
    const std::shared_ptr<Pattern::TestPattern>& GetProcessorTestPattern(void) const;
    const std::vector<Basic::Logic>& GetProcessorStartState(void) const;
    const std::vector<Basic::Logic>& GetProcessorEndState(void) const;
    const std::shared_ptr<Pattern::TestPattern>& GetVcmTestPattern(void) const;
    const std::vector<Basic::Logic>& GetVcmStartState(void) const;
    const std::vector<Basic::Logic>& GetVcmEndState(void) const;

	template<typename T>
	void AddProperty(std::string name, T value)
	{
		properties.emplace(name, value);
	}

	void ApplyProperties(const std::map<std::string, std::any>& other)
	{
		for (auto const& [key, value] : other)
		{
			properties.emplace(key, value);
		}
	}

	template<typename T>
	bool HasProperty(const std::string& name) const
	{
		return GetProperty<T>(name).has_value();
	}

	template<typename T>
	std::optional<T> GetProperty(const std::string& name) const
	{
		if (auto it = properties.find(name); it != properties.end() && it->second.has_value())
		{
			try
			{
				return std::any_cast<T>(it->second);
			}
			catch (const std::bad_any_cast& e)
			{
				throw std::runtime_error("The property could not be cast to the given type!");
			}
		}

		return std::nullopt;
	}

    friend std::string to_string(const SbstProgram& program);

private:
	std::vector<std::string> tags;
    std::vector<SbstInstructionSequence> instructionSequences;

    std::vector<Instruction> instructions;
    std::vector<DataAccess> dataAccesses;
	std::vector<FaultPropagation> faultPropagations;
	std::vector<std::map<std::string, std::any>> faultStatistics;
	std::map<std::string, std::any> properties;

    std::shared_ptr<Pattern::TestPattern> processorPattern;
    std::vector<Basic::Logic> processorStartState;
    std::vector<Basic::Logic> processorEndState;

    std::shared_ptr<Pattern::TestPattern> vcmPattern;
    std::vector<Basic::Logic> vcmStartState;
    std::vector<Basic::Logic> vcmEndState;

};

std::string to_string(const SbstProgram& program);

};
};
};
