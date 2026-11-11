#pragma once

#include <vector>
#include <unordered_map>

#include "Basic/Fault/Specification/CellAwareSpecification.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest
{
namespace Io
{
namespace Udfm
{

Fault::CellAwareModelSpecification ConvertToSpecification(const Io::Udfm::UdfmModel& udfm);

};
};
};
