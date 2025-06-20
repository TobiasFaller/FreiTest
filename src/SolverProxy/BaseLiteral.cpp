#include "SolverProxy/BaseLiteral.hpp"

#include <iostream>
#include <string>

#include "Basic/Logging.hpp"

namespace SolverProxy
{

BaseLiteral BaseLiteral::operator *(const int multiplier) const
{
	DASSERT(multiplier == 1 || multiplier == -1) << "Invalid use of conditional inversion with multiplier of " + std::to_string(multiplier);
	if (multiplier == 1)
	{
		return *this;
	}
	else
	{
		return -(*this);
	}
}

std::ostream& operator<<(std::ostream& os, const BaseLiteral& literal)
{
	os << to_string(literal);
	return os;
}

std::string to_string(const BaseLiteral& literal)
{
	return (literal.IsNegated() ? "-" : "") + std::to_string(literal.GetVariable() + 1);
}

};
