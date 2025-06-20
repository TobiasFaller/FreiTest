#define BOOST_TEST_MODULE VerilogPreprocessor
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/config/warning_disable.hpp>
#include <boost/test/included/unit_test.hpp>

#include <streambuf>
#include <string>
#include <fstream>
#include <iostream>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Io/VerilogPreprocessor/VerilogPreprocessor.hpp"

using namespace std;
using namespace FreiTest::Io::Verilog;

int main(int argc, char* argv[], char* envp[])
{
  Logging::Initialize({ "--log-level=trace", "--log-verbose" });
  return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( VerilogPreprocessorTest )

BOOST_AUTO_TEST_CASE( TestCommentPreprocessing )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);

    VerilogPreprocessor preprocessor;

    std::vector<std::string> fileList = { "test/data/verilog/comments.v" };
    std::ostringstream outputStream;

    std::ifstream expected_file("test/data/verilog/comments_expected.v");
    BOOST_VERIFY(expected_file.good());

    std::string expected_str(
        (std::istreambuf_iterator<char>(expected_file)),
        std::istreambuf_iterator<char>()
    );

    BOOST_TEST(preprocessor.Transform(fileList, outputStream));
    BOOST_CHECK_EQUAL(outputStream.str(), expected_str);
}

BOOST_AUTO_TEST_CASE( TestIncludePreprocessing )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);

    VerilogPreprocessor preprocessor;

    std::vector<std::string> fileList = { "test/data/verilog/include.v" };
    std::ostringstream outputStream;

    std::ifstream expected_file("test/data/verilog/include_expected.v");
    BOOST_VERIFY(expected_file.good());

    std::string expected_str(
        (std::istreambuf_iterator<char>(expected_file)),
        std::istreambuf_iterator<char>()
    );

    BOOST_TEST(preprocessor.Transform(fileList, outputStream));
    BOOST_CHECK_EQUAL(outputStream.str(), expected_str);
}

BOOST_AUTO_TEST_SUITE_END()
