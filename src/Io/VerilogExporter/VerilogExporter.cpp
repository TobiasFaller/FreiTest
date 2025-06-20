#include "Io/VerilogExporter/VerilogExporter.hpp"

#include <boost/variant.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>

#include <string>
#include <iostream>

#include "Basic/Overload.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

VerilogExporter::VerilogExporter(std::ostream& output):
	_output(output)
{
}

VerilogExporter::~VerilogExporter(void) = default;

bool VerilogExporter::ExportModule(const Verilog::Module& module)
{
	const auto size_to_string = [&](const Verilog::VectorSize& size) -> std::string {
		return "[" + std::to_string(size.top) + ":" + std::to_string(size.bottom) + "]";
	};

	const auto index_to_string = [&](const Verilog::VectorSize& size) -> std::string {
		return (size.top != size.bottom)
			? "[" + std::to_string(size.top) + ":" + std::to_string(size.bottom) + "]"
			: "[" + std::to_string(size.top) + "]";
	};
	const auto port_to_string = [&](const Verilog::PortType& type) -> std::string {
		switch (type)
		{
		case PortType::PORT_IN: return "input";
		case PortType::PORT_OUT: return "output";
		case PortType::PORT_INOUT: return "inout";
		default: return "unknown";
		}
	};

	size_t placeholderId = 0u;
	const auto bus_to_string = [&](const Verilog::Bus& bus) -> std::string {
		std::string returnValue = "{";
		size_t index = 0u;
		for (auto source : bus.sources)
		{
			if (index++ != 0u) returnValue += ", ";
			returnValue += boost::apply_visitor(
				overload(
					// Handling source type "wire reference" here
					[&](const WireReference& reference) -> std::string
					{
						return std::string("\\") + reference.name
							+ " " + index_to_string(reference.size);
					},

					// Handling source type "static value" here
					[&](const VectorInitializer& initializer) -> std::string
					{
						std::string returnValue = std::to_string(initializer.size()) + "'b ";
						for (size_t bit = initializer.size(); bit > 0u; --bit)
						{
							returnValue += to_string(initializer.value[bit - 1u]);
						}

						return returnValue;
					},

					// Handling source type "placeholder" here
					[&](const VectorPlaceholder& placeholder) -> std::string
					{
						return boost::str(boost::format("\\freitest_unconnected_wires [%s]") % placeholderId++);
					}
				),
				source
			);
		}
		returnValue += "}";
		return returnValue;
	};
	const auto count_placeholders = [&](const Verilog::Bus& bus) -> size_t {
		size_t placeholders { 0u };
		for (auto source : bus.sources)
		{
			placeholders += boost::apply_visitor(
				overload(
					[&](const WireReference& reference) -> size_t { return 0u; },
					[&](const VectorInitializer& initializer) -> size_t { return 0u; },
					[&](const VectorPlaceholder& placeholder) -> size_t { return 1u; }
				),
				source
			);
		}
		return placeholders;
	};

	// Module header
	size_t portIndex = 0u;
	_output << "module \\" << module.name << " (";
	for (auto port : module.ports)
	{
		if (portIndex++ != 0u) _output << " ,";

		_output << std::endl << "\t" << port_to_string(port.type)
			<< " " << size_to_string(port.size) << " "
			<< "\\" << port.name;
	}
	_output << std::endl << ");" << std::endl;
	_output << std::endl;

	// Module wire list
	_output << "\t// Wires" << std::endl;
	for (auto wire : module.wires)
	{
		_output << "\t" << "wire"
			<< " " << size_to_string(wire.size) << " "
			<< "\\" << wire.name << " ;" << std::endl;
	}
	_output << std::endl;

	// Module placeholder list (unconnected wires)
	size_t placeholders { 0u };
	for (auto assignment : module.assignments)
	{
		placeholders += count_placeholders(assignment.source);
		placeholders += count_placeholders(assignment.target);
	}
	for (auto instantiation : module.instantiations)
	{
		for (auto portAssignment : instantiation.ports)
		{
			placeholders += count_placeholders(portAssignment.bus);
		}
	}
	_output << "\t// Unconnected wires" << std::endl;
	if (placeholders != 0u)
	{
		_output << "\twire [" << (placeholders - 1u)
			<< ":0] \\freitest_unconnected_wires ;" << std::endl;
	}
	_output << std::endl;

	// Assignments
	_output << "\t// Assignments" << std::endl;
	for (auto assignment : module.assignments)
	{
		_output << "\t" << "assign "
			<< bus_to_string(assignment.target)
			<< " = "
			<< bus_to_string(assignment.source)
			<< ";" << std::endl;
	}
	_output << std::endl;

	// Instantiations
	_output << "\t// Instantiations" << std::endl;
	for (auto instantiation : module.instantiations)
	{
		_output << "\t\\" << instantiation.type
			<< " \\" << instantiation.name << " "
			<< "(";

		size_t portAssignmentIndex = 0u;
		for (auto portAssignment : instantiation.ports)
		{
			if (portAssignmentIndex++ != 0u) _output << ",";
			_output << std::endl << "\t\t" << "." << portAssignment.name
				<< "(" << bus_to_string(portAssignment.bus)
				<< ")";
		}

		_output << std::endl << "\t" << ");" << std::endl;
	}
	_output << std::endl;

	_output << "endmodule" << std::endl;

	return _output.good();
}

};
};
};
