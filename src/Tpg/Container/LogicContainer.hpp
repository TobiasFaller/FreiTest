#pragma once

#include <array>
#include <iostream>
#include <type_traits>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "SolverProxy/BaseLiteral.hpp"

namespace FreiTest
{
namespace Tpg
{

/* Container for 2-valued logic:
 *
 * l0 | logic
 * 0  | 0
 * 1  | 1
 */
template<typename T>
struct LogicContainer01Base
{
	explicit LogicContainer01Base()
		: l0(SolverProxy::UNDEFINED_LIT)
	{
	}
	explicit LogicContainer01Base(SolverProxy::BaseLiteral l0)
		: l0(l0)
	{
	}

	T& operator=(const T& other)
	{
		l0 = other.l0;
		return *static_cast<T*>(this);
	}

	bool operator==(T const& rhs) const { return l0 == rhs.l0; }
	bool operator!=(T const& rhs) const { return l0 != rhs.l0; }

	T operator -() const { return T(-l0); }
	T operator !() const { return T(-l0); }
	T operator *(int multiplier) const
	{
		DASSERT(multiplier == 1 || multiplier == -1) << "Invalid use of conditional inversion with multiplier of " << multiplier;
		return (multiplier == 1) ? *static_cast<const T*>(this) : !*static_cast<const T*>(this);
	}

	bool IsSet() const { return l0.IsSet(); }

	SolverProxy::BaseLiteral l0;

};

/* Container for 3-valued logic:
 *
 * l1 l0 | logic
 * 0  0  | X
 * 0  1  | 0
 * 1  0  | 1
 */
template<typename T>
struct LogicContainer01XBase
{
	explicit LogicContainer01XBase():
		l0(SolverProxy::UNDEFINED_LIT),
		l1(SolverProxy::UNDEFINED_LIT)
	{
	}
	explicit LogicContainer01XBase(SolverProxy::BaseLiteral l0, SolverProxy::BaseLiteral l1):
		l0(l0),
		l1(l1)
	{
	}

	T& operator=(const T& other)
	{
		l0 = other.l0;
		l1 = other.l1;
		return *static_cast<T*>(this);
	}

	bool operator==(T const& rhs) const { return ((l0 == rhs.l0) && (l1 == rhs.l1)); }
	bool operator!=(T const& rhs) const { return ((l0 != rhs.l0) || (l1 != rhs.l1)); }

	T operator -() const { return T(l1, l0); }
	T operator !() const { return T(l1, l0); }
	T operator *(int multiplier) const
	{
		DASSERT(multiplier == 1 || multiplier == -1) << "Invalid use of conditional inversion with multiplier of " << multiplier;
		return (multiplier == 1) ? *static_cast<const T*>(this) : !*static_cast<const T*>(this);
	}

	bool IsSet() const { return l0.IsSet() && l1.IsSet(); }

	SolverProxy::BaseLiteral l0;
	SolverProxy::BaseLiteral l1;

};

/* Container for 4-valued logic:
 *
 * l1 l0 | logic
 * 0  0  | U
 * 0  1  | 0
 * 1  0  | 1
 * 1  1  | X
 */
template<typename T>
struct LogicContainerU01XBase
{
	explicit LogicContainerU01XBase():
		l1(SolverProxy::UNDEFINED_LIT),
		l0(SolverProxy::UNDEFINED_LIT)
	{
	}
	explicit LogicContainerU01XBase(SolverProxy::BaseLiteral l1, SolverProxy::BaseLiteral l0):
		l1(l1),
		l0(l0)
	{
	}

	T& operator=(const T& other)
	{
		l0 = other.l0;
		l1 = other.l1;
		return *static_cast<T*>(this);
	}

	bool operator==(T const& rhs) const { return ((l0 == rhs.l0) && (l1 == rhs.l1)); }
	bool operator!=(T const& rhs) const { return ((l0 != rhs.l0) || (l1 != rhs.l1)); }

	T operator -() const { return T(l0, l1); }
	T operator !() const { return T(l0, l1); }
	T operator *(int multiplier) const
	{
		DASSERT(multiplier == 1 || multiplier == -1) << "Invalid use of conditional inversion with multiplier of " << multiplier;
		return (multiplier == 1) ? *static_cast<const T*>(this) : !*static_cast<const T*>(this);
	}

	bool IsSet() const { return l1.IsSet() && l0.IsSet(); }

	SolverProxy::BaseLiteral l1;
	SolverProxy::BaseLiteral l0;

};

// ----------------------------------------------------------------------------
// Predefined logic containers
// ----------------------------------------------------------------------------

template<typename LogicContainer> struct get_logic_container_name_impl { };
template<typename LogicContainer> constexpr auto get_logic_container_name = get_logic_container_name_impl<LogicContainer>::value;

template<typename LogicContainer> struct get_logic_container_values_impl {};
template<typename LogicContainer> constexpr auto get_logic_container_values = get_logic_container_values_impl<LogicContainer>::value;

#define NUM_VALUES(...)  (sizeof((Basic::Logic[]){__VA_ARGS__}) / sizeof(Basic::Logic))
#define DEFINE_CONTAINER(NAME, BASE, ...) \
	struct LogicContainer ## NAME: public BASE<LogicContainer ## NAME> { \
		using BASE<LogicContainer ## NAME>::BASE; \
		LogicContainer ## NAME& operator=(const LogicContainer ## NAME& other) {  *static_cast<BASE<LogicContainer ## NAME>*>(this) = *static_cast<const BASE<LogicContainer ## NAME>*>(&other); return *this; } \
		using BASE<LogicContainer ## NAME>::operator==; \
		using BASE<LogicContainer ## NAME>::operator!=; \
		friend std::ostream& operator<<(std::ostream& os, const LogicContainer ## NAME& logicContainer) { os << to_string(logicContainer); return os; } \
		friend std::string to_string(const LogicContainer ## NAME& logicContainer); \
	}; \
	inline LogicContainer ## NAME operator*(const int lhs, LogicContainer ## NAME& rhs) { return rhs * lhs; } \
	template<typename LogicContainer> using is_ ## NAME ## _encoding = std::is_same<LogicContainer, LogicContainer ## NAME>; \
	template<typename LogicContainer> constexpr bool is_ ## NAME ## _encoding_v = is_ ## NAME ## _encoding<LogicContainer>::value; \
	template<> struct get_logic_container_name_impl<LogicContainer ## NAME> { static constexpr const char* value = #NAME; }; \
	template<> struct get_logic_container_values_impl<LogicContainer ## NAME> { static constexpr const std::array<Basic::Logic, NUM_VALUES(__VA_ARGS__)> value = { __VA_ARGS__ }; }

DEFINE_CONTAINER(01,   LogicContainer01Base,   Basic::Logic::LOGIC_ZERO,      Basic::Logic::LOGIC_ONE);
DEFINE_CONTAINER(01X,  LogicContainer01XBase,  Basic::Logic::LOGIC_DONT_CARE, Basic::Logic::LOGIC_ZERO, Basic::Logic::LOGIC_ONE);
DEFINE_CONTAINER(01F,  LogicContainer01XBase,  Basic::Logic::LOGIC_DONT_CARE, Basic::Logic::LOGIC_ZERO, Basic::Logic::LOGIC_ONE);
DEFINE_CONTAINER(U01X, LogicContainerU01XBase, Basic::Logic::LOGIC_UNKNOWN,   Basic::Logic::LOGIC_ZERO, Basic::Logic::LOGIC_ONE, Basic::Logic::LOGIC_DONT_CARE);
DEFINE_CONTAINER(U01F, LogicContainerU01XBase, Basic::Logic::LOGIC_UNKNOWN,   Basic::Logic::LOGIC_ZERO, Basic::Logic::LOGIC_ONE, Basic::Logic::LOGIC_DONT_CARE);

#undef NUM_VALUES
#undef DEFINE_CONTAINER

// Define the macro FOR_CONTAINER(CONTAINER) and do the work in there
#define INSTANTIATE_ALL_CONTAINER \
	FOR_CONTAINER(LogicContainer01) \
	FOR_CONTAINER(LogicContainer01X) \
	FOR_CONTAINER(LogicContainer01F) \
	FOR_CONTAINER(LogicContainerU01X) \
	FOR_CONTAINER(LogicContainerU01F)

// Simplifying definitions
template<typename LogicContainer> using is_classical_encoding = std::disjunction<is_01_encoding<LogicContainer>, is_01X_encoding<LogicContainer>, is_U01X_encoding<LogicContainer>>;
template<typename LogicContainer> constexpr bool is_classical_encoding_v = is_classical_encoding<LogicContainer>::value;

template<typename LogicContainer> using is_forgetful_encoding = std::disjunction<is_01F_encoding<LogicContainer>, is_U01F_encoding<LogicContainer>>;
template<typename LogicContainer> constexpr bool is_forgetful_encoding_v = is_forgetful_encoding<LogicContainer>::value;

// ----------------------------------------------------------------------------
// Sanity checks for containers
// ----------------------------------------------------------------------------

static_assert( is_01_encoding_v<LogicContainer01>);
static_assert(!is_01_encoding_v<LogicContainer01X>);
static_assert(!is_01_encoding_v<LogicContainerU01X>);

static_assert(!is_01X_encoding_v<LogicContainer01>);
static_assert( is_01X_encoding_v<LogicContainer01X>);
static_assert(!is_01X_encoding_v<LogicContainerU01X>);

static_assert(!is_U01X_encoding_v<LogicContainer01>);
static_assert(!is_U01X_encoding_v<LogicContainer01X>);
static_assert( is_U01X_encoding_v<LogicContainerU01X>);

};
};
