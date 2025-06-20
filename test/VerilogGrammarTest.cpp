#define BOOST_TEST_MODULE VerilogGrammar
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/config/warning_disable.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>
#include <sstream>

#include "Basic/Logging.hpp"
#include "Io/VerilogSpiritParser/VerilogGrammar.hpp"

using namespace std;
using namespace FreiTest::Io;

namespace x3 = boost::spirit::x3;

template <typename Result, typename Parser, typename Skipper>
bool test_parser(const std::string& input, const Parser& parser, const Skipper& skipper, Result& result, bool full_match = true)
{
    // we don't care about the result of the "what" function.
    // we only care that all parsers have it:
    x3::what(parser);
    x3::what(skipper);

    std::string::const_iterator begin = input.cbegin();
    std::string::const_iterator end = input.cend();

	try
	{
    	return x3::phrase_parse(begin, end, parser, skipper, result) && (!full_match || (begin == end));
	}
	catch(x3::expectation_failure<std::string::const_iterator>& exception)
	{
		std::ostringstream output;
		x3::error_handler<std::string::const_iterator> error_handler(begin, end, output);
		error_handler(exception.where(), "Parsing Verilog source failed. Expecting: " + std::string(exception.which()) + " here:");
		LOG(WARNING) << output.str();
		throw exception;
	}
}

template <typename Result, typename Parser>
bool test_parser(const std::string& input, const Parser& parser, Result& result, bool full_match = true)
{
    return test_parser(input, parser, x3::space, result, full_match);
}

int main(int argc, char* argv[], char* envp[])
{
  Logging::Initialize({ "--log-level=trace", "--log-verbose" });
  return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( VerilogGrammarTest )

BOOST_AUTO_TEST_CASE( TestCommentSkipper )
{
    using Verilog::Grammar::skipper;

    std::string result;

    BOOST_TEST(test_parser("Hello /* this */ is a test", *x3::char_, skipper(), result));
    BOOST_CHECK_EQUAL(result, "Helloisatest");
    result.clear();

    BOOST_TEST(test_parser("Hello // this is another\n test", *x3::char_, skipper(), result));
    BOOST_CHECK_EQUAL(result, "Hellotest");
    result.clear();
}

BOOST_AUTO_TEST_CASE( TestQuotedStringParser )
{
    using Verilog::Grammar::quoted_string;

    std::string result;

    BOOST_TEST(test_parser("\"Hello World\"", quoted_string(), result));
    BOOST_CHECK_EQUAL(result, "Hello World");
    result.clear();

    BOOST_TEST(test_parser("\"This is another test\"", quoted_string(), result));
    BOOST_CHECK_EQUAL(result, "This is another test");
    result.clear();
}

BOOST_AUTO_TEST_CASE( TestIdentifierParser )
{
    using Verilog::Grammar::identifier;

    std::string result;

    BOOST_TEST(test_parser("hello_world", identifier(), result));
    BOOST_CHECK_EQUAL(result, "hello_world");
    result.clear();

    BOOST_TEST(test_parser("\\this_is_a_test", identifier(), result));
    BOOST_CHECK_EQUAL(result, "this_is_a_test");
    result.clear();

    BOOST_TEST(!test_parser("Hello World", identifier(), result));
    BOOST_TEST(!test_parser("$invalid_identifier", identifier(), result));
}

BOOST_AUTO_TEST_CASE( TestVectorSize )
{
    using Verilog::Grammar::vector_size;

    Verilog::VectorSize result(1, 1);

    BOOST_TEST(test_parser("[5:2]", vector_size(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorSize(5, 2));

    BOOST_TEST(test_parser("[ 7 : 0 ]", vector_size(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorSize(7, 0));

    BOOST_TEST(!test_parser("[5,2]", vector_size(), result));
    BOOST_TEST(!test_parser("5:2", vector_size(), result));
    BOOST_TEST(!test_parser("10", vector_size(), result));
    BOOST_TEST(!test_parser("[10 5: 8]", vector_size(), result));
    BOOST_TEST(!test_parser("[1: 10 0]", vector_size(), result));
}

BOOST_AUTO_TEST_CASE( TestVectorInitializer )
{
    using Verilog::Grammar::vector_initializer;

    Verilog::VectorInitializer result(0u, 0u);

    BOOST_TEST(test_parser("8'h FF", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0xFFu));

    BOOST_TEST(test_parser("16'h AFF", vector_initializer(), result));
	LOG(INFO) << "Result: " << to_string(result);
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(16u, 0xAFFu));

    BOOST_TEST(test_parser("64'h 0000000000000000", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(64u, 0x0000000000000000u));

    BOOST_TEST(test_parser("64'h FFFFFFFFFFFFFFFF", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(64u, 0xFFFFFFFFFFFFFFFFu));

    BOOST_TEST(test_parser("0'h 0", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(0u, 0u));

    BOOST_TEST(test_parser("8'd 255", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0xFFu));

    BOOST_TEST(test_parser("8'b 10100101", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0xA5u));

    BOOST_TEST(test_parser("8'b 00000101", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0x05u));

    BOOST_TEST(test_parser("8'o 377", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0xFFu));

    // Check if clipping works
    BOOST_TEST(test_parser("8'b 1110100101", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0xA5u));

    BOOST_TEST(test_parser("8'd 256", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(8u, 0x00u));

    BOOST_TEST(test_parser("16'h DEADBEEF", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer(16u, 0xBEEF));

	// Check if DONT_CARE and UNKNOWN works
    BOOST_TEST(test_parser("32'h DEADXXXX", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer({
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,

		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,

		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,

		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,

		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_0,
		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_1,

		Verilog::VectorBit::BIT_0,
		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_0,
		Verilog::VectorBit::BIT_1,

		Verilog::VectorBit::BIT_0,
		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_1,

		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_0,
		Verilog::VectorBit::BIT_1,
		Verilog::VectorBit::BIT_1,
	}));

    BOOST_TEST(test_parser("8'b UUXXUXUX", vector_initializer(), result));
    BOOST_CHECK_EQUAL(result, Verilog::VectorInitializer({
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_U,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_U,

		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_X,
		Verilog::VectorBit::BIT_U,
		Verilog::VectorBit::BIT_U,
	}));
}

BOOST_AUTO_TEST_CASE( TestPortTypeParser )
{
    using Verilog::Grammar::port_type;

    Verilog::PortType result = Verilog::PortType::PORT_UNKNOWN;

    BOOST_TEST(test_parser("input", port_type(), result));
    BOOST_CHECK_EQUAL(result, Verilog::PortType::PORT_IN);

    BOOST_TEST(test_parser("output", port_type(), result));
    BOOST_CHECK_EQUAL(result, Verilog::PortType::PORT_OUT);

    BOOST_TEST(test_parser("inout", port_type(), result));
    BOOST_CHECK_EQUAL(result, Verilog::PortType::PORT_INOUT);
}

BOOST_AUTO_TEST_CASE( TestPortNameParser )
{
    using Verilog::Grammar::port_name;

    std::string result;

    BOOST_TEST(test_parser("hello_world", port_name(), result));
    BOOST_CHECK_EQUAL(result, "hello_world");
    result.clear();

    BOOST_TEST(test_parser("ThisIsAnIdentifier42", port_name(), result));
    BOOST_CHECK_EQUAL(result, "ThisIsAnIdentifier42");
    result.clear();

    BOOST_TEST(!test_parser("!This is not an identifier", port_name(), result));
    BOOST_TEST(!test_parser("this is also not a valid identifier", port_name(), result));
}

BOOST_AUTO_TEST_CASE( TestPortParser )
{
    using Verilog::Grammar::port_statement;

    std::vector<Verilog::Port> result;

    BOOST_TEST(test_parser("output dout0", port_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Port("dout0", Verilog::PortType::PORT_OUT, { 0, 0 }));

    result.clear();
    BOOST_TEST(test_parser("input clk", port_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Port("clk", Verilog::PortType::PORT_IN, { 0, 0 }));

    result.clear();
    BOOST_TEST(test_parser("output [31:0] data_out", port_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Port("data_out", Verilog::PortType::PORT_OUT, { 31, 0 }));

    result.clear();
    BOOST_TEST(test_parser("inout [31:0] some_bus", port_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Port("some_bus", Verilog::PortType::PORT_INOUT, { 31, 0 }));

    BOOST_TEST(!test_parser("whatever some_bus", port_statement(), result));
    BOOST_TEST(!test_parser("input hi_what's_up", port_statement(), result));

    // TODO: Test multiple ports in one definition
}

BOOST_AUTO_TEST_CASE( TestAttributeParser )
{
    using Verilog::Grammar::attribute;

    auto always_true = [](auto const& exception) -> bool { return true; };

    Verilog::Attribute result;

    BOOST_TEST(test_parser("(* hello = \"world\" *)", attribute(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Attribute("hello", "world"));

    result = Verilog::Attribute();
    BOOST_TEST(test_parser("(* src = \"./output/darkriscv-vcm-rv32e.sv:131.14-559.3\" *)", attribute(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Attribute("src", "./output/darkriscv-vcm-rv32e.sv:131.14-559.3"));

    result = Verilog::Attribute();
    BOOST_TEST(test_parser("(* init = 8'b 00110011 *)", attribute(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Attribute("init", Verilog::VectorInitializer(8u, 0x33u)));

    result = Verilog::Attribute();
    BOOST_TEST(test_parser("(* random_value = 10 *)", attribute(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Attribute("random_value", 10u));

    result = Verilog::Attribute();
    BOOST_TEST(test_parser("(* module_not_derived = 32'd1 *)", attribute(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Attribute("module_not_derived", Verilog::VectorInitializer(32u, 1u)));

    result = Verilog::Attribute();
    BOOST_CHECK_EXCEPTION(test_parser("(* random_value 10 *)", attribute(), result),
        x3::expectation_failure<std::string::const_iterator>,
        always_true
    );
    BOOST_CHECK_EXCEPTION(test_parser("(* \"test\" = 10 *)", attribute(), result),
        x3::expectation_failure<std::string::const_iterator>,
        always_true
    );
    BOOST_TEST(!test_parser("\"test\" = 10 *)", attribute(), result));
}

BOOST_AUTO_TEST_CASE( TestWireParser )
{
    using Verilog::Grammar::wire_statement;

    auto always_true = [](auto const& exception) -> bool { return true; };

    std::vector<Verilog::Wire> result;
    BOOST_TEST(test_parser("wire hello_world", wire_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Wire("hello_world", { 0, 0 }));

    result.clear();
    BOOST_TEST(test_parser("wire [5:1] standing_on_the_long_wire", wire_statement(), result));
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], Verilog::Wire("standing_on_the_long_wire", { 5, 1 }));

    BOOST_CHECK_EXCEPTION(test_parser("wire [5:-1] world", wire_statement(), result),
        x3::expectation_failure<std::string::const_iterator>,
        always_true
    );

    // TODO: Test mutliple wires in one definition
}

BOOST_AUTO_TEST_CASE( TestBusParser )
{
    using Verilog::Grammar::bus;

    Verilog::Bus result;

    BOOST_TEST(test_parser("test [5:0]", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({ Verilog::WireReference("test", {5, 0}) }));

    result = Verilog::Bus();
    BOOST_TEST(test_parser("write", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({ Verilog::WireReference("write", {0, 0}) }));

    result = Verilog::Bus();
    BOOST_TEST(test_parser("{test1 [5:0], test2}", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({
        Verilog::WireReference("test1", {5, 0}),
        Verilog::WireReference("test2", {0, 0})
    }));

    result = Verilog::Bus();
    BOOST_TEST(test_parser("{data_in [31:0], instr_in [31:0]}", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({
        Verilog::WireReference( "data_in", {31, 0}),
        Verilog::WireReference( "instr_in", {31, 0})
    }));

    result = Verilog::Bus();
    BOOST_TEST(test_parser("{{data_in [31:0]}, {instr_in [31:0]}}", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({
        Verilog::WireReference("data_in", {31, 0}),
        Verilog::WireReference("instr_in", {31, 0})
    }));

    result = Verilog::Bus();
    BOOST_TEST(test_parser("{{data_in [31:0], data_out [31:0]}, {instr_in[31:0],instr_out[15:0]}}", bus(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Bus({
        Verilog::WireReference("data_in", {31, 0}),
        Verilog::WireReference("data_out", {31, 0}),
        Verilog::WireReference("instr_in", {31, 0}),
        Verilog::WireReference("instr_out", {15, 0})
    }));
}

BOOST_AUTO_TEST_CASE( TestAssignmentParser )
{
    using Verilog::Grammar::assignment_statement;

    Verilog::Assignment result;

    BOOST_TEST(test_parser("assign test [5:0] = data_in [10]", assignment_statement(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Assignment(
        Verilog::Bus({ Verilog::WireReference("data_in", {10, 10}) }),
        Verilog::Bus({ Verilog::WireReference("test", {5, 0}) })
    ));

	result = Verilog::Assignment();
    BOOST_TEST(test_parser("assign data_out [5:0] = {data_in [10], data_in [5:1]}", assignment_statement(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Assignment(
        Verilog::Bus({
            Verilog::WireReference("data_in", {10, 10}),
            Verilog::WireReference("data_in", {5, 1})
        }),
        Verilog::Bus({
            Verilog::WireReference("data_out", {5, 0})
        })
    ));

    std::vector<Verilog::VectorBit> dontCares;
    for (size_t index { 0u }; index < 96u; index++) {
        dontCares.push_back(Verilog::VectorBit::BIT_X);
    }

    result = Verilog::Assignment();
    BOOST_TEST(test_parser("assign { \\vcm_instance.data_bus_address [127:96], \\vcm_instance.data_bus_address [63:0] } = 96'hxxxxxxxxxxxxxxxxxxxxxxxx", assignment_statement(), result));
    BOOST_CHECK_EQUAL(result, Verilog::Assignment(
        Verilog::Bus({
            Verilog::VectorInitializer(dontCares)
        }),
        Verilog::Bus({
            Verilog::WireReference("vcm_instance.data_bus_address", {127, 96}),
            Verilog::WireReference("vcm_instance.data_bus_address", {63, 0})
        })
    ));
}

BOOST_AUTO_TEST_CASE( TestModuleParser )
{
    using Verilog::Grammar::verilog_module;

    Verilog::Module result;
    BOOST_TEST(test_parser("module test_module();\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "test_module");
    BOOST_CHECK_EQUAL(result.ports.size(), 0);

    result = {};
    BOOST_TEST(test_parser("module antenna(input clk);\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "antenna");
    BOOST_CHECK_EQUAL(result.ports.size(), 1);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("clk", Verilog::PortType::PORT_IN, { 0, 0 }));

    result = {};
    BOOST_TEST(test_parser("module test(input1, output1);\ninput input1;\noutput output1;\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "test");
    BOOST_CHECK_EQUAL(result.ports.size(), 2);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("input1", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("output1", Verilog::PortType::PORT_OUT, { 0, 0 }));

    result = {};
    BOOST_TEST(test_parser("module test(input input1, output output1);\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "test");
    BOOST_CHECK_EQUAL(result.ports.size(), 2);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("input1", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("output1", Verilog::PortType::PORT_OUT, { 0, 0 }));

    result = {};
    BOOST_TEST(test_parser("module counter(input clk, output [7:0] leds);\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "counter");
    BOOST_CHECK_EQUAL(result.ports.size(), 2);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("clk", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("leds", Verilog::PortType::PORT_OUT, { 7, 0 }));

    result = {};
    BOOST_TEST(test_parser("module counter(clk, leds);\ninput clk;\noutput [7:0] leds;\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "counter");
    BOOST_CHECK_EQUAL(result.ports.size(), 2);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("clk", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("leds", Verilog::PortType::PORT_OUT, { 7, 0 }));

    result = {};
    BOOST_TEST(test_parser("module counter #(parameter N = 10)(input clk, output [7:0] leds);\nendmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "counter");
    BOOST_CHECK_EQUAL(result.ports.size(), 2);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("clk", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("leds", Verilog::PortType::PORT_OUT, { 7, 0 }));
    BOOST_CHECK_EQUAL(result.parameter.size(), 1);
    BOOST_CHECK_EQUAL(result.parameter[0], Verilog::Parameter("N", 10));

    result = {};
    BOOST_TEST(test_parser(
        "module full_adder(input bit1, input bit2, input carry_in, output sum_out, output carry_out);\n"
        "wire wire1;\n"
        "wire wire2;\n"
        "wire wire3;\n"
        "xor xor1(wire1, bit1, bit2);\n"
        "and and1(wire2, wire1, carry);\n"
        "and and2(wire3, bit1, bit2);\n"
        "xor xor2(sum_out, wire1, carry);\n"
        "or or1(carry_out, wire2, wire3);\n"
        "endmodule", verilog_module(), result));
    BOOST_CHECK_EQUAL(result.name, "full_adder");
    BOOST_CHECK_EQUAL(result.ports.size(), 5);
    BOOST_CHECK_EQUAL(result.ports[0], Verilog::Port("bit1", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[1], Verilog::Port("bit2", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[2], Verilog::Port("carry_in", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[3], Verilog::Port("sum_out", Verilog::PortType::PORT_OUT, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.ports[4], Verilog::Port("carry_out", Verilog::PortType::PORT_OUT, { 0, 0 }));
    BOOST_CHECK_EQUAL(result.parameter.size(), 0);
    BOOST_CHECK_EQUAL(result.wires.size(), 3);
    BOOST_CHECK_EQUAL(result.wires[0], Verilog::Wire("wire1", { 0, 0 }));
    BOOST_CHECK_EQUAL(result.wires[1], Verilog::Wire("wire2", { 0, 0 }));
    BOOST_CHECK_EQUAL(result.wires[2], Verilog::Wire("wire3", { 0, 0 }));
    BOOST_CHECK_EQUAL(result.instantiations.size(), 5);
    BOOST_CHECK_EQUAL(result.instantiations[0], Verilog::Instantiation("xor1", "xor", {}, {
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire1", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("bit1", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("bit2", 0) }))
    }));
    BOOST_CHECK_EQUAL(result.instantiations[1], Verilog::Instantiation("and1", "and", {}, {
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire2", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire1", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("carry", 0) }))
    }));
    BOOST_CHECK_EQUAL(result.instantiations[2], Verilog::Instantiation("and2", "and", {}, {
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire3", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("bit1", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("bit2", 0) }))
    }));
    BOOST_CHECK_EQUAL(result.instantiations[3], Verilog::Instantiation("xor2", "xor", {}, {
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("sum_out", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire1", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("carry", 0) }))
    }));
    BOOST_CHECK_EQUAL(result.instantiations[4], Verilog::Instantiation("or1", "or", {}, {
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("carry_out", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire2", 0) })),
        Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("wire3", 0) }))
    }));
}

BOOST_AUTO_TEST_CASE( TestVerilogParser )
{
    using Verilog::Grammar::verilog_source;

    Verilog::ModuleCollection result;
    BOOST_TEST(test_parser("module a();\nendmodule\n(* c = \"d\" *)\nmodule b();\nendmodule", verilog_source(), result));
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK(result.get<Verilog::name_index_tag>().find("a") != result.get<Verilog::name_index_tag>().end());
    BOOST_CHECK(result.get<Verilog::name_index_tag>().find("b") != result.get<Verilog::name_index_tag>().end());
}

BOOST_AUTO_TEST_SUITE_END()
