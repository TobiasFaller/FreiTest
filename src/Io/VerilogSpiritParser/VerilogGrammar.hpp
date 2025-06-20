#pragma once

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/variant.hpp>

#include <cstdint>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest {
namespace Io {
namespace Verilog {
namespace Grammar {

    namespace spirit = boost::spirit;
	namespace x3 = boost::spirit::x3;

    using test_iterator_type = std::string::const_iterator;
    using test_context_type = x3::phrase_parse_context<x3::space_type>::type;

    using base_iterator_type = std::istreambuf_iterator<char>;
    using iterator_type = spirit::multi_pass<base_iterator_type>;
    using context_type = x3::phrase_parse_context<x3::space_type>::type;

	namespace Comment {

		using skipper_type = x3::rule<class skipper_class, x3::unused_type const>;

		BOOST_SPIRIT_DECLARE(skipper_type);

	};

	namespace Generic {

		using quoted_string_type = x3::rule<class quoted_string_class, std::string>;
		using identifier_type = x3::rule<class identifier_class, std::string>;

		BOOST_SPIRIT_DECLARE(quoted_string_type);
		BOOST_SPIRIT_DECLARE(identifier_type);

	};

	namespace Vector {

		using vector_size_type = x3::rule<class vector_size_class, Verilog::VectorSize>;
		using vector_initializer_type = x3::rule<class vector_initializer_class, Verilog::VectorInitializer>;

		BOOST_SPIRIT_DECLARE(vector_size_type);
		BOOST_SPIRIT_DECLARE(vector_initializer_type);

	};

	namespace Port {

		using type_type = x3::rule<class port_type_class, Verilog::PortType>;
		using name_type = x3::rule<class port_name_class, std::string>;
		using port_type = x3::rule<class port_class, std::vector<Verilog::Port>>;

		BOOST_SPIRIT_DECLARE(type_type);
		BOOST_SPIRIT_DECLARE(name_type);
		BOOST_SPIRIT_DECLARE(port_type);

	};

	namespace Parameter {

		using value_type = x3::rule<class parameter_value_class, Verilog::ParameterValue>;
		using parameter_type = x3::rule<class parameter_class, Verilog::Parameter>;

		BOOST_SPIRIT_DECLARE(value_type);
		BOOST_SPIRIT_DECLARE(parameter_type);

	};

	namespace Attribute {

		using name_type = x3::rule<class attribute_name_class, std::string>;
		using value_type = x3::rule<class attribute_value_class, Verilog::AttributeValue>;
		using attribute_type = x3::rule<class attribute_class, Verilog::Attribute>;

		BOOST_SPIRIT_DECLARE(name_type);
		BOOST_SPIRIT_DECLARE(value_type);
		BOOST_SPIRIT_DECLARE(attribute_type);

	};

	namespace Wire {

		using name_type = x3::rule<class wire_name_class, std::string>;
		using wire_type = x3::rule<class wire_class, std::vector<Verilog::Wire>>;

		BOOST_SPIRIT_DECLARE(name_type);
		BOOST_SPIRIT_DECLARE(wire_type);

	};

	namespace WireReference {

		using name_type = x3::rule<class wire_reference_name_class, std::string>;
		using wire_reference_type = x3::rule<class wire_reference_class, Verilog::WireReference>;

		BOOST_SPIRIT_DECLARE(name_type);
		BOOST_SPIRIT_DECLARE(wire_reference_type);

	};

	namespace Bus {

		using bus_type = x3::rule<class bus_class, Verilog::Bus>;

		BOOST_SPIRIT_DECLARE(bus_type);

	};

	namespace Assignment {

		using assignment_type = x3::rule<class assignment_class, Verilog::Assignment>;

		BOOST_SPIRIT_DECLARE(assignment_type);

	};

	namespace Instantiation {

		using parameter_list_type = x3::rule<class parameter_mapping_class, std::vector<Verilog::ParameterMapping>>;
		using port_list_type = x3::rule<class port_mapping_class, std::vector<Verilog::PortMapping>>;
		using instantiation_type = x3::rule<class instantiation_class, Verilog::Instantiation>;

		BOOST_SPIRIT_DECLARE(parameter_list_type);
		BOOST_SPIRIT_DECLARE(port_list_type);
		BOOST_SPIRIT_DECLARE(instantiation_type);

	};

	namespace Module {

		using module_type = x3::rule<class module_class, Verilog::Module>;

		BOOST_SPIRIT_DECLARE(module_type);

	};

	namespace Source {

		using source_type = x3::rule<class source_class, Verilog::ModuleCollection>;

		BOOST_SPIRIT_DECLARE(source_type);

	};

	/**
	 * A skipper that skips all whitespace,
	 * single-line comments and multi-line comments.
	 */
	const Grammar::Comment::skipper_type& skipper(void);

	/**
	 * A parser which allows to parse quoted strings.
	 *
	 * Example:
	 * "Hello World"
	 */
	const Grammar::Generic::quoted_string_type& quoted_string(void);

	/**
	 * A parser which allows to parse quoted strings.
	 *
	 * Example:
	 * "Hello World"
	 */
	const Grammar::Generic::identifier_type& identifier(void);

	/**
	 * This parser reads a vector size which consists of two integers.
	 *
	 * Examples:
	 * [top : bottom]
	 * [7:0]
	 */
	const Grammar::Vector::vector_size_type& vector_size(void);

	/**
	 * A parser which reads a vector initializer which can have
	 * one of the radixes 2, 8, 10 or 16.
	 * The maximum number of allowed bits is 64.
	 *
	 * Examples:
	 * 8'b 00101010
	 * 8'o 52
	 * 8'd 42
	 * 8'h 2A
	 */
	const Grammar::Vector::vector_initializer_type& vector_initializer(void);

	/**
	 * Parses a wire reference which consists of a wire name, followed
	 * by a vector range or index.
	 *
	 * Examples:
	 * name
	 * name[42]
	 * name[7:0]
	 */
	const Grammar::WireReference::wire_reference_type& wire_reference(void);

	/**
	 * Parses a bus component which consist of a wire reference
	 * or a concatenation of multiple bus components (recursive).
	 *
	 * Examples:
	 * wire_ref [7:0]
	 * wire_ref [1]
	 * wire_ref
	 * {bus, bus, ...}
	 */
	const Grammar::Bus::bus_type& bus(void);

	/**
	 * Parses an attribute which can occur before a statement in
	 * a verilog module or which direclty precedes a module.
	 *
	 * Examples:
	 * (* auto_generated = "TRUE" *)
	 * (* instance_id = 42 *)
	 */
	const Grammar::Attribute::attribute_type& attribute(void);

	/**
	 * Parses a port name.
	 *
	 * Examples:
	 * clk
	 * data_in
	 */
	const Grammar::Port::name_type& port_name(void);

	/**
	 * Parses a port type.
	 *
	 * Examples:
	 * input
	 * output
	 * inout
	 */
	const Grammar::Port::type_type& port_type(void);

	/**
	 * Parses a port statement which may include a type (wire)
	 * and a size.
	 *
	 * Examples:
	 * input [7:0] name
	 * input clk
	 * output [31:0] mem_port
	 */
	const Grammar::Port::port_type& port_statement(void);

	/**
	 * Parses a parameter value which is the source operand of
	 * a parameter assignment. The value can be a signed integer
	 * or a vector initializer.
	 *
	 * Examples:
	 * 8
	 * 32'h 00000000
	 */
	const Grammar::Parameter::value_type& parameter_value(void);

	/**
	 * Parses a parameter statement with a value which can either be
	 * an integer or a vector initializer.
	 *
	 * Examples:
	 * paramaeter N = 8
	 * parameter START_ADDRESS = 32'h 00000000
	 */
	const Grammar::Parameter::parameter_type& parameter_statement(void);

	/**
	 * Parses an assignment statement inside a verilog module.
	 * The source can either be another vector or a vector initializer.
	 *
	 * Examples:
	 * assign target_bus = source_bus
	 * assign target_bus = vector_intializer
	 */
	const Grammar::Assignment::assignment_type& assignment_statement(void);

	/**
	 * Parses a wire statement inside a verilog module.
	 *
	 * Examples:
	 * wire a
	 * wire [7:0] another_wire
	 */
	const Grammar::Wire::wire_type& wire_statement(void);

	/**
	 * Parses a module instantiation inside a verilog module.
	 * The name and the parameter list is optional.
	 * Parameter and ports can be assigned by order (without dot)
	 * and by name (with dot).
	 *
	 * Examples:
	 *
	 * module_type name #(
	 *     .param(value),
	 *     . ...
	 * )
	 * (
	 *     .port(bus),
	 *     . ...
	 * )
	 *
	 * module_type #(
	 *     value,
	 *     ...
	 * )
	 * (
	 *     bus,
	 *     ...
	 * )
	 */
	const Grammar::Instantiation::instantiation_type& module_instantiation(void);

	/**
	 * Parses a whole verilog module declaration inclusive ports,
	 * parameter, wires, assignments and instantiations.
	 *
	 * Not all verilog features are supported.
	 * Registers are not allowed.
	 *
	 * Examples:
	 *
	 * module module_name #(
	 *     parameter parameter_name = default_value,
	 *     ...
	 * )
	 * (
	 *     port_type [vec_start : vec_end] port_name
	 * );
	 *
	 * TODO
	 *
	 * endmodule
	 */
	const Grammar::Module::module_type& verilog_module(void);

	/**
	 * Parses a verilog source file with all included module definitions.
	 *
	 * Examples:
	 * module a
	 * (
	 *     port_type [vec_start : vec_end] port_name
	 * );
	 *
	 * GATES
	 *
	 * endmodule
	 *
	 * module b
	 * (
	 *     port_type [vec_start : vec_end] port_name
	 * );
	 *
	 * GATES
	 *
	 * endmodule
	 */
	const Grammar::Source::source_type& verilog_source(void);

};
};
};
};
