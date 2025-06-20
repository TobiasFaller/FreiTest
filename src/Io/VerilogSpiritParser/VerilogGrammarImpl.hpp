#pragma once

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/std_tuple.hpp>
#include <boost/fusion/include/deque.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/variant.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <type_traits>

#include "Basic/Logging.hpp"
#include "Io/VerilogSpiritParser/VerilogGrammar.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::VectorSize,
	top, bottom, unknown
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::VectorInitializer,
	value
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Port,
	name, type, size
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Wire,
	size, name
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Attribute,
	name, value
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Parameter,
	name, value
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::WireReference,
	name, size
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Bus,
	sources
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Assignment,
	target, source
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::ParameterMapping,
	name, value
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::PortMapping,
	name, bus
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Instantiation,
	type, name, parameter, ports
)
BOOST_FUSION_ADAPT_STRUCT(
	FreiTest::Io::Verilog::Module,
	name, ports, parameter, wires, attributes
)

namespace FreiTest {
namespace Io {
namespace Verilog {
namespace Grammar {

	// Implementation
	// See https://www.boost.org/doc/libs/1_77_0/libs/spirit/doc/x3/html/index.html for more details

	namespace x3 = boost::spirit::x3;
	namespace spirit = boost::spirit;

	namespace Grammar = Verilog::Grammar;

	// Helper function to set value in the context.
	// See https://stackoverflow.com/questions/62628317/parsing-variant-of-struct-with-a-single-member-using-boost-spirit-x3-and-fusion
	// This is required for:
	// 1. structs with only a single value
	// 2. alternative parsers to overwrite container contents
	const auto set_value = [](auto& context) {
		x3::_val(context) = x3::_attr(context);
	};

    namespace Comment {

        const skipper_type skipper_impl = "Comment or Whitespace";
		const auto skipper_impl_def = x3::space
			| ("/*" > *(x3::char_ - "*/") > "*/")
			| ("//" > *(x3::char_ - x3::eol) > x3::eol);
		BOOST_SPIRIT_DEFINE(skipper_impl);

    }

    namespace Generic {

		const std::vector<std::string> protected_identifiers = {
			"module", "endmodule", "wire", "reg", "logic", "assign"
		};

		auto check_protected_keyword = [](auto& ctx) {
			std::string identifier =
				boost::fusion::at_c<0>(x3::_attr(ctx))
				+ boost::fusion::at_c<1>(x3::_attr(ctx));

			x3::_pass(ctx) = (
				std::find(protected_identifiers.begin(), protected_identifiers.end(), identifier)
					== protected_identifiers.end());
			x3::_val(ctx) = identifier;
		};

		const quoted_string_type quoted_string_impl = "Quoted String";
		const auto quoted_string_impl_def = x3::lexeme[
				x3::lit('"') > *(x3::char_ - '"')  > '"'
			];
		BOOST_SPIRIT_DEFINE(quoted_string_impl);

		const x3::rule<class escaped_identifier_class, std::string> escaped_identifier_impl = "Escaped Identifier";
		const auto escaped_identifier_impl_def = x3::lit('\\')
			> x3::lexeme[
				+(x3::char_("a-zA-Z0-9[](){}<>#$'=&%?\"!^|-_.:,;\\/+*"))
			];
		BOOST_SPIRIT_DEFINE(escaped_identifier_impl);

		const x3::rule<class unescaped_identifier_class, std::string> unescaped_identifier_impl = "Unescaped Identifier";
		const auto unescaped_identifier_impl_def = x3::lexeme[
				x3::char_("a-zA-Z_") >> *(x3::char_("a-zA-Z0-9_"))
			] [ check_protected_keyword ];
		BOOST_SPIRIT_DEFINE(unescaped_identifier_impl);

		const identifier_type identifier_impl = "Identifier";
		const auto identifier_impl_def = escaped_identifier_impl | unescaped_identifier_impl;
		BOOST_SPIRIT_DEFINE(identifier_impl);

	}

	namespace Vector {

		const vector_size_type size_impl = "Vector Size";
		const auto size_impl_def = x3::lit('[') >> x3::uint64 >> ':' >> x3::uint64 >> ']' >> x3::attr(false);
		BOOST_SPIRIT_DEFINE(size_impl);

		const auto uint64_hex_parser = x3::int_parser<uint64_t, 16>{};
		const auto uint64_dec_parser = x3::int_parser<uint64_t, 10>{};
		const auto uint64_oct_parser = x3::int_parser<uint64_t, 8>{};
		const auto uint64_bin_parser = x3::int_parser<uint64_t, 2>{};

		auto convert_value_to_vector = [](auto value) {
			std::vector<VectorBit> result;
			while (value > 0u)
			{
				result.push_back((value & 1u) ? VectorBit::BIT_1 : VectorBit::BIT_0);
				value /= 2u;
			}
			return result;
		};

		auto convert_negative_value = [](auto& ctx) {
			x3::_val(ctx) = convert_value_to_vector((~x3::_attr(ctx)) + 1u);
		};
		auto convert_positive_value = [](auto& ctx) {
			x3::_val(ctx) = convert_value_to_vector(x3::_attr(ctx));
		};

		auto convert_binary_to_vec = [](auto& ctx) {
			std::vector<VectorBit> bits;
			for (auto& value : x3::_attr(ctx))
			{
				switch (value)
				{
					case '0': bits.push_back(VectorBit::BIT_0); break;
					case '1': bits.push_back(VectorBit::BIT_1); break;
					case 'X': case 'x': bits.push_back(VectorBit::BIT_X); break;
					case 'U': case 'u': bits.push_back(VectorBit::BIT_U); break;
				}
			}

			std::reverse(bits.begin(), bits.end());
			x3::_val(ctx) = bits;
		};

		auto convert_octal_to_vec = [](auto& ctx) {
			std::vector<VectorBit> bits;
			for (auto& value : x3::_attr(ctx))
			{
				switch (value)
				{
					case '0': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case '1': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case '2': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case '3': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case '4': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case '5': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case '6': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case '7': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case 'X': case 'x': bits.insert(bits.end(), { VectorBit::BIT_X, VectorBit::BIT_X, VectorBit::BIT_X }); break;
					case 'U': case 'u': bits.insert(bits.end(), { VectorBit::BIT_U, VectorBit::BIT_U, VectorBit::BIT_U }); break;
				}
			}

			std::reverse(bits.begin(), bits.end());
			x3::_val(ctx) = bits;
		};

		auto convert_hex_to_vec = [](auto& ctx) {
			std::vector<VectorBit> bits;
			for (auto& value : x3::_attr(ctx))
			{
				switch (value)
				{
					case '0': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case '1': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case '2': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case '3': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case '4': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case '5': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case '6': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case '7': bits.insert(bits.end(), { VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case '8': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case '9': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case 'A': case 'a': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case 'B': case 'b': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case 'C': case 'c': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_0 }); break;
					case 'D': case 'd': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_0, VectorBit::BIT_1 }); break;
					case 'E': case 'e': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_0 }); break;
					case 'F': case 'f': bits.insert(bits.end(), { VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_1, VectorBit::BIT_1 }); break;
					case 'X': case 'x': bits.insert(bits.end(), { VectorBit::BIT_X, VectorBit::BIT_X, VectorBit::BIT_X, VectorBit::BIT_X }); break;
					case 'U': case 'u': bits.insert(bits.end(), { VectorBit::BIT_U, VectorBit::BIT_U, VectorBit::BIT_U, VectorBit::BIT_U }); break;
				}
			}

			std::reverse(bits.begin(), bits.end());
			x3::_val(ctx) = bits;
		};

		const x3::rule<class binary_initializer_class, std::vector<VectorBit>> binary_initializer_parser = "Binary Vector Initializer";
		const auto binary_initializer_parser_def = (x3::lit("-") > uint64_bin_parser)[ convert_negative_value ]
			| (+x3::char_("01XUxu")) [ convert_binary_to_vec ];
		BOOST_SPIRIT_DEFINE(binary_initializer_parser);

		const x3::rule<class octal_initializer_class, std::vector<VectorBit>> octal_initializer_parser = "Octal Vector Initializer";
		const auto octal_initializer_parser_def = (x3::lit("-") > uint64_oct_parser)[ convert_negative_value ]
			| (+x3::char_("01234567XUxu")) [ convert_octal_to_vec ];
		BOOST_SPIRIT_DEFINE(octal_initializer_parser);

		const x3::rule<class decimal_initializer_class, std::vector<VectorBit>> decimal_initializer_parser = "Decimal Vector Initializer";
		const auto decimal_initializer_parser_def = (x3::lit("-") > uint64_dec_parser)[ convert_negative_value ]
			| uint64_dec_parser[ convert_positive_value ];
		BOOST_SPIRIT_DEFINE(decimal_initializer_parser);

		const x3::rule<class hexadec_initializer_class, std::vector<VectorBit>> hexadec_initializer_parser = "Hexadecimal Vector Initializer";
		const auto hexadec_initializer_parser_def = (x3::lit("-") > uint64_hex_parser)[ convert_negative_value ]
			| (+x3::char_("0123456789ABCDEFXUabcdefxu")) [ convert_hex_to_vec ];
		BOOST_SPIRIT_DEFINE(hexadec_initializer_parser);

		struct bits_tag {};

		auto assign_vector_bits = [](auto& ctx) {
			x3::_pass(ctx) = (x3::_attr(ctx) >= 0);
			x3::get<bits_tag>(ctx) = x3::_attr(ctx);
		};
		auto assign_vector_value = [](auto& ctx) {
			// Clip or expand bits
			while (x3::_attr(ctx).size() > x3::get<bits_tag>(ctx))
				x3::_attr(ctx).pop_back();
			while (x3::_attr(ctx).size() < x3::get<bits_tag>(ctx))
				x3::_attr(ctx).push_back(VectorBit::BIT_0);

			x3::_val(ctx).value = x3::_attr(ctx);
		};

		const vector_initializer_type initializer_impl = "Vector Initializer";
		const auto initializer_impl_def = x3::with<bits_tag>(0u)[
			x3::uint64[ assign_vector_bits ]
			>> ('\'' > (
				('b' > binary_initializer_parser[ assign_vector_value ])
				| ('d' > decimal_initializer_parser[ assign_vector_value ])
				| ('h' > hexadec_initializer_parser[ assign_vector_value ])
				| ('o' > octal_initializer_parser[ assign_vector_value ])
			))
		];
		BOOST_SPIRIT_DEFINE(initializer_impl);

	}

	namespace Port {

		struct port_data_tag {};
		typedef struct port_data_s {
			port_data_s():
				type(Verilog::PortType::PORT_UNKNOWN),
				size(0, 0)
			{
			}

			Verilog::PortType type;
			Verilog::VectorSize size;
		} port_data;

		auto reset_tag = [](auto& ctx) {
			x3::get<port_data_tag>(ctx) = port_data();
		};
		auto set_type = [](auto& ctx) {
			x3::get<port_data_tag>(ctx).type = x3::_attr(ctx);
		};
		auto set_size = [](auto& ctx) {
			x3::get<port_data_tag>(ctx).size = x3::_attr(ctx);
		};

		auto add_ports = [](auto& ctx) {
			Verilog::PortType& type = x3::get<port_data_tag>(ctx).type;
			Verilog::VectorSize& size = x3::get<port_data_tag>(ctx).size;

			for (const std::string& portName : x3::_attr(ctx)) {
				x3::_val(ctx).push_back(Verilog::Port(portName, type, size));
			}
		};

		struct type_symbols : x3::symbols<Verilog::PortType>
		{
			type_symbols()
			{
				add
					("input", Verilog::PortType::PORT_IN)
					("output", Verilog::PortType::PORT_OUT)
					("inout", Verilog::PortType::PORT_INOUT)
					;
			}
		};

		const type_type type_impl = "Port Type";
		const auto type_impl_def = type_symbols();
		BOOST_SPIRIT_DEFINE(type_impl);

		const name_type name_impl = "Port Name";
		const auto name_impl_def = identifier();
		BOOST_SPIRIT_DEFINE(name_impl);

		const port_type port_impl = "Port";
		const auto port_impl_def = x3::with<port_data_tag>(port_data())
			[
				x3::eps [ reset_tag ]
				>> type_impl[ set_type ]
				>> -x3::lit("wire")
				>> -vector_size()[ set_size ]
				>> (name_impl % ',')[ add_ports ]
			];
		BOOST_SPIRIT_DEFINE(port_impl);

	}

	namespace Parameter {

		const value_type value_impl = "Parameter Value";
		const auto value_impl_def = vector_initializer() | x3::int64;
		BOOST_SPIRIT_DEFINE(value_impl);

		const parameter_type parameter_impl = "Parameter";
		const auto parameter_impl_def = x3::lit("parameter")
			> identifier()
			> '=' > value_impl;
		BOOST_SPIRIT_DEFINE(parameter_impl);

	}

	namespace Attribute {

		const name_type name_impl = "Attribute Name";
		const auto name_impl_def = identifier();
		BOOST_SPIRIT_DEFINE(name_impl);

		const value_type value_impl = "Attribute Value";
		const auto value_impl_def = quoted_string()
			| vector_initializer()
			| x3::int64;
		BOOST_SPIRIT_DEFINE(value_impl);

		const attribute_type attribute_impl = "Attribute";
		const auto attribute_impl_def =
			x3::lit("(*") > name_impl > "=" > value_impl > "*)";
		BOOST_SPIRIT_DEFINE(attribute_impl);

	}

    namespace Wire {

		struct wire_data_tag {};
		typedef struct wire_data_s {
			wire_data_s():
				size(0, 0)
			{
			}

			Verilog::VectorSize size;
		} wire_data;

		auto reset_tag = [](auto& ctx) {
			x3::get<wire_data_tag>(ctx) = wire_data();
		};
		auto set_size = [](auto& ctx) {
			x3::get<wire_data_tag>(ctx).size = x3::_attr(ctx);
		};
		auto add_wires = [](auto& ctx) {
			Verilog::VectorSize& size = x3::get<wire_data_tag>(ctx).size;

			for (const std::string& wireName : x3::_attr(ctx)) {
				x3::_val(ctx).push_back(Verilog::Wire(wireName, size));
			}
		};

		const name_type name_impl = "Wire Name";
		const auto name_impl_def = identifier();
		BOOST_SPIRIT_DEFINE(name_impl);

		const wire_type wire_impl = "Wire";
		const auto wire_impl_def = x3::with<wire_data_tag>(wire_data())
			[
				x3::eps [ reset_tag ]
				>> (
					x3::lit("wire")
					> -vector_size()[ set_size ]
					> (name_impl % ',')[ add_wires ]
				)
			];
		BOOST_SPIRIT_DEFINE(wire_impl);

	}

	namespace WireReference {

		const name_type name_impl = "Wire Name";
		const auto name_impl_def = identifier();
		BOOST_SPIRIT_DEFINE(name_impl);

		auto set_index = [](auto& ctx) {
			const uint64_t value = x3::_attr(ctx);
			x3::_val(ctx) = { value, value };
		};

		const x3::rule<class index_class, Verilog::VectorSize> index_impl = "Wire Index";
		const auto index_impl_def = '[' >> x3::uint64[ set_index ] >> ']';
		BOOST_SPIRIT_DEFINE(index_impl);

		const x3::rule<class size_class, Verilog::VectorSize> size_impl = "Wire Size";
		const auto size_impl_def = vector_size();
		BOOST_SPIRIT_DEFINE(size_impl);

		const wire_reference_type wire_reference_impl = "Wire Reference";
		const auto wire_reference_impl_def = name_impl
			> (index_impl | size_impl | x3::attr(VectorSize()));
		BOOST_SPIRIT_DEFINE(wire_reference_impl);

	}

	namespace Bus {

		auto reset_bus = [](auto& ctx) {
			x3::_attr(ctx) = {};
		};

		auto add_bus = [](auto& ctx) {
			Verilog::Bus& sourceBus = x3::_attr(ctx);
			Verilog::Bus& targetBus = x3::_val(ctx);

			targetBus.sources.insert(targetBus.sources.end(),
				sourceBus.sources.begin(), sourceBus.sources.end());
		};
		auto convert_to_bus = [](auto& ctx) {
			x3::_val(ctx) = Verilog::Bus({ x3::_attr(ctx) });
		};

		const bus_type bus_impl = "Bus";
		const auto bus_impl_def =
			// Concatenation of bus list
			( x3::lit('{') >> (bus_impl[ add_bus ] % ',') >> '}' )
			// Single vector initializer (constant)
			| vector_initializer()[ convert_to_bus ]
			// Single wire reference
			| wire_reference()[ convert_to_bus ];
		BOOST_SPIRIT_DEFINE(bus_impl);

	}

	namespace Assignment {

		const assignment_type assignment_impl = "Assignment";
		const auto assignment_impl_def = x3::lit("assign")
			// Target bus
			> bus()
			> '='
			// Source bus
			> bus();
		BOOST_SPIRIT_DEFINE(assignment_impl);

	}

	namespace Instantiation {

		const parameter_list_type parameter_list_impl = "Parameter Mapping";
		const auto parameter_list_impl_def =
			(
				"#("
				> (
					// 1.) Assigned by name
					// 2.) Assigned by order
					(x3::lit('.') > identifier() > '(' > parameter_value() > ')')
					| (x3::attr(std::string("")) > parameter_value())
				) % ','
				> ')'
			)
			// 3.) Empty list
			| x3::attr(std::vector<Verilog::ParameterMapping>());
		BOOST_SPIRIT_DEFINE(parameter_list_impl);

		const x3::rule<class bus_or_empty_parser_class, Verilog::Bus> bus_or_empty_parser = "Verilog Bus";
		const auto bus_or_empty_parser_def =
			bus() [ set_value ]
			| x3::attr(Verilog::Bus());
		BOOST_SPIRIT_DEFINE(bus_or_empty_parser);

		const port_list_type port_list_impl = "Port Mapping";
		const auto port_list_impl_def =
			(
				// 1.) Assigned by name
				// 2.) Assigned by order
				(x3::lit('.') > identifier() > '(' > bus_or_empty_parser > ')')
				| (x3::attr(std::string("")) > bus_or_empty_parser)
			) % ','
			// 3.) Empty list
			| x3::attr(std::vector<Verilog::PortMapping>());
		BOOST_SPIRIT_DEFINE(port_list_impl);

		const instantiation_type instantiation_impl = "Instantiation";
		const auto instantiation_impl_def =
			// Module type
			identifier()
			// Optional name
			>> (identifier() | x3::attr(std::string("")))
			// Parameter list
			>> (parameter_list_impl | x3::attr(std::vector<Verilog::ParameterMapping>()))
			// Port list
			>> '(' > port_list_impl > ')';
		BOOST_SPIRIT_DEFINE(instantiation_impl);

	}

	namespace Module {

		template<typename Test, template<typename...> class Ref>
		struct is_specialization : std::false_type {};

		template<template<typename...> class Ref, typename... Args>
		struct is_specialization<Ref<Args...>, Ref>: std::true_type {};
		template<template<typename...> class Ref, typename... Args>
		struct is_specialization<Ref<Args...>&, Ref>: std::true_type {};

		const x3::rule<class name_class, std::string> name_parser = "Module Name";
		const auto name_parser_def = identifier();
		BOOST_SPIRIT_DEFINE(name_parser);

		const auto reset_port = [](auto& ctx) {
			x3::_val(ctx) = Verilog::Port();
		};
		const auto set_name = [](auto& ctx) {
			x3::_val(ctx).name = x3::_attr(ctx);
		};
		const auto set_size = [](auto& ctx) {
			x3::_val(ctx).size = x3::_attr(ctx);
		};
		const auto set_type = [](auto& ctx) {
			x3::_val(ctx).type = x3::_attr(ctx);
		};

		const x3::rule<class port_declaration_class, Verilog::Port> port_declaration_impl = "Module Port Declaration";
		const auto port_declaration_impl_def = x3::eps [ reset_port ]
			>> port_type() [ set_type ]
			>> -x3::lit("wire")
			>> -vector_size() [ set_size ]
			>> port_name() [ set_name ];
		BOOST_SPIRIT_DEFINE(port_declaration_impl);

		const x3::rule<class port_name_class, Verilog::Port> port_name_impl = "Module Port Name";
		const auto port_name_impl_def = port_name()
			>> x3::attr(Verilog::PortType::PORT_UNKNOWN)
			>> x3::attr(Verilog::VectorSize());
		BOOST_SPIRIT_DEFINE(port_name_impl);

		const x3::rule<class port_list_alt1_class, std::vector<Verilog::Port>> port_list_alt1_impl = "Module Port List (Names)";
		const x3::rule<class port_list_alt2_class, std::vector<Verilog::Port>> port_list_alt2_impl = "Module Port List (Declaration)";
		const x3::rule<class port_list_alt3_class, std::vector<Verilog::Port>> port_list_alt3_impl = "Module Port List (Empty)";
		const x3::rule<class port_list_class, std::vector<Verilog::Port>> port_list_impl = "Module Port List";
		const auto port_list_alt1_impl_def = x3::lit('(') >> (port_name_impl % ',') >> ')';
		const auto port_list_alt2_impl_def = x3::lit('(') >> (port_declaration_impl % ',') >> ')';
		const auto port_list_alt3_impl_def = x3::lit('(') >> x3::attr(std::vector<Verilog::Port> {}) >> ')';
		const auto port_list_impl_def = port_list_alt1_impl[ set_value ] | port_list_alt2_impl[ set_value ] | port_list_alt3_impl[ set_value ];
		BOOST_SPIRIT_DEFINE(port_list_alt1_impl);
		BOOST_SPIRIT_DEFINE(port_list_alt2_impl);
		BOOST_SPIRIT_DEFINE(port_list_alt3_impl);
		BOOST_SPIRIT_DEFINE(port_list_impl);

		const x3::rule<class parameter_list_alt1_class, std::vector<Verilog::Parameter>> parameter_list_alt1_impl = "Module Parameter List (Full)";
		const x3::rule<class parameter_list_alt2_class, std::vector<Verilog::Parameter>> parameter_list_alt2_impl = "Module Parameter List (Empty)";
		const x3::rule<class parameter_list_class, std::vector<Verilog::Parameter>> parameter_list_impl = "Module Parameter List";
		const auto parameter_list_alt1_impl_def = x3::lit("#(") >> (parameter_statement() % ',') >> ')';
		const auto parameter_list_alt2_impl_def = x3::lit("#(") >> x3::attr(std::vector<Verilog::Parameter> {}) >> ')';
		const auto parameter_list_impl_def = parameter_list_alt1_impl [ set_value ] | parameter_list_alt2_impl [ set_value ];
		BOOST_SPIRIT_DEFINE(parameter_list_alt1_impl);
		BOOST_SPIRIT_DEFINE(parameter_list_alt2_impl);
		BOOST_SPIRIT_DEFINE(parameter_list_impl);

		auto set_module_name = [](auto& ctx) {
			x3::_val(ctx).name = x3::_attr(ctx);
		};
		auto set_port_list = [](auto& ctx) {
			// Convert vectors to collections
			x3::_val(ctx).ports = PortCollection(x3::_attr(ctx).begin(), x3::_attr(ctx).end());
		};
		auto set_parameter_list = [](auto& ctx) {
			// Convert vectors to collections
			x3::_val(ctx).parameter = ParameterCollection(x3::_attr(ctx).begin(), x3::_attr(ctx).end());
		};

		struct unused_attributes_tag {};

		auto reset_tag = [](auto& ctx) {
			x3::get<unused_attributes_tag>(ctx) = std::vector<Verilog::Attribute>();
		};
		auto store_attribute = [](auto& ctx) {
			x3::get<unused_attributes_tag>(ctx).push_back(x3::_attr(ctx));
		};
		auto assign_attributes = [](auto& ctx, auto& target) {
			auto& unusedAttributes = x3::get<unused_attributes_tag>(ctx);

			if constexpr (is_specialization<decltype(target), std::vector>::value)
			{
				for (auto& element: target) {
					element.attributes.insert(element.attributes.end(),
						unusedAttributes.cbegin(), unusedAttributes.cend());
				}
			}
			else
			{
				target.attributes.insert(target.attributes.end(),
						unusedAttributes.cbegin(), unusedAttributes.cend());
			}

			unusedAttributes.clear();
		};

		auto add_port_statement = [](auto& ctx) {
			std::vector<Verilog::Port>& newPorts = x3::_attr(ctx);
			assign_attributes(ctx, newPorts);

			Verilog::PortCollection& ports = x3::_val(ctx).ports;
			for (auto newPort : newPorts)
			{
				auto iter = ports.get<Verilog::name_index_tag>().find(newPort.name);
				if (iter != ports.get<Verilog::name_index_tag>().end())
				{
					if (iter->type == Verilog::PortType::PORT_UNKNOWN)
					{
						ports.get<Verilog::name_index_tag>().replace(iter, newPort);
						continue;
					}

					// Redeclaration of a port is not allowed
					LOG(ERROR) << "The port " << newPort.name << " was declared twice in the module " << x3::_val(ctx).name;
					x3::_pass(ctx) = false;
					return;
				}

				// Port does not exist
				ports.push_back(newPort);
			}
		};
		auto add_wire_statement = [](auto& ctx) {
			std::vector<Verilog::Wire>& newWires = x3::_attr(ctx);
			assign_attributes(ctx, newWires);

			Verilog::WireCollection& wires = x3::_val(ctx).wires;
			Verilog::PortCollection& ports = x3::_val(ctx).ports;
			for (auto newWire : newWires)
			{
				if (auto iter = ports.get<Verilog::name_index_tag>().find(newWire.name);
					iter != ports.get<Verilog::name_index_tag>().end())
				{
					continue; // Port has already been declared
				}

				if (auto iter = wires.get<Verilog::name_index_tag>().find(newWire.name);
					iter != wires.get<Verilog::name_index_tag>().end())
				{
					// Redeclaration of a wire is not allowed
					std::cerr << "The wire " << newWire.name << " was declared twice in the module "
							<< x3::_val(ctx).name << std::endl;
					x3::_pass(ctx) = false;
					return;
				}

				wires.push_back(newWire);
			}
		};
		auto add_assignment_statement = [](auto& ctx) {
			Verilog::Assignment& newAssignment = x3::_attr(ctx);
			assign_attributes(ctx, newAssignment);

			Verilog::AssignmentCollection& assignments = x3::_val(ctx).assignments;
			assignments.push_back(newAssignment);
		};
		auto add_module_instantiation = [](auto& ctx) {
			Verilog::Instantiation& moduleInstantiation = x3::_attr(ctx);
			assign_attributes(ctx, moduleInstantiation);

			Verilog::InstantiationCollection& instantiations = x3::_val(ctx).instantiations;
			instantiations.push_back(moduleInstantiation);
		};

		const module_type module_impl = "Module";
		const auto module_impl_def =
			// Module header with name
			x3::lit("module") > name_parser[ set_module_name ]
			// Optional parameter list
			> -parameter_list_impl[ set_parameter_list ]
			// Port list
			> port_list_impl[ set_port_list ] > ';'
			// Use the unused_attributes_tag to collect attributes
			// until the next valid statement is encountered
			> x3::with<unused_attributes_tag>(
				std::vector<Verilog::Attribute>()
			)[
				x3::eps[ reset_tag ]
				// Statements inside module
				> *(
					attribute() [ store_attribute ]
					| (port_statement() [ add_port_statement ] > ';')
					| (wire_statement() [ add_wire_statement ] > ';')
					| (assignment_statement() [ add_assignment_statement ] > ';')
					| (module_instantiation() [ add_module_instantiation ] > ';' )
				)
			]
			// Module footer
			> "endmodule";
		BOOST_SPIRIT_DEFINE(module_impl);

	}

	namespace Source {

		struct global_state_tag {};

		typedef struct global_state_s {

			std::vector<Verilog::Attribute> unusedAttributes;
			bool libraryModule;

			global_state_s():
				unusedAttributes(),
				libraryModule(false)
			{
			}

		} global_state;

		auto reset_tag = [](auto& ctx) {
			x3::get<global_state_tag>(ctx) = global_state();
		};
		auto store_attribute = [](auto& ctx) {
			global_state& state = x3::get<global_state_tag>(ctx);
			state.unusedAttributes.push_back(x3::_attr(ctx));
		};
		auto assign_attributes = [](auto& ctx, auto& target) {
			global_state& state = x3::get<global_state_tag>(ctx);

			target.attributes.insert(target.attributes.end(),
				state.unusedAttributes.begin(), state.unusedAttributes.end());
			state.unusedAttributes.clear();
		};

		auto add_module = [](auto& ctx) {
			global_state& state = x3::get<global_state_tag>(ctx);
			Verilog::Module& module = x3::_attr(ctx);

			module.libraryModule = state.libraryModule;
			assign_attributes(ctx, module);

			Verilog::ModuleCollection& modules = x3::_val(ctx);
			modules.push_back(module);
		};

		auto set_library_flag = [](auto& ctx) {
			global_state& state = x3::get<global_state_tag>(ctx);
			state.libraryModule = true;
		};
		auto unset_library_flag = [](auto& ctx) {
			global_state& state = x3::get<global_state_tag>(ctx);
			state.libraryModule = false;
		};

		const x3::rule<class celldefine_class> celldefine_impl = "Celldefine Start";
		const auto celldefine_impl_def = x3::lit("`celldefine");
		BOOST_SPIRIT_DEFINE(celldefine_impl);

		const x3::rule<class endcelldefine_class> endcelldefine_impl = "Celldefine End";
		const auto endcelldefine_impl_def = x3::lit("`endcelldefine");
		BOOST_SPIRIT_DEFINE(endcelldefine_impl);

		const source_type module_list_impl = "Module List";
		const auto module_list_impl_def =
			x3::with<global_state_tag>(
				global_state()
			)[
				x3::eps[ reset_tag ]
				> *(
					// Macros celldefine and endcelldefine
					celldefine_impl [ set_library_flag ]
					| endcelldefine_impl [ unset_library_flag ]
					// Verilog modules and relevant attributes
					| attribute() [ store_attribute ]
					| verilog_module() [ add_module ]
				)
			];
		BOOST_SPIRIT_DEFINE(module_list_impl);

	}

};
};
};
};

