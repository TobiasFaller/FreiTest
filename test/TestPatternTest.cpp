#define BOOST_TEST_MODULE TestPattern
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Basic/Settings.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitBuilder.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Pattern;

template<typename... InputT>
static TestPattern createTestPatternFromString(InputT... values)
{
	const std::vector<std::string> inputPatterns = { values... };

	const size_t timeframes = inputPatterns.size();
	const size_t noOfPIs = inputPatterns[0].find('/');
	const size_t noOfSIs = inputPatterns[0].size() - noOfPIs - 1;

	TestPattern newPattern(timeframes, noOfPIs, noOfSIs, Logic::LOGIC_DONT_CARE);
	for (size_t timeframe = 0u; timeframe < timeframes; ++timeframe)
	{
		for (size_t i = 0; i < noOfPIs; i++)
		{
			newPattern.SetPrimaryInput(timeframe, i, GetLogicForCharacter(inputPatterns[timeframe][i]));
		}

		for (size_t i = noOfPIs+1; i < noOfPIs+noOfSIs+1; i++)
		{
			newPattern.SetSecondaryInput(timeframe, i-noOfPIs-1, GetLogicForCharacter(inputPatterns[timeframe][i]));
		}
	}

	return newPattern;
}

template<typename... InputT>
static std::vector<TestPattern*> createPatternList(InputT... values)
{
	const std::vector<TestPattern*> inputPatterns = { values... };
	return inputPatterns;
}


int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace", "--log-verbose", "--log-verbose-modules=CircuitBuilder=9" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( TestPatternTest )


BOOST_AUTO_TEST_CASE( TestCompatibilityCheck_FullScan )
{
	TestPattern pattern1 = createTestPatternFromString("X110/XX11");
	TestPattern pattern2 = createTestPatternFromString("X11X/1X1X");
	TestPattern pattern3 = createTestPatternFromString("1111/0000");
	TestPattern pattern4 = createTestPatternFromString("1X1X/X0X0");
	TestPattern pattern5 = createTestPatternFromString("0000/1111");
	TestPattern pattern6 = createTestPatternFromString("0000/0111");

	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern2), true);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern3), false);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern4), false);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern5), false);

	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern3), false);
	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern4), true);
	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern5), false);

	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern4), true);
	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern5), false);

	BOOST_CHECK_EQUAL(pattern4.IsCompatible(pattern5), false);
	BOOST_CHECK_EQUAL(pattern4.IsCompatible(pattern6), false);

	BOOST_CHECK_EQUAL(pattern5.IsCompatible(pattern6), false);

	TestPattern pattern7  = createTestPatternFromString("101/0XX1X");
	TestPattern pattern8  = createTestPatternFromString("010/1111X");
	TestPattern pattern9  = createTestPatternFromString("XX1/XX0X0");
	TestPattern pattern10 = createTestPatternFromString("XXX/11XX0");
	TestPattern pattern11 = createTestPatternFromString("111/XXX1X");
	TestPattern pattern12 = createTestPatternFromString("01X/11XX1");

	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern8), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern9), true);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern10), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern9), false);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern10), true);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern12), true);

	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern10), true);
	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern11), true);
	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern11), true);
	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern11.IsCompatible(pattern12), false);
}


BOOST_AUTO_TEST_CASE( TestCompatibilityCheck_Sequential )
{
	TestPattern pattern1 = createTestPatternFromString("X11/XX1", "111/00X");
	TestPattern pattern2 = createTestPatternFromString("X11/10X", "X1X/X01");
	TestPattern pattern3 = createTestPatternFromString("01X/001", "X1X/0X0", "010/X11");
	TestPattern pattern4 = createTestPatternFromString("00X/0X1", "001/XX1", "X1X/000", "0X1/111");
	TestPattern pattern5 = createTestPatternFromString("001/X01");
	TestPattern pattern6 = createTestPatternFromString("XXX/101");

	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern2), true);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern3), false);
	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern1), true);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern4), false);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern5), false);
	BOOST_CHECK_EQUAL(pattern1.IsCompatible(pattern6), true);

	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern3), false);
	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern4), false);
	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern5), false);
	BOOST_CHECK_EQUAL(pattern2.IsCompatible(pattern6), true);

	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern4), false);
	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern5), false);
	BOOST_CHECK_EQUAL(pattern3.IsCompatible(pattern6), false);

	BOOST_CHECK_EQUAL(pattern4.IsCompatible(pattern5), true);
	BOOST_CHECK_EQUAL(pattern4.IsCompatible(pattern6), false);

	BOOST_CHECK_EQUAL(pattern5.IsCompatible(pattern6), true);

	TestPattern pattern7  = createTestPatternFromString("X1/X1", "XX/XX", "XX/11", "11/XX", "0X/10");
	TestPattern pattern8  = createTestPatternFromString("11/0X", "11/00", "01/XX", "1X/00", "01/10");
	TestPattern pattern9  = createTestPatternFromString("1X/01", "XX/00", "0X/01", "10/0X");
	TestPattern pattern10 = createTestPatternFromString("X1/01", "11/X0", "01/01");
	TestPattern pattern11 = createTestPatternFromString("00/11", "X1/00");
	TestPattern pattern12 = createTestPatternFromString("0X/1X");

	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern8), true);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern9), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern10), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern7.IsCompatible(pattern12), true);
	BOOST_CHECK_EQUAL(pattern12.IsCompatible(pattern7), false);

	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern9), true);
	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern8), false);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern10), true);
	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern8), false);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern8.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern10), true);
	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern9), false);
	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern9.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern11), false);
	BOOST_CHECK_EQUAL(pattern10.IsCompatible(pattern12), false);

	BOOST_CHECK_EQUAL(pattern11.IsCompatible(pattern12), true);
	BOOST_CHECK_EQUAL(pattern12.IsCompatible(pattern11), false);
}


BOOST_AUTO_TEST_CASE( TestCompactionOfTwoPatterns_FullScan )
{
	TestPattern pattern1 = createTestPatternFromString("XXXX/100XX");
	TestPattern pattern2 = createTestPatternFromString("XX10/10X0X");
	TestPattern pattern3 = createTestPatternFromString("1010/10X00");
	TestPattern pattern4 = createTestPatternFromString("11XX/10XXX");
	TestPattern pattern5 = createTestPatternFromString("1XX0/XX100");
	TestPattern pattern6 = createTestPatternFromString("1010/10X00");

	pattern1.Compact(pattern1);

	BOOST_CHECK_EQUAL(to_string(pattern1), "PI/SI: XXXX/100XX");

	pattern1.Compact(pattern2);	// pattern1 changes, pattern2 unchanged

	BOOST_CHECK_EQUAL(to_string(pattern1), "PI/SI: XX10/1000X");
	BOOST_CHECK_EQUAL(to_string(pattern2), "PI/SI: XX10/10X0X");

	pattern2.Compact(pattern3);	// pattern2 changes, pattern3 unchanged

	BOOST_CHECK_EQUAL(to_string(pattern2), "PI/SI: 1010/10X00");
	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: 1010/10X00");

	pattern3.Compact(pattern4);	// Not compatible, no change

	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: 1010/10X00");

	pattern3.Compact(pattern5);

	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: 1010/10100");

	pattern4.Compact(pattern5);

	BOOST_CHECK_EQUAL(to_string(pattern4), "PI/SI: 11X0/10100");

	pattern4.Compact(pattern6);	// Not compatible, no change

	BOOST_CHECK_EQUAL(to_string(pattern4), "PI/SI: 11X0/10100");

	pattern5.Compact(pattern6);

	BOOST_CHECK_EQUAL(to_string(pattern5), "PI/SI: 1010/10100");
}


BOOST_AUTO_TEST_CASE( TestCompactionOfTwoPatterns_Sequential )
{
	TestPattern pattern1 = createTestPatternFromString("XXX/100", "0XX/1X0", "110/XX1", "XXX/1XX");
	TestPattern pattern2 = createTestPatternFromString("X00/100", "01X/1X0", "110/X0X", "X1X/100");
	TestPattern pattern3 = createTestPatternFromString("XXX/100", "0XX/1X0", "110/XX1");
	TestPattern pattern4 = createTestPatternFromString("100/00X", "1XX/111");
	TestPattern pattern5 = createTestPatternFromString("10X/XX0", "011/1X0");
	TestPattern pattern6 = createTestPatternFromString("XXX/100");

	pattern1.Compact(pattern2);

	BOOST_CHECK_EQUAL(to_string(pattern1), "PI/SI: X00/100 -> 01X/1X0 -> 110/X01 -> X1X/100");

	pattern3.Compact(pattern2);	// Not compatible, because pattern3 has less timeframes than pattern2 => No change

	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: XXX/100 -> 0XX/1X0 -> 110/XX1");

	pattern2.Compact(pattern3);

	BOOST_CHECK_EQUAL(to_string(pattern2), "PI/SI: X00/100 -> 01X/1X0 -> 110/X01 -> X1X/100");

	pattern3.Compact(pattern4);	// Not compatible, no change

	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: XXX/100 -> 0XX/1X0 -> 110/XX1");

	pattern3.Compact(pattern5);

	BOOST_CHECK_EQUAL(to_string(pattern3), "PI/SI: 10X/100 -> 011/1X0 -> 110/XX1");

	pattern4.Compact(pattern5);	// Not compatible, no change

	BOOST_CHECK_EQUAL(to_string(pattern4), "PI/SI: 100/00X -> 1XX/111");

	pattern4.Compact(pattern6);	// Not compatible, no change

	BOOST_CHECK_EQUAL(to_string(pattern4), "PI/SI: 100/00X -> 1XX/111");

	pattern5.Compact(pattern6);

	BOOST_CHECK_EQUAL(to_string(pattern5), "PI/SI: 10X/100 -> 011/1X0");
}


BOOST_AUTO_TEST_SUITE_END()
