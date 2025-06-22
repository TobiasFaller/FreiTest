#include "Io/StilExporter/StilExporter.hpp"

#include <tuple>

#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Simulation/CircuitSimulator.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Io
{

// 1450.1-2005 - IEEE Standard for Extensions to Standard Test Interface Language (STIL) (IEEE Std 1450-1999) for Semiconductor Design Environments
// https://ieeexplore.ieee.org/document/1519742

using PortList = std::vector<const Circuit::PortMetaData*>;
using ScanList = std::vector<const Circuit::MappedNode*>;

enum class Scan { No, Yes };

static std::tuple<PortList, PortList> GetPrimaryPorts(const Circuit::CircuitEnvironment& circuit)
{
	auto& metaData = circuit.GetMetaData();
	auto& ports = metaData.GetRoot()->GetPorts();

	PortList inputPorts { };
	PortList outputPorts { };
	for (auto const& port : ports)
	{
		auto type = port.GetSourceInfo().GetProperty<std::string>("port-type");
		if (type == "input") inputPorts.push_back(&port);
		if (type == "output") outputPorts.push_back(&port);
	}

	return std::make_tuple(inputPorts, outputPorts);
}

static std::tuple<ScanList, ScanList> GetScanChains(const Circuit::CircuitEnvironment& circuit)
{
	auto& mappedCircuit = circuit.GetMappedCircuit();

	ScanList flipFlopInputs { };
	ScanList flipFlopOutputs { };
	for (auto const* input : mappedCircuit.GetSecondaryInputs())
	{
		flipFlopInputs.push_back(input);
	}
	for (auto const* output : mappedCircuit.GetSecondaryOutputs())
	{
		flipFlopOutputs.push_back(output);
	}

	return std::make_tuple(flipFlopInputs, flipFlopOutputs);
}

static std::string GetPrimaryInputVectors(const PortList& primaryInputs, const Simulation::SimulationResult& simulation, size_t timeframe)
{
	std::string result;
	for (auto& port : primaryInputs)
	{
		for (size_t index : port->GetSize().GetIndicesTopToBottom())
		{
			auto connection = port->GetConnectionForIndex(index);
			if (connection == nullptr)
			{
				result += 'Z';
				continue;
			}

			auto source = connection->GetMappedSources();
			if (source.size() == 0u)
			{
				result += 'Z';
				continue;
			}

			size_t nodeIndex = source[0u].node->GetNodeId();
			switch (simulation[timeframe][nodeIndex])
			{
				case Logic::LOGIC_ZERO: result += '0'; break;
				case Logic::LOGIC_ONE: result += '1'; break;
				default: result += 'Z'; break;
			}
		}
	}
	return result;
}

static std::string GetPrimaryOutputVectors(const PortList& ports, const Simulation::SimulationResult& simulation, size_t timeframe)
{
	std::string result;
	for (auto& port : ports)
	{
		for (size_t index : port->GetSize().GetIndicesTopToBottom())
		{
			auto connection = port->GetConnectionForIndex(index);
			if (connection == nullptr)
			{
				result += 'X';
				continue;
			}

			auto source = connection->GetMappedSources();
			if (source.size() == 0u)
			{
				result += 'X';
				continue;
			}

			size_t nodeIndex = source[0u].node->GetNodeId();
			switch (simulation[timeframe][nodeIndex])
			{
				case Logic::LOGIC_ZERO: result += 'L'; break;
				case Logic::LOGIC_ONE: result += 'H'; break;
				default: result += 'X'; break;
			}
		}
	}
	return result;
}

static std::string GetSecondaryInputVectors(const ScanList& secondaryInputs, const Simulation::SimulationResult& simulation, size_t timeframe)
{
	std::string result;
	for (auto& ff : secondaryInputs)
	{
		size_t nodeIndex = ff->GetNodeId();
		switch (simulation[timeframe][nodeIndex])
		{
			case Logic::LOGIC_ZERO: result += '0'; break;
			case Logic::LOGIC_ONE: result += '1'; break;
			default: result += 'Z'; break;
		}
	}
	return result;
}

static std::string GetSecondaryOutputVectors(const ScanList& secondaryOutputs, const Simulation::SimulationResult& simulation, size_t timeframe)
{
	std::string result;
	for (auto& ff : secondaryOutputs)
	{
		size_t nodeIndex = ff->GetNodeId();
		switch (simulation[timeframe][nodeIndex])
		{
			case Logic::LOGIC_ZERO: result += 'D'; break;
			case Logic::LOGIC_ONE: result += 'U'; break;
			default: result += 'X'; break;
		}
	}
	return result;
}

static void ExportHeader(const Circuit::CircuitEnvironment& circuit, std::ostream& output)
{
	output << "STIL 1.0 { Design 2005; }" << std::endl;
	output << std::endl;

	output << "Header {" << std::endl;
	output << "    Title \"" << circuit.GetName() << "\"" << std::endl;
	output << "    Source \"FreiTest (c) by University of Freiburg\"" << std::endl;
	output << "}" << std::endl;
	output << std::endl;
}

static void ExportPorts(const std::tuple<PortList, PortList>& ports, Scan scan, std::ostream& output)
{
	auto [inputPorts, outputPorts] = ports;

	const auto port_to_string = [&](auto& port) -> std::string {
		std::string result = "\"" + port->GetName() + "\"";
		if (auto size = port->GetSize(); size.GetSize() > 1u)
		{
			result += "[" + std::to_string(size.GetTop()) + ".." + std::to_string(size.GetBottom()) + "]";
		}
		return result;
	};

	output << "Signals {" << std::endl;
	for (auto& port : inputPorts)
	{
		output << "    " << port_to_string(port) << " In;" << std::endl;
	}
	for (auto& port : outputPorts)
	{
		output << "    " << port_to_string(port) << " Out;" << std::endl;
	}
	if (scan == Scan::Yes)
	{
		output << "    \"TCK\" In;" << std::endl;
		output << "    \"SE\" In;" << std::endl;
		output << "    \"SI\" In;" << std::endl;
		output << "    \"SO\" Out;" << std::endl;
	}
	output << "}" << std::endl;
	output << std::endl;

	output << "SignalGroups {" << std::endl;
	output << "    _PI_ = '";
	size_t inputIndex { 0u };
	for (auto& port : inputPorts)
	{
		if (inputIndex++) output << " + ";
		output << port_to_string(port);
	}
	output << "';" << std::endl;

	output << "    _PO_ = '";
	size_t outputIndex { 0u };
	for (auto& port : outputPorts)
	{
		if (outputIndex++) output << " + ";
		output << port_to_string(port);
	}
	output << "';" << std::endl;

	if (scan == Scan::Yes)
	{
		output << "    _SI_ = 'TCK + SE + SI';" << std::endl;
		output << "    _SO_ = 'SO';" << std::endl;
	}

	output << "}" << std::endl;
	output << std::endl;
}

static void ExportScanChain(std::tuple<ScanList, ScanList> scanChain, std::ostream& output)
{
	auto [flipFlopInputs, flipFlopOutputs] = scanChain;

	output << "ScanStructures {" << std::endl;
	output << "    ScanChain SCAN_CHAIN {" << std::endl;
	output << "        ScanLength " << flipFlopInputs.size() << ";" << std::endl;
	output << "        ScanInversion 0;" << std::endl;
	output << "        ScanIn \"SI\";" << std::endl;
	output << "        ScanOut \"SO\";" << std::endl;
	output << "        ScanMasterClock \"TCK\";" << std::endl;
	output << "    }" << std::endl;
	output << "}" << std::endl;
	output << std::endl;
}

static void ExportScanMacros(std::ostream& output)
{
	output << "MacroDefs {" << std::endl;
	output << "    SCAN_LOAD_UNLOAD {" << std::endl;
	output << "        Shift { V { \"SE\" = 1; \"SI\" = #; \"SO\" = #; \"TCK\" = P; } }" << std::endl;
	output << "    }" << std::endl;
	output << "}" << std::endl;
	output << std::endl;
}

static void ExportPatternExec(std::ostream& output)
{
	output << "PatternBurst PATTERNS {" << std::endl;
	output << "    PatList { PATTERN; }" << std::endl;
	output << "}" << std::endl;
	output << std::endl;

	output << "PatternExec {" << std::endl;
	output << "    PatternBurst PATTERNS;" << std::endl;
	output << "}" << std::endl;
	output << std::endl;
}

void ExportStilFullScan(const Circuit::CircuitEnvironment& circuit, const Pattern::TestPatternList& patterns, std::ostream& output)
{
	auto& mappedCircuit = circuit.GetMappedCircuit();

	auto primaryPorts = GetPrimaryPorts(circuit);
	auto secondaryPorts = GetScanChains(circuit);

	auto [primaryInputs, primaryOutputs] = primaryPorts;
	auto [secondaryInputs, secondaryOutputs] = secondaryPorts;

	ExportHeader(circuit, output);
	ExportPorts(primaryPorts, Scan::Yes, output);
	ExportScanChain(secondaryPorts, output);
	ExportScanMacros(output);
	ExportPatternExec(output);

	std::string noSpecPrimaryInputs;
	std::string noSpecPrimaryOutputs;
	std::string noSpecSecondaryInputs;
	std::string noSpecSecondaryOutputs;
	for (auto& port : primaryInputs)
	{
		noSpecPrimaryInputs += std::string(port->GetSize().GetSize(), 'Z');
	}
	for (auto& port : primaryOutputs)
	{
		noSpecPrimaryOutputs += std::string(port->GetSize().GetSize(), 'X');
	}
	noSpecSecondaryInputs = std::string(secondaryInputs.size(), 'Z');
	noSpecSecondaryOutputs = std::string(secondaryOutputs.size(), 'X');

	output << "Pattern PATTERN {" << std::endl;
	output << "    Ann {* Initialization Pattern, has to be fully defined *}" << std::endl;
	output << "    V {" << std::endl;
	output << "        \"_PI_\" = " << noSpecPrimaryInputs << ";" << std::endl;
	output << "        \"_PO_\" = " << noSpecPrimaryOutputs << ";" << std::endl;
	output << "        \"_SI_\" = 000; \"_SO_\" = X;" << std::endl;
	output << "    }" << std::endl;
	output << std::endl;

	size_t patternId { 0u };
	for (auto const& pattern : patterns)
	{
		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		Simulation::SimulationResult simulation(pattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
		Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, *pattern, {}, simulation, simConfig);

		output << "    Ann {* Pattern " << patternId++ << " *}" << std::endl;
		output << "    Ann {* Step 1: Apply primary inputs *}" << std::endl;
		output << "    V {" << std::endl;
		output << "        \"_PI_\" = " << GetPrimaryInputVectors(primaryInputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_PO_\" = " << noSpecPrimaryOutputs << ";" << std::endl;
		output << "        \"_SI_\" = 000; \"_SO_\" = X;" << std::endl;
		output << "    }" << std::endl;
		output << "    Ann {* Step 2: Apply scan chain inputs *}" << std::endl;
		output << "    Macro SCAN_LOAD_UNLOAD {" << std::endl;
		output << "        \"_SI_\" = " << GetSecondaryInputVectors(secondaryInputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_SO_\" = " << noSpecSecondaryOutputs << ";" << std::endl;
		output << "    }" << std::endl;
		output << "    Ann {* Step 3: Check primary outputs *}" << std::endl;
		output << "    V {" << std::endl;
		output << "        \"_PI_\" = " << GetPrimaryInputVectors(primaryInputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_PO_\" = " << GetPrimaryOutputVectors(primaryOutputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_SI_\" = 000; \"_SO_\" = X;" << std::endl;
		output << "    }" << std::endl;
		output << "    Ann {* Step 4: Load scan chain with next state *}" << std::endl;
		output << "    V {" << std::endl;
		output << "        \"_PI_\" = " << GetPrimaryInputVectors(primaryInputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_PO_\" = " << noSpecPrimaryOutputs << ";" << std::endl;
		output << "        \"_SI_\" = P00; \"_SO_\" = X;" << std::endl;
		output << "    }" << std::endl;
		output << "    Ann {* Step 5: Check scan chain outputs *}" << std::endl;
		output << "    Macro SCAN_LOAD_UNLOAD {" << std::endl;
		output << "        \"_SI_\" = " << GetSecondaryInputVectors(secondaryInputs, simulation, 0u) << ";" << std::endl;
		output << "        \"_SO_\" = " << GetSecondaryOutputVectors(secondaryOutputs, simulation, 0u) << ";" << std::endl;
		output << "    }" << std::endl;
		output << std::endl;
	}
	output << "}" << std::endl;
}

void ExportStilSequential(const Circuit::CircuitEnvironment& circuit, const Pattern::TestPatternList& patterns, std::ostream& output)
{
	auto& mappedCircuit = circuit.GetMappedCircuit();

	auto primaryPorts = GetPrimaryPorts(circuit);
	auto [primaryInputs, primaryOutputs] = primaryPorts;

	ExportHeader(circuit, output);
	ExportPorts(primaryPorts, Scan::No, output);
	ExportPatternExec(output);

	std::string noSpecPrimaryInputs;
	std::string noSpecPrimaryOutputs;
	for (auto& port : primaryInputs)
	{
		noSpecPrimaryInputs += std::string(port->GetSize().GetSize(), 'Z');
	}
	for (auto& port : primaryOutputs)
	{
		noSpecPrimaryOutputs += std::string(port->GetSize().GetSize(), 'X');
	}

	output << "Pattern PATTERN {" << std::endl;
	output << "    Ann {* Initialization Pattern, has to be fully defined *}" << std::endl;
	output << "    V {" << std::endl;
	output << "        \"_PI_\" = " << noSpecPrimaryInputs << ";" << std::endl;
	output << "        \"_PO_\" = " << noSpecPrimaryOutputs << ";" << std::endl;
	output << "    }" << std::endl;
	output << std::endl;

	output << "Pattern PATTERN {" << std::endl;
	size_t patternId { 0u };
	for (auto const& pattern : patterns)
	{
		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		Simulation::SimulationResult simulation(pattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
		Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, *pattern, {}, simulation, simConfig);

		output << "    Ann {* Pattern " << patternId++ << " *}" << std::endl;
		for (size_t timeframe { 0u }; timeframe < pattern->GetNumberOfTimeframes(); timeframe++)
		{
			output << "    V { \"_PI_\" = " << GetPrimaryInputVectors(primaryInputs, simulation, timeframe) << "; \"_PO_\" = "
				<< GetPrimaryOutputVectors(primaryOutputs, simulation, timeframe) << "}" << std::endl;
		}
		output << std::endl;
	}
	output << "}" << std::endl;
}

void ExportStilPatterns(const Circuit::CircuitEnvironment& circuit, const Pattern::TestPatternList& patterns, StilPatternType type, std::ostream& output)
{
	switch (type)
	{
		case StilPatternType::FullScan:
			ExportStilFullScan(circuit, patterns, output);
			break;
		case StilPatternType::Sequential:
			ExportStilSequential(circuit, patterns, output);
			break;
		default:
			LOG(FATAL) << "Unimplemented STIL export type";
			__builtin_unreachable();
	}
}

};
};
