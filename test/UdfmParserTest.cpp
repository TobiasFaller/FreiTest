#define BOOST_TEST_MODULE UdfmParser
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/UserDefinedFaultModel/UdfmParser.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Io::Udfm;

int main(int argc, char* argv[], char* envp[])
{
	Logging::Initialize({ "--log-level=trace" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_AUTO_TEST_SUITE( UdfmParserTest )

BOOST_AUTO_TEST_CASE( ParseSimpleEmptyThings )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);
	FileHandle emptyCell("test/data/udfm/simple_empty_cell.udfm", true);
	FileHandle emptyFault("test/data/udfm/simple_empty_fault.udfm", true);
	FileHandle emptyAlternative("test/data/udfm/simple_empty_alternative.udfm", true);

	auto udfmEmptyCell = ParseUdfm(emptyCell.GetStream());
	BOOST_CHECK(udfmEmptyCell);

	BOOST_CHECK_EQUAL(1u, udfmEmptyCell->GetCells().size());
	BOOST_CHECK(udfmEmptyCell->HasCell("EMPTY_CELL"));
	BOOST_CHECK_EQUAL(0u, udfmEmptyCell->GetCell("EMPTY_CELL")->GetFaults().size());

	auto udfmEmptyFault = ParseUdfm(emptyFault.GetStream());
	BOOST_CHECK(udfmEmptyFault);

	BOOST_CHECK_EQUAL(1u, udfmEmptyFault->GetCells().size());
	BOOST_CHECK(udfmEmptyFault->HasCell("SIMPLE_CELL"));
	auto emptyFaultCell = udfmEmptyFault->GetCell("SIMPLE_CELL");
	BOOST_CHECK_EQUAL(2u, emptyFaultCell->GetFaults().size());
	BOOST_CHECK(emptyFaultCell->HasFault("FAULT1"));
	BOOST_CHECK(emptyFaultCell->HasFault("FAULT2"));
	BOOST_CHECK_EQUAL(0u, emptyFaultCell->GetFault("FAULT1")->GetAlternatives().size());
	BOOST_CHECK_EQUAL(1u, emptyFaultCell->GetFault("FAULT2")->GetAlternatives().size());

	BOOST_CHECK(!ParseUdfm(emptyAlternative.GetStream()));
}

BOOST_AUTO_TEST_CASE( ParseSimpleExampleUdfm )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);
	FileHandle example("test/data/udfm/simple_example.udfm", true);

	auto udfmExample = ParseUdfm(example.GetStream());
	BOOST_CHECK(udfmExample);

	BOOST_CHECK_EQUAL(1u, udfmExample->GetCells().size());
	BOOST_CHECK(udfmExample->HasCell("SIMPLE_CELL"));
	auto cell = udfmExample->GetCell("SIMPLE_CELL");
	BOOST_CHECK_EQUAL(2u, cell->GetFaults().size());
	BOOST_CHECK(cell->HasFault("FAULT1"));
	BOOST_CHECK(cell->HasFault("FAULT2"));

	auto fault1 = cell->GetFault("FAULT1");
	auto fault2 = cell->GetFault("FAULT2");
	BOOST_CHECK_EQUAL(1u, fault1->GetAlternatives().size());
	BOOST_CHECK_EQUAL(2u, fault2->GetAlternatives().size());

	BOOST_CHECK_EQUAL(2u, fault1->GetAlternativeConditions(0).size());
	BOOST_CHECK_EQUAL(1u, fault1->GetAlternativeEffects(0).size());
	BOOST_CHECK(fault1->GetAlternativeConditions(0).find("I1") != fault1->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault1->GetAlternativeConditions(0).find("I2") != fault1->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault1->GetAlternativeEffects(0).find("O") != fault1->GetAlternativeEffects(0).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault1->GetAlternativeConditions(0).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault1->GetAlternativeConditions(0).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault1->GetAlternativeEffects(0).find("O")->second);

	BOOST_CHECK_EQUAL(2u, fault2->GetAlternativeConditions(0).size());
	BOOST_CHECK_EQUAL(1u, fault2->GetAlternativeEffects(0).size());
	BOOST_CHECK(fault2->GetAlternativeConditions(0).find("I1") != fault2->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault2->GetAlternativeConditions(0).find("I2") != fault2->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault2->GetAlternativeEffects(0).find("O") != fault2->GetAlternativeEffects(0).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault2->GetAlternativeConditions(0).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(0).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault2->GetAlternativeEffects(0).find("O")->second);

	BOOST_CHECK_EQUAL(2u, fault2->GetAlternativeConditions(1).size());
	BOOST_CHECK_EQUAL(1u, fault2->GetAlternativeEffects(1).size());
	BOOST_CHECK(fault2->GetAlternativeConditions(1).find("I1") != fault2->GetAlternativeConditions(1).end());
	BOOST_CHECK(fault2->GetAlternativeConditions(1).find("I2") != fault2->GetAlternativeConditions(1).end());
	BOOST_CHECK(fault2->GetAlternativeEffects(1).find("O") != fault2->GetAlternativeEffects(1).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(1).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(1).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeEffects(1).find("O")->second);
}

BOOST_AUTO_TEST_CASE( ParseTessentExampleUdfm )
{
    auto settings = std::make_shared<Settings>();
    Settings::SetInstance(settings);
	FileHandle example("test/data/udfm/tessent_example.udfm", true);

	auto udfmExample = ParseUdfm(example.GetStream());
	BOOST_CHECK(udfmExample);

	BOOST_CHECK_EQUAL(1u, udfmExample->GetCells().size());
	BOOST_CHECK(udfmExample->HasCell("TESSENT_CELL"));
	auto cell = udfmExample->GetCell("TESSENT_CELL");
	BOOST_CHECK_EQUAL(2u, cell->GetFaults().size());
	BOOST_CHECK(cell->HasFault("FAULT1"));
	BOOST_CHECK(cell->HasFault("FAULT2"));

	auto fault1 = cell->GetFault("FAULT1");
	auto fault2 = cell->GetFault("FAULT2");
	BOOST_CHECK_EQUAL(1u, fault1->GetAlternatives().size());
	BOOST_CHECK_EQUAL(2u, fault2->GetAlternatives().size());
	BOOST_CHECK_EQUAL("CATEGORY 1", fault1->GetFaultCategory());
	BOOST_CHECK_EQUAL("CATEGORY 2", fault2->GetFaultCategory());

	BOOST_CHECK_EQUAL(2u, fault1->GetAlternativeConditions(0).size());
	BOOST_CHECK_EQUAL(1u, fault1->GetAlternativeEffects(0).size());
	BOOST_CHECK(fault1->GetAlternativeConditions(0).find("I1") != fault1->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault1->GetAlternativeConditions(0).find("I2") != fault1->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault1->GetAlternativeEffects(0).find("O") != fault1->GetAlternativeEffects(0).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault1->GetAlternativeConditions(0).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault1->GetAlternativeConditions(0).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault1->GetAlternativeEffects(0).find("O")->second);

	BOOST_CHECK_EQUAL(2u, fault2->GetAlternativeConditions(0).size());
	BOOST_CHECK_EQUAL(1u, fault2->GetAlternativeEffects(0).size());
	BOOST_CHECK(fault2->GetAlternativeConditions(0).find("I1") != fault2->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault2->GetAlternativeConditions(0).find("I2") != fault2->GetAlternativeConditions(0).end());
	BOOST_CHECK(fault2->GetAlternativeEffects(0).find("O") != fault2->GetAlternativeEffects(0).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault2->GetAlternativeConditions(0).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(0).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ONE}, fault2->GetAlternativeEffects(0).find("O")->second);

	BOOST_CHECK_EQUAL(2u, fault2->GetAlternativeConditions(1).size());
	BOOST_CHECK_EQUAL(1u, fault2->GetAlternativeEffects(1).size());
	BOOST_CHECK(fault2->GetAlternativeConditions(1).find("I1") != fault2->GetAlternativeConditions(1).end());
	BOOST_CHECK(fault2->GetAlternativeConditions(1).find("I2") != fault2->GetAlternativeConditions(1).end());
	BOOST_CHECK(fault2->GetAlternativeEffects(1).find("O") != fault2->GetAlternativeEffects(1).end());
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(1).find("I1")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeConditions(1).find("I2")->second);
	BOOST_CHECK_EQUAL(std::vector<LogicConstraint> {LogicConstraint::ONLY_LOGIC_ZERO}, fault2->GetAlternativeEffects(1).find("O")->second);
}

BOOST_AUTO_TEST_SUITE_END()
