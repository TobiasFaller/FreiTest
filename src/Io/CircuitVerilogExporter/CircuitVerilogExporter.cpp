#include "Io/CircuitVerilogExporter/CircuitVerilogExporter.hpp"

#include <set>
#include <boost/algorithm/string.hpp>

#include "Circuit/CircuitEnvironment.hpp"
#include "Io/VerilogExporter/VerilogExporter.hpp"

using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Io
{

CircuitVerilogExporter::CircuitVerilogExporter(void) = default;
CircuitVerilogExporter::~CircuitVerilogExporter(void) = default;

void CircuitVerilogExporter::BuildModule(const Circuit::CircuitEnvironment& circuitEnvironment, const GroupMetaData* group)
{
	auto sourceInfo = group->GetSourceInfo();
	auto name = sourceInfo.GetProperty<std::string>("module-name").value_or("");
	auto type = sourceInfo.GetProperty<std::string>("module-type").value_or("");

	Verilog::Module module { group->GetHierarchyName() };
	VLOG(3) << "Module: " << name << " (" << type << ")";

	const auto get_constant_value = [](auto const& gate) -> Verilog::VectorBit {
		const auto type = gate.GetSourceInfo().template GetProperty<std::string>("constant-value").value_or("U");
		if (type == "0") return Verilog::VectorBit::BIT_0;
		if (type == "1") return Verilog::VectorBit::BIT_1;
		if (type == "X") return Verilog::VectorBit::BIT_X;
		return Verilog::VectorBit::BIT_U;
	};
	const auto get_port_type = [](auto const& port) -> Verilog::PortType {
		const auto type = port.GetSourceInfo().template GetProperty<std::string>("port-type").value_or("unknown");
		if (type == "input") return Verilog::PortType::PORT_IN;
		if (type == "output") return Verilog::PortType::PORT_OUT;
		if (type == "inout") return Verilog::PortType::PORT_INOUT;
		return Verilog::PortType::PORT_UNKNOWN;
	};
	const auto get_size = [](auto const& element) -> Verilog::VectorSize {
		auto bottom = element.GetSize().GetBottom();
		auto top = element.GetSize().GetTop();
		return Verilog::VectorSize(top, bottom);
	};
	const auto convert_wire_to_connection = [&](const ConnectionMetaData* connection) -> std::optional<Verilog::BusValue> {
		if (connection == nullptr)
		{
			return std::nullopt;
		}
		ASSERT(connection->GetMappedSources().size() != 0) << "There is no driver for a wire";
		ASSERT(connection->GetMappedSources().size() == 1) << "There is more than one driver for a wire";

		for (const auto& port : group->GetPorts())
		{
			for (auto index : port.GetSize().GetIndicesTopToBottom())
			{
				if (const auto& portConnection = port.GetConnectionForIndex(index);
					portConnection != nullptr &&
					portConnection->GetConnectionId() == connection->GetConnectionId())
				{
					return Verilog::WireReference { port.GetName(), index };
				}
			}
		}
		for (const auto& wire : group->GetWires())
		{
			for (auto index : wire.GetSize().GetIndicesTopToBottom())
			{
				if (const auto& wireConnection = wire.GetConnectionForIndex(index);
					wireConnection != nullptr &&
					wireConnection->GetConnectionId() == connection->GetConnectionId())
				{
					return Verilog::WireReference { wire.GetName(), index };
				}
			}
		}

		auto const driverMetaData { circuitEnvironment.GetMetaData()
			.GetGroup(connection->GetMappedSources()[0u].node) };
		if (driverMetaData->GetSourceInfo().sourceType == "constant")
		{
			return Verilog::VectorInitializer { { get_constant_value(*driverMetaData) } };
		}

		return std::nullopt;
	};
	const auto convert_connections_to_bus = [&](auto const* port) -> Verilog::Bus {
		ASSERT(port != nullptr) << "Port should not be null";

		std::vector<Verilog::BusValue> wires;
		for (auto const* connection : port->GetConnections())
		{
			const auto wire = convert_wire_to_connection(connection);
			wires.emplace_back(wire.has_value()
				? wire.value()
				: Verilog::VectorPlaceholder {});
		}
		return { wires };
	};

	// -------------------------------------------------------------------
	// 1. Create the ports => group.GetPorts()
	// 2. Create the wires => group.GetWires()
	// 3. Create the instantiations with the submodules
	// 4. Create the assignments with the submodules
	// -------------------------------------------------------------------
	for (const auto& port: group->GetPorts())
	{
		module.ports.emplace_back(port.GetName(), get_port_type(port), get_size(port));
	}
	for (const auto& wire: group->GetWires())
	{
		module.wires.emplace_back(wire.GetName(), get_size(wire));
	}
	for (auto subGroup: group->GetSubGroups())
	{
		auto sourceInfo = subGroup->GetSourceInfo();
		if (sourceInfo.sourceType == "module")
		{
			Verilog::PortMappingList portMapping;
			for (auto port: subGroup->GetPorts())
			{
				Verilog::Bus bus { convert_connections_to_bus(&port) };
				portMapping.emplace_back(port.GetName(), bus);
			}

			const auto instance = sourceInfo.GetProperty<std::string>("module-name").value_or("NO-NAME");
			const auto instanceType = sourceInfo.GetProperty<std::string>("module-type").value_or("NO-TYPE");
			module.instantiations.push_back(Verilog::Instantiation { instance, instanceType, {}, portMapping });
		}
		else if (sourceInfo.sourceType == "assignment")
		{
			Verilog::Bus source { convert_connections_to_bus(subGroup->GetPort("source")) };
			Verilog::Bus target { convert_connections_to_bus(subGroup->GetPort("target")) };
			module.assignments.emplace_back(source, target);
		}
	}

	modules.push_back(module);
	for (auto subGroup: group->GetSubGroups())
	{
		const auto& sourceInfo = subGroup->GetSourceInfo();
		if (sourceInfo.sourceType != "module"
			|| sourceInfo.GetProperty<bool>("module-is-cell").value_or(false)
			|| sourceInfo.GetProperty<bool>("module-is-primitive").value_or(false))
		{
			continue;
		}

		BuildModule(circuitEnvironment, subGroup);
	}
}

void CircuitVerilogExporter::ExportCircuit(const Circuit::CircuitEnvironment& circuitEnvironment, std::ostream& out)
{
	auto& metaData = circuitEnvironment.GetMetaData();

	const auto* root = metaData.GetRoot();
	BuildModule(circuitEnvironment, root);

	Io::Verilog::VerilogExporter exporter(out);
	for (auto module: modules)
	{
		exporter.ExportModule(module);
	}
}

};
};
