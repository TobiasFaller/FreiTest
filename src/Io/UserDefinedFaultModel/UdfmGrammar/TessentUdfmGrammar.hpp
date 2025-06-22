#pragma once

#include <boost/spirit/home/x3.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <string>

#include "Basic/Logging.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"
#include "Io/UserDefinedFaultModel/UdfmGrammar/SimpleUdfmGrammar.hpp"


namespace FreiTest
{
namespace Io
{
namespace Udfm
{
namespace TessentGrammar
{

struct property_assignment_struct {
	std::string key;
	std::string value;
};

struct tessent_fault {
	std::string name;
	std::vector<SimpleGrammar::test> tests;
};

struct tessent_cell_or_instance {
	bool isCell;
	std::string type;
	std::vector<tessent_fault> faults;
};

struct udfm_type_struct {
	std::string faultType;
	bool observation;
	std::vector<tessent_cell_or_instance> cells;
};

struct tessent_udfm_struct {
	float version;
	std::vector<property_assignment_struct> properties;
	std::vector<udfm_type_struct> categories;
};

};
};
};
};

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::TessentGrammar::property_assignment_struct,
	key, value
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::TessentGrammar::tessent_fault,
	name, tests
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::TessentGrammar::tessent_cell_or_instance,
	isCell, type, faults
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::TessentGrammar::udfm_type_struct,
	faultType, observation, cells
)

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Udfm::TessentGrammar::tessent_udfm_struct,
	version, properties, categories
)

namespace FreiTest
{
namespace Io
{
namespace Udfm
{
namespace TessentGrammar
{
	// Implementation
	// See https://www.boost.org/doc/libs/1_77_0/libs/spirit/doc/x3/html/index.html for more details

	namespace x3 = boost::spirit::x3;

	const auto assign_true = [](auto& value) {
		x3::_val(value) = true;
	};

	const auto assign_false = [](auto& value) {
		x3::_val(value) = false;
	};

	const auto convert_to_float = [](auto& value) {
		x3::_val(value) = std::stof(x3::_attr(value));
	};

	const x3::rule<class identifier_class, std::string> version_number = "Version Number";
	const auto version_number_def = +(x3::char_("0-9_."));
	BOOST_SPIRIT_DEFINE(version_number);

	const x3::rule<class boolean_class, bool> boolean_string = "Boolean String";
	const auto boolean_string_def = (x3::lit("true") [ assign_true ]) | (x3::lit("false") [ assign_false ]);
	BOOST_SPIRIT_DEFINE(boolean_string);

	const x3::rule<class property_assignment_class, property_assignment_struct> property_assignment = "Property Assignment";
	const auto property_assignment_def = SimpleGrammar::quoted_string > ':' > SimpleGrammar::quoted_string;
	BOOST_SPIRIT_DEFINE(property_assignment);

	const x3::rule<class version_class, float> version = "Udfm Version";
	const auto version_def = x3::lit("version") > ':' > version_number [ convert_to_float ] > ';';
	BOOST_SPIRIT_DEFINE(version);

	const x3::rule<class properties_class, std::vector<property_assignment_struct>> properties = "Udfm Properties";
	const auto properties_def = x3::lit("Properties") > '{' > (property_assignment % ';') > '}';
	BOOST_SPIRIT_DEFINE(properties);

	const x3::rule<class effect_assignment_class, SimpleGrammar::effect_assignment> tessent_effect_port_assignment = "Tessent Single Effect Assignment";
	const auto tessent_effect_port_assignment_def = SimpleGrammar::quoted_string > ':' > SimpleGrammar::logic_constraints;
	BOOST_SPIRIT_DEFINE(tessent_effect_port_assignment);

	const x3::rule<class condition_assignment_class, SimpleGrammar::condition_assignment> tessent_condition_port_assignment = "Tessent Single Condition Assignment";
	const auto tessent_condition_port_assignment_def = SimpleGrammar::quoted_string > ':' > SimpleGrammar::logic_constraints;
	BOOST_SPIRIT_DEFINE(tessent_condition_port_assignment);

	const x3::rule<class test_class, SimpleGrammar::test> tessent_single_test = "Tessent Test";
	const auto tessent_single_test_def =
		x3::lit("Test") > '{'
		> SimpleGrammar::test_type > '{' > (tessent_effect_port_assignment % ';') > ';' > '}'
		> x3::lit("Conditions") > '{' > (tessent_condition_port_assignment % ';') > ';' > '}'
		> '}';
	BOOST_SPIRIT_DEFINE(tessent_single_test);

	const x3::rule<class fault_class, tessent_fault> tessent_single_fault = "Tessent Single Fault";
	const auto tessent_single_fault_def = x3::lit("Fault") > '(' > SimpleGrammar::quoted_string > ')' > '{' > +(tessent_single_test) > '}';
	BOOST_SPIRIT_DEFINE(tessent_single_fault);

	const x3::rule<class check_cell_or_instance_class, bool> check_cell_or_instance = "Check if cell or instance";
	const auto check_cell_or_instance_def = (x3::lit("Cell") [ assign_true ]) | (x3::lit("Instance") [ assign_false ]);
	BOOST_SPIRIT_DEFINE(check_cell_or_instance);

	const x3::rule<class cell_class, tessent_cell_or_instance> tessent_single_cell = "Tessent Single Cell";
	const auto tessent_single_cell_def =
		check_cell_or_instance
		> '(' > SimpleGrammar::quoted_string > ')'
		> '{' > +(tessent_single_fault) > '}';
	BOOST_SPIRIT_DEFINE(tessent_single_cell);

	const x3::rule<class cell_class, udfm_type_struct> single_udfm_type_struct = "Single Udfm Type";
	const auto single_udfm_type_struct_def =
		x3::lit("UdfmType") > '(' > SimpleGrammar::quoted_string > ')' > '{'
		> -(x3::lit("Observation") > ':' > boolean_string > ';')
		> +tessent_single_cell > '}';
	BOOST_SPIRIT_DEFINE(single_udfm_type_struct);

	const x3::rule<class tessent_udfm_class, tessent_udfm_struct> tessent_udfm = "Tessent Udfm";
	const auto tessent_udfm_def =
		x3::lit("UDFM") > '{'
		> -version
		> -properties
		> +single_udfm_type_struct
		> '}';
	BOOST_SPIRIT_DEFINE(tessent_udfm);

};
};
};
};
