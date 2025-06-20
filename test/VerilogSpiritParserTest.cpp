#define BOOST_TEST_MODULE VerilogSpiritParser
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/config/warning_disable.hpp>
#include <boost/test/included/unit_test.hpp>

#include <memory>
#include <streambuf>
#include <string>
#include <fstream>
#include <iostream>

#include "Basic/Settings.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogSpiritParser/VerilogParser.hpp"
#include "Io/VerilogPreprocessor/VerilogPreprocessor.hpp"

using namespace std;
using namespace FreiTest::Io;

int main(int argc, char* argv[], char* envp[])
{
  Logging::Initialize({ "--log-level=trace", "--log-verbose" });
  return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

#define BOOST_CHECK_INSTANTATION(what, name, type, ...) \
  do { \
    Verilog::Instantiation instantation(name, type); \
    vector<Verilog::PortMapping> tmpPorts = { __VA_ARGS__ }; \
    BOOST_CHECK_EQUAL(what.ports.size(), tmpPorts.size()); \
    for (size_t i = 0; i < tmpPorts.size(); ++i) { \
      BOOST_CHECK_EQUAL(what.ports[i], tmpPorts[i]); \
    } \
  } while (0);

BOOST_AUTO_TEST_SUITE( VerilogSpiritParserTest )

BOOST_AUTO_TEST_CASE( TestWithBasicFile )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);

    vector<string> testFiles {
      "test/data/verilog/basic.v"
    };

    Verilog::VerilogPreprocessor preprocessor;
    ostringstream preprocessorOutput;
    BOOST_TEST(preprocessor.Transform(testFiles, preprocessorOutput));

    Verilog::VerilogParser parser;
    istringstream parserInput(preprocessorOutput.str());
    BOOST_CHECK_EQUAL(parser.Parse(parserInput), true);

    Verilog::ModuleCollection& modules = parser.GetModules();
    BOOST_CHECK_EQUAL(modules.size(), 1);

    const Verilog::Module& module = modules[0];
    BOOST_CHECK_EQUAL(module.name, "basic_test");
    BOOST_CHECK_EQUAL(module.ports.size(), 3);
    BOOST_CHECK_EQUAL(module.ports[0], Verilog::Port("A", Verilog::PortType::PORT_IN, { 7, 0 }));
    BOOST_CHECK_EQUAL(module.ports[1], Verilog::Port("B", Verilog::PortType::PORT_IN, { 7, 0 }));
    BOOST_CHECK_EQUAL(module.ports[2], Verilog::Port("OUT", Verilog::PortType::PORT_OUT, { 7, 0 }));
    BOOST_CHECK_EQUAL(module.instantiations.size(), 8);
    BOOST_CHECK_INSTANTATION(
      module.instantiations[0],
      "g1",
      "and",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 0, 0 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 0, 0 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 0, 0 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[1],
      "g2",
      "or",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 1, 1 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 1, 1 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 1, 1 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[2],
      "g3",
      "xor",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 2, 2 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 2, 2 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 2, 2 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[3],
      "g4",
      "nand",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 3, 3 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 3, 3 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 3, 3 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[4],
      "g5",
      "nor",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 4, 4 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 4, 4 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 4, 4 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[5],
      "g6",
      "buf",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 5, 5 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 5, 5 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[6],
      "g7",
      "not",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 6, 6 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("A", { 6, 6 }) }))
    );
    BOOST_CHECK_INSTANTATION(
      module.instantiations[7],
      "g8",
      "not",
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("OUT", { 7, 7 }) })),
      Verilog::PortMapping("", Verilog::Bus({ Verilog::WireReference("B", { 7, 7 }) }))
    );
}

BOOST_AUTO_TEST_CASE( TestWithFlipFlop )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);

    vector<string> testFiles {
      "test/data/verilog/flipflop.v"
    };

    Verilog::VerilogPreprocessor preprocessor;
    ostringstream preprocessorOutput;
    BOOST_TEST(preprocessor.Transform(testFiles, preprocessorOutput));

    Verilog::VerilogParser parser;
    istringstream parserInput(preprocessorOutput.str());
    BOOST_CHECK_EQUAL(parser.Parse(parserInput), true);

    Verilog::ModuleCollection& modules = parser.GetModules();
    BOOST_CHECK_EQUAL(modules.size(), 1);

    const Verilog::Module& module = modules[0];
    BOOST_CHECK_EQUAL(module.name, "flip_flop");
    BOOST_CHECK_EQUAL(module.ports.size(), 3);
    BOOST_CHECK_EQUAL(module.ports[0], Verilog::Port("clock", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(module.ports[1], Verilog::Port("data_in", Verilog::PortType::PORT_IN, { 0, 0 }));
    BOOST_CHECK_EQUAL(module.ports[2], Verilog::Port("data_out", Verilog::PortType::PORT_OUT, { 0, 0 }));
    BOOST_CHECK_EQUAL(module.instantiations.size(), 1);
    BOOST_CHECK_INSTANTATION(
      module.instantiations[0],
      "",
      "dflipflop",
      Verilog::PortMapping("CK", Verilog::Bus({ Verilog::WireReference("clock", { 0, 0 }) })),
      Verilog::PortMapping("D", Verilog::Bus({ Verilog::WireReference("data_in", { 0, 0 }) })),
      Verilog::PortMapping("Q", Verilog::Bus({ Verilog::WireReference("data_out", { 0, 0 }) }))
    );
}

BOOST_AUTO_TEST_SUITE_END()
