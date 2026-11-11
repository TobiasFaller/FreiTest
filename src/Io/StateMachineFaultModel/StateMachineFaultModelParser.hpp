#pragma once

#include <istream>
#include <memory>

#include "Basic/Fault/Specification/CellAwareSpecification.hpp"

namespace FreiTest
{
namespace Io
{
namespace StateMachineFaultModel
{

std::unique_ptr<Fault::CellAwareModelSpecification> Parse(std::istream& input);

};
};
};
