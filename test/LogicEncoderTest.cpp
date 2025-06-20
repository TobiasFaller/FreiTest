#define BOOST_TEST_MODULE LogicEncoder
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/config/warning_disable.hpp>
#include <boost/test/included/unit_test.hpp>

#include <streambuf>
#include <string>
#include <fstream>
#include <iostream>

#include "Basic/Logic.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Logging.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/ISolverProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace SolverProxy;
using namespace SolverProxy::Sat;

int main(int argc, char* argv[], char* envp[])
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);

    Logging::Initialize({});
    // Logging::Initialize({ "--log-level=trace", "--log-verbose", "--log-debug=extended", "--log-verbose-modules=SatDebugProxy=6" });
    return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

struct Fixture {

    Fixture():
        solver(SatSolverProxy::CreateSatSolver(SatSolver::PROD_SAT_SINGLE_GLUCOSE_421)),
        generator(std::dynamic_pointer_cast<SolverProxy::ISolverProxy>(solver))
    {
    }

    ~Fixture() = default;

	template<typename LogicContainer>
	void ExcludeCurrentSolution(const std::vector<LogicContainer>& containers)
	{
		solver->NewClause();
		for (auto& container : containers)
		{
			if constexpr (is_01_encoding_v<LogicContainer>)
			{
				solver->AddLiteral((solver->GetLiteralValue(container.l0) == Value::Positive) ? -container.l0 : container.l0);
			}
			else if constexpr (is_01X_encoding_v<LogicContainer> || is_U01X_encoding_v<LogicContainer>
				|| is_01F_encoding_v<LogicContainer> || is_U01F_encoding_v<LogicContainer>)
			{
				solver->AddLiteral((solver->GetLiteralValue(container.l0) == Value::Positive) ? -container.l0 : container.l0);
				solver->AddLiteral((solver->GetLiteralValue(container.l1) == Value::Positive) ? -container.l1 : container.l1);
			}
			else
			{
				Logging::Panic("Can not exclude current solution! (Not implemented)");
			}
		}
		solver->CommitClause();
	}

    std::shared_ptr<SatSolverProxy> solver;
    FreiTest::Tpg::LogicEncoder generator;

};

// TODO: Remove all these macros and replace with generic lambdas when C++ 20 support is enabled!
#define FOR_EACH_LOGIC_VALUE_1D(LogicContainer, macro, function, expected, ...) \
	do { \
		const auto values = get_logic_container_values<LogicContainer>; \
		assert(strlen(expected) == values.size()); \
		for (size_t i { 0u }; i < values.size(); i++) { \
			solver->Reset(); \
			auto container { generator.NewLogicContainer<LogicContainer>(values[i]) }; \
			macro(LogicContainer, values[i], container, function, expected[i], ##__VA_ARGS__); \
		} \
	} while(false)

#define FOR_EACH_LOGIC_VALUE_2D(LogicContainer, macro, function, expected, ...) \
	do { \
		const auto values = get_logic_container_values<LogicContainer>; \
		assert(strlen(expected) == values.size() * values.size()); \
		for (size_t i { 0u }; i < values.size(); i++) { \
			for (size_t j { 0u }; j < values.size(); j++) { \
				solver->Reset(); \
				auto container1 { generator.NewLogicContainer<LogicContainer>(values[i]) }; \
				auto container2 { generator.NewLogicContainer<LogicContainer>(values[j]) }; \
				macro(LogicContainer, values[i], values[j], container1, container2, function, expected[i * values.size() + j], ##__VA_ARGS__); \
			} \
		} \
	} while(false)

#define FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainer, macro, function, expected, ...) \
	do { \
		const auto values = get_logic_container_values<LogicContainer>; \
		const std::string constraints = "*=!KL01XUNV"; \
		assert(strlen(expected) == values.size() * constraints.size()); \
		for (size_t i { 0u }; i < values.size(); i++) { \
			for (size_t j { 0u }; j < constraints.size(); j++) { \
				solver->Reset(); \
				auto container1 { generator.NewLogicContainer<LogicContainer>(values[i]) }; \
				auto constraint1 { GetLogicConstraintForCharacter(constraints[j]) }; \
				macro(LogicContainer, values[i], GetLogicConstraintForCharacter(constraints[j]), container1, constraint1, function, expected[i * constraints.size() + j], ##__VA_ARGS__); \
			} \
		} \
	} while(false)

#define FOR_EACH_LOGIC_VALUE_3D(LogicContainer, macro, function, expected, ...) \
	do { \
		const auto values = get_logic_container_values<LogicContainer>; \
		assert(strlen(expected) == (values.size() * values.size() * values.size())); \
		for (size_t i { 0u }; i < values.size(); i++) { \
			for (size_t j { 0u }; j < values.size(); j++) { \
				for (size_t k { 0u }; k < values.size(); k++) { \
					solver->Reset(); \
					auto container1 { generator.NewLogicContainer<LogicContainer>(values[i]) }; \
					auto container2 { generator.NewLogicContainer<LogicContainer>(values[j]) }; \
					auto container3 { generator.NewLogicContainer<LogicContainer>(values[k]) }; \
					macro(LogicContainer, values[i], values[j], values[k], container1, container2, container3, function, expected[(i * values.size() + j) * values.size() + k], ##__VA_ARGS__); \
				} \
			} \
		} \
	} while(false)

#define LOGIC_TEST_ENCODE_DECODE(LogicContainer, value, container, function, expected) \
	BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(container), GetLogicForCharacter(expected));

#define LOGIC_TEST_REQUIREMENT(LogicContainer, value, container, function, expected) \
	container = generator.NewLogicContainer<decltype(container)>(); \
	generator.EncodeLogicValue(container, value); \
	BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
	BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(container), GetLogicForCharacter(expected)); \
	ExcludeCurrentSolution(std::vector<decltype(container)> { container }); \
	BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve());

#define LOGIC_TEST_VALUE_DETECTOR(LogicContainer, value1, value2, container1, container2, function, expected) \
	auto out = generator.function(container1, value2); \
	BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
	\
	LOG(INFO) << "Result of detector method " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and inputs " << to_char(value1) << " and target " << to_char(value2) \
		<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(out), GetLogicForCharacter(expected)); \
	\
	ExcludeCurrentSolution(std::vector<decltype(out)> { out }); \
	BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve());

#define LOGIC_TEST_LOGIC_DETECTOR(LogicContainer, value1, value2, container1, container2, function, expected) \
	auto out = generator.function(container1, container2); \
	BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
	\
	LOG(INFO) << "Result of detector method " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and inputs " << to_char(value1) << " and target " << to_char(value2) \
		<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(out), GetLogicForCharacter(expected)); \
	\
	ExcludeCurrentSolution(std::vector<decltype(out)> { out }); \
	BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve());

#define LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC(LogicContainer, value, container, function, expected, ...) \
	{ LOGIC_TEST_GATE_1D_STATIC(LogicContainer, value, container, function, expected, ##__VA_ARGS__) } \
	{ LOGIC_TEST_GATE_1D_DYNAMIC(LogicContainer, value, container, function, expected, ##__VA_ARGS__) }

#define LOGIC_TEST_GATE_1D_STATIC(LogicContainer, value, container, function, expected) \
	auto out = generator.function(container); \
	LOG(INFO) << "Result of constant " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and input " << to_char(value) \
		<< " is " << to_char(generator.GetConstantLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetConstantLogicValue(out), GetLogicForCharacter(expected));

#define LOGIC_TEST_VALIDATE(LogicContainer, inputs, container, function, expected) \
	BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
	\
	if constexpr (is_classical_encoding_v<LogicContainer>) \
	{ \
		LOG(INFO) << "Result of non-trivial " << #function \
			<< " with logic container " << #LogicContainer \
			<< " and " << inputs \
			<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
			<< " expected " << expected; \
		BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(out), GetLogicForCharacter(expected)); \
		ExcludeCurrentSolution(std::vector<LogicContainer> { out }); \
		BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve()); \
	} \
	else if constexpr (is_forgetful_encoding_v<LogicContainer>) \
	{ \
		bool allowsForgetting = (expected == '0' || expected == '1'); \
		LOG(INFO) << "Result of non-trivial " << #function \
			<< " with logic container " << #LogicContainer \
			<< " and " << inputs \
			<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
			<< " expected (" << expected << (allowsForgetting ? " or X)" : ")"); \
		BOOST_CHECK(generator.GetSolvedLogicValue(out) == GetLogicForCharacter(expected) \
			|| (allowsForgetting && generator.GetSolvedLogicValue(out) == Logic::LOGIC_DONT_CARE)); \
		ExcludeCurrentSolution(std::vector<LogicContainer> { out }); \
		\
		if (allowsForgetting) \
		{ \
			BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
			LOG(INFO) << "Result of non-trivial " << #function \
				<< " with logic container " << #LogicContainer \
				<< " and " << inputs \
				<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
				<< " expected (" << expected << (allowsForgetting ? " or X)" : ")"); \
			BOOST_CHECK(generator.GetSolvedLogicValue(out) == GetLogicForCharacter(expected) \
				|| generator.GetSolvedLogicValue(out) == Logic::LOGIC_DONT_CARE); \
			ExcludeCurrentSolution(std::vector<LogicContainer> { out }); \
		} \
		BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve()); \
	}

#define LOGIC_TEST_GATE_1D_DYNAMIC(LogicContainer, value, container, function, expected) \
	auto out = generator.NewLogicContainer<decltype(generator.function(container))>(); \
	LOG(INFO) << to_string(out); \
	generator.function(container, out); \
	LOGIC_TEST_VALIDATE(LogicContainer, (std::string("input ") + to_char(value)), container, function, expected)

#define LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC(LogicContainer, value1, value2, container1, container2, function, expected, ...) \
	{ LOGIC_TEST_GATE_2D_STATIC(LogicContainer, value1, value2, container1, container2, function, expected, ##__VA_ARGS__) } \
	{ LOGIC_TEST_GATE_2D_DYNAMIC(LogicContainer, value1, value2, container1, container2, function, expected, ##__VA_ARGS__) }

#define LOGIC_TEST_GATE_2D_STATIC(LogicContainer, value1, value2, container1, container2, function, expected) \
	auto out = generator.function(container1, container2); \
	LOG(INFO) << "Result of constant " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and inputs " << to_char(value1) << ", " << to_char(value2) \
		<< " is " << to_char(generator.GetConstantLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetConstantLogicValue(out), GetLogicForCharacter(expected));

#define LOGIC_TEST_GATE_2D_DYNAMIC(LogicContainer, value1, value2, container1, container2, function, expected) \
	auto out = generator.NewLogicContainer<decltype(generator.function(container1, container2))>(); \
	generator.function(container1, container2, out); \
	LOGIC_TEST_VALIDATE(LogicContainer, (std::string("inputs ") + to_char(value1) + ", " + to_char(value2)), container, function, expected)

#define LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT(LogicContainer, value, container, function, expected, ...) \
	{ LOGIC_TEST_GATE_2D_STATIC_LEFT(LogicContainer, value, container, function, expected, ##__VA_ARGS__) } \
	{ LOGIC_TEST_GATE_2D_STATIC_RIGHT(LogicContainer, value, container, function, expected, ##__VA_ARGS__) }

// Test if the trivial simplifications work.
// An expected result of ' ' means to not test this case as no assumptions
// about the result can be made. These cases are already tested by the other
// exhaustive tests.
// An expected result of '+' has the meaning of "the same as the other input",
// where '-' is "the inverse of the other input".
#define LOGIC_TEST_GATE_2D_STATIC_LEFT(LogicContainer, value, container, function, expected) \
	if (expected != ' ') { \
		auto dynamic = generator.NewLogicContainer<LogicContainer>(); \
		auto out = generator.function(container, dynamic); \
		char result = to_char(generator.GetConstantLogicValue(out)); \
		if ((expected == '+' && out == dynamic) || (expected == '-' && out == -dynamic)) result = expected; \
		LOG(INFO) << "Result of partial static " << #function \
			<< " with logic container " << #LogicContainer \
			<< " and input " << to_char(value) \
			<< " is " << result << " expected " << expected; \
		BOOST_CHECK_EQUAL(result, expected); \
	}

#define LOGIC_TEST_GATE_2D_STATIC_RIGHT(LogicContainer, value, container, function, expected) \
	if (expected != ' ') { \
		auto dynamic = generator.NewLogicContainer<LogicContainer>(); \
		auto out = generator.function(dynamic, container); \
		char result = to_char(generator.GetConstantLogicValue(out)); \
		if ((expected == '+' && out == dynamic) || (expected == '-' && out == -dynamic)) result = expected; \
		LOG(INFO) << "Result of partial static " << #function \
			<< " with logic container " << #LogicContainer \
			<< " and input " << to_char(value) \
			<< " is " << result << " expected " << expected; \
		BOOST_CHECK_EQUAL(result, expected); \
	}

#define LOGIC_TEST_DETECTOR_2D_DYNAMIC(LogicContainer, value1, value2, container1, constraint1, function, expected) \
	auto out = generator.NewLogicContainer<decltype(generator.function(container1, constraint1))>(); \
	generator.function(container1, constraint1, out); \
	BOOST_CHECK_EQUAL(SatResult::SAT, solver->Solve()); \
	\
	LOG(INFO) << "Result of non-trivial " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and input " << to_char(value1) << ", constraint " << to_char(value2) \
		<< " is " << to_char(generator.GetSolvedLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetSolvedLogicValue(out), GetLogicForCharacter(expected)); \
	ExcludeCurrentSolution(std::vector<LogicContainer01> { out }); \
	BOOST_CHECK_EQUAL(SatResult::UNSAT, solver->Solve());

#define LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC(LogicContainer, value1, value2, value3, container1, container2, container3, function, expected, ...) \
	{ LOGIC_TEST_GATE_3D_STATIC(LogicContainer, value1, value2, value3, container1, container2, container3, function, expected, ##__VA_ARGS__) } \
	{ LOGIC_TEST_GATE_3D_DYNAMIC(LogicContainer, value1, value2, value3, container1, container2, container3, function, expected, ##__VA_ARGS__) }

#define LOGIC_TEST_GATE_3D_STATIC(LogicContainer, value1, value2, value3, container1, container2, container3, function, expected) \
	auto out = generator.function(container1, container2, container3); \
	LOG(INFO) << "Result of constant " << #function \
		<< " with logic container " << #LogicContainer \
		<< " and inputs " << to_char(value1) << ", " << to_char(value2) << ", " << to_char(value3) \
		<< " is " << to_char(generator.GetConstantLogicValue(out)) \
		<< " expected " << expected; \
	BOOST_CHECK_EQUAL(generator.GetConstantLogicValue(out), GetLogicForCharacter(expected));

#define LOGIC_TEST_GATE_3D_DYNAMIC(LogicContainer, value1, value2, value3, container1, container2, container3, function, expected) \
	auto out = generator.NewLogicContainer<decltype(generator.function(container1, container2, container3))>(); \
	generator.function(container1, container2, container3, out); \
	LOGIC_TEST_VALIDATE(LogicContainer, (std::string("inputs ") + to_char(value1) + ", " + to_char(value2) + ", " + to_char(value3)), container, function, expected)

// Test if the trivial simplifications work.
// An expected result of ' ' means to not test this case as no assumptions
// about the result can be made. These cases are already tested by the other
// exhaustive tests.
// An expected result of '+' has the meaning of "the same as the left input",
// where '-' is "the inverse of the left input".
// An expected result of '%' has the meaning of "the same as the middle input",
// where '^' is "the inverse of the middle input".
#define LOGIC_TEST_GATE_3D_STATIC_RIGHT(LogicContainer, value, container, function, expected) \
	if (expected != ' ') { \
		auto dynamic1 = generator.NewLogicContainer<LogicContainer>(); \
		auto dynamic2 = generator.NewLogicContainer<LogicContainer>(); \
		auto out = generator.function(dynamic1, dynamic2, container); \
		char result = to_char(generator.GetConstantLogicValue(out)); \
		if ((expected == '+' && out == dynamic1) || (expected == '-' && out == -dynamic1) \
				|| (expected == '%' && out == dynamic2) || (expected == '^' && out == -dynamic2)) \
			result = expected; \
		LOG(INFO) << "Result of partial static " << #function \
			<< " with logic container " << #LogicContainer \
			<< " and input " << to_char(value) \
			<< " is " << result << " expected " << expected; \
		BOOST_CHECK_EQUAL(result, expected); \
	}

BOOST_FIXTURE_TEST_SUITE( LogicEncoderTest, Fixture )

BOOST_AUTO_TEST_CASE( TestConstantEncodeDecode )
{
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_ENCODE_DECODE, NewLogicContainer, "01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_ENCODE_DECODE, NewLogicContainer, "X01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_ENCODE_DECODE, NewLogicContainer, "U01X");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_ENCODE_DECODE, NewLogicContainer, "X01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_ENCODE_DECODE, NewLogicContainer, "U01X");
}

BOOST_AUTO_TEST_CASE( TestRequirements )
{
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_REQUIREMENT, EncodeLogicValue, "01");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_REQUIREMENT, EncodeLogicValue, "X01");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_REQUIREMENT, EncodeLogicValue, "U01X");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_REQUIREMENT, EncodeLogicValue, "X01");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_REQUIREMENT, EncodeLogicValue, "U01X");
}

BOOST_AUTO_TEST_CASE( TestLogicValueDetector )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_VALUE_DETECTOR, EncodeLogicValueDetector, "1001");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_VALUE_DETECTOR, EncodeLogicValueDetector, "100010001");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_VALUE_DETECTOR, EncodeLogicValueDetector, "1000010000100001");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_VALUE_DETECTOR, EncodeLogicValueDetector, "100010001");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_VALUE_DETECTOR, EncodeLogicValueDetector, "1000010000100001");
}

BOOST_AUTO_TEST_CASE( TestLogicEquivalenceDetector )
{
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogicEquivalenceDetector, "1001");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogicEquivalenceDetector, "100010001");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogicEquivalenceDetector, "1000010000100001");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogicEquivalenceDetector, "100010001");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogicEquivalenceDetector, "1000010000100001");
}

BOOST_AUTO_TEST_CASE( TestLogic01DifferenceDetector )
{
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogic01DifferenceDetector, "0110");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogic01DifferenceDetector, "000001010");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogic01DifferenceDetector, "0000001001000000");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogic01DifferenceDetector, "000001010");
    FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_LOGIC_DETECTOR, EncodeLogic01DifferenceDetector, "0000001001000000");
}

BOOST_AUTO_TEST_CASE( TestBuffer )
{
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeBuffer, "01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeBuffer, "X01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeBuffer, "U01X");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeBuffer, "X01");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeBuffer, "U01X");
}

BOOST_AUTO_TEST_CASE( TestInverter )
{
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeInverter, "10");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeInverter, "X10");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeInverter, "U10X");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeInverter, "X10");
    FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_1D_STATIC_AND_DYNAMIC, EncodeInverter, "U10X");
}

BOOST_AUTO_TEST_CASE( TestLogicValueConstraintDetector )
{
	// * NO_CONSTRAINT
	// = NOT_LOGIC_ZERO
	// ! NOT_LOGIC_ONE
	// K NOT_LOGIC_DONT_CARE
	// L NOT_LOGIC_UNKNOWN
	// 0 ONLY_LOGIC_ZERO
	// 1 ONLY_LOGIC_ONE
	// X ONLY_LOGIC_DONT_CARE
	// U ONLY_LOGIC_UNKNOWN
	// N ONLY_LOGIC_01
	// V ONLY_LOGIC_01X

    FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainer01, LOGIC_TEST_DETECTOR_2D_DYNAMIC, EncodeLogicValueConstraintDetector,
        /*   constraint      */
		/*       *=!KL01XUNV */
        /* in1 ------------- */
        /* 0 */ "10111100011"
        /* 1 */ "11011010011");
    FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainer01X, LOGIC_TEST_DETECTOR_2D_DYNAMIC, EncodeLogicValueConstraintDetector,
        /*   constraint      */
		/*       *=!KL01XUNV */
        /* in1 ------------- */
        /* X */ "11101001001"
        /* 0 */ "10111100011"
        /* 1 */ "11011010011");
    FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainerU01X, LOGIC_TEST_DETECTOR_2D_DYNAMIC, EncodeLogicValueConstraintDetector,
        /*   constraint      */
		/*       *=!KL01XUNV */
        /* in1 ------------- */
        /* U */ "11110000100"
        /* 0 */ "10111100011"
        /* 1 */ "11011010011"
        /* X */ "11101001001");
    FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainer01F, LOGIC_TEST_DETECTOR_2D_DYNAMIC, EncodeLogicValueConstraintDetector,
        /*   constraint      */
		/*       *=!KL01XUNV */
        /* in1 ------------- */
        /* X */ "11101001001"
        /* 0 */ "10111100011"
        /* 1 */ "11011010011");
    FOR_EACH_LOGIC_VALUE_LOGIC_CONSTRAINT(LogicContainerU01F, LOGIC_TEST_DETECTOR_2D_DYNAMIC, EncodeLogicValueConstraintDetector,
        /*   constraint      */
		/*       *=!KL01XUNV */
        /* in1 ------------- */
        /* U */ "11110000100"
        /* 0 */ "10111100011"
        /* 1 */ "11011010011"
        /* X */ "11101001001");
}

BOOST_AUTO_TEST_CASE( TestXorGate )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXor,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "01"
        /* 1 */ "10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XXX"
        /* 0 */ "X01"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UUUU"
        /* 0 */ "U01X"
        /* 1 */ "U10X"
        /* X */ "UXXX");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XXX"
        /* 0 */ "X01"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UUUU"
        /* 0 */ "U01X"
        /* 1 */ "U10X"
        /* X */ "UXXX");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXor, "+-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXor, " +-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXor, " +- ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXor, " +-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXor, " +- ");
}

BOOST_AUTO_TEST_CASE( TestAndGate )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeAnd,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "00"
        /* 1 */ "01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeAnd,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "X0X"
        /* 0 */ "000"
        /* 1 */ "X01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeAnd,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "U0UU"
        /* 0 */ "0000"
        /* 1 */ "U01X"
        /* X */ "U0XX");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeAnd,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "X0X"
        /* 0 */ "000"
        /* 1 */ "X01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeAnd,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "U0UU"
        /* 0 */ "0000"
        /* 1 */ "U01X"
        /* X */ "U0XX");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeAnd, "0+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeAnd, " 0+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeAnd, " 0+ ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeAnd, " 0+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeAnd, " 0+ ");
}

BOOST_AUTO_TEST_CASE( TestOrGate )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeOr,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "01"
        /* 1 */ "11");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeOr,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XX1"
        /* 0 */ "X01"
        /* 1 */ "111");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeOr,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UU1U"
        /* 0 */ "U01X"
        /* 1 */ "1111"
        /* X */ "UX1X");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeOr,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XX1"
        /* 0 */ "X01"
        /* 1 */ "111");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeOr,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UU1U"
        /* 0 */ "U01X"
        /* 1 */ "1111"
        /* X */ "UX1X");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeOr, "+1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeOr, " +1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeOr, " +1 ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeOr, " +1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeOr, " +1 ");
}

BOOST_AUTO_TEST_CASE( TestXnorGate )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXnor,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "10"
        /* 1 */ "01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXnor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XXX"
        /* 0 */ "X10"
        /* 1 */ "X01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXnor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UUUU"
        /* 0 */ "U10X"
        /* 1 */ "U01X"
        /* X */ "UXXX");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXnor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XXX"
        /* 0 */ "X10"
        /* 1 */ "X01");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeXnor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UUUU"
        /* 0 */ "U10X"
        /* 1 */ "U01X"
        /* X */ "UXXX");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXnor, "-+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXnor, " -+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXnor, " -+ ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXnor, " -+");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeXnor, " -+ ");
}

BOOST_AUTO_TEST_CASE( TestNand )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNand,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "11"
        /* 1 */ "10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNand,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "X1X"
        /* 0 */ "111"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNand,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "U1UU"
        /* 0 */ "1111"
        /* 1 */ "U10X"
        /* X */ "U1XX");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNand,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "X1X"
        /* 0 */ "111"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNand,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "U1UU"
        /* 0 */ "1111"
        /* 1 */ "U10X"
        /* X */ "U1XX");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNand, "1-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNand, " 1-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNand, " 1- ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNand, " 1-");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNand, " 1- ");
}

BOOST_AUTO_TEST_CASE( TestNor )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNor,
        /*   in2 01  */
        /* in1 ----- */
        /* 0 */ "10"
        /* 1 */ "00");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XX0"
        /* 0 */ "X10"
        /* 1 */ "000");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UU0U"
        /* 0 */ "U10X"
        /* 1 */ "0000"
        /* X */ "UX0X");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNor,
        /*   in2 X01   */
        /* in1 ----- */
        /* X */ "XX0"
        /* 0 */ "X10"
        /* 1 */ "000");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeNor,
        /*   in2 U01X  */
        /* in1 ------- */
        /* U */ "UU0U"
        /* 0 */ "U10X"
        /* 1 */ "0000"
        /* X */ "UX0X");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNor, "-0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNor, " -0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNor, " -0 ");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNor, " -0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_LEFT_AND_RIGHT, EncodeNor, " -0 ");
}

BOOST_AUTO_TEST_CASE( TestSetControl )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeSetControl,
        /*   set 01  */
        /* in ------ */
        /* 0 */ "01"
        /* 1 */ "11");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeSetControl,
        /*   set X01   */
        /* in ------ */
        /* X */ "XX1"
        /* 0 */ "X01"
        /* 1 */ "X11");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeSetControl,
        /*   set U01X  */
        /* in -------- */
        /* U */ "UU1X"
        /* 0 */ "U01X"
        /* 1 */ "U11X"
        /* X */ "UX1X");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeSetControl,
        /*   set X01   */
        /* in ------ */
        /* X */ "XX1"
        /* 0 */ "X01"
        /* 1 */ "X11");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeSetControl,
        /*   set U01X  */
        /* in -------- */
        /* U */ "UU1X"
        /* 0 */ "U01X"
        /* 1 */ "U11X"
        /* X */ "UX1X");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeSetControl, "+1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeSetControl, "X+1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeSetControl, "U+1X");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeSetControl, "X+1");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeSetControl, "U+1X");
}

BOOST_AUTO_TEST_CASE( TestResetControl )
{
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeResetControl,
        /*   set 01  */
        /* in ------ */
        /* 0 */ "00"
        /* 1 */ "10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeResetControl,
        /*   set X01   */
        /* in ------ */
        /* X */ "XX0"
        /* 0 */ "X00"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeResetControl,
        /*   set U01X  */
        /* in -------- */
        /* U */ "UU0X"
        /* 0 */ "U00X"
        /* 1 */ "U10X"
        /* X */ "UX0X");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeResetControl,
        /*   set X01   */
        /* in ------ */
        /* X */ "XX0"
        /* 0 */ "X00"
        /* 1 */ "X10");
	FOR_EACH_LOGIC_VALUE_2D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_AND_DYNAMIC, EncodeResetControl,
        /*   set U01X  */
        /* in -------- */
        /* U */ "UU0X"
        /* 0 */ "U00X"
        /* 1 */ "U10X"
        /* X */ "UX0X");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeResetControl, "+0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeResetControl, "X+0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeResetControl, "U+0X");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeResetControl, "X+0");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_2D_STATIC_RIGHT, EncodeResetControl, "U+0X");
}

BOOST_AUTO_TEST_CASE( TestMultiplexer )
{
	FOR_EACH_LOGIC_VALUE_3D(LogicContainer01, LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC, EncodeMultiplexer,
        /*    sel 01  */
		/*  in2       */
		/* in1 ------ */
        /* 00 */ "00"
        /* 01 */ "01"
        /* 10 */ "10"
        /* 11 */ "11");
	FOR_EACH_LOGIC_VALUE_3D(LogicContainer01X, LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC, EncodeMultiplexer,
        /*    sel X01  */
		/*  in2       */
		/* in1 ------ */
        /* XX */ "XXX"
        /* X0 */ "XX0"
        /* X1 */ "XX1"
		/* ----------- */
        /* 0X */ "X0X"
        /* 00 */ "X00"
        /* 01 */ "X01"
		/* ----------- */
        /* 1X */ "X1X"
        /* 10 */ "X10"
        /* 11 */ "X11");
	FOR_EACH_LOGIC_VALUE_3D(LogicContainerU01X, LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC, EncodeMultiplexer,
        /*    sel U01X  */
		/*  in2         */
		/* in1 -------- */
        /* UU */ "UUUX"
        /* U0 */ "UU0X"
        /* U1 */ "UU1X"
        /* UX */ "UUXX"
		/* ------------ */
        /* 0U */ "U0UX"
        /* 00 */ "U00X"
        /* 01 */ "U01X"
        /* 0X */ "U0XX"
		/* ------------ */
        /* 1U */ "U1UX"
        /* 10 */ "U10X"
        /* 11 */ "U11X"
        /* 1X */ "U1XX"
		/* ------------ */
        /* XU */ "UXUX"
        /* X0 */ "UX0X"
        /* X1 */ "UX1X"
        /* XX */ "UXXX");
	FOR_EACH_LOGIC_VALUE_3D(LogicContainer01F, LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC, EncodeMultiplexer,
        /*    sel X01  */
		/*  in2       */
		/* in1 ------ */
        /* XX */ "XXX"
        /* X0 */ "XX0"
        /* X1 */ "XX1"
		/* ----------- */
        /* 0X */ "X0X"
        /* 00 */ "X00"
        /* 01 */ "X01"
		/* ----------- */
        /* 1X */ "X1X"
        /* 10 */ "X10"
        /* 11 */ "X11");
	FOR_EACH_LOGIC_VALUE_3D(LogicContainerU01F, LOGIC_TEST_GATE_3D_STATIC_AND_DYNAMIC, EncodeMultiplexer,
        /*    sel U01X  */
		/*  in2         */
		/* in1 -------- */
        /* UU */ "UUUX"
        /* U0 */ "UU0X"
        /* U1 */ "UU1X"
        /* UX */ "UUXX"
		/* ------------ */
        /* 0U */ "U0UX"
        /* 00 */ "U00X"
        /* 01 */ "U01X"
        /* 0X */ "U0XX"
		/* ------------ */
        /* 1U */ "U1UX"
        /* 10 */ "U10X"
        /* 11 */ "U11X"
        /* 1X */ "U1XX"
		/* ------------ */
        /* XU */ "UXUX"
        /* X0 */ "UX0X"
        /* X1 */ "UX1X"
        /* XX */ "UXXX");

	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01, LOGIC_TEST_GATE_3D_STATIC_RIGHT, EncodeMultiplexer, "+%");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01X, LOGIC_TEST_GATE_3D_STATIC_RIGHT, EncodeMultiplexer, "X+%");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01X, LOGIC_TEST_GATE_3D_STATIC_RIGHT, EncodeMultiplexer, "U+%X");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainer01F, LOGIC_TEST_GATE_3D_STATIC_RIGHT, EncodeMultiplexer, "X+%");
	FOR_EACH_LOGIC_VALUE_1D(LogicContainerU01F, LOGIC_TEST_GATE_3D_STATIC_RIGHT, EncodeMultiplexer, "U+%X");
}

BOOST_AUTO_TEST_SUITE_END()
