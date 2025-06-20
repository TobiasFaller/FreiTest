#include "Tpg/Container/LogicContainer.hpp"

namespace FreiTest
{
namespace Tpg
{

std::string to_string(const LogicContainer01& logicContainer)
{
	return "LogicContainer01 l0: " + to_string(logicContainer.l0);
}

std::string to_string(const LogicContainer01X& logicContainer)
{
	return "LogicContainer01X l1: " + to_string(logicContainer.l1) + " l0: " + to_string(logicContainer.l0);
}

std::string to_string(const LogicContainerU01X& logicContainer)
{
	return "LogicContainerU01X l1: " + to_string(logicContainer.l1) + " l0: " + to_string(logicContainer.l0);
}

std::string to_string(const LogicContainer01F& logicContainer)
{
	return "LogicContainer01F l1: " + to_string(logicContainer.l1) + " l0: " + to_string(logicContainer.l0);
}

std::string to_string(const LogicContainerU01F& logicContainer)
{
	return "LogicContainerU01F l1: " + to_string(logicContainer.l1) + " l0: " + to_string(logicContainer.l0);
}

};
};
