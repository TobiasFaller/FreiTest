#define BOOST_TEST_MODULE LuaInvoker
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>

#include "Basic/Logging.hpp"
#include "Helper/LuaInvoker.hpp"

using namespace FreiTest::Helper;

int main(int argc, char* argv[], char* envp[])
{
	Logging::Initialize({ "--log-level=trace" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( LuaInvokerTest )

BOOST_AUTO_TEST_CASE( InvokeEmptyFunction )
{
	auto lua = Lua();

	const std::string nopFunction = "function doNothing() end";
	lua.LoadCode(nopFunction);
	lua.Invoke<void>("doNothing");
}

BOOST_AUTO_TEST_CASE( InvokeWithSimpleArguments )
{
	auto lua = Lua();

	const std::string sumFunction = "function sumValues(value1, value2) return value1 + value2 end";
	BOOST_CHECK(lua.LoadCode(sumFunction));
	const auto sum = lua.Invoke<int>("sumValues", 1, 2);
	BOOST_CHECK(sum.has_value());
	BOOST_CHECK_EQUAL(3, sum.value());

	const std::string concatFunction = "function concatenateStrings(string1, string2) return string1 .. \" \" .. string2 end";
	BOOST_CHECK(lua.LoadCode(concatFunction));
	const auto outputString = lua.Invoke<std::string>("concatenateStrings", "Hello", "World");
	BOOST_CHECK(outputString.has_value());
	BOOST_CHECK_EQUAL("Hello World", outputString.value());
}

BOOST_AUTO_TEST_CASE( InvokeWithVector )
{
	auto lua = Lua();

	const std::vector<int> input { 1, 2, 3, 4, 5, 6 };

	const std::string sumFunction = "function sumValues(values) sum = 0; for _, value in pairs(values) do sum = sum + value end; return sum; end";
	BOOST_CHECK(lua.LoadCode(sumFunction));
	const auto sum = lua.Invoke<int>("sumValues", input);
	BOOST_CHECK(sum.has_value());
	BOOST_CHECK_EQUAL(21, sum.value());

	const std::string passThroughFunction = "function passThrough(values) return values; end";
	BOOST_CHECK(lua.LoadCode(passThroughFunction));
	const auto output = lua.Invoke<std::vector<int>>("passThrough", input);
	BOOST_CHECK(output.has_value());
	BOOST_CHECK_EQUAL(6, output.value().size());
	BOOST_CHECK_EQUAL(input[0], output.value()[0]);
	BOOST_CHECK_EQUAL(input[1], output.value()[1]);
	BOOST_CHECK_EQUAL(input[2], output.value()[2]);
	BOOST_CHECK_EQUAL(input[3], output.value()[3]);
	BOOST_CHECK_EQUAL(input[4], output.value()[4]);
	BOOST_CHECK_EQUAL(input[5], output.value()[5]);
}

BOOST_AUTO_TEST_CASE( InvokeWithMap )
{
	auto lua = Lua();

	const std::map<std::string, int> input {
		{ "red", 1 },
		{ "green", 2 },
		{ "blue", 3 }
	};

	const std::string modifyMapFunction = "function modifyMap(values) for key, value in pairs(values) do values[key] = value * 2 end; return values; end";
	BOOST_CHECK(lua.LoadCode(modifyMapFunction));
	const auto output = lua.Invoke<std::map<std::string, int>>("modifyMap", input);
	BOOST_CHECK(output.has_value());
	BOOST_CHECK_EQUAL(3, output.value().size());
	BOOST_CHECK(output.value().find("red") != output.value().end());
	BOOST_CHECK(output.value().find("green") != output.value().end());
	BOOST_CHECK(output.value().find("blue") != output.value().end());
	BOOST_CHECK_EQUAL(output.value().find("red")->second, 2);
	BOOST_CHECK_EQUAL(output.value().find("green")->second, 4);
	BOOST_CHECK_EQUAL(output.value().find("blue")->second, 6);
}

BOOST_AUTO_TEST_CASE( InvokeWithDifferentArgumentTypes )
{
	auto lua = Lua();

	const std::string concatFunction = "function concatInt(string, int) return string .. int end";
	BOOST_CHECK(lua.LoadCode(concatFunction));
	const auto result = lua.Invoke<std::string>("concatInt", "Four", 20);
	BOOST_CHECK(result.has_value());
	BOOST_CHECK_EQUAL("Four20", result.value());
}

BOOST_AUTO_TEST_CASE( InvokeCallback )
{
	std::function<std::string(void)> callback = []() -> std::string { return "Hello World"; };

	auto lua = Lua();
	lua.SetCallback<std::string>("greet", callback);

	const std::string callCallbackFunction = "function callCallback() return greet() end";
	BOOST_CHECK(lua.LoadCode(callCallbackFunction));
	const auto result = lua.Invoke<std::string>("callCallback");
	BOOST_CHECK(result.has_value());
	BOOST_CHECK_EQUAL("Hello World", result.value());
}

BOOST_AUTO_TEST_SUITE_END()
