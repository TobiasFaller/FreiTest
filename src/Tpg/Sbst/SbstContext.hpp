#pragma once

#include <string>
#include <map>
#include <limits>

#include "Tpg/Vcm/VcmContext.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

constexpr size_t NO_INSTRUCTION_SEQUENCE = std::numeric_limits<size_t>::max();
constexpr size_t NO_TEST_PATTERN = std::numeric_limits<size_t>::max();

class SbstContext:
	public Vcm::VcmContext
{
public:
	SbstContext(std::string target, std::string displayName);
	SbstContext(const SbstContext& defaults, std::string target, std::string displayName);
	virtual ~SbstContext(void);

	void SetInstructionSequenceIndex(size_t index) const;
	void SetTestPatternIndex(size_t index) const;
	size_t GetInstructionSequenceIndex(void) const;
	size_t GetTestPatternIndex(void) const;

private:
	// Allow the indices to be stored when generating an instruction sequence.
	mutable size_t instructionSequenceIndex;
	mutable size_t testPatternIndex;

};

};
};
};
