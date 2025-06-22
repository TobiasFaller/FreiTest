#pragma once

#include <boost/property_tree/ptree.hpp>

#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Io
{

bool ValidateCircuitGuard(const boost::property_tree::ptree& root, const Circuit::CircuitEnvironment& circuit);
boost::property_tree::ptree CreateCircuitGuard(const Circuit::CircuitEnvironment& circuit);

};
};
