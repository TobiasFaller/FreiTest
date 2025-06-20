#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace FreiTest
{
namespace Basic
{

enum class Logic: uint_fast8_t
{
	LOGIC_INVALID = '-',

	// Logic values
	LOGIC_UNKNOWN = 'U',
	LOGIC_ZERO = '0',
	LOGIC_ONE = '1',
	LOGIC_DONT_CARE = 'X'
};

enum class LogicConstraint: uint_fast8_t
{
	NO_CONSTRAINT = '*',

	NOT_LOGIC_ZERO = '=',
	NOT_LOGIC_ONE = '!',
	NOT_LOGIC_DONT_CARE = 'K',
	NOT_LOGIC_UNKNOWN = 'L',

	ONLY_LOGIC_ZERO = '0',
	ONLY_LOGIC_ONE = '1',
	ONLY_LOGIC_DONT_CARE = 'X',
	ONLY_LOGIC_UNKNOWN = 'U',

	ONLY_LOGIC_01 = 'N',
	ONLY_LOGIC_01X = 'V',
};

enum class StabilityConstraint: uint_fast8_t
{
	NO_CONSTRAINT = '*',

	SIGNAL_STABLE = '?',
	SIGNAL_RISING = 'R',
	SIGNAL_FALLING = 'F',
	SIGNAL_RISEFALL = 'T'
};

enum class LogicType: uint_fast8_t
{
	LOGIC_UNSPECIFIED = 0,
	LOGIC_01 = 1,
	LOGIC_01X = 2,
	LOGIC_01XU = 3
};

std::ostream& operator<<(std::ostream& os, const Logic& logicValue);
std::ostream& operator<<(std::ostream& os, const LogicConstraint& constraint);
std::ostream& operator<<(std::ostream& os, const StabilityConstraint& constraint);
std::ostream& operator<<(std::ostream& os, const LogicType& logicType);
std::ostream& operator<<(std::ostream& os, const std::vector<Logic>& logicValue);
std::ostream& operator<<(std::ostream& os, const std::vector<LogicConstraint>& logicConstraintList);

std::string to_string(const Logic& logicValue);
std::string to_string(const LogicConstraint& constraint);
std::string to_string(const StabilityConstraint& constraint);
std::string to_string(const LogicType& logicType);
std::string to_string(const std::vector<Logic>& logicValueList);
std::string to_string(const std::vector<LogicConstraint>& constraintList);

char to_char(const Logic& logicValue);
char to_char(const LogicConstraint& constraint);
char to_char(const StabilityConstraint& constraint);

Logic GetLogicForCharacter(const char& value);
Logic GetLogicForString(const std::string& value);
Logic GetLogicForConstraint(const LogicConstraint& constraint);
LogicConstraint GetLogicConstraintForCharacter(const char& value);
LogicConstraint GetLogicConstraintForString(const std::string& value);
LogicConstraint GetLogicConstraintForLogic(const Logic& value);
StabilityConstraint GetStabilityConstraintForCharacter(const char& value);
StabilityConstraint GetStabilityConstraintForString(const std::string& value);

std::vector<Logic> GetLogicValuesForString(const std::string& value);
std::vector<LogicConstraint> GetLogicConstraintsForString(const std::string& value);

Logic InvertLogicValue(const Logic& value);
std::vector<Logic> InvertLogicValues(const std::vector<Logic>& value);

bool IsValidLogic01(const Logic& value);
bool IsValidLogic01X(const Logic& value);
bool IsValidLogicU01X(const Logic& value);
bool IsConstraintTrueForLogic(const Logic& logic, const LogicConstraint& constraint);

};
};
