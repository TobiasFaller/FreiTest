#define BOOST_TEST_MODULE FaultList
#define BOOST_TEST_NO_MAIN
#define BOOST_TEST_ALTERNATIVE_INIT_API

#include <boost/test/included/unit_test.hpp>

#include <string>
#include <iostream>
#include <memory>

#include "Basic/Logging.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Fault/Lists/SingleTransitionDelayFaultList.hpp"
#include "Basic/Fault/Lists/CellAwareFaultList.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogImporter/VerilogConverter.hpp"

using namespace FreiTest;

namespace FreiTest
{
namespace Fault
{

std::ostream& operator<<(std::ostream& output, const Fault::SingleStuckAtFault& fault)
{
	return output << to_string(fault);
}

std::ostream& operator<<(std::ostream& output, const Fault::SingleTransitionDelayFault& fault)
{
	return output << to_string(fault);
}

std::ostream& operator<<(std::ostream& output, const Fault::CellAwareFault& fault)
{
	return output << to_string(fault);
}

bool operator!=(const Fault::CellAwareFault& f1, const Fault::CellAwareFault& f2)
{
	return (f1.GetCellName() != f2.GetCellName())
		|| (f1.GetCell() != f2.GetCell());
}

};
};

struct Fixture
{

    Fixture()
	{
		std::stringstream source {
			"`celldefine\n"
			"module INV(input IN1, output OUT); wire w; not gate1(w, IN1); buf gate2(OUT, w); endmodule\n"
			"module AND(input IN1, input IN2, output OUT); and gate1(OUT, IN1, IN2); endmodule\n"
			"module NAND(input IN1, input IN2, output OUT); nand gate1(OUT, IN1, IN2); endmodule\n"
			"module OR(input IN1, input IN2, output OUT); or gate1(OUT, IN1, IN2); endmodule\n"
			"`endcelldefine\n"
			"module circuit(input IN1, input IN2, input IN3, output OUT1, output OUT2);\n"
			"wire w1, w2;\n"
			"AND  cell1 (.IN1(IN2), .IN2(IN3), .OUT(w1));\n"
			"INV  cell2 (.IN1(w1), .OUT(w2));\n"
			"OR   cell3 (.IN1(w2), .IN2(IN1), .OUT(OUT1));\n"
			"NAND cell4 (.IN1(w2), .IN2(IN1), .OUT(OUT2));\n"
			"endmodule\n"
		};

    	std::shared_ptr<Settings> settings { std::make_shared<Settings>() };
		settings->CircuitName = "circuit";
		Io::Verilog::VerilogConverter converter { };
		auto modules { converter.ParseVerilog(settings, source) };
		circuit = converter.InstantiateModule(settings, *modules);

		portIn1 = circuit->GetMetaData().GetGroup("circuit/IN1 [0]/port")->GetMappedNodes()[0];
		portIn2 = circuit->GetMetaData().GetGroup("circuit/IN2 [0]/port")->GetMappedNodes()[0];
		portIn3 = circuit->GetMetaData().GetGroup("circuit/IN3 [0]/port")->GetMappedNodes()[0];
		portOut1 = circuit->GetMetaData().GetGroup("circuit/OUT1 [0]/port")->GetMappedNodes()[0];
		portOut2 = circuit->GetMetaData().GetGroup("circuit/OUT2 [0]/port")->GetMappedNodes()[0];
		gate11 = circuit->GetMetaData().GetGroup("circuit/cell1/gate1")->GetMappedNodes()[0];
		gate21 = circuit->GetMetaData().GetGroup("circuit/cell2/gate1")->GetMappedNodes()[0];
		gate22 = circuit->GetMetaData().GetGroup("circuit/cell2/gate2")->GetMappedNodes()[0];
		gate31 = circuit->GetMetaData().GetGroup("circuit/cell3/gate1")->GetMappedNodes()[0];
		gate41 = circuit->GetMetaData().GetGroup("circuit/cell4/gate1")->GetMappedNodes()[0];

		cell1 = circuit->GetMetaData().GetGroup("circuit/cell1");
		cell2 = circuit->GetMetaData().GetGroup("circuit/cell2");
		cell3 = circuit->GetMetaData().GetGroup("circuit/cell3");
		cell4 = circuit->GetMetaData().GetGroup("circuit/cell4");
    }
    ~Fixture() = default;

    std::shared_ptr<Circuit::CircuitEnvironment> circuit;

	Circuit::MappedNode const* portIn1;
	Circuit::MappedNode const* portIn2;
	Circuit::MappedNode const* portIn3;
	Circuit::MappedNode const* portOut1;
	Circuit::MappedNode const* portOut2;

	Circuit::MappedNode const* gate11;
	Circuit::MappedNode const* gate21;
	Circuit::MappedNode const* gate22;
	Circuit::MappedNode const* gate31;
	Circuit::MappedNode const* gate41;

	Circuit::GroupMetaData const* cell1;
	Circuit::GroupMetaData const* cell2;
	Circuit::GroupMetaData const* cell3;
	Circuit::GroupMetaData const* cell4;

};

int main(int argc, char* argv[], char* envp[])
{
	auto settings = std::make_shared<Settings>();
	Settings::SetInstance(settings);

	Logging::Initialize({ "--log-level=trace" });
	return boost::unit_test::unit_test_main(::init_unit_test, argc, argv);
}

BOOST_FIXTURE_TEST_SUITE( FaultListTest, Fixture )

BOOST_AUTO_TEST_CASE( TestFullStuckAt )
{
	auto faultList { Fault::GenerateStuckAtFaultList(*circuit) };

	/* No filter */
	// Inputs: IN1, IN2, IN3
	// Gates:
	//   - Cell 1: G11/I1, G11/I2, G11/O
	//   - Cell 2: G21/I, G21/O, G22/I, G22/O
	//   - Cell 3: G31/I1, G31/I2, G31/O
	//   - Cell 4: G41/I1, G41/I2, G41/O
	// Outputs: OUT1, OUT2
	std::vector<Fault::SingleStuckAtFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate21, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate21, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate22, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate22, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
	};
	std::vector<size_t> expectedMapping {
		 0u,  1u,  2u,  3u,  4u,  5u, // IN1, IN2, IN3
		 6u,  7u,  8u,  9u, 10u, 11u, // G11
		12u, 13u, 14u, 15u,           // G21
		16u, 17u, 18u, 19u,           // G22
		20u, 21u, 22u, 23u, 24u, 25u, // G31
		26u, 27u, 28u, 29u, 30u, 31u, // G41
		32u, 33u,                     // OUT1
		34u, 35u,                     // OUT2
	};

	auto reduction { Fault::StuckAtFaultReduction::RemoveNone };
	auto [reduced, mapping] = Fault::ReduceStuckAtFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestEquivalentStuckAt )
{
	auto faultList { Fault::GenerateStuckAtFaultList(*circuit) };

	/* Stuck-at fault equivalence filter */
	// 0)  IN1=0
	// 1)  IN1=1
	// 2)  G11/I1=1 <- IN2=1
	// 3)  G11/I2=1 <- IN3=1
	// 4)  G22/O=0 <- G22/I=0 <- G21/O=0 <- G21/I=1 <- G11/O=1
	// 5)  G22/O=1 <- G22/I=1 <- G21/O=0 <- G21/I=0 <- G11/O=0 <+- G11/I1=0 <- IN2=0
	//                                                          *- G11/I2=0 <- IN3=0
	// 6)  G31/I1=0
	// 7)  G31/I2=0
	// 8)  G41/I1=1
	// 9)  G41/I2=1
	// 10) OUT1=0 <- G31/O=0
	// 11) OUT1=1 <- G31/O=1 <+- G31/I1=1
	//                        *- G31/I2=1
	// 12) OUT2=0 <- G41/O=0
	// 13) OUT2=1 <- G41/O=1 <+- G41/I1=0
	//                        *- G41/I2=0
	std::vector<Fault::SingleStuckAtFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { portOut1, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portOut2, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
	};
	std::vector<size_t> expectedMapping {
		0u,   1u,  5u,  2u,  5u,  3u, // IN1, IN2, IN3
		5u,   2u,  5u,  3u,  5u,  4u, // G11
		5u,   4u,  4u,  5u,           // G21
		4u,   5u,  4u,  5u,           // G22
		6u,  11u,  7u, 11u, 10u, 11u, // G31
		13u,  8u, 13u,  9u, 12u, 13u, // G41
		10u, 11u,                     // OUT1
		12u, 13u,                     // OUT2
	};

	auto reduction { Fault::StuckAtFaultReduction::RemoveEquivalent };
	auto [reduced, mapping] = Fault::ReduceStuckAtFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestCellInternalStuckAt )
{
	auto faultList { Fault::GenerateStuckAtFaultList(*circuit) };

	/* Cell internal filter */
	// Inputs: IN1, IN2, IN3
	// Gates:
	//   - Cell 1: G11/I1, G11/I2, G11/O
	//   - Cell 2: G21/I, G22/O (Removed: G21/O, G22/I)
	//   - Cell 3: G31/I1, G31/I2, G31/O
	//   - Cell 4: G41/I1, G41/I2, G41/O
	// Outputs: OUT1, OUT2
	std::vector<Fault::SingleStuckAtFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
	};
	const auto REM = Fault::REMOVED_FAULT;
	std::vector<size_t> expectedMapping {
		0u,  1u,  2u,  3u,   4u,  5u, // IN1, IN2, IN3
		6u,  7u,  8u,  9u,  10u, 11u, // G11
		12u, 13u, REM, REM,           // G21
		REM, REM, 14u, 15u,           // G22
		16u, 17u, 18u, 19u, 20u, 21u, // G31
		22u, 23u, 24u, 25u, 26u, 27u, // G41
		28u, 29u,                     // OUT1
		30u, 31u,                     // OUT2
	};

	auto reduction { Fault::StuckAtFaultReduction::RemoveCellInternal };
	auto [reduced, mapping] = Fault::ReduceStuckAtFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestCellInternalAndEquivalentStuckAt )
{
	auto faultList { Fault::GenerateStuckAtFaultList(*circuit) };

	/* Remove equivalent and cell internal filter */
	// Stuck-at fault equivalences:
	// 0)  IN1=0
	// 1)  IN1=1
	// 2)  G11/I1=1 <- IN2=1
	// 3)  G11/I2=1 <- IN3=1
	// 4)  G21/I=0 <- G11/O=0 <+- G11/I1=0 <- IN2=0
	//                         *- G11/I2=0 <- IN3=0
	// 5)  G21/I=1 <- G11/O=1
	// 6)  G22/O=0
	// 7)  G22/O=1
	// 8)  G31/I1=0
	// 9)  G31/I2=0
	// 10) G41/I1=1
	// 11) G41/I2=1
	// 12) OUT1=0 <- G31/O=0
	// 13) OUT1=1 <- G31/O=1 <+- G31/I1=1
	//                        *- G31/I2=1
	// 14) OUT2=0 <- G41/O=0
	// 15) OUT2=1 <- G41/O=1 <+- G41/I1=0
	//                        *- G41/I2=0
	std::vector<Fault::SingleStuckAtFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },

		{ { portOut1, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
		{ { portOut2, { Circuit::PortType::Input, 0u } }, Fault::StuckAtFaultType::STUCK_AT_0 },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::StuckAtFaultType::STUCK_AT_1 },
	};
	const auto REM = Fault::REMOVED_FAULT;
	std::vector<size_t> expectedMapping {
		0u,  1u,   4u,  2u,  4u,  3u, // IN1, IN2, IN3
		4u,  2u,   4u,  3u,  4u,  5u, // G11
		4u,  5u,  REM, REM,           // G21
		REM, REM,  6u,  7u,           // G22
		8u,  13u,  9u, 13u, 12u, 13u, // G31
		15u, 10u, 15u, 11u, 14u, 15u, // G41
		12u, 13u,                     // OUT1
		14u, 15u,                     // OUT2
	};

	auto reduction { Fault::StuckAtFaultReduction::RemoveEquivalent |  Fault::StuckAtFaultReduction::RemoveCellInternal };
	auto [reduced, mapping] = Fault::ReduceStuckAtFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestFullTransitionDelay )
{
	auto faultList { Fault::GenerateTransitionDelayFaultList(*circuit) };

	/* No filter */
	// Inputs: IN1, IN2, IN3
	// Gates:
	//   - Cell 1: G11/I1, G11/I2, G11/O
	//   - Cell 2: G21/I, G21/O, G22/I, G22/O
	//   - Cell 3: G31/I1, G31/I2, G31/O
	//   - Cell 4: G41/I1, G41/I2, G41/O
	// Outputs: OUT1, OUT2
	std::vector<Fault::SingleTransitionDelayFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate21, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate21, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate22, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate22, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
	};
	std::vector<size_t> expectedMapping {
		 0u,  1u,  2u,  3u,  4u,  5u, // IN1, IN2, IN3
		 6u,  7u,  8u,  9u, 10u, 11u, // G11
		12u, 13u, 14u, 15u,           // G21
		16u, 17u, 18u, 19u,           // G22
		20u, 21u, 22u, 23u, 24u, 25u, // G31
		26u, 27u, 28u, 29u, 30u, 31u, // G41
		32u, 33u,                     // OUT1
		34u, 35u,                     // OUT2
	};

	auto reduction { Fault::TransitionDelayFaultReduction::RemoveNone };
	auto [reduced, mapping] = Fault::ReduceTransitionDelayFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestEquivalentTransitionDelay )
{
	auto faultList { Fault::GenerateTransitionDelayFaultList(*circuit) };

	/* Transition-delay fault equivalence filter */
	// 0)  IN1=R
	// 1)  IN1=F
	// 2)  G11/I1=F <- IN2=F
	// 3)  G11/I2=F <- IN3=F
	// 4)  G22/O=R <- G22/I=R <- G21/O=R <- G21/I=F <- G11/O=F
	// 5)  G22/O=F <- G22/I=F <- G21/O=R <- G21/I=R <- G11/O=R <+- G11/I1=R <- IN2=R
	//                                                          *- G11/I2=R <- IN3=R
	// 6)  G31/I1=R
	// 7)  G31/I2=R
	// 8)  G41/I1=F
	// 9)  G41/I2=F
	// 10) OUT1=R <- G31/O=R
	// 11) OUT1=F <- G31/O=F <+- G31/I1=F
	//                        *- G31/I2=F
	// 12) OUT2=R <- G41/O=R
	// 13) OUT2=F <- G41/O=F <+- G41/I1=R
	//                        *- G41/I2=R
	std::vector<Fault::SingleTransitionDelayFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { portOut1, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portOut2, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
	};
	std::vector<size_t> expectedMapping {
		0u,   1u,  5u,  2u,  5u,  3u, // IN1, IN2, IN3
		5u,   2u,  5u,  3u,  5u,  4u, // G11
		5u,   4u,  4u,  5u,           // G21
		4u,   5u,  4u,  5u,           // G22
		6u,  11u,  7u, 11u, 10u, 11u, // G31
		13u,  8u, 13u,  9u, 12u, 13u, // G41
		10u, 11u,                     // OUT1
		12u, 13u,                     // OUT2
	};

	auto reduction { Fault::TransitionDelayFaultReduction::RemoveEquivalent };
	auto [reduced, mapping] = Fault::ReduceTransitionDelayFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestCellInternalTransitionDelay )
{
	auto faultList { Fault::GenerateTransitionDelayFaultList(*circuit) };

	/* Cell internal filter */
	// Inputs: IN1, IN2, IN3
	// Gates:
	//   - Cell 1: G11/I1, G11/I2, G11/O
	//   - Cell 2: G21/I, G22/O (Removed: G21/O, G22/I)
	//   - Cell 3: G31/I1, G31/I2, G31/O
	//   - Cell 4: G41/I1, G41/I2, G41/O
	// Outputs: OUT1, OUT2
	std::vector<Fault::SingleTransitionDelayFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn2, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn3, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate11, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
	};
	const auto REM = Fault::REMOVED_FAULT;
	std::vector<size_t> expectedMapping {
		0u,  1u,  2u,  3u,   4u,  5u, // IN1, IN2, IN3
		6u,  7u,  8u,  9u,  10u, 11u, // G11
		12u, 13u, REM, REM,           // G21
		REM, REM, 14u, 15u,           // G22
		16u, 17u, 18u, 19u, 20u, 21u, // G31
		22u, 23u, 24u, 25u, 26u, 27u, // G41
		28u, 29u,                     // OUT1
		30u, 31u,                     // OUT2
	};

	auto reduction { Fault::TransitionDelayFaultReduction::RemoveCellInternal };
	auto [reduced, mapping] = Fault::ReduceTransitionDelayFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestCellInternalAndEquivalentTransitionDelay )
{
	auto faultList { Fault::GenerateTransitionDelayFaultList(*circuit) };

	/* Remove equivalent and cell internal filter */
	// Transition-delay fault equivalences:
	// 0)  IN1=R
	// 1)  IN1=F
	// 2)  G11/I1=F <- IN2=F
	// 3)  G11/I2=F <- IN3=F
	// 4)  G21/I=R <- G11/O=R <+- G11/I1=R <- IN2=R
	//                         *- G11/I2=R <- IN3=R
	// 5)  G21/I=F <- G11/O=F
	// 6)  G22/O=R
	// 7)  G22/O=F
	// 8)  G31/I1=R
	// 9)  G31/I2=R
	// 10) G41/I1=F
	// 11) G41/I2=F
	// 12) OUT1=R <- G31/O=R
	// 13) OUT1=F <- G31/O=F <+- G31/I1=F
	//                        *- G31/I2=F
	// 14) OUT2=R <- G41/O=R
	// 15) OUT2=F <- G41/O=F <+- G41/I1=R
	//                        *- G41/I2=R
	std::vector<Fault::SingleTransitionDelayFault> expectedFaults {
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portIn1, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { gate11, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate11, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate21, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate22, { Circuit::PortType::Output, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate31, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate31, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { gate41, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { gate41, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },

		{ { portOut1, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut1, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
		{ { portOut2, { Circuit::PortType::Input, 0u } }, Fault::TransitionDelayFaultType::SLOW_TO_RISE },
		{ { portOut2, { Circuit::PortType::Input, 1u } }, Fault::TransitionDelayFaultType::SLOW_TO_FALL },
	};
	const auto REM = Fault::REMOVED_FAULT;
	std::vector<size_t> expectedMapping {
		0u,  1u,   4u,  2u,  4u,  3u, // IN1, IN2, IN3
		4u,  2u,   4u,  3u,  4u,  5u, // G11
		4u,  5u,  REM, REM,           // G21
		REM, REM,  6u,  7u,           // G22
		8u,  13u,  9u, 13u, 12u, 13u, // G31
		15u, 10u, 15u, 11u, 14u, 15u, // G41
		12u, 13u,                     // OUT1
		14u, 15u,                     // OUT2
	};

	auto reduction { Fault::TransitionDelayFaultReduction::RemoveEquivalent |  Fault::TransitionDelayFaultReduction::RemoveCellInternal };
	auto [reduced, mapping] = Fault::ReduceTransitionDelayFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_CASE( TestFullCellAware )
{
	std::vector<Fault::CellAwareFaultSpecification::State> SPEC_STATES { { {} } };
	Fault::CellAwareModelSpecification specification { {
		{ "AND",  { { "FA1", SPEC_STATES }, { "FA2", SPEC_STATES }, { "FA3", SPEC_STATES } } },
		{ "NAND", { { "FN1", SPEC_STATES }, { "FN2", SPEC_STATES }, { "FN3", SPEC_STATES } } },
		{ "INV",  { { "FI1", SPEC_STATES }, { "FI2", SPEC_STATES } } },
		{ "OR",   { { "FO1", SPEC_STATES }, { "FO2", SPEC_STATES }, { "FO3", SPEC_STATES } } },
	} };
	auto faultList { Fault::GenerateCellAwareFaultList(*circuit, specification) };

	/* No filter */
	// Cells:
	//   - Cell 1 (AND)
	//   - Cell 2 (INV)
	//   - Cell 3 (OR)
	//   - Cell 4 (NAND)
	std::vector<Fault::CellAwareFaultStateMachine::State> FAULT_STATES { { {}, 0u } };
	std::vector<Fault::CellAwareFault> expectedFaults {
		{ "FA1", cell1, FAULT_STATES }, { "FA2", cell1, FAULT_STATES }, { "FA3", cell1, FAULT_STATES },
		{ "FI1", cell2, FAULT_STATES }, { "FI2", cell2, FAULT_STATES },
		{ "FO1", cell3, FAULT_STATES }, { "FO2", cell3, FAULT_STATES }, { "FO3", cell3, FAULT_STATES },
		{ "FN1", cell4, FAULT_STATES }, { "FN2", cell4, FAULT_STATES }, { "FN3", cell4, FAULT_STATES },
	};
	std::vector<size_t> expectedMapping {
		 0u,  1u,  2u,       // Cell 1
		 3u,  4u,            // Cell 2
		 5u,  6u,  7u,       // Cell 3
		 8u,  9u, 10u,       // Cell 3
	};

	auto reduction { Fault::CellAwareFaultReduction::RemoveNone };
	auto [reduced, mapping] = Fault::ReduceCellAwareFaultList(*circuit, reduction, faultList);
	BOOST_CHECK_EQUAL_COLLECTIONS(reduced.begin(), reduced.end(), expectedFaults.begin(), expectedFaults.end());
	BOOST_CHECK_EQUAL_COLLECTIONS(mapping.begin(), mapping.end(), expectedMapping.begin(), expectedMapping.end());
}

BOOST_AUTO_TEST_SUITE_END()
