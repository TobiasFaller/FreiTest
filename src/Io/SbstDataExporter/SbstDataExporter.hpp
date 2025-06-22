#pragma once

#include <iostream>

#include "Tpg/Sbst/SbstProgram.hpp"
#include "Tpg/Sbst/SbstMemory.hpp"

namespace FreiTest
{
namespace Io
{
namespace Sbst
{

enum class InstructionSequenceFormat { Assembly, Hexadecimal };

bool ExportInstructionSequence(std::ostream& output, const Tpg::Sbst::SbstInstructionSequence& sequence, size_t sequenceNumber, InstructionSequenceFormat format);

enum class SbstProgramFormat { Assembly, LinkerFile, Hexadecimal };
enum class SbstProgramType { Online, Startup };

bool ExportSbstProgram(std::ostream& output, const Tpg::Sbst::SbstProgram& program, const Tpg::Sbst::SbstMemory& memory, SbstProgramFormat format);
bool ExportSbstData(std::ostream& output, const Tpg::Sbst::SbstProgram& program, const Tpg::Sbst::SbstMemory& memory);
bool ExportSbstTrace(std::ostream& output, const Tpg::Sbst::SbstProgram& program, SbstProgramType type);

};
};
};
