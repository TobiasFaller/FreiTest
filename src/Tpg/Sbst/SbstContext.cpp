#include "Tpg/Sbst/SbstContext.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Sbst
{

SbstContext::SbstContext(std::string target, std::string displayName):
	Vcm::VcmContext(target, displayName),
	instructionSequenceIndex(NO_INSTRUCTION_SEQUENCE),
	testPatternIndex(NO_TEST_PATTERN)
{
}

SbstContext::SbstContext(const SbstContext& defaults, std::string target, std::string displayName):
	Vcm::VcmContext(defaults, target, displayName),
	instructionSequenceIndex(defaults.instructionSequenceIndex),
	testPatternIndex(defaults.testPatternIndex)
{
}

SbstContext::~SbstContext(void) = default;

void SbstContext::SetInstructionSequenceIndex(size_t index) const
{
	instructionSequenceIndex = index;
}

void SbstContext::SetTestPatternIndex(size_t index) const
{
	testPatternIndex = index;
}

size_t SbstContext::GetInstructionSequenceIndex(void) const
{
	return instructionSequenceIndex;
}

size_t SbstContext::GetTestPatternIndex(void) const
{
	return testPatternIndex;
}

};
};
};
