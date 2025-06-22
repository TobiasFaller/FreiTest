#pragma once

#include <istream>
#include <memory>

#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest {
namespace Io {
namespace Udfm {

std::unique_ptr<UdfmModel> ParseUdfm(std::istream& input);

};
};
};

