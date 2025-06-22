#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>
#include <vector>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"


namespace FreiTest
{
namespace Io
{
namespace Udfm
{
namespace SimpleGrammar
{
	struct effect_assignment {
		std::string name;
		std::vector<Basic::LogicConstraint> logicConstraints;
	};

	struct condition_assignment {
		std::string name;
		std::vector<Basic::LogicConstraint> logicConstraints;
	};

	struct test {
		std::string type;
		std::vector<effect_assignment> effects;
		std::vector<condition_assignment> conditions;
	};

	struct fault {
		std::string name;
		std::vector<test> tests;
	};

	struct cell {
		std::string type;
		std::vector<fault> faults;
	};

};
};
};
};

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::SimpleGrammar::effect_assignment,
	name, logicConstraints
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::SimpleGrammar::condition_assignment,
	name, logicConstraints
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::SimpleGrammar::test,
	type, effects, conditions
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::SimpleGrammar::fault,
	name, tests
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::SimpleGrammar::cell,
	type, faults
)


namespace FreiTest
{
namespace Io
{
namespace Udfm
{
namespace SimpleGrammar
{
	// Implementation
	// See https://www.boost.org/doc/libs/1_77_0/libs/spirit/doc/x3/html/index.html for more details

	namespace x3 = boost::spirit::x3;

	const x3::rule<class quoted_string_class, std::string> quoted_string = "Quoted String";
	const auto quoted_string_def = x3::lexeme[
			x3::lit('"') > +(x3::char_ - '"')  > '"'
		];
	BOOST_SPIRIT_DEFINE(quoted_string);

	const x3::rule<class node_value_symbol_class, std::string> node_value_symbols = "Node Value Symbols";
	const auto node_value_symbols_def = +(x3::char_("*=!KL01XUNV"));	// see @Logic.hpp
	BOOST_SPIRIT_DEFINE(node_value_symbols);

	const x3::rule<class test_type_class, std::string> test_type = "Test Type";
	const auto test_type_def = +(x3::char_("a-zA-Z0-9_"));
	BOOST_SPIRIT_DEFINE(test_type);

	const auto convert_to_logic_list = [](auto& value) {
		x3::_val(value) = Basic::GetLogicConstraintsForString(x3::_attr(value));
	};

	const x3::rule<class logic_constraints_class, std::vector<Basic::LogicConstraint>> logic_constraints = "Node Constraints";
	const auto logic_constraints_def = node_value_symbols [convert_to_logic_list];
	BOOST_SPIRIT_DEFINE(logic_constraints);


	const x3::rule<class effect_assignment_class, effect_assignment> effect_port_assignment = "Single Effect Assignment";
	const auto effect_port_assignment_def = quoted_string > '=' > logic_constraints;
	BOOST_SPIRIT_DEFINE(effect_port_assignment);

	const x3::rule<class condition_assignment_class, condition_assignment> condition_port_assignment = "Single Condition Assignment";
	const auto condition_port_assignment_def = quoted_string > '=' > logic_constraints;
	BOOST_SPIRIT_DEFINE(condition_port_assignment);

	const x3::rule<class test_class, test> single_test = "Test";
	const auto single_test_def =
		x3::lit("test") > '{'
		> test_type > (effect_port_assignment % ',') > ';'
		> x3::lit("Condition") > (condition_port_assignment % ',') > ';'
		> '}';
	BOOST_SPIRIT_DEFINE(single_test);

	const x3::rule<class fault_class, fault> single_fault = "Single Fault";
	const auto single_fault_def = x3::lit("Fault") > quoted_string > '{' > *(single_test) > '}';
	BOOST_SPIRIT_DEFINE(single_fault);

	const x3::rule<class cell_class, cell> single_cell = "Single Cell";
	const auto single_cell_def = x3::lit("Cell") > quoted_string > '{' > *(single_fault) > '}';
	BOOST_SPIRIT_DEFINE(single_cell);

	const x3::rule<class simple_udfm_class, std::vector<cell>> simple_udfm = "Simple Udfm";
	const auto simple_udfm_def = (+(single_cell));
	BOOST_SPIRIT_DEFINE(simple_udfm);

};
};
};
};
