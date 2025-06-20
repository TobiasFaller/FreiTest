#pragma once

#include <iostream>

#include "Basic/Logging.hpp"

namespace SolverProxy
{

class BaseLiteral
{
public:

	explicit inline BaseLiteral(void):
		id(0u)
	{
	}

	static constexpr BaseLiteral MakeFromUnsigned(unsigned int id)
	{
		return BaseLiteral(id);
	}

	static constexpr BaseLiteral MakeFromSigned(int id)
	{
		return BaseLiteral(((std::abs(id) - 1) << 1u) | ((id < 0) ? 1 : 0));
	}

	static constexpr BaseLiteral MakeFromVariable(unsigned int id)
	{
		return BaseLiteral(id << 1u);
	}

	unsigned int GetVariable(void) const
	{
		return id >> 1;
	}

	int GetSigned(void) const
	{
		return (static_cast<int>(id >> 1) + 1) * ((id & 1) ? -1 : 1);
	}

	unsigned int GetUnsigned(void) const
	{
		return id;
	}

	unsigned int GetLiteralID(void) const
	{
		return id;
	}

	void SetLiteralID(unsigned int idParam)
	{
		id = idParam;
	}

	inline bool IsNegated(void) const
	{
		return id & 1;
	}

	inline bool IsPositive(void) const
	{
		return !IsNegated();
	}

	BaseLiteral GetPositive(void) const
	{
		ASSERT(id != 0) << "UNDEFINED_LIT literal has no sign";
		return BaseLiteral(id & ~1u);
	}

	bool IsSet(void) const
	{
		return id > 1;
	}

	BaseLiteral operator -(void) const
	{
		ASSERT(id != 0) << "UNDEFINED_LIT literal can not be inverted";
		return BaseLiteral(id ^ 1);
	}

	// These operators are required for usage in container
	bool operator==(BaseLiteral const& rhs) const
	{
		return id == rhs.id;
	}

	bool operator!=(BaseLiteral const& rhs) const
	{
		return id != rhs.id;
	}

	bool operator<(BaseLiteral const& rhs) const
	{
		return id < rhs.id;
	}
	bool operator>(BaseLiteral const& rhs) const
	{
		return id > rhs.id;
	}

	bool operator<=(BaseLiteral const& rhs) const
	{
		return id <= rhs.id;
	}

	bool operator>=(BaseLiteral const& rhs) const
	{
		return id >= rhs.id;
	}

	BaseLiteral operator *(const int multiplier) const;

	friend std::ostream& operator<<(std::ostream& os, const BaseLiteral& literal);
	friend std::string to_string(const BaseLiteral& literal);

private:
	unsigned int id;

	inline constexpr BaseLiteral(unsigned int literalParam):
		id(literalParam)
	{
	}

};

// Hack needed for conditional inversion construction by multiply with 1 / -1
inline BaseLiteral operator*(const int lhs, const BaseLiteral& rhs)
{
	return rhs * lhs;
}

constexpr BaseLiteral UNDEFINED_LIT = BaseLiteral::MakeFromUnsigned(0u);
constexpr BaseLiteral NEG_LIT = BaseLiteral::MakeFromUnsigned(3u);
constexpr BaseLiteral POS_LIT = BaseLiteral::MakeFromUnsigned(2u);

};
