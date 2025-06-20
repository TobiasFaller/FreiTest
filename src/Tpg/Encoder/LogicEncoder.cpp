#include "Tpg/Encoder/LogicEncoder.hpp"

#include <boost/core/demangle.hpp>

#include <cassert>
#include <vector>
#include <string>

#include "Basic/Logic.hpp"
#include "SolverProxy/ISolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Tpg/Container/LogicContainer.hpp"

using namespace std;
using namespace SolverProxy;
using namespace SolverProxy::Sat;
using namespace SolverProxy::Bmc;
using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Tpg
{

template<typename LogicContainer>
constexpr void NotImplemented(void);

template<typename LogicContainer>
[[noreturn]] void InvalidLogicValue(const char* function, Logic value);

template<typename LogicContainer>
[[noreturn]] void InvalidLogicConstraint(const char* function, LogicConstraint constraint);

template<typename LogicContainer>
constexpr void AssertSet(const char* function, const char* containerName, LogicContainer container);

}
}

#define NOT_IMPLEMENTED NotImplemented<LogicContainer>()
#define INVALID_VALUE(value) InvalidLogicValue<LogicContainer>(__PRETTY_FUNCTION__, value)
#define INVALID_CONSTRAINT(constraint) InvalidLogicConstraint<LogicContainer>(__PRETTY_FUNCTION__, constraint)
#define ASSERT_SET(container) AssertSet(__PRETTY_FUNCTION__, #container, container)

// Include all automatically generated Encode... methods
// Run the gate logic optimization workflow to regenerate them.

#define BASIC_LOGIC_ENCODER_GEN
#include "Tpg/Encoder/LogicEncoder.gen.ipp"
#undef BASIC_LOGIC_ENCODER_GEN

namespace FreiTest
{
namespace Tpg
{

template<typename LogicContainer>
constexpr void NotImplemented(void)
{
	// Ugly hack to make static_assert dependent on the type used in LogicContainer template parameter
	static_assert(std::is_same_v<LogicContainer, int>, "The logic container is not implemented");
}

template<typename LogicContainer>
[[noreturn]] void InvalidLogicValue(const char* function, Logic value)
{
	Logging::Panic(std::string("Invalid logic value \"") + static_cast<char>(value) + "\" in function " + function
		+ " with logic container " + boost::core::demangle(typeid(LogicContainer).name()));
}

template<typename LogicContainer>
[[noreturn]] void InvalidLogicConstraint(const char* function, LogicConstraint constraint)
{
	Logging::Panic(std::string("Invalid logic constraint \"") + static_cast<char>(constraint) + "\" in function " + function
		+ " with logic container " + boost::core::demangle(typeid(LogicContainer).name()));
}

template<typename LogicContainer>
constexpr void AssertSet(const char* function, const char* containerName, LogicContainer container)
{
	if constexpr (std::is_same_v<LogicContainer, BaseLiteral>)
	{
		ASSERT(container.IsSet()) << "Literal " << containerName << " is not set in function " << function;
	}
	else if constexpr (is_01_encoding_v<LogicContainer>)
	{
		ASSERT(container.l0.IsSet()) << "Logic container " << containerName << " is not set in function " << function;
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		ASSERT(container.l1.IsSet()) << "Logic container " << containerName << " is not set in function " << function;
		ASSERT(container.l0.IsSet()) << "Logic container " << containerName << " is not set in function " << function;
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		ASSERT(container.l1.IsSet()) << "Logic container " << containerName << " is not set in function " << function;
		ASSERT(container.l0.IsSet()) << "Logic container " << containerName << " is not set in function " << function;
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

LogicEncoder::LogicEncoder(shared_ptr<ISolverProxy> solver):
	_solver(solver)
{
}

LogicEncoder::~LogicEncoder(void) = default;

template<typename LogicContainer>
LogicContainer LogicEncoder::NewLogicContainer(void)
{
	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		return NewLogicContainer<LogicContainer, Literal0>(_solver->NewLiteral());
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		auto container = NewLogicContainer<LogicContainer, Literal1, Literal0>(_solver->NewLiteral(), _solver->NewLiteral());
		// Forbid invalid literal combination (unassigned value in encoding)
		_solver->CommitClause(-container.l0, -container.l1);
		return container;
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		return NewLogicContainer<LogicContainer, Literal1, Literal0>(_solver->NewLiteral(), _solver->NewLiteral());
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
LogicContainer LogicEncoder::NewLogicContainer(Logic value) const
{
	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		/*
			Value | l0
			------+----
			0	 | 0
			1	 | 1
		*/
		switch (value)
		{
			case Logic::LOGIC_ZERO:
				return NewLogicContainer<LogicContainer, Literal0>(NEG_LIT);
			case Logic::LOGIC_ONE:
				return NewLogicContainer<LogicContainer, Literal0>(POS_LIT);
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		/*
			Value | l1  l0
			------+--------
			X	 | 0   0
			0	 | 0   1
			1	 | 1   0
		*/
		switch (value)
		{
			case Logic::LOGIC_DONT_CARE:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(NEG_LIT, NEG_LIT);
			case Logic::LOGIC_ZERO:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(NEG_LIT, POS_LIT);
			case Logic::LOGIC_ONE:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(POS_LIT, NEG_LIT);
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		/*
			Value | l1  l0
			------+--------
			U	 | 0   0
			0	 | 0   1
			1	 | 1   0
			X	 | 1   1
		*/
		switch (value)
		{
			case Logic::LOGIC_UNKNOWN:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(NEG_LIT, NEG_LIT);
			case Logic::LOGIC_ZERO:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(NEG_LIT, POS_LIT);
			case Logic::LOGIC_ONE:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(POS_LIT, NEG_LIT);
			case Logic::LOGIC_DONT_CARE:
				return NewLogicContainer<LogicContainer, Literal1, Literal0>(POS_LIT, POS_LIT);
			default:
				INVALID_VALUE(value);
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<>
LogicContainer01 LogicEncoder::NewLogicContainer<LogicContainer01, Literal0>(BaseLiteral literal) const
{
	ASSERT_SET(literal);

	// Constructor: LogicContainer01(l0)
	return LogicContainer01(literal);
}

template<>
LogicContainer01X LogicEncoder::NewLogicContainer<LogicContainer01X, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const
{
	ASSERT_SET(literal1);
	ASSERT_SET(literal0);

	// Constructor: LogicContainer01X(l0, l1)
	return LogicContainer01X(literal0, literal1);
}

template<>
LogicContainer01F LogicEncoder::NewLogicContainer<LogicContainer01F, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const
{
	ASSERT_SET(literal1);
	ASSERT_SET(literal0);

	// Constructor: LogicContainer01F(l0, l1)
	return LogicContainer01F(literal0, literal1);
}

template<>
LogicContainerU01X LogicEncoder::NewLogicContainer<LogicContainerU01X, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const
{
	ASSERT_SET(literal1);
	ASSERT_SET(literal0);

	// Constructor: LogicContainerU01X(l1, l0)
	return LogicContainerU01X(literal1, literal0);
}

template<>
LogicContainerU01F LogicEncoder::NewLogicContainer<LogicContainerU01F, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const
{
	ASSERT_SET(literal1);
	ASSERT_SET(literal0);

	// Constructor: LogicContainerU01F(l1, l0)
	return LogicContainerU01F(literal1, literal0);
}

template<typename LogicContainer, typename SourceLogicContainer>
LogicContainer LogicEncoder::ConvertLogicContainer(const SourceLogicContainer& value)
{
	ASSERT_SET(value);

	if constexpr (std::is_same_v<LogicContainer, LogicContainer01>)
	{
		if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01>)
		{
			return NewLogicContainer<LogicContainer01, Literal0>(value.l0);
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01X> || std::is_same_v<SourceLogicContainer, LogicContainer01F>)
		{
			return NewLogicContainer<LogicContainer01, Literal0>(value.l1); // (literal1 == 1) == LOGIC_ONE
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainerU01X> || std::is_same_v<SourceLogicContainer, LogicContainerU01F>)
		{
			return NewLogicContainer<LogicContainer01, Literal0>(value.l1); // (literal1 == 1) == LOGIC_ONE
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	else if constexpr (std::is_same_v<LogicContainer, LogicContainer01X> || std::is_same_v<LogicContainer, LogicContainer01F>)
	{
		if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01>)
		{
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(value.l0, -value.l0); // Pseudo 01X encoding
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01X> || std::is_same_v<SourceLogicContainer, LogicContainer01F>)
		{
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(value.l1, value.l0);
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainerU01X> || std::is_same_v<SourceLogicContainer, LogicContainerU01F>)
		{
			// U01X         01X
			// l1 l0     -> l1 l0
			// 0  0 (U)  -> 0  0 (X)
			// 0  1 (0)  -> 0  1 (0)
			// 1  0 (1)  -> 1  0 (1)
			// 1  1 (X)  -> 0  0 (X)
			auto const sourceL0 { NewLogicContainer<LogicContainer01, Literal0>(value.l0) };
			auto const sourceL1 { NewLogicContainer<LogicContainer01, Literal0>(value.l1) };
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(
				EncodeAnd(sourceL1, -sourceL0).l0,
				EncodeAnd(-sourceL1, sourceL0).l0
			);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	else if constexpr (std::is_same_v<LogicContainer, LogicContainerU01X> || std::is_same_v<LogicContainer, LogicContainerU01F>)
	{
		if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01>)
		{
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(value.l0, -value.l0); // Pseudo 01X encoding
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainer01X> || std::is_same_v<SourceLogicContainer, LogicContainer01F>)
		{
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(-value.l0, -value.l1); // Invert and swap literals
		}
		else if constexpr (std::is_same_v<SourceLogicContainer, LogicContainerU01X> || std::is_same_v<SourceLogicContainer, LogicContainerU01F>)
		{
			return NewLogicContainer<LogicContainer, Literal1, Literal0>(value.l1, value.l0);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
Logic LogicEncoder::GetSolvedLogicValue(const LogicContainer& target) const
{
	ASSERT_SET(target);

	Logic constant = GetConstantLogicValue<LogicContainer>(target);
	if (constant != Logic::LOGIC_INVALID)
	{
		return constant;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		auto l0 { _solver->GetLiteralValue(target.l0) };
		if (l0 == Value::DontCare)
		{
			return Logic::LOGIC_DONT_CARE;
		}
		else if (l0 == Value::Negative)
		{
			return Logic::LOGIC_ZERO;
		}
		else if (l0 == Value::Positive)
		{
			return Logic::LOGIC_ONE;
		}

		Logging::Panic("Invalid result returned by Solver GetLiteralValue calls");
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		auto l1 { _solver->GetLiteralValue(target.l1) };
		auto l0 { _solver->GetLiteralValue(target.l0) };

		if ((l1 == Value::Negative && l0 == Value::Negative)
			|| (l1 == Value::Negative && l0 == Value::DontCare)
			|| (l1 == Value::DontCare && l0 == Value::Negative)
			|| (l1 == Value::DontCare && l0 == Value::DontCare))
		{
			return Logic::LOGIC_DONT_CARE;
		}
		else if ((l1 == Value::Negative && l0 == Value::Positive)
			|| (l1 == Value::DontCare && l0 == Value::Positive))
		{
			return Logic::LOGIC_ZERO;
		}
		else if ((l1 == Value::Positive && l0 == Value::Negative)
			|| (l1 == Value::Positive && l0 == Value::DontCare))
		{
			return Logic::LOGIC_ONE;
		}

		Logging::Panic("Invalid result returned by Solver GetLiteralValue calls");
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		auto l1 { _solver->GetLiteralValue(target.l1) };
		auto l0 { _solver->GetLiteralValue(target.l0) };

		if ((l1 == Value::Negative && l0 == Value::Negative)
			|| (l1 == Value::Negative && l0 == Value::DontCare)
			|| (l1 == Value::DontCare && l0 == Value::Negative)
			|| (l1 == Value::DontCare && l0 == Value::DontCare))
		{
			return Logic::LOGIC_UNKNOWN;
		}
		else if ((l1 == Value::Negative && l0 == Value::Positive)
			|| (l1 == Value::DontCare && l0 == Value::Positive))
		{
			return Logic::LOGIC_ZERO;
		}
		else if ((l1 == Value::Positive && l0 == Value::Negative)
			|| (l1 == Value::Positive && l0 == Value::DontCare))
		{
			return Logic::LOGIC_ONE;
		}
		else if (l1 == Value::Positive && l0 == Value::Positive)
		{
			return Logic::LOGIC_DONT_CARE;
		}

		Logging::Panic("Invalid result returned by Solver GetLiteralValue calls");
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
bool LogicEncoder::IsConstantLogicValue(const LogicContainer& target) const
{
	return (GetConstantLogicValue<LogicContainer>(target) != Logic::LOGIC_INVALID);
}

template<typename LogicContainer>
Logic LogicEncoder::GetConstantLogicValue(const LogicContainer& target) const
{
	ASSERT_SET(target);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (target.l0 == NEG_LIT)
		{
			return Logic::LOGIC_ZERO;
		}
		else if (target.l0 == POS_LIT)
		{
			return Logic::LOGIC_ONE;
		}

		return Logic::LOGIC_INVALID;
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		if (target.l1 == NEG_LIT && target.l0 == NEG_LIT)
		{
			return Logic::LOGIC_DONT_CARE;
		}
		else if (target.l1 == NEG_LIT && target.l0 == POS_LIT)
		{
			return Logic::LOGIC_ZERO;
		}
		else if (target.l1 == POS_LIT && target.l0 == NEG_LIT)
		{
			return Logic::LOGIC_ONE;
		}

		return Logic::LOGIC_INVALID;
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		if (target.l1 == NEG_LIT && target.l0 == NEG_LIT)
		{
			return Logic::LOGIC_UNKNOWN;
		}
		else if (target.l1 == NEG_LIT && target.l0 == POS_LIT)
		{
			return Logic::LOGIC_ZERO;
		}
		else if (target.l1 == POS_LIT && target.l0 == NEG_LIT)
		{
			return Logic::LOGIC_ONE;
		}
		else if (target.l1 == POS_LIT && target.l0 == POS_LIT)
		{
			return Logic::LOGIC_DONT_CARE;
		}

		return Logic::LOGIC_INVALID;
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValue(const LogicContainer& target, Logic value)
{
	ASSERT_SET(target);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				_solver->CommitClause( target.l0);
				break;
			case Logic::LOGIC_ZERO:
				_solver->CommitClause(-target.l0);
				break;
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case Logic::LOGIC_ZERO:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause( target.l0);
				break;
			case Logic::LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause(-target.l0);
				break;
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case Logic::LOGIC_ZERO:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause( target.l0);
				break;
			case Logic::LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause( target.l0);
				break;
			case Logic::LOGIC_UNKNOWN:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause(-target.l0);
				break;
			default:
				INVALID_VALUE(value);
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueConstraint(const LogicContainer& target, LogicConstraint constraint)
{
	ASSERT_SET(target);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
			case LogicConstraint::NOT_LOGIC_ZERO:
				_solver->CommitClause( target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_ZERO:
			case LogicConstraint::NOT_LOGIC_ONE:
				_solver->CommitClause(-target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
			case LogicConstraint::ONLY_LOGIC_01:
			case LogicConstraint::ONLY_LOGIC_01X:
			case LogicConstraint::NO_CONSTRAINT:
				break;
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_ZERO:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause( target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1,  target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01X:
				_solver->CommitClause(-target.l1, -target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_01:
				_solver->CommitClause(-target.l1, -target.l0);
				_solver->CommitClause( target.l1,  target.l0);
				break;
			case LogicConstraint::NO_CONSTRAINT:
				break;
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_ZERO:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause( target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1);
				_solver->CommitClause( target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_UNKNOWN:
				_solver->CommitClause(-target.l1);
				_solver->CommitClause(-target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1, -target.l0);
				break;
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01X:
				_solver->CommitClause( target.l1,  target.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_01:
				_solver->CommitClause(-target.l1, -target.l0);
				_solver->CommitClause( target.l1,  target.l0);
				break;
			case LogicConstraint::NO_CONSTRAINT:
				break;
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicEquivalenceConstraint(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l0, -in2.l0);
		_solver->CommitClause(-in1.l0,  in2.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1, -in2.l1);
		_solver->CommitClause(-in1.l1,  in2.l1);
		_solver->CommitClause( in1.l0, -in2.l0);
		_solver->CommitClause(-in1.l0,  in2.l0);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1, -in2.l1);
		_solver->CommitClause(-in1.l1,  in2.l1);
		_solver->CommitClause( in1.l0, -in2.l0);
		_solver->CommitClause(-in1.l0,  in2.l0);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogic01DifferenceConstraint(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l0,  in2.l0); // Disallow in1 0, in2 0 values
		_solver->CommitClause(-in1.l0, -in2.l0); // Disallow in1 1, in2 1 values
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1,  in1.l0); // Disallow in1 X value
		_solver->CommitClause( in2.l1,  in2.l0); // Disallow in2 X value
		_solver->CommitClause( in1.l1, -in1.l0,  in2.l1, -in2.l0); // Disallow in1 0, in2 0 values
		_solver->CommitClause(-in1.l1,  in1.l0, -in2.l1,  in2.l0); // Disallow in1 1, in2 1 values
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1,  in1.l0); // Disallow in1 U value
		_solver->CommitClause(-in1.l1, -in1.l0); // Disallow in1 X value
		_solver->CommitClause( in2.l1,  in2.l0); // Disallow in2 U value
		_solver->CommitClause(-in2.l1, -in2.l0); // Disallow in2 X value
		_solver->CommitClause( in1.l1, -in1.l0,  in2.l1, -in2.l0); // Disallow 0, 0 values
		_solver->CommitClause(-in1.l1,  in1.l0, -in2.l1,  in2.l0); // Disallow 1, 1 values
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
LogicContainer01 LogicEncoder::EncodeLogicValueDetector(const LogicContainer& target, Logic value)
{
	ASSERT_SET(target);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				return target;
			case Logic::LOGIC_ZERO:
				return -target;
			case Logic::LOGIC_UNKNOWN:
			case Logic::LOGIC_DONT_CARE:
				return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		if (value == Logic::LOGIC_UNKNOWN)
		{
			return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
		}

		LogicContainer01 result = NewLogicContainer<LogicContainer01>();
		EncodeLogicValueDetector(target, value, result);
		return result;
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		LogicContainer01 result = NewLogicContainer<LogicContainer01>();
		EncodeLogicValueDetector(target, value, result);
		return result;
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
LogicContainer01 LogicEncoder::EncodeLogicValueConstraintDetector(const LogicContainer& target, LogicConstraint constraint)
{
	ASSERT_SET(target);

	if (constraint == LogicConstraint::NO_CONSTRAINT)
	{
		return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE);
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
			case LogicConstraint::NOT_LOGIC_ZERO:
				return target;
			case LogicConstraint::ONLY_LOGIC_ZERO:
			case LogicConstraint::NOT_LOGIC_ONE:
				return -target;
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01:
			case LogicConstraint::ONLY_LOGIC_01X:
				return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE);
			case LogicConstraint::ONLY_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		if (constraint == LogicConstraint::NOT_LOGIC_UNKNOWN)
		{
			return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE);
		}
		if (constraint == LogicConstraint::ONLY_LOGIC_UNKNOWN)
		{
			return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
		}

		LogicContainer01 result = NewLogicContainer<LogicContainer01>();
		EncodeLogicValueConstraintDetector(target, constraint, result);
		return result;
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		LogicContainer01 result = NewLogicContainer<LogicContainer01>();
		EncodeLogicValueConstraintDetector(target, constraint, result);
		return result;
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
LogicContainer01 LogicEncoder::EncodeLogicEquivalenceDetector(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ONE);
	}

	LogicContainer01 result = NewLogicContainer<LogicContainer01>();
	EncodeLogicEquivalenceDetector(in1, in2, result);
	return result;
}

template<typename LogicContainer>
LogicContainer01 LogicEncoder::EncodeLogic01DifferenceDetector(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return NewLogicContainer<LogicContainer01>(Logic::LOGIC_ZERO);
	}

	LogicContainer01 result = NewLogicContainer<LogicContainer01>();
	EncodeLogic01DifferenceDetector(in1, in2, result);
	return result;
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueDetector(const LogicContainer& target, Logic value, const LogicContainer01& out)
{
	ASSERT_SET(target);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				EncodeBuffer(target, out);
				return;
			case Logic::LOGIC_ZERO:
				EncodeInverter(target, out);
				return;
			case Logic::LOGIC_UNKNOWN:
			case Logic::LOGIC_DONT_CARE:
				EncodeLogicValue(out, Logic::LOGIC_ZERO);
				return;
			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case Logic::LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;
			case Logic::LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1,  target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;

			default:
				INVALID_VALUE(value);
		}
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		switch (value)
		{
			case Logic::LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case Logic::LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;
			case Logic::LOGIC_UNKNOWN:
				_solver->CommitClause( target.l1,  target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case Logic::LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1, -target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;

			default:
				INVALID_VALUE(value);
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueConstraintDetector(const LogicContainer& target, LogicConstraint constraint, const LogicContainer01& out)
{
	ASSERT_SET(target);
	ASSERT_SET(out);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
			case LogicConstraint::NOT_LOGIC_ZERO:
				EncodeBuffer(target, out);
				return;
			case LogicConstraint::ONLY_LOGIC_ZERO:
			case LogicConstraint::NOT_LOGIC_ONE:
				EncodeInverter(target, out);
				return;
			case LogicConstraint::NO_CONSTRAINT:
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01:
			case LogicConstraint::ONLY_LOGIC_01X:
				EncodeLogicValue(out, Logic::LOGIC_ONE);
				return;
			case LogicConstraint::ONLY_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				EncodeLogicValue(out, Logic::LOGIC_ZERO);
				return;
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1,  target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ONE:
				_solver->CommitClause( target.l1,			  out.l0);
				_solver->CommitClause(			-target.l0,  out.l0);
				_solver->CommitClause(-target.l1,  target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ZERO:
				_solver->CommitClause(-target.l1,			  out.l0);
				_solver->CommitClause(			 target.l0,  out.l0);
				_solver->CommitClause( target.l1, -target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1,			  out.l0);
				_solver->CommitClause(			-target.l0,  out.l0);
				_solver->CommitClause( target.l1,  target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_01:
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1,  target.l0, -out.l0);
				_solver->CommitClause(-target.l1, -target.l0, -out.l0);
				break;
			case LogicConstraint::NO_CONSTRAINT:
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01X:
				EncodeLogicValue(out, Logic::LOGIC_ONE);
				break;
			case LogicConstraint::ONLY_LOGIC_UNKNOWN:
				EncodeLogicValue(out, Logic::LOGIC_ZERO);
				return;
			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		switch (constraint)
		{
			case LogicConstraint::ONLY_LOGIC_ONE:
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_ZERO:
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_UNKNOWN:
				_solver->CommitClause( target.l1,  target.l0,  out.l0);
				_solver->CommitClause(-target.l1,			 -out.l0);
				_solver->CommitClause(			-target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_DONT_CARE:
				_solver->CommitClause(-target.l1, -target.l0,  out.l0);
				_solver->CommitClause( target.l1,			 -out.l0);
				_solver->CommitClause(			 target.l0, -out.l0);
				break;

			case LogicConstraint::NOT_LOGIC_ONE:
				_solver->CommitClause( target.l1,			  out.l0);
				_solver->CommitClause(			-target.l0,  out.l0);
				_solver->CommitClause(-target.l1,  target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_ZERO:
				_solver->CommitClause(-target.l1,			  out.l0);
				_solver->CommitClause(			 target.l0,  out.l0);
				_solver->CommitClause( target.l1, -target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_UNKNOWN:
			case LogicConstraint::ONLY_LOGIC_01X:
				_solver->CommitClause(-target.l1,			  out.l0);
				_solver->CommitClause(			-target.l0,  out.l0);
				_solver->CommitClause( target.l1,  target.l0, -out.l0);
				break;
			case LogicConstraint::NOT_LOGIC_DONT_CARE:
				_solver->CommitClause( target.l1,			  out.l0);
				_solver->CommitClause(			 target.l0,  out.l0);
				_solver->CommitClause(-target.l1, -target.l0, -out.l0);
				break;
			case LogicConstraint::ONLY_LOGIC_01:
				_solver->CommitClause(-target.l1,  target.l0,  out.l0);
				_solver->CommitClause( target.l1, -target.l0,  out.l0);
				_solver->CommitClause( target.l1,  target.l0, -out.l0);
				_solver->CommitClause(-target.l1, -target.l0, -out.l0);
				break;

			case LogicConstraint::NO_CONSTRAINT:
				EncodeLogicValue(out, Logic::LOGIC_ONE);
				return;

			default:
				INVALID_CONSTRAINT(constraint);
		}
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValue(const std::vector<LogicContainer>& target, const std::vector<Logic>& value)
{
	ASSERT(target.size() == value.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < target.size(); ++index)
		EncodeLogicValue(target[index], value[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueConstraint(const std::vector<LogicContainer>& target, const std::vector<LogicConstraint>& constraint)
{
	ASSERT(target.size() == constraint.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < target.size(); ++index)
		EncodeLogicValueConstraint(target[index], constraint[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicEquivalenceConstraint(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeLogicEquivalenceConstraint(in1[index], in2[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogic01DifferenceConstraint(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeLogic01DifferenceConstraint(in1[index], in2[index]);
}

template<typename LogicContainer>
std::vector<LogicContainer01> LogicEncoder::EncodeLogicValueDetector(const std::vector<LogicContainer>& target, std::vector<Logic>& value)
{
	ASSERT(target.size() == value.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer01> result;
	result.reserve(target.size());

	for (size_t index = 0u; index < target.size(); ++index)
		result.emplace_back(EncodeLogicValueDetector(target[index], value[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer01> LogicEncoder::EncodeLogicValueConstraintDetector(const std::vector<LogicContainer>& target, std::vector<LogicConstraint>& constraint)
{
	ASSERT(target.size() == constraint.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer01> result;
	result.reserve(target.size());

	for (size_t index = 0u; index < target.size(); ++index)
		result.emplace_back(EncodeLogicValueConstraintDetector(target[index], constraint[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer01> LogicEncoder::EncodeLogicEquivalenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer01> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeLogicEquivalenceDetector(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer01> LogicEncoder::EncodeLogic01DifferenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer01> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeLogic01DifferenceDetector(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueDetector(const std::vector<LogicContainer>& target, const std::vector<Logic>& value, const std::vector<LogicContainer01>& out)
{
	ASSERT(target.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(target.size() == value.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < target.size(); ++index)
		EncodeLogicValueDetector(target[index], value[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicValueConstraintDetector(const std::vector<LogicContainer>& target, const std::vector<LogicConstraint>& constraint, const std::vector<LogicContainer01>& out)
{
	ASSERT(target.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(target.size() == constraint.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < target.size(); ++index)
		EncodeLogicValueConstraintDetector(target[index], constraint[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogicEquivalenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer01>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeLogicEquivalenceDetector(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeLogic01DifferenceDetector(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer01>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeLogic01DifferenceDetector(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeBuffer(const LogicContainer& in)
{
	ASSERT_SET(in);

	return in;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeInverter(const LogicContainer& in)
{
	ASSERT_SET(in);

	return -in;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeXor(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	if (const1 == Logic::LOGIC_ZERO)
	{
		return in2;
	}
	if (const1 == Logic::LOGIC_ONE)
	{
		return -in2;
	}

	if (const2 == Logic::LOGIC_ZERO)
	{
		return in1;
	}
	if (const2 == Logic::LOGIC_ONE)
	{
		return -in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}
	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeXor(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeAnd(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return in1;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	// Controlling value
	if (const1 == Logic::LOGIC_ZERO || const2 == Logic::LOGIC_ZERO)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
	}

	if (const1 == Logic::LOGIC_ONE)
	{
		return in2;
	}
	if (const2 == Logic::LOGIC_ONE)
	{
		return in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}

	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeAnd(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeOr(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return in1;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	// Controlling value
	if (const1 == Logic::LOGIC_ONE || const2 == Logic::LOGIC_ONE)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
	}

	if (const1 == Logic::LOGIC_ZERO)
	{
		return in2;
	}
	if (const2 == Logic::LOGIC_ZERO)
	{
		return in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}

	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeOr(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeXnor(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	if (const1 == Logic::LOGIC_ZERO)
	{
		return -in2;
	}
	if (const1 == Logic::LOGIC_ONE)
	{
		return in2;
	}

	if (const2 == Logic::LOGIC_ZERO)
	{
		return -in1;
	}
	if (const2 == Logic::LOGIC_ONE)
	{
		return in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}

	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeXnor(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeNand(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return -in1;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	// Controlling value
	if (const1 == Logic::LOGIC_ZERO || const2 == Logic::LOGIC_ZERO)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
	}

	if (const1 == Logic::LOGIC_ONE)
	{
		return -in2;
	}
	if (const2 == Logic::LOGIC_ONE)
	{
		return -in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}

	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeNand(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeNor(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return -in1;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		if (in1 == -in2)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
	}

	const Logic const1 = GetConstantLogicValue<LogicContainer>(in1);
	const Logic const2 = GetConstantLogicValue<LogicContainer>(in2);

	// Controlling value
	if (const1 == Logic::LOGIC_ONE || const2 == Logic::LOGIC_ONE)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
	}

	if (const1 == Logic::LOGIC_ZERO)
	{
		return -in2;
	}
	if (const2 == Logic::LOGIC_ZERO)
	{
		return -in1;
	}

	if ((const1 == Logic::LOGIC_UNKNOWN && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_UNKNOWN && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
	}

	if ((const1 == Logic::LOGIC_DONT_CARE && const2 != Logic::LOGIC_INVALID)
		|| (const2 == Logic::LOGIC_DONT_CARE && const1 != Logic::LOGIC_INVALID))
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeNor(in1, in2, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeTristateBuffer(const LogicContainer& in, const LogicContainer& en)
{
	ASSERT_SET(in);
	ASSERT_SET(en);

	const Logic constEn = GetConstantLogicValue<LogicContainer>(en);
	if (constEn == Logic::LOGIC_ONE)
	{
		return in;
	}
	if (constEn == Logic::LOGIC_ZERO)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constEn == Logic::LOGIC_DONT_CARE)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found DON'T CARE value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constEn == Logic::LOGIC_UNKNOWN)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01X encoding";
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeTristateBuffer(in, en, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeTristateInverter(const LogicContainer& in, const LogicContainer& en)
{
	ASSERT_SET(in);
	ASSERT_SET(en);

	const Logic constEn = GetConstantLogicValue<LogicContainer>(en);
	if (constEn == Logic::LOGIC_ONE)
	{
		return -in;
	}
	if (constEn == Logic::LOGIC_ZERO)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constEn == Logic::LOGIC_DONT_CARE)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found DON'T CARE value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constEn == Logic::LOGIC_UNKNOWN)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01X encoding";
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeTristateInverter(in, en, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeMultiplexer(const LogicContainer& in1, const LogicContainer& in2, const LogicContainer& sel)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);
	ASSERT_SET(sel);

	const Logic constSel = GetConstantLogicValue<LogicContainer>(sel);

	if (constSel == Logic::LOGIC_ZERO)
	{
		return in1;
	}
	if (constSel == Logic::LOGIC_ONE)
	{
		return in2;
	}

	if (constSel == Logic::LOGIC_DONT_CARE)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found DON'T CARE value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
			|| is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constSel == Logic::LOGIC_UNKNOWN)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01X encoding";
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeMultiplexer(in1, in2, sel, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeSetControl(const LogicContainer& in, const LogicContainer& set)
{
	ASSERT_SET(in);
	ASSERT_SET(set);

	const Logic constSet = GetConstantLogicValue<LogicContainer>(set);

	if (constSet == Logic::LOGIC_ZERO)
	{
		return in;
	}
	if (constSet == Logic::LOGIC_ONE)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
	}
	if (constSet == Logic::LOGIC_DONT_CARE)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found DON'T CARE value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
			|| is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constSet == Logic::LOGIC_UNKNOWN)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01X encoding";
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeSetControl(in, set, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeResetControl(const LogicContainer& in, const LogicContainer& reset)
{
	ASSERT_SET(in);
	ASSERT_SET(reset);

	const Logic constReset = GetConstantLogicValue<LogicContainer>(reset);

	if (constReset == Logic::LOGIC_ZERO)
	{
		return in;
	}
	if (constReset == Logic::LOGIC_ONE)
	{
		return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
	}
	if (constReset == Logic::LOGIC_DONT_CARE)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found DON'T CARE value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
			|| is_U01X_encoding_v<LogicContainer>|| is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (constReset == Logic::LOGIC_UNKNOWN)
	{
		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01 encoding";
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
		{
			LOG(FATAL) << "Found UNKNOWN value in 01X encoding";
		}
		else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeResetControl(in, reset, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeXor(const std::vector<LogicContainer>& in)
{
	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeXor(in, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeAnd(const std::vector<LogicContainer>& in)
{
	for (const LogicContainer& container : in)
	{
		if (GetConstantLogicValue<LogicContainer>(container) == Logic::LOGIC_ZERO)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeAnd(in, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeOr(const std::vector<LogicContainer>& in)
{
	for (const LogicContainer& container : in)
	{
		if (GetConstantLogicValue<LogicContainer>(container) == Logic::LOGIC_ONE)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
	}

	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		return EncodeBuffer(in[0]);
	}
	else if (elements == 2)
	{
		return EncodeOr(in[0], in[1]);
	}
	else if (elements == 3)
	{
		return EncodeOr(EncodeOr(in[0], in[1]), in[2]);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());
		return EncodeOr(EncodeOr(left), EncodeOr(right));
	}
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeXnor(const std::vector<LogicContainer>& in)
{
	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeXnor(in, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeNand(const std::vector<LogicContainer>& in)
{
	for (const LogicContainer& container : in)
	{
		if (GetConstantLogicValue<LogicContainer>(container) == Logic::LOGIC_ZERO)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeNand(in, out);
	return out;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::EncodeNor(const std::vector<LogicContainer>& in)
{
	for (const LogicContainer& container : in)
	{
		if (GetConstantLogicValue<LogicContainer>(container) == Logic::LOGIC_ONE)
		{
			return NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		}
	}

	LogicContainer out = NewLogicContainer<LogicContainer>();
	EncodeNor(in, out);
	return out;
}

template<typename LogicContainer>
void LogicEncoder::EncodeXor(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeBuffer(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeXor(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeXor(in[0], in[1], temp);
		EncodeXor(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeXor(left, tempLeft);
		EncodeXor(right, tempRight);
		EncodeXor(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeAnd(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeBuffer(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeAnd(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeAnd(in[0], in[1], temp);
		EncodeAnd(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeAnd(left, tempLeft);
		EncodeAnd(right, tempRight);
		EncodeAnd(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeOr(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeBuffer(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeOr(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeOr(in[0], in[1], temp);
		EncodeOr(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeOr(left, tempLeft);
		EncodeOr(right, tempRight);
		EncodeOr(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeXnor(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeInverter(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeXnor(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeXor(in[0], in[1], temp);
		EncodeXnor(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeXor(left, tempLeft);
		EncodeXor(right, tempRight);
		EncodeXnor(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeNand(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeInverter(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeNand(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeAnd(in[0], in[1], temp);
		EncodeNand(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeAnd(left, tempLeft);
		EncodeAnd(right, tempRight);
		EncodeNand(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeNor(const std::vector<LogicContainer>& in, const LogicContainer& out)
{
	const size_t elements = in.size();
	ASSERT(elements != 0);

	if (elements == 1)
	{
		EncodeInverter(in[0], out);
	}
	else if (elements == 2)
	{
		EncodeNor(in[0], in[1], out);
	}
	else if (elements == 3)
	{
		LogicContainer temp = NewLogicContainer<LogicContainer>();
		EncodeOr(in[0], in[1], temp);
		EncodeNor(temp, in[2], out);
	}
	else
	{
		size_t middle = elements / 2;
		const std::vector<LogicContainer> left(in.begin(), in.begin() + middle);
		const std::vector<LogicContainer> right(in.begin() + middle, in.end());

		LogicContainer tempLeft = NewLogicContainer<LogicContainer>();
		LogicContainer tempRight = NewLogicContainer<LogicContainer>();
		EncodeOr(left, tempLeft);
		EncodeOr(right, tempRight);
		EncodeNor(tempLeft, tempRight, out);
	}
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeBuffer(const std::vector<LogicContainer>& in)
{
	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeBuffer(in[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeInverter(const std::vector<LogicContainer>& in)
{
	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeInverter(in[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeXor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeXor(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeAnd(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeAnd(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeOr(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeOr(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeXnor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeXnor(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeNand(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeNand(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeNor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeNor(in1[index], in2[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeTristateBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en)
{
	ASSERT(in.size() == en.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeTristateBuffer(in[index], en[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeTristateInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en)
{
	ASSERT(in.size() == en.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeTristateInverter(in[index], en[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeMultiplexer(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& sel)
{
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";
	ASSERT(in1.size() == sel.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in1.size());

	for (size_t index = 0u; index < in1.size(); ++index)
		result.emplace_back(EncodeMultiplexer(in1[index], in2[index], sel[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeSetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& set)
{
	ASSERT(in.size() == set.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeSetControl(in[index], set[index]));

	return result;
}

template<typename LogicContainer>
std::vector<LogicContainer> LogicEncoder::EncodeResetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& reset)
{
	ASSERT(in.size() == reset.size()) << "The size of the input vectors has to match.";

	std::vector<LogicContainer> result;
	result.reserve(in.size());

	for (size_t index = 0u; index < in.size(); ++index)
		result.emplace_back(EncodeResetControl(in[index], reset[index]));

	return result;
}

template<typename LogicContainer>
void LogicEncoder::EncodeBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeBuffer(in[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeInverter(in[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeXor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeXor(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeAnd(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeAnd(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeOr(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeOr(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeXnor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeXnor(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeNand(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeNand(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeNor(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeNor(in1[index], in2[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeTristateBuffer(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in.size() == en.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeTristateBuffer(in[index], en[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeTristateInverter(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& en, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in.size() == en.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeTristateInverter(in[index], en[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeMultiplexer(const std::vector<LogicContainer>& in1, const std::vector<LogicContainer>& in2, const std::vector<LogicContainer>& sel, const std::vector<LogicContainer>& out)
{
	ASSERT(in1.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in1.size() == in2.size()) << "The size of the input vectors has to match.";
	ASSERT(in1.size() == sel.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in1.size(); ++index)
		EncodeMultiplexer(in1[index], in2[index], sel[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeSetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& set, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in.size() == set.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeSetControl(in[index], set[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeResetControl(const std::vector<LogicContainer>& in, const std::vector<LogicContainer>& reset, const std::vector<LogicContainer>& out)
{
	ASSERT(in.size() == out.size()) << "The size of the input and output vectors has to match.";
	ASSERT(in.size() == reset.size()) << "The size of the input vectors has to match.";

	for (size_t index = 0u; index < in.size(); ++index)
		EncodeResetControl(in[index], reset[index], out[index]);
}

template<typename LogicContainer>
void LogicEncoder::EncodeEquality(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if (in1 == in2)
	{
		return;
	}

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		_solver->CommitClause(-in1.l0,  in2.l0);
		_solver->CommitClause( in1.l0, -in2.l0);
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause(-in1.l1,  in2.l1);
		_solver->CommitClause( in1.l1, -in2.l1);
		_solver->CommitClause(-in1.l0,  in2.l0);
		_solver->CommitClause( in1.l0, -in2.l0);
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause(-in1.l1,  in2.l1);
		_solver->CommitClause( in1.l1, -in2.l1);
		_solver->CommitClause(-in1.l0,  in2.l0);
		_solver->CommitClause( in1.l0, -in2.l0);
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeInequality(const LogicContainer& in1, const LogicContainer& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	if constexpr (is_01_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l0,  in2.l0); // Disallow in1 0, in2 0 values
		_solver->CommitClause(-in1.l0, -in2.l0); // Disallow in1 1, in2 1 values
	}
	else if constexpr (is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1,  in1.l0,  in2.l1,  in2.l0); // Disallow in1 X, in2 X values
		_solver->CommitClause( in1.l1, -in1.l0,  in2.l1, -in2.l0); // Disallow in1 0, in2 0 values
		_solver->CommitClause(-in1.l1,  in1.l0, -in2.l1,  in2.l0); // Disallow in1 1, in2 1 values
	}
	else if constexpr (is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		_solver->CommitClause( in1.l1,  in1.l0,  in2.l1,  in2.l0); // Disallow in1 U, in2 U values
		_solver->CommitClause( in1.l1, -in1.l0,  in2.l1, -in2.l0); // Disallow in1 0, in2 0 values
		_solver->CommitClause(-in1.l1,  in1.l0, -in2.l1,  in2.l0); // Disallow in1 1, in2 1 values
		_solver->CommitClause(-in1.l1, -in1.l0, -in2.l1, -in2.l0); // Disallow in1 X, in2 X values
	}
	else
	{
		NOT_IMPLEMENTED;
	}
}

template<typename LogicContainer>
LogicContainer LogicEncoder::NewInputContainer()
{
	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Input);
	}

	auto container { NewLogicContainer<LogicContainer>() };

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	return container;
}

template<typename LogicContainer>
LogicContainer LogicEncoder::NewStateContainer()
{
	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Latch);
	}

	auto container { NewLogicContainer<LogicContainer>() };

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	return container;
}

template<typename LogicContainer>
void LogicEncoder::EncodeNextState(const LogicContainer& out, const LogicContainer& next)
{
	ASSERT_SET(out);
	ASSERT_SET(next);

	if (auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(_solver); satSolver)
	{
		if constexpr (is_01_encoding_v<LogicContainer>
			|| is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
			|| is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
		{
			EncodeBuffer(next, out);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);

		if constexpr (is_01_encoding_v<LogicContainer>)
		{
			bmcSolver->CommitTimeframeClause(-next.l0, 0u,  out.l0, 1u);
			bmcSolver->CommitTimeframeClause( next.l0, 0u, -out.l0, 1u);
		}
		else if constexpr (is_01X_encoding_v<LogicContainer> || is_U01X_encoding_v<LogicContainer>)
		{
			bmcSolver->CommitTimeframeClause(-next.l0, 0u,  out.l0, 1u);
			bmcSolver->CommitTimeframeClause( next.l0, 0u, -out.l0, 1u);
			bmcSolver->CommitTimeframeClause(-next.l1, 0u,  out.l1, 1u);
			bmcSolver->CommitTimeframeClause( next.l1, 0u, -out.l1, 1u);
		}
		else if constexpr (is_01F_encoding_v<LogicContainer>)
		{
			bmcSolver->CommitTimeframeClause(next.l0, 0u, -out.l0, 1u);
			bmcSolver->CommitTimeframeClause(next.l1, 0u, -out.l1, 1u);
		}
		else if constexpr (is_U01F_encoding_v<LogicContainer>)
		{
			bmcSolver->CommitTimeframeClause(-next.l0, 0u,               out.l0, 1u);
			bmcSolver->CommitTimeframeClause(-next.l1, 0u,               out.l1, 1u);
			bmcSolver->CommitTimeframeClause( next.l0, 0u, next.l1, 0u, -out.l0, 1u);
			bmcSolver->CommitTimeframeClause( next.l0, 0u, next.l1, 0u, -out.l1, 1u);
		}
		else
		{
			NOT_IMPLEMENTED;
		}
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeInitialStateValue(const LogicContainer& out, Basic::Logic value)
{
	ASSERT_SET(out);

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Initial);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	if constexpr (is_01_encoding_v<LogicContainer>
		|| is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
		|| is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		EncodeLogicValue(out, value);
	}
	else
	{
		NOT_IMPLEMENTED;
	}

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
	}
}

template<typename LogicContainer>
void LogicEncoder::EncodeTargetStateValue(const LogicContainer& out, Basic::Logic value)
{
	ASSERT_SET(out);

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Target);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	if constexpr (is_01_encoding_v<LogicContainer>
		|| is_01X_encoding_v<LogicContainer> || is_01F_encoding_v<LogicContainer>
		|| is_U01X_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
	{
		EncodeLogicValue(out, value);
	}
	else
	{
		NOT_IMPLEMENTED;
	}

	if (auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver); bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
	}
}

LogicContainer01 LogicEncoder::EncodeImplication(const LogicContainer01& in)
{
	LogicContainer01 container = NewLogicContainer<LogicContainer01>();
	EncodeImplication(container, in);
	return container;
}

LogicContainer01 LogicEncoder::EncodeImplication(const std::vector<LogicContainer01>& in)
{
	LogicContainer01 container = NewLogicContainer<LogicContainer01>();
	EncodeImplication(container, in);
	return container;
}

void LogicEncoder::EncodeImplication(const LogicContainer01& in1, const LogicContainer01& in2)
{
	ASSERT_SET(in1);
	ASSERT_SET(in2);

	// (in1.l0 -> in2.l0) <=> (-in1.l0 v in2.l0)
	_solver->CommitClause(-in1.l0, in2.l0);
}

void LogicEncoder::EncodeImplication(const LogicContainer01& in1, const std::vector<LogicContainer01>& in)
{
	ASSERT_SET(in1);
	for (const auto& literal : in)
	{
		ASSERT_SET(literal);
	}

	_solver->NewClause();
	_solver->AddLiteral(-in1.l0);
	for (const auto& literal : in)
	{
		_solver->AddLiteral(literal.l0);
	}
	_solver->CommitClause();
}

void LogicEncoder::EncodeClause(const std::vector<LogicContainer01>& in)
{
	for (const auto& literal : in)
	{
		ASSERT_SET(literal);
	}

	_solver->NewClause();
	for (const auto& literal : in)
	{
		_solver->AddLiteral(literal.l0);
	}
	_solver->CommitClause();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winstantiation-after-specialization"

#define FOR_CONTAINER(CONTAINER) \
	template CONTAINER LogicEncoder::NewLogicContainer<CONTAINER>(void); \
	template CONTAINER LogicEncoder::NewLogicContainer<CONTAINER>(Logic value) const; \
	template CONTAINER LogicEncoder::ConvertLogicContainer<CONTAINER, LogicContainer01>(const LogicContainer01& source); \
	template CONTAINER LogicEncoder::ConvertLogicContainer<CONTAINER, LogicContainer01X>(const LogicContainer01X& source); \
	template CONTAINER LogicEncoder::ConvertLogicContainer<CONTAINER, LogicContainer01F>(const LogicContainer01F& source); \
	template CONTAINER LogicEncoder::ConvertLogicContainer<CONTAINER, LogicContainerU01X>(const LogicContainerU01X& source); \
	template CONTAINER LogicEncoder::ConvertLogicContainer<CONTAINER, LogicContainerU01F>(const LogicContainerU01F& source); \
	template Logic LogicEncoder::GetSolvedLogicValue<CONTAINER>(const CONTAINER& target) const; \
	\
	template bool LogicEncoder::IsConstantLogicValue<CONTAINER>(const CONTAINER& target) const; \
	template Logic LogicEncoder::GetConstantLogicValue<CONTAINER>(const CONTAINER& target) const; \
	\
	template void LogicEncoder::EncodeLogicValue<CONTAINER>(const CONTAINER& target, Logic value); \
	template void LogicEncoder::EncodeLogicValueConstraint<CONTAINER>(const CONTAINER& target, LogicConstraint constraint); \
	template void LogicEncoder::EncodeLogicEquivalenceConstraint<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template void LogicEncoder::EncodeLogic01DifferenceConstraint<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template LogicContainer01 LogicEncoder::EncodeLogicValueDetector<CONTAINER>(const CONTAINER& target, Logic value); \
	template LogicContainer01 LogicEncoder::EncodeLogicValueConstraintDetector<CONTAINER>(const CONTAINER& target, LogicConstraint constraint); \
	template LogicContainer01 LogicEncoder::EncodeLogicEquivalenceDetector<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template LogicContainer01 LogicEncoder::EncodeLogic01DifferenceDetector<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	\
	template void LogicEncoder::EncodeLogicValue<CONTAINER>(const std::vector<CONTAINER>& target, const std::vector<Logic>& value); \
	template void LogicEncoder::EncodeLogicValueConstraint<CONTAINER>(const std::vector<CONTAINER>& target, const std::vector<LogicConstraint>& constraint); \
	template void LogicEncoder::EncodeLogicEquivalenceConstraint<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template void LogicEncoder::EncodeLogic01DifferenceConstraint<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<LogicContainer01> LogicEncoder::EncodeLogicValueDetector<CONTAINER>(const std::vector<CONTAINER>& target, std::vector<Logic>& value); \
	template std::vector<LogicContainer01> LogicEncoder::EncodeLogicValueConstraintDetector<CONTAINER>(const std::vector<CONTAINER>& target, std::vector<LogicConstraint>& constraint); \
	template std::vector<LogicContainer01> LogicEncoder::EncodeLogicEquivalenceDetector<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<LogicContainer01> LogicEncoder::EncodeLogic01DifferenceDetector<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	\
	template void LogicEncoder::EncodeLogicValueDetector<CONTAINER>(const CONTAINER& target, Logic value, const LogicContainer01& out); \
	template void LogicEncoder::EncodeLogicValueConstraintDetector<CONTAINER>(const CONTAINER& target, LogicConstraint constraint, const LogicContainer01& out); \
	template void LogicEncoder::EncodeLogicEquivalenceDetector<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const LogicContainer01& out); \
	template void LogicEncoder::EncodeLogic01DifferenceDetector<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const LogicContainer01& out); \
	\
	template void LogicEncoder::EncodeLogicValueDetector<CONTAINER>(const std::vector<CONTAINER>& target, const std::vector<Logic>& value, const std::vector<LogicContainer01>& out); \
	template void LogicEncoder::EncodeLogicValueConstraintDetector<CONTAINER>(const std::vector<CONTAINER>& target, const std::vector<LogicConstraint>& constraint, const std::vector<LogicContainer01>& out); \
	template void LogicEncoder::EncodeLogicEquivalenceDetector<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<LogicContainer01>& out); \
	template void LogicEncoder::EncodeLogic01DifferenceDetector<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<LogicContainer01>& out); \
	\
	template CONTAINER LogicEncoder::EncodeBuffer<CONTAINER>(const CONTAINER& in); \
	template CONTAINER LogicEncoder::EncodeInverter<CONTAINER>(const CONTAINER& in); \
	template CONTAINER LogicEncoder::EncodeXor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeAnd<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeOr<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeXnor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeNand<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeNor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::EncodeTristateBuffer<CONTAINER>(const CONTAINER& in, const CONTAINER& en); \
	template CONTAINER LogicEncoder::EncodeTristateInverter<CONTAINER>(const CONTAINER& in, const CONTAINER& en); \
	template CONTAINER LogicEncoder::EncodeMultiplexer<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& sel); \
	template CONTAINER LogicEncoder::EncodeSetControl<CONTAINER>(const CONTAINER& in, const CONTAINER& set); \
	template CONTAINER LogicEncoder::EncodeResetControl<CONTAINER>(const CONTAINER& in, const CONTAINER& reset); \
	\
	template CONTAINER LogicEncoder::EncodeXor<CONTAINER>(const std::vector<CONTAINER>& in); \
	template CONTAINER LogicEncoder::EncodeAnd<CONTAINER>(const std::vector<CONTAINER>& in); \
	template CONTAINER LogicEncoder::EncodeOr<CONTAINER>(const std::vector<CONTAINER>& in); \
	template CONTAINER LogicEncoder::EncodeXnor<CONTAINER>(const std::vector<CONTAINER>& in); \
	template CONTAINER LogicEncoder::EncodeNand<CONTAINER>(const std::vector<CONTAINER>& in); \
	template CONTAINER LogicEncoder::EncodeNor<CONTAINER>(const std::vector<CONTAINER>& in); \
	\
	template std::vector<CONTAINER> LogicEncoder::EncodeBuffer<CONTAINER>(const std::vector<CONTAINER>& in); \
	template std::vector<CONTAINER> LogicEncoder::EncodeInverter<CONTAINER>(const std::vector<CONTAINER>& in); \
	template std::vector<CONTAINER> LogicEncoder::EncodeXor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeAnd<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeOr<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeXnor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeNand<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeNor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2); \
	template std::vector<CONTAINER> LogicEncoder::EncodeTristateBuffer<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& en); \
	template std::vector<CONTAINER> LogicEncoder::EncodeTristateInverter<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& en); \
	template std::vector<CONTAINER> LogicEncoder::EncodeMultiplexer<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& sel); \
	template std::vector<CONTAINER> LogicEncoder::EncodeSetControl<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& set); \
	template std::vector<CONTAINER> LogicEncoder::EncodeResetControl<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& reset); \
	\
	template void LogicEncoder::EncodeBuffer<CONTAINER>(const CONTAINER& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeInverter<CONTAINER>(const CONTAINER& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeXor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeAnd<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeOr<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeXnor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeNand<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeNor<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& out); \
	template void LogicEncoder::EncodeTristateBuffer<CONTAINER>(const CONTAINER& in, const CONTAINER& en, const CONTAINER& out); \
	template void LogicEncoder::EncodeTristateInverter<CONTAINER>(const CONTAINER& in, const CONTAINER& en, const CONTAINER& out); \
	template void LogicEncoder::EncodeMultiplexer<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2, const CONTAINER& sel, const CONTAINER& out); \
	template void LogicEncoder::EncodeSetControl<CONTAINER>(const CONTAINER& in, const CONTAINER& set, const CONTAINER& out); \
	template void LogicEncoder::EncodeResetControl<CONTAINER>(const CONTAINER& in, const CONTAINER& reset, const CONTAINER& out); \
	\
	template void LogicEncoder::EncodeXor<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeAnd<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeOr<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeXnor<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeNand<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	template void LogicEncoder::EncodeNor<CONTAINER>(const std::vector<CONTAINER>& in, const CONTAINER& out); \
	\
	template void LogicEncoder::EncodeBuffer<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeInverter<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeXor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeAnd<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeOr<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeXnor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeNand<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeNor<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeTristateBuffer<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& en, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeTristateInverter<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& en, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeMultiplexer<CONTAINER>(const std::vector<CONTAINER>& in1, const std::vector<CONTAINER>& in2, const std::vector<CONTAINER>& sel, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeSetControl<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& set, const std::vector<CONTAINER>& out); \
	template void LogicEncoder::EncodeResetControl<CONTAINER>(const std::vector<CONTAINER>& in, const std::vector<CONTAINER>& reset, const std::vector<CONTAINER>& out); \
	\
	template void LogicEncoder::EncodeEquality<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template void LogicEncoder::EncodeInequality<CONTAINER>(const CONTAINER& in1, const CONTAINER& in2); \
	template CONTAINER LogicEncoder::NewInputContainer<CONTAINER>(void); \
	template CONTAINER LogicEncoder::NewStateContainer<CONTAINER>(void); \
	template void LogicEncoder::EncodeNextState(const CONTAINER& out, const CONTAINER& next); \
	template void LogicEncoder::EncodeInitialStateValue(const CONTAINER& out, const Basic::Logic value); \
	template void LogicEncoder::EncodeTargetStateValue(const CONTAINER& out, const Basic::Logic value);
INSTANTIATE_ALL_CONTAINER

template LogicContainer01 LogicEncoder::NewLogicContainer<LogicContainer01, Literal0>(BaseLiteral literal) const;
template LogicContainer01X LogicEncoder::NewLogicContainer<LogicContainer01X, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const;
template LogicContainer01F LogicEncoder::NewLogicContainer<LogicContainer01F, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const;
template LogicContainerU01X LogicEncoder::NewLogicContainer<LogicContainerU01X, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const;
template LogicContainerU01F LogicEncoder::NewLogicContainer<LogicContainerU01F, Literal1, Literal0>(BaseLiteral literal1, BaseLiteral literal0) const;

#pragma clang diagnostic pop

};
};
