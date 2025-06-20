#include "Basic/Logic.hpp"

#include <string>
#include <iostream>

#include "Basic/Logging.hpp"

namespace FreiTest
{
namespace Basic
{

std::ostream& operator<<(std::ostream& os, const Logic& logicValue)
{
	os << to_string(logicValue);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Logic>& logicValueList)
{
	os << to_string(logicValueList);
	return os;
}

std::ostream& operator<<(std::ostream& os, const LogicConstraint& constraint)
{
	os << to_string(constraint);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<LogicConstraint>& logicConstraintList)
{
	os << to_string(logicConstraintList);
	return os;
}

std::ostream& operator<<(std::ostream& os, const StabilityConstraint& constraint)
{
	os << to_string(constraint);
	return os;
}

std::ostream& operator<<(std::ostream& os, const LogicType& logicType)
{
	os << to_string(logicType);
	return os;
}

std::string to_string(const Logic& logicValue)
{
	return std::string(1u, static_cast<char>(logicValue));
}

std::string to_string(const std::vector<Logic>& logicValueList)
{
	std::string returnValue;
	for (const auto& logicValue : logicValueList)
	{
		returnValue += to_string(logicValue);
	}
	return returnValue;
}

std::string to_string(const LogicConstraint& constraint)
{
	return std::string(1u, static_cast<char>(constraint));
}

std::string to_string(const std::vector<LogicConstraint>& constraintList)
{
	std::string returnValue;
	for (const auto& constraint : constraintList)
	{
		returnValue += to_string(constraint);
	}
	return returnValue;
}

std::string to_string(const StabilityConstraint& constraint)
{
	return std::string(1u, static_cast<char>(constraint));
}

std::string to_string(const LogicType& logicType)
{
	switch (logicType)
	{
		case LogicType::LOGIC_UNSPECIFIED:
			return "LOGIC_UNSPECIFIED";
		case LogicType::LOGIC_01:
			return "LOGIC_01";
		case LogicType::LOGIC_01X:
			return "LOGIC_01X";
		case LogicType::LOGIC_01XU:
			return "LOGIC_01XU";
		default:
			Logging::Panic();
	}
}

char to_char(const Logic& logicValue)
{
	return static_cast<char>(logicValue);
}

char to_char(const LogicConstraint& constraint)
{
	return static_cast<char>(constraint);
}

char to_char(const StabilityConstraint& constraint)
{
	return static_cast<char>(constraint);
}

Logic GetLogicForCharacter(const char& value)
{
	switch (value)
	{
		case 'U': return Logic::LOGIC_UNKNOWN;
		case '0': return Logic::LOGIC_ZERO;
		case '1': return Logic::LOGIC_ONE;
		case 'X': return Logic::LOGIC_DONT_CARE;
		case '-': return Logic::LOGIC_INVALID;
		default: Logging::Panic();
	}
}

LogicConstraint GetLogicConstraintForCharacter(const char& value)
{
	switch (value)
	{
		case '*': return LogicConstraint::NO_CONSTRAINT;
		case '0': return LogicConstraint::ONLY_LOGIC_ZERO;
		case '=': return LogicConstraint::NOT_LOGIC_ZERO;
		case '1': return LogicConstraint::ONLY_LOGIC_ONE;
		case '!': return LogicConstraint::NOT_LOGIC_ONE;
		case 'X': return LogicConstraint::ONLY_LOGIC_DONT_CARE;
		case 'K': return LogicConstraint::NOT_LOGIC_DONT_CARE;
		case 'U': return LogicConstraint::ONLY_LOGIC_UNKNOWN;
		case 'L': return LogicConstraint::NOT_LOGIC_UNKNOWN;
		case 'N': return LogicConstraint::ONLY_LOGIC_01;
		case 'V': return LogicConstraint::ONLY_LOGIC_01X;
		default: Logging::Panic();
	}
}

StabilityConstraint GetStabilityConstraintForCharacter(const char& value)
{
	switch (value)
	{
		case '*': return StabilityConstraint::NO_CONSTRAINT;
		case '?': return StabilityConstraint::SIGNAL_STABLE;
		case 'R': return StabilityConstraint::SIGNAL_RISING;
		case 'F': return StabilityConstraint::SIGNAL_FALLING;
		case 'T': return StabilityConstraint::SIGNAL_RISEFALL;
		default: Logging::Panic();
	}
}

Logic GetLogicForString(const std::string& value)
{
	if (value == "U") return Logic::LOGIC_UNKNOWN;
	if (value == "0") return Logic::LOGIC_ZERO;
	if (value == "1") return Logic::LOGIC_ONE;
	if (value == "X") return Logic::LOGIC_DONT_CARE;
	return Logic::LOGIC_INVALID;
}

Logic GetLogicForConstraint(const LogicConstraint& constraint)
{
	switch (constraint)
	{
		case LogicConstraint::ONLY_LOGIC_ZERO:
			return Logic::LOGIC_ZERO;
		case LogicConstraint::ONLY_LOGIC_ONE:
			return Logic::LOGIC_ONE;
		case LogicConstraint::ONLY_LOGIC_DONT_CARE:
			return Logic::LOGIC_DONT_CARE;
		case LogicConstraint::ONLY_LOGIC_UNKNOWN:
			return Logic::LOGIC_UNKNOWN;
		default:
			return Logic::LOGIC_INVALID;
	}
}

LogicConstraint GetLogicConstraintForString(const std::string& value)
{
	if (value == "*") return LogicConstraint::NO_CONSTRAINT;
	if (value == "0") return LogicConstraint::ONLY_LOGIC_ZERO;
	if (value == "=") return LogicConstraint::NOT_LOGIC_ZERO;
	if (value == "1") return LogicConstraint::ONLY_LOGIC_ONE;
	if (value == "!") return LogicConstraint::NOT_LOGIC_ONE;
	if (value == "X") return LogicConstraint::ONLY_LOGIC_DONT_CARE;
	if (value == "K") return LogicConstraint::NOT_LOGIC_DONT_CARE;
	if (value == "U") return LogicConstraint::ONLY_LOGIC_UNKNOWN;
	if (value == "L") return LogicConstraint::NOT_LOGIC_UNKNOWN;
	if (value == "N") return LogicConstraint::ONLY_LOGIC_01;
	if (value == "V") return LogicConstraint::ONLY_LOGIC_01X;

	Logging::Panic();
}

StabilityConstraint GetStabilityConstraintForString(const std::string& value)
{
	if (value == "*") return StabilityConstraint::NO_CONSTRAINT;
	if (value == "?") return StabilityConstraint::SIGNAL_STABLE;
	if (value == "R") return StabilityConstraint::SIGNAL_RISING;
	if (value == "F") return StabilityConstraint::SIGNAL_FALLING;
	if (value == "T") return StabilityConstraint::SIGNAL_RISEFALL;

	Logging::Panic();
}

std::vector<Logic> GetLogicValuesForString(const std::string& value)
{
	std::vector<Logic> result;
	for (auto const& element : value)
	{
		if (element == ' ') continue;
		result.push_back(GetLogicForCharacter(element));
	}
	return result;
}

std::vector<LogicConstraint> GetLogicConstraintsForString(const std::string& value)
{
	std::vector<LogicConstraint> result;
	for (auto const& element : value)
	{
		if (element == ' ') continue;
		result.push_back(GetLogicConstraintForCharacter(element));
	}
	return result;
}

LogicConstraint GetLogicConstraintForLogic(const Logic& value)
{
	switch (value)
	{
		case Logic::LOGIC_UNKNOWN:
			return LogicConstraint::ONLY_LOGIC_UNKNOWN;
		case Logic::LOGIC_ZERO:
			return LogicConstraint::ONLY_LOGIC_ZERO;
		case Logic::LOGIC_ONE:
			return LogicConstraint::ONLY_LOGIC_ONE;
		case Logic::LOGIC_DONT_CARE:
			return LogicConstraint::ONLY_LOGIC_DONT_CARE;
		default:
			Logging::Panic("Can not convert logic value " + to_string(value) + " to a constraint");
	}
}

Logic InvertLogicValue(const Logic& value)
{
	switch (value)
	{
		case Logic::LOGIC_ZERO:
			return Logic::LOGIC_ONE;
		case Logic::LOGIC_ONE:
			return Logic::LOGIC_ZERO;
		case Logic::LOGIC_DONT_CARE:
			return Logic::LOGIC_DONT_CARE;
		case Logic::LOGIC_UNKNOWN:
			return Logic::LOGIC_UNKNOWN;
		default:
			Logging::Panic();
	}
}

std::vector<Logic> InvertLogicValues(const std::vector<Logic>& value)
{
	std::vector<Logic> invertedValue;
	std::transform(value.begin(), value.end(), std::back_inserter(invertedValue), InvertLogicValue);
	return invertedValue;
}

bool IsValidLogic01(const Logic& value)
{
	return value == Logic::LOGIC_ZERO
		|| value == Logic::LOGIC_ONE;
}

bool IsValidLogic01X(const Logic& value)
{
	return value == Logic::LOGIC_ZERO
		|| value == Logic::LOGIC_ONE
		|| value == Logic::LOGIC_DONT_CARE;
}

bool IsValidLogicU01X(const Logic& value)
{
	return value == Logic::LOGIC_ZERO
		|| value == Logic::LOGIC_ONE
		|| value == Logic::LOGIC_DONT_CARE;
}

bool IsConstraintTrueForLogic(const Logic& logic, const LogicConstraint& constraint)
{
	switch (constraint)
	{
		case LogicConstraint::NO_CONSTRAINT:
			return true;
		case LogicConstraint::NOT_LOGIC_ZERO:
			return logic != Logic::LOGIC_ZERO;
		case LogicConstraint::NOT_LOGIC_ONE:
			return logic != Logic::LOGIC_ONE;
		case LogicConstraint::NOT_LOGIC_DONT_CARE:
			return logic != Logic::LOGIC_DONT_CARE;
		case LogicConstraint::NOT_LOGIC_UNKNOWN:
			return logic != Logic::LOGIC_UNKNOWN;
		case LogicConstraint::ONLY_LOGIC_ZERO:
			return logic == Logic::LOGIC_ZERO;
		case LogicConstraint::ONLY_LOGIC_ONE:
			return logic == Logic::LOGIC_ONE;
		case LogicConstraint::ONLY_LOGIC_DONT_CARE:
			return logic == Logic::LOGIC_DONT_CARE;
		case LogicConstraint::ONLY_LOGIC_UNKNOWN:
			return logic == Logic::LOGIC_UNKNOWN;
		case LogicConstraint::ONLY_LOGIC_01:
			return logic == Logic::LOGIC_ZERO || logic == Logic::LOGIC_ZERO;
		case LogicConstraint::ONLY_LOGIC_01X:
			return logic == Logic::LOGIC_ZERO || logic == Logic::LOGIC_ZERO || logic == Logic::LOGIC_DONT_CARE;
		default:
			Logging::Panic("Unknown LogicConstraint");
	}
}

};
};
