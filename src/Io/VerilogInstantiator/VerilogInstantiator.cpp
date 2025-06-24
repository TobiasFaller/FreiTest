#include "Io/VerilogInstantiator/VerilogInstantiator.hpp"

#include <boost/format.hpp>
#include <boost/variant/apply_visitor.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <tuple>
#include <variant>

#include "Basic/Logging.hpp"
#include "Basic/Overload.hpp"
#include "Io/VerilogInstantiator/Primitive.hpp"
#include "Io/VerilogInstantiator/InstantiationContext.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

using namespace std;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Io::Verilog::Instance;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

struct FlattenedName
{
	FlattenedName(std::string name, VectorSize size, std::vector<size_t> connections, PortType type = PortType::PORT_UNKNOWN):
		name(name),
		size(size),
		type(type),
		connections(connections)
	{
	}

	std::string name;
	VectorSize size;
	PortType type;
	std::vector<size_t> connections;
};

struct flattened_index_tag {};
using FlattenedNames = boost::multi_index_container<
	FlattenedName,
	boost::multi_index::indexed_by<
		boost::multi_index::random_access<>,
		boost::multi_index::hashed_unique<
			boost::multi_index::tag<flattened_index_tag>,
			boost::multi_index::member<FlattenedName, std::string, &FlattenedName::name>
		>
	>
>;

struct PassedContext
{
	FlattenedNames ports;
};

struct NameContext
{
	FlattenedNames ports;
	FlattenedNames wires;
};

static bool InstantiateModule(InstantiationContext& context, const Instantiation& instantiation, const PassedContext& passed, const Module& module);
static bool InstantiatePrimitive(InstantiationContext& context, const Instantiation& instantiation, const PassedContext& passed, std::shared_ptr<Primitives::Primitive> primitive);
static bool InstantiatePorts(InstantiationContext& context, const Module& module, const PassedContext& passed, NameContext& outNames);
static bool InstantiateWires(InstantiationContext& context, const Module& module, const PassedContext& passed, NameContext& outNames);

static std::string MapBusToString(InstantiationContext& context, const NameContext& names, const Bus& source);
static bool MapBusToConnections(InstantiationContext& context, const NameContext& names, const Bus& source, std::vector<size_t>& outConnections);
static bool MapInstantiationToConnections(InstantiationContext& context, const NameContext& names, const Instantiation& instantiation, const Module& module, PassedContext& outPorts);

// Extend this list to globally mute printing warnings regarding these attributes
const std::vector<std::string> KNOWN_ATTRIBUTES = { "top", "src", "module_not_derived", "unused_bits", "keep_hierarchy", "hdlname", "wiretype", "force_downto" };
const std::vector<std::string> KNOWN_PARAMETERS = { };
template<typename Collection>
static bool ReportUnknownElements(const Collection& elements, const std::vector<std::string>& known, const std::string& message);

static std::vector<size_t> IterateVectorTopToDown(const VectorSize& vector);
static std::vector<size_t> IterateVectorMinToMax(const VectorSize& vector);
static std::string PortTypeToString(PortType type);
static Instance::PortType PortTypeToInstance(PortType type);

const size_t PORT_NO_INDEX { std::numeric_limits<size_t>::max() };
const size_t WIRE_NO_INDEX { std::numeric_limits<size_t>::max() };
enum class GroupType { Module, Cell, Primitive, Virtual };
static void AddPort(InstantiationContext& context, const std::string& name, size_t top, size_t bottom, Instance::PortType type, size_t index, const std::vector<size_t>& connections);
static size_t AddConstant(InstantiationContext& context,  VectorBit value);
static void AddPortInfo(SourceInformation& info, const std::string& name, size_t top, size_t bottom, Instance::PortType type, size_t index);
static void AddWireInfo(SourceInformation& info, const std::string& name, size_t top, size_t bottom, size_t index);
static void AddGeneralInfo(SourceInformation& info, const std::string& name, const std::string& type, GroupType category, const std::string& subCategory);
static void AddAttributeInfo(SourceInformation& info, const AttributeCollection& attributes);

VerilogInstantiator::VerilogInstantiator(void) = default;
VerilogInstantiator::~VerilogInstantiator(void) = default;

std::unique_ptr<ModuleInstance> VerilogInstantiator::Instantiate(const Modules& modules, const string& moduleName) const
{
	std::string name;
	if (moduleName == "FIRST_DEFINED" || moduleName == "LAST_DEFINED")
	{
		if (modules.modules.size() == 0u)
		{
			LOG(ERROR) << "There are no modules defined to instantiate";
			return {};
		}

		name = (moduleName == "FIRST_DEFINED")
			? modules.modules.front().name
			: modules.modules.back().name;
	}
	else
	{
		name = moduleName;
	}

	if (auto it = modules.modules.get<name_index_tag>().find(name);
			it != modules.modules.get<name_index_tag>().end())
	{
		const Module& module = *it;
		LOG(INFO) << "Found module " << module.name << " to instantiate";

		if (module.libraryModule)
		{
			LOG(ERROR) << "Module " << name << " to instantiate is declared as library module!";
			return {};
		}

		DVLOG(5) << "Instantiating module " << module;
		InstantiationContext context { modules };
		const PrefixedScopeGuard instantiationGuard { context, module.name };

		// Create a primary input / output for all top-level ports
		PassedContext passedContext;
		for (auto const& port : module.ports)
		{
			if (port.size.unknown)
			{
				LOG(ERROR) << "The size of the input port " << port.name << " has not been specified";
				return {};
			}

			std::vector<size_t> connections;
			for (size_t index : IterateVectorMinToMax(port.size))
			{
				// Create a cell for the gate
				const std::string portName { boost::str(boost::format("%s [%u]") % port.name % index) };
				const std::string portType { PortTypeToString(port.type) } ;
				const size_t portConnection { connections.emplace_back(context.AddConnection(portName)) };
				const PrefixedScopeGuard cellGuard { context, portName };
				SourceInformation cellInfo { NO_SOURCE_FILE, context.GetPrefix(), portName, "port" };
				AddGeneralInfo(cellInfo, portName, portType, GroupType::Virtual, "port");
				AddPortInfo(cellInfo, portName, port.size.top, port.size.bottom, PortTypeToInstance(port.type), index);
				context.GetCurrentGroup().sourceInfo = cellInfo;
				switch (port.type)
				{
					case PortType::PORT_IN: AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { portConnection }); break;
					case PortType::PORT_OUT: AddPort(context, "in", 0u, 0u, Instance::PortType::Input, PORT_NO_INDEX, { portConnection }); break;
					case PortType::PORT_INOUT: AddPort(context, "inout", 0u, 0u, Instance::PortType::Bidirectional, PORT_NO_INDEX, { portConnection }); break;
					default: Logging::Panic("Unsupported port type");
				}

				// Create a gate for the primitive (inside the cell)
				const std::string primitiveType { PortTypeToString(port.type) } ;
				size_t primitiveInputCount;
				size_t primitiveOutputCount;
				switch (port.type)
				{
					case PortType::PORT_IN: primitiveInputCount = 0; primitiveOutputCount = 1; break;
					case PortType::PORT_OUT: primitiveInputCount = 1; primitiveOutputCount = 1; break;
					case PortType::PORT_INOUT: primitiveInputCount = 1; primitiveOutputCount = 1; break;
					default: Logging::Panic("Unsupported port type");
				}
				auto primitive = context.FindPrimitive(primitiveType, primitiveInputCount + primitiveOutputCount);
				LOG_IF(!primitive, FATAL) << "Could not find port " << primitiveType << " in primitives. Can not create port!";

				PrefixedScopeGuard primitiveGuard { context, portName };
				SourceInformation primitiveInfo { NO_SOURCE_FILE, context.GetPrefix(), portName, "primitive" };
				AddGeneralInfo(primitiveInfo, portName, primitive->name, GroupType::Primitive, "port");
				context.GetCurrentGroup().sourceInfo = primitiveInfo;

				const size_t gate = context.AddGate(primitiveType, primitive, primitiveInputCount, primitiveOutputCount);
				context.gates[gate].sourceInfo = primitiveInfo;
				switch (port.type)
				{
					case PortType::PORT_IN:
						context.connections[portConnection].drivingGates.push_back(gate);
						context.gates[gate].outputs[0] = portConnection;
						AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { portConnection });
						break;

					case PortType::PORT_OUT:
						context.connections[portConnection].drivenGates.push_back(gate);
						context.gates[gate].inputs[0] = portConnection;
						AddPort(context, "in", 0u, 0u, Instance::PortType::Input, PORT_NO_INDEX, { portConnection });
						AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { GATE_NO_CONNECTION }); // Dummy output
						break;

					case PortType::PORT_INOUT:
						context.connections[portConnection].drivingGates.push_back(gate);
						context.connections[portConnection].drivenGates.push_back(gate);
						context.gates[gate].inputs[0] = portConnection;
						context.gates[gate].outputs[0] = portConnection;
						AddPort(context, "in", 0u, 0u, Instance::PortType::Input, PORT_NO_INDEX, { portConnection });
						AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { portConnection });
						break;

					default:
						__builtin_unreachable();
				}
			}
			AddPort(context, port.name, port.size.top, port.size.bottom, PortTypeToInstance(port.type), PORT_NO_INDEX, connections);
			passedContext.ports.emplace_back(port.name, port.size, std::move(connections), port.type);
		}

		// Instantiate the top-level module recursively
		Instantiation instantiation { "top", name };
		if (!InstantiateModule(context, instantiation, passedContext, module))
		{
			return { };
		}

		// If a wire has been marked as unused and no driver exists,
		// then add a driver with constant don't care value.
		for (size_t connectionId { 0u }; connectionId < context.connections.size(); connectionId++)
		{
			if (!context.connections[connectionId].unused
				|| context.connections[connectionId].drivingGates.size() > 0u)
			{
				continue;
			}

			// Connect unused gate to a don't care driver.
			// Note: Take reference to connection only after adding the constant connection.
			auto const newConnectionId { AddConstant(context, VectorBit::BIT_X) };
			auto& oldConnection { context.connections[connectionId] };
			auto& newConnection { context.connections[newConnectionId] };
			for (auto& gateId : oldConnection.drivenGates)
			{
				auto& gate { context.gates[gateId] };
				for (auto& input : gate.inputs)
				{
					if (input == connectionId) input = newConnectionId;
				}
				newConnection.drivenGates.push_back(gateId);

				size_t groupId { gate.group };
				while (true)
				{
					auto& group { context.groups[groupId] };
					for (auto& port : group.ports)
					{
						for (auto& connection : port.connectionIds)
						{
							if (connection == connectionId)
							{
								connection = newConnectionId;
							}
						}
					}
					for (auto& wire : group.wires)
					{
						for (auto& connection : wire.connectionIds)
						{
							if (connection == connectionId)
							{
								connection = newConnectionId;
							}
						}
					}
					if (groupId == 0u)
					{
						break;
					}
					groupId = group.parent;
				}
			}
			oldConnection.drivenGates = { };
		}

		return std::make_unique<ModuleInstance>(name, std::move(context.gates),
			std::move(context.connections), std::move(context.groups));
	}
	else
	{
		LOG(ERROR) << "The top-level module " << name << " was not found";
		LOG(WARNING) << "Modules available:";
		for (auto& module : modules.modules)
		{
			LOG(WARNING) << module.name << (module.libraryModule ? " (library)" : "");
		}
	}
	return { };
}

static bool InstantiateModule(InstantiationContext& context, const Instantiation& instantiation, const PassedContext& passed, const Module& module)
{
	SourceInformation moduleInfo { NO_SOURCE_FILE, context.GetPrefix(), instantiation.name, "module" };
	AddGeneralInfo(moduleInfo, instantiation.name, module.name, module.libraryModule ? GroupType::Cell : GroupType::Module, "generic");
	AddAttributeInfo(moduleInfo, module.attributes);
	AddAttributeInfo(moduleInfo, instantiation.attributes);
	context.GetCurrentGroup().sourceInfo = moduleInfo;

	if (!ReportUnknownElements(module.parameter, KNOWN_PARAMETERS,
		boost::str(boost::format("Module %s of type %s has parameters(s). Parameters are not supported!")
			% context.GetPrefix() % module.name)))
	{
		return false;
	}
	ReportUnknownElements(module.attributes, KNOWN_ATTRIBUTES,
		boost::str(boost::format("Module %s of type %s has unknown attribute(s). These attributes are currently ignored!")
			% context.GetPrefix() % module.name));

	NameContext nameContext;
	if (!InstantiatePorts(context, module, passed, nameContext)
		|| !InstantiateWires(context, module, passed, nameContext))
	{
		return false;
	}

	// ------------------------------------------------------------------------------------------
	// Handle assignment statements
	// ------------------------------------------------------------------------------------------

	size_t assignmentNumber = 0u;
	for (const Assignment& assignment : module.assignments)
	{
		ReportUnknownElements(assignment.attributes, KNOWN_ATTRIBUTES,
			boost::str(boost::format("Assignment %d in %s module %s has unknown attribute(s). These attributes are currently ignored!")
				% (assignmentNumber + 1u) % module.name % context.GetPrefix()));

		vector<size_t> sourceConnections;
		vector<size_t> targetConnections;
		if (!MapBusToConnections(context, nameContext, assignment.source, sourceConnections))
		{
			LOG(ERROR) << "Assignment source in " << module.name << " module " << context.GetPrefix() << " could not be resolved!";
			return false;
		}
		if (!MapBusToConnections(context, nameContext, assignment.target, targetConnections))
		{
			LOG(ERROR) << "Assignment target in " << module.name << " module " << context.GetPrefix() << " could not be resolved!";
			return false;
		}
		if (sourceConnections.size() != targetConnections.size())
		{
			LOG(ERROR) << "Assignment in " << module.name << " module " << context.GetPrefix() << " has unmatched sides!";
			LOG(INFO) << "Source side (" << sourceConnections.size() << " elements): " << MapBusToString(context, nameContext, assignment.source);
			LOG(INFO) << "Target side (" << targetConnections.size() << " elements): " << MapBusToString(context, nameContext, assignment.target);
			return false;
		}

		auto bufferPrimitive = context.FindPrimitive("buf", 2u);
		LOG_IF(!bufferPrimitive, FATAL) << "Could not find buffer primitive in primitive list. Can not create assignment buffer!";

		const string assignmentName { boost::str(boost::format("assignment_%u") % assignmentNumber) };
		PrefixedScopeGuard assignmentGuard { context, assignmentName };
		SourceInformation assignmentInfo { NO_SOURCE_FILE, context.GetPrefix(), assignmentName, "assignment" };
		AddGeneralInfo(assignmentInfo, assignmentName, "assignment", GroupType::Virtual, "assignment");
		AddAttributeInfo(assignmentInfo, assignment.attributes);
		assignmentInfo.AddProperty<size_t>("assignment-number", assignmentNumber);
		assignmentInfo.AddProperty<size_t>("assignment-size", sourceConnections.size());
		context.GetCurrentGroup().sourceInfo = assignmentInfo;

		AddPort(context, "source", sourceConnections.size() - 1u, 0u, Instance::PortType::Input, PORT_NO_INDEX, sourceConnections);
		AddPort(context, "target", targetConnections.size() - 1u, 0u, Instance::PortType::Output, PORT_NO_INDEX, targetConnections);

		for (size_t index = 0; index < sourceConnections.size(); ++index)
		{
			const size_t sourceConnection = (sourceConnections[index] != GATE_NO_CONNECTION)
				? sourceConnections[index]
				: context.AddConnection(boost::str(boost::format("assignment_%d_source [%d]") % assignmentNumber % index));
			const size_t targetConnection = (targetConnections[index] != GATE_NO_CONNECTION)
				? targetConnections[index]
				: context.AddConnection(boost::str(boost::format("assignment_%d_target [%d]") % assignmentNumber % index));

			const std::string bufferName { boost::str(boost::format("assignment_%d_buffer_%d") % assignmentNumber % index) };
			const PrefixedScopeGuard bufferGuard { context, bufferName };
			SourceInformation bufferInfo { NO_SOURCE_FILE, context.GetPrefix(), bufferName, "primitive" };
			AddGeneralInfo(bufferInfo, bufferName, bufferPrimitive->name, GroupType::Primitive, "assignment");
			context.GetCurrentGroup().sourceInfo = bufferInfo;

			AddPort(context, "in", 0u, 0u, Instance::PortType::Input, index, { sourceConnection });
			AddPort(context, "out", 0u, 0u, Instance::PortType::Output, index, { targetConnection });

			const size_t gate = context.AddGate(bufferName, bufferPrimitive, 1, 1);
			context.gates[gate].sourceInfo = bufferInfo;
			if (sourceConnection != GATE_NO_CONNECTION)
			{
				context.connections[sourceConnection].drivenGates.push_back(gate);
				context.gates[gate].inputs[0] = sourceConnection;
			}
			if (targetConnection != GATE_NO_CONNECTION)
			{
				context.connections[targetConnection].drivingGates.push_back(gate);
				context.gates[gate].outputs[0] = targetConnection;
			}
		}

		assignmentNumber++;
	}

	// ------------------------------------------------------------------------------------------
	// Handle module instantiations
	// ------------------------------------------------------------------------------------------

	for (const Instantiation& instantiation : module.instantiations)
	{
		PrefixedScopeGuard instantiationGuard { context, instantiation.name };

		if (!ReportUnknownElements(instantiation.parameter, KNOWN_PARAMETERS,
			boost::str(boost::format("Instantiation %s of type %s in %s has parameters(s). Parameters are not supported!")
				% instantiation.name % instantiation.type % context.GetPrefix())))
		{
			return false;
		}
		ReportUnknownElements(instantiation.attributes, KNOWN_ATTRIBUTES,
			boost::str(boost::format("Instantiation %s of type %s in %s has unknown attribute(s). These attributes are currently ignored!")
				% instantiation.name % instantiation.type % context.GetPrefix()));

		if (module.libraryModule)
		{
			// Only allow predefined components (primitives) for library modules
			if (auto it = context.modules.primitives.get<Primitives::primitive_name_and_ports_tag>().find(std::make_pair(instantiation.type, instantiation.ports.size()));
				it != context.modules.primitives.get<Primitives::primitive_name_and_ports_tag>().end())
			{
				if (context.modules.primitives.get<Primitives::primitive_name_and_ports_tag>().count(std::make_pair(instantiation.type, instantiation.ports.size())) > 1u)
				{
					LOG(ERROR) << "Could not decide on implementation for " << instantiation.type << " primitive " << instantiation.name
						<< " in " << module.name << " module " << context.GetPrefix() << " as multiple implementations exist! "
						<< " Potentially the same technology library source was included >1 times in the settings file";
					return false;
				}

				PassedContext passedContext;
				if (!MapInstantiationToConnections(context, nameContext, instantiation, *it->second, passedContext))
				{
					LOG(ERROR) << "Could not map ports for " << instantiation.type << " primitive " << instantiation.name
						<< " in " << module.name << " module " << context.GetPrefix() << "!";
					return false;
				}
				if (!InstantiatePrimitive(context, instantiation, passedContext, it->second))
				{
					LOG(ERROR) << "Could not instantiate " << instantiation.type << " primitive " << instantiation.name
						<< " in " << module.name << " module " << context.GetPrefix() << "!";
					return false;
				}
			}
			else
			{
				LOG(ERROR) << "Could not instantiate " << instantiation.type << " primitive " << instantiation.name
					<< " in " << module.name << " module " << context.GetPrefix() << " which was not found in the primitive library!";
				return false;
			}
		}
		else if (auto it = context.modules.modules.get<name_index_tag>().find(instantiation.type);
				it != context.modules.modules.get<name_index_tag>().end())
		{
			if (context.modules.modules.get<name_index_tag>().count(instantiation.type) > 1u)
			{
				LOG(ERROR) << "Could not decide on implementation for " << instantiation.type << " submodule " << instantiation.name
					<< " in " << module.name << " module " << context.GetPrefix() << " as multiple implementations exist! "
					<< " Potentially the same verilog source was included >1 times in the settings file";
				return false;
			}

			PassedContext passedContext;
			if (!MapInstantiationToConnections(context, nameContext, instantiation, *it, passedContext))
			{
				LOG(ERROR) << "Could not map ports for " << instantiation.type << " submodule " << instantiation.name
					<< " in " << module.name << " module " << context.GetPrefix() << "!";
				return false;
			}
			if (!InstantiateModule(context, instantiation, passedContext, *it))
			{
				LOG(ERROR) << "Could not instantiate " << instantiation.type << " submodule " << instantiation.name
					<< " in " << module.name << " module " << context.GetPrefix() << " which was found in the module list!";
				return false;
			}
		}
		else
		{
			LOG(ERROR) << "Could not instantiate " << instantiation.type << " submodule " << instantiation.name
				<< " in " << module.name << " module " << context.GetPrefix() << ". It was not in the module list!";
			return false;
		}
	}

	return true;
}

static bool InstantiatePrimitive(InstantiationContext& context, const Instantiation& instantiation, const PassedContext& passed, std::shared_ptr<Primitives::Primitive> primitive)
{
	SourceInformation primitiveInfo { NO_SOURCE_FILE, context.GetPrefix(), instantiation.name, "primitive" };
	AddGeneralInfo(primitiveInfo, instantiation.name, primitive->name, GroupType::Primitive, "gate");
	AddAttributeInfo(primitiveInfo, instantiation.attributes);
	context.GetCurrentGroup().sourceInfo = primitiveInfo;

	NameContext nameContext;
	if (!InstantiatePorts(context, *primitive, passed, nameContext))
	{
		return false;
	}

	const size_t gate = context.AddGate(instantiation.name, primitive, primitive->inputPorts.size(), primitive->outputPorts.size());
	context.gates[gate].sourceInfo = primitiveInfo;
	ASSERT((primitive->inputPorts.size() + primitive->outputPorts.size()) == primitive->ports.size());

	size_t inputIndex = 0u;
	size_t outputIndex = 0u;
	for (auto const& port : primitive->ports)
	{
		if (auto it = nameContext.ports.get<flattened_index_tag>().find(port.name);
				it != nameContext.ports.get<flattened_index_tag>().end())
		{
			if (it->connections.size() != 1)
			{
				LOG(ERROR) << "The port " << port.name << " of " << instantiation.type << " gate "
					<< context.GetPrefix() << " is connected to bus of size " << it->connections.size()
					<< " which is not supported for primitives. Only single-wire connections are supported!";
				return false;
			}

			const size_t connectionId = it->connections[0];
			switch (port.type)
			{
				case PortType::PORT_IN:
					context.gates[gate].inputs[inputIndex] = connectionId;
					if (connectionId != GATE_NO_CONNECTION)
					{
						context.connections[connectionId].drivenGates.push_back(gate);
					}
					break;
				case PortType::PORT_OUT:
					context.gates[gate].outputs[outputIndex] = connectionId;
					if (connectionId != GATE_NO_CONNECTION)
					{
						context.connections[connectionId].drivingGates.push_back(gate);
					}
					break;
				default:
					Logging::Panic("Unknown port type specified");
					break;
			}
		}
		else
		{
			LOG(ERROR) << "The port " << port.name << " of " << instantiation.type << " gate "
				<< context.GetPrefix() << " is not connected!";
			return false;
		}

		switch (port.type)
		{
			case PortType::PORT_IN: inputIndex++; break;
			case PortType::PORT_OUT: outputIndex++; break;
			default: Logging::Panic("Unknown port type specified");
		}
	}

	return true;
}

static bool InstantiatePorts(InstantiationContext& context, const Module& module, const PassedContext& passed, NameContext& outNames)
{
	for (auto const& port : passed.ports)
	{
		if (auto it = module.ports.get<name_index_tag>().find(port.name);
			it == module.ports.get<name_index_tag>().end())
		{
			LOG(ERROR) << "The port " << port.name << "was not found in " << module.name
				<< " during insantiation of module " << context.GetPrefix() << ".";
			return false;
		}
	}

	for (auto const& port : module.ports)
	{
		std::vector<size_t> connections(port.size.size(), GATE_NO_CONNECTION);
		if (auto it = passed.ports.get<flattened_index_tag>().find(port.name);
			it != passed.ports.get<flattened_index_tag>().end())
		{
			connections = it->connections;
			if (connections.size() < port.size.size())
			{
				connections.resize(port.size.size(), GATE_NO_CONNECTION);
			}
		}
		for (auto index : IterateVectorMinToMax(port.size))
		{
			if (connections[index - port.size.min()] == GATE_NO_CONNECTION)
			{
				const auto name { boost::str(boost::format("%s [%d]") % port.name % index) };
				connections[index - port.size.min()] = context.AddConnection(name);
			}
		}
		assert(connections.size() == port.size.size());
		outNames.ports.emplace_back(port.name, port.size, connections, port.type);
		AddPort(context, port.name, port.size.top, port.size.bottom, PortTypeToInstance(port.type), PORT_NO_INDEX, connections);
	}

	return true;
}

static bool InstantiateWires(InstantiationContext& context, const Module& module, const PassedContext& passed, NameContext& names)
{
	(void) passed;
	for (auto const& wire : module.wires)
	{
		ReportUnknownElements(wire.attributes, KNOWN_ATTRIBUTES,
			boost::str(boost::format("Wire %s in module %s has unknown attribute(s). These attributes are currently ignored!") % wire.name % context.GetPrefix()));

		std::vector<size_t> connections;
		for (size_t index : IterateVectorMinToMax(wire.size))
		{
			const std::string name { boost::str(boost::format("%s [%u]") % wire.name % index) };
			connections.push_back(context.AddConnection(name));
		}
		names.wires.emplace_back(wire.name, wire.size, connections);

		SourceInformation wireInfo { NO_SOURCE_FILE, context.GetPrefix(), wire.name, "wire" };
		AddWireInfo(wireInfo, wire.name, wire.size.top, wire.size.bottom, WIRE_NO_INDEX);
		AddAttributeInfo(wireInfo, wire.attributes);
		context.GetCurrentGroup().wires.emplace_back(wire.name, std::make_pair(wire.size.top, wire.size.bottom), wireInfo, connections);

		if (auto unused { wireInfo.GetProperty<std::vector<size_t>>("attribute-unused-bits") }; unused.has_value())
		{
			for (size_t index : IterateVectorMinToMax(wire.size))
			{
				if (std::find(unused->begin(), unused->end(), index) != unused->end())
				{
					context.connections[connections[index - wire.size.min()]].unused = true;
				}
			}
		}
	}
	return true;
}

static bool MapBusToConnections(InstantiationContext& context, const NameContext& names, const Bus& source, vector<size_t>& outConnections)
{
	for (const BusValue& sourceValue : source.sources)
	{
		bool assignResultOk = boost::apply_visitor(overload(
			[&](const WireReference& reference) -> bool {
				DVLOG(5) << "Mapping reference " << reference;
				if (auto portIt = names.ports.get<flattened_index_tag>().find(reference.name);
						portIt != names.ports.get<flattened_index_tag>().end())
				{
					DVLOG(7) << "Found port that matches name " << reference.name;
					const VectorSize& portSize = portIt->size;
					const VectorSize& referenceSize = reference.size;
					const VectorSize& assignmentSize = referenceSize.unknown ? portSize : referenceSize;
					if (portSize.unknown)
					{
						LOG(ERROR) << "Can not map port " << portIt->name << " which has an unknown size in " << context.GetPrefix() << "."
							<< " Maybe an input / output specifier or a size is missing.";
						return false;
					}

					if (assignmentSize.max() > portSize.max() || assignmentSize.min() < portSize.min())
					{
						LOG(ERROR) << "The port " << reference.name << " with indices from " << assignmentSize.top << " to " << assignmentSize.bottom
							<< " was referenced in a submodule of " << context.GetPrefix()
							<< ", but the port is only specified from " << portSize.top << " to " << portSize.bottom << ".";
						return false;
					}

					for (size_t index : IterateVectorTopToDown(assignmentSize))
					{
						outConnections.push_back(portIt->connections[index - portSize.min()]);
					}
					return true;
				}
				if (auto wireIt = names.wires.get<flattened_index_tag>().find(reference.name);
						wireIt != names.wires.get<flattened_index_tag>().end())
				{
					DVLOG(7) << "Found wire that matches name " << reference.name;
					const VectorSize& wireSize = wireIt->size;
					const VectorSize& referenceSize = reference.size;
					const VectorSize& assignmentSize = referenceSize.unknown ? wireSize : referenceSize;

					if (assignmentSize.max() > wireSize.max() || assignmentSize.min() < wireSize.min())
					{
						LOG(ERROR) << "The wire " << reference.name << " with indices from " << assignmentSize.top << " to " << assignmentSize.bottom
							<< " was referenced in a submodule of " << context.GetPrefix()
							<< ", but the wire is only specified from " << wireSize.top << " to " << wireSize.bottom << ".";
						return false;
					}

					for (size_t index : IterateVectorTopToDown(assignmentSize))
					{
						outConnections.push_back(wireIt->connections[index - wireSize.min()]);
					}
					return true;
				}

				LOG(ERROR) << "The reference " << reference.name << " was used in " << context.GetPrefix()
						<< ", but no corresponding port or wire with that name was found.";
				return false;
			},
			[&](const VectorInitializer& initializer) -> bool {
				DVLOG(5) << "Mapping constant vector " << initializer;
				for (size_t bit = initializer.size(); bit > 0u; --bit)
				{
					DVLOG(5) << "Mapping constant value " << to_string(initializer[bit - 1u]) << " for bit " << bit - 1u;
					outConnections.push_back(AddConstant(context, initializer[bit - 1u]));
				}
				return true;
			},
			[&](const VectorPlaceholder& placeholder) -> bool {
				LOG(ERROR) << "Placeholder is not expected in Verilog source";
				return false;
			}
		), sourceValue);

		// Check if assignment was successful
		if (!assignResultOk)
		{
			return false;
		}
	}

	return true;
}

static std::string MapBusToString(InstantiationContext& context, const NameContext& names, const Bus& source)
{
	std::string result;
	for (const BusValue& sourceValue : source.sources)
	{
		if (result != "")
		{
			result += ", ";
		}

		result += boost::apply_visitor(overload(
			[&](const WireReference& reference) -> std::string {
				if (auto portIt = names.ports.get<flattened_index_tag>().find(reference.name);
						portIt != names.ports.get<flattened_index_tag>().end())
				{
					const VectorSize& portSize = portIt->size;
					const VectorSize& referenceSize = reference.size;
					const VectorSize& assignmentSize = referenceSize.unknown ? portSize : referenceSize;
					if (portSize.unknown || assignmentSize.max() > portSize.max() || assignmentSize.min() < portSize.min())
					{
						return "<error>";
					}
					return reference.name + "[" + std::to_string(portSize.top) + ":" + std::to_string(portSize.bottom) + "]";
				}
				if (auto wireIt = names.wires.get<flattened_index_tag>().find(reference.name);
						wireIt != names.wires.get<flattened_index_tag>().end())
				{
					const VectorSize& wireSize = wireIt->size;
					const VectorSize& referenceSize = reference.size;
					const VectorSize& assignmentSize = referenceSize.unknown ? wireSize : referenceSize;
					if (wireSize.unknown || assignmentSize.max() > wireSize.max() || assignmentSize.min() < wireSize.min())
					{
						return "<error>";
					}
					return reference.name + "[" + std::to_string(wireSize.top) + ":" + std::to_string(wireSize.bottom) + "]";
				}

				return reference.size.unknown ? "?'b <error>" : (std::to_string(reference.size.size()) + "'b <error>");
			},
			[&](const VectorInitializer& initializer) -> std::string {
				std::string result;
				result += std::to_string(initializer.size());
				result += "b' ";
				for (size_t bit = initializer.size(); bit > 0u; --bit)
				{
					result += to_string(initializer.value[0u]);
				}
				return result;
			},
			[&](const VectorPlaceholder& placeholder) -> std::string {
				return "{}";
			}
		), sourceValue);
	}
	return result;
}


static bool MapInstantiationToConnections(InstantiationContext& context, const NameContext& names, const Instantiation& instantiation, const Module& module, PassedContext& outPorts)
{
	size_t portIndex = 0;
	for (auto mappingIt = instantiation.ports.cbegin(); mappingIt != instantiation.ports.cend(); ++mappingIt, ++portIndex)
	{
		const Port* subPort;

		// Search port by name or by index
		if (mappingIt->name == "")
		{
			// Search port by index in module
			if (portIndex >= module.ports.size())
			{
				LOG(ERROR) << "Instantiation of module " << instantiation.type << " with name "
					<< instantiation.name << " tried to assign to port number " << portIndex
					<< " even though the module has only " << module.ports.size() << " ports!";
				return false;
			}

			subPort = &(module.ports[portIndex]);
		}
		else
		{
			// Search port by name in module
			if (auto subPortIt = module.ports.get<name_index_tag>().find(mappingIt->name);
					subPortIt != module.ports.get<name_index_tag>().end())
			{
				subPort = &(*subPortIt);
			}
			else
			{
				LOG(ERROR) << "Instantiation of module " << instantiation.type << " with name "
					<< instantiation.name << " tried to assign to port " << mappingIt->name
					<< " which does not exist!";
				return false;
			}
		}

		vector<size_t> connections;
		if (!MapBusToConnections(context, names, mappingIt->bus, connections))
		{
			LOG(ERROR) << "Assignment source in module " << module.name << " could not be resolved!";
			return false;
		}

		if (connections.size() < subPort->size.size())
		{
			LOG(WARNING) << "Instantiation of module " << instantiation.type << " with name "
				<< instantiation.name << " has less wires (" << connections.size() << ") for port "
				<< subPort->name << " than required " << subPort->size.size()
				<< ". Extending assignments to match port size!";

			for (size_t index = connections.size(); index < subPort->size.size(); ++index)
			{
				// Prepend missing connections
				connections.insert(connections.begin(),
					context.AddConnection(boost::str(boost::format("%s [%d]") % subPort->name %  index)));
			}
		}

		if (outPorts.ports.get<flattened_index_tag>().find(subPort->name) != outPorts.ports.get<flattened_index_tag>().end())
		{
			LOG(ERROR) << "Instantiation of module " << instantiation.type << " with name "
				<< instantiation.name << " has port (" << subPort->name << ") that has been assigned multiple times!";
			return false;
		}

		outPorts.ports.emplace_back(subPort->name, subPort->size, connections, subPort->type);
	}

	// Add missing ports
	for (auto& port : module.ports)
	{
		if (outPorts.ports.get<flattened_index_tag>().find(port.name) == outPorts.ports.get<flattened_index_tag>().end())
		{
			std::vector<size_t> connections;
			for (auto index : IterateVectorMinToMax(port.size))
			{
				connections.push_back(context.AddConnection(boost::str(boost::format("%s [%d]") % port.name %  index)));
			}
			outPorts.ports.emplace_back(port.name, port.size, connections, port.type);
		}
	}

	return true;
}

static std::vector<size_t> IterateVectorTopToDown(const VectorSize& vector)
{
	std::vector<size_t> result;
	for (size_t index { vector.top }; (index >= vector.min()) && (index <= vector.max());
		index += (vector.top > vector.bottom) ? -1 : 1)
	{
		result.push_back(index);
	}
	return result;
}

static std::vector<size_t> IterateVectorMinToMax(const VectorSize& vector)
{
	std::vector<size_t> result;
	for (size_t index { vector.min() }; index <= vector.max(); index++)
	{
		result.push_back(index);
	}
	return result;
}

template<typename Collection>
static bool ReportUnknownElements(const Collection& elements, const std::vector<std::string>& known, const std::string& message)
{
	if (std::count_if(elements.begin(), elements.end(), [&](const auto& element) -> bool {
			return std::find(std::begin(known), std::end(known), element.name) == std::end(known);
		}) == 0)
	{
		return true;
	}

	LOG(WARNING) << message;
	for (auto& element : elements)
	{
		if (std::find(std::begin(known), std::end(known), element.name) == std::end(known))
		{
			LOG(WARNING) << element.name << ": " << element.value;
		}
	}
	return false;
}

static size_t AddConstant(InstantiationContext& context,  VectorBit value)
{
	switch (value)
	{
		case VectorBit::BIT_0: if (context.constant0Id != CONNECTION_INVALID_ID) return context.constant0Id; break;
		case VectorBit::BIT_1: if (context.constant1Id != CONNECTION_INVALID_ID) return context.constant1Id; break;
		case VectorBit::BIT_X: if (context.constantXId != CONNECTION_INVALID_ID) return context.constantXId; break;
		case VectorBit::BIT_U: if (context.constantUId != CONNECTION_INVALID_ID) return context.constantUId; break;
	}

	const auto primitive = context.FindPrimitive(boost::str(boost::format("tie%c") % value), 1u);
	LOG_IF(!primitive, FATAL) << "Could not find constant primitive " << value << " in the primitive list. Can not create constant gate!";

	const std::string cellName { boost::str(boost::format("constant_%c") % value) };
	const std::string cellType { boost::str(boost::format("%c") % value) };
	const PrefixedScopeGuard cellGuard { context, 0u, "constant" };
	SourceInformation cellInfo { NO_SOURCE_FILE, context.GetPrefix(), cellName, "constant" };
	AddGeneralInfo(cellInfo, cellName, cellType, GroupType::Virtual, "constant");
	cellInfo.AddProperty<std::string>("constant-value", cellType);
	cellInfo.AddProperty<std::string>("constant-type", "constant-input");
	context.GetCurrentGroup().sourceInfo = cellInfo;

	const size_t connectionId = context.AddConnection(boost::str(boost::format("constant_%c") % value));
	AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { connectionId });

	const std::string primitiveName { boost::str(boost::format("constant_%c") % value) };
	const std::string primitiveType { boost::str(boost::format("tie%c") % value) };
	const std::string primitiveValue { boost::str(boost::format("%c") % value) };
	const PrefixedScopeGuard primitiveGuard { context, "constant" };
	SourceInformation primitiveInfo { NO_SOURCE_FILE, context.GetPrefix(), primitiveName, "constant" };
	AddGeneralInfo(primitiveInfo, primitiveName, primitiveType, GroupType::Primitive, "constant");
	primitiveInfo.AddProperty<std::string>("constant-type", "constant-input");
	primitiveInfo.AddProperty<std::string>("constant-value", primitiveValue);
	context.GetCurrentGroup().sourceInfo = primitiveInfo;

	AddPort(context, "out", 0u, 0u, Instance::PortType::Output, PORT_NO_INDEX, { connectionId });

	const size_t gateId = context.AddGate(primitiveValue, primitive, 0, 1);
	context.gates[gateId].outputs[0u] = connectionId;
	context.connections[connectionId].drivingGates.push_back(gateId);
	switch (value)
	{
		case VectorBit::BIT_0: context.constant0Id = connectionId; break;
		case VectorBit::BIT_1: context.constant1Id = connectionId; break;
		case VectorBit::BIT_X: context.constantXId = connectionId; break;
		case VectorBit::BIT_U: context.constantUId = connectionId; break;
	}

	return connectionId;
}

static void AddPort(InstantiationContext& context, const std::string& name, size_t top, size_t bottom, Instance::PortType type, size_t index, const std::vector<size_t>& connections)
{
	SourceInformation info { NO_SOURCE_FILE, context.GetPrefix(), name, "port" };
	AddPortInfo(info, name, top, bottom, type, index);
	context.GetCurrentGroup().ports.emplace_back(name, std::make_pair(top, bottom), type, info, connections);
}

static void AddPortInfo(SourceInformation& info, const std::string& name, size_t top, size_t bottom, Instance::PortType type, size_t index)
{
	info.AddProperty<std::string>("port-name", name);
	info.AddProperty<size_t>("port-size", std::max(top, bottom) + 1u - std::min(top, bottom));
	info.AddProperty<size_t>("port-top", top);
	info.AddProperty<size_t>("port-bottom", bottom);
	if (index != PORT_NO_INDEX)
	{
		info.AddProperty<size_t>("port-absolute-index", index);
		info.AddProperty<size_t>("port-relative-index", index - std::min(top, bottom));
	}
	switch (type)
	{
		case Instance::PortType::Input: info.AddProperty<std::string>("port-type", "input"); break;
		case Instance::PortType::Output: info.AddProperty<std::string>("port-type", "output"); break;
		case Instance::PortType::Bidirectional: info.AddProperty<std::string>("port-type", "inout"); break;
		default: Logging::Panic("Invalid port type");
	}
}

static void AddWireInfo(SourceInformation& info, const std::string& name, size_t top, size_t bottom, size_t index)
{
	info.AddProperty<std::string>("wire-name", name);
	info.AddProperty<size_t>("wire-size", std::max(top, bottom) + 1u - std::min(top, bottom));
	info.AddProperty<size_t>("wire-top", top);
	info.AddProperty<size_t>("wire-bottom", bottom);
	if (index != WIRE_NO_INDEX)
	{
		info.AddProperty<size_t>("wire-absolute-index", index);
		info.AddProperty<size_t>("wire-relative-index", index - std::min(top, bottom));
	}
}

static void AddGeneralInfo(SourceInformation& info, const std::string& name, const std::string& type, GroupType category, const std::string& subCategory)
{
	info.AddProperty<std::string>("module-name", name);
	info.AddProperty<std::string>("module-type", type);
	info.AddProperty<bool>("module-is-module", category == GroupType::Module);
	info.AddProperty<bool>("module-is-cell", category == GroupType::Cell);
	info.AddProperty<bool>("module-is-primitive", category == GroupType::Primitive);
	info.AddProperty<bool>("module-is-virtual", category == GroupType::Virtual);

	const auto prefix = [&]() -> std::string {
		switch (category)
		{
			case GroupType::Module: return "module";
			case GroupType::Cell: return "cell";
			case GroupType::Primitive: return "primitive";
			case GroupType::Virtual: return "virtual";
			default: __builtin_unreachable();
		}
	}();
	info.AddProperty<std::string>(prefix + "-name", name);
	info.AddProperty<std::string>(prefix + "-type", type);
	info.AddProperty<std::string>(prefix + "-category", subCategory);
}

static void AddAttributeInfo(SourceInformation& info, const AttributeCollection& attributes)
{
	constexpr auto attribute_to_bool = [](const auto& value) -> bool {
		return boost::apply_visitor(Basic::overload(
			[](const std::string& value) -> bool {
				return value == "true";
			},
			[](const int64_t& value) -> bool {
				return value > 0;
			},
			[](const VectorInitializer& value) -> bool {
				for (size_t index { 0u }; index < value.size(); index++)
				{
					switch (value[index])
					{
						case VectorBit::BIT_1:
							return true;
						case VectorBit::BIT_0:
						case VectorBit::BIT_X:
						case VectorBit::BIT_U:
							continue;
					}
				}

				return false;
			}
		), value);
	};
	constexpr auto attribute_to_string = [](const auto& value) -> std::string {
		return boost::apply_visitor(Basic::overload(
			[](const std::string& value) -> std::string {
				return value;
			},
			[](const int64_t& value) -> std::string {
				return std::to_string(value);
			},
			[](const VectorInitializer& value) -> std::string {
				return to_string(value);
			}
		), value);
	};
	constexpr auto attribute_to_size_t_vector = [](const auto& value) -> std::vector<size_t> {
		return boost::apply_visitor(Basic::overload(
			[](const std::string& value) -> std::vector<size_t> {
				stringstream stream(value);
				std::vector<size_t> indices;
				while(stream.good() && !stream.eof())
				{
					size_t index;
					stream >> index;
					if (!stream.fail())
					{
						indices.push_back(index);
					}
				}
				return indices;
			},
			[](const int64_t& value) -> std::vector<size_t> {
				return { static_cast<size_t>(value) };
			},
			[](const VectorInitializer& value) -> std::vector<size_t> {
				return { };
			}
		), value);
	};

	for (auto& attribute : attributes)
	{
		if (attribute.name == "top") info.AddProperty<bool>("attribute-top", attribute_to_bool(attribute.value));
		if (attribute.name == "src") info.AddProperty<std::string>("attribute-src", attribute_to_string(attribute.value));
		if (attribute.name == "module_not_derived") info.AddProperty<bool>("attribute-module-not-derived", attribute_to_bool(attribute.value));
		if (attribute.name == "unused_bits") info.AddProperty<std::vector<size_t>>("attribute-unused-bits", attribute_to_size_t_vector(attribute.value));
		if (attribute.name == "keep_hierarchy") info.AddProperty<bool>("attribute-keep-hierarchy", attribute_to_bool(attribute.value));
	}
}

static std::string PortTypeToString(PortType type)
{
	switch (type)
	{
		case PortType::PORT_IN: return "input";
		case PortType::PORT_OUT: return "output";
		case PortType::PORT_INOUT: return "inout";
		default: Logging::Panic("Invalid port type");
	}
}

static Instance::PortType PortTypeToInstance(PortType type)
{
	switch (type)
	{
		case PortType::PORT_IN: return Instance::PortType::Input;
		case PortType::PORT_OUT: return Instance::PortType::Output;
		case PortType::PORT_INOUT: return Instance::PortType::Bidirectional;
		default: Logging::Panic("Invalid port type");
	}
}

std::unique_ptr<CircuitEnvironment> VerilogInstantiator::CreateCircuit(const Modules& modules, const ModuleInstance& instance, const Builder::BuildConfiguration& configuration) const
{
	auto builder = CreateBuilder(modules, instance, configuration);
	if (!builder)
	{
		return std::unique_ptr<CircuitEnvironment>();
	}

	LOG(INFO) << "Converting temporary unmapped and mapped circuit to final representation";
	return builder->BuildCircuitEnvironment(configuration);
}

std::unique_ptr<Builder::CircuitBuilder> VerilogInstantiator::CreateBuilder(const Modules& modules, const ModuleInstance& instance, const Builder::BuildConfiguration& configuration) const
{
	std::unique_ptr<Builder::CircuitBuilder> builder = std::make_unique<Builder::CircuitBuilder>();
	builder->SetName(instance.name);

	// ------------------------------------------------------------------------
	// Forward the connection information into the circuit builder
	// ------------------------------------------------------------------------

	std::vector<Builder::ConnectionId> connections;
	for (const Instance::Connection& connection : instance.connections)
	{
		(void) connection;
		connections.push_back(builder->EmplaceConnection());
	}

	// ------------------------------------------------------------------------
	// Forward the group information into the circuit builder
	// ------------------------------------------------------------------------

	// Create a one-to-one copy of the existing groups because we don't know
	// which groups are eliminated further down in the processing chain.
	for (const Instance::Group& group : instance.groups)
	{
		Builder::GroupId groupId = builder->EmplaceGroup(group.name);
		Builder::Group& groupRef = builder->GetGroup(groupId);
		groupRef.SetSourceInfo(builder->AddSourceInfo(group.sourceInfo));
		groupRef.SetParent(group.parent);
	}

	for (Builder::GroupId groupId = 0u; groupId < instance.groups.size(); ++groupId)
	{
		Builder::Group& group = builder->GetGroup(groupId);

		// Copy the list of sub-groups to the newly created groups from above.
		for (const Builder::GroupId& subGroup : instance.groups[groupId].groups)
		{
			group.AddGroup(subGroup);
		}

		// Copy the list of wires to the newly created groups from above.
		for (auto& wire : instance.groups[groupId].wires)
		{
			auto [top, bottom] = wire.size;
			auto [min, max] = std::minmax(top, bottom);
			ASSERT(wire.connectionIds.size() == (max + 1u - min))
				<< "Connection count and wire size does not match for wire " << wire.name << " of group " << group.name;

			Builder::WireId wireId = group.EmplaceWire(wire.name);
			Builder::Wire& wireRef = group.GetWire(wireId);
			wireRef.SetSize(wire.size);
			wireRef.SetSourceInfo(builder->AddSourceInfo(wire.sourceInfo));
			wireRef.SetConnections(wire.connectionIds);
		}

		// Copy the list of ports to the newly created groups from above.
		for (auto& port : instance.groups[groupId].ports)
		{
			auto [top, bottom] = port.size;
			auto [min, max] = std::minmax(top, bottom);
			ASSERT(port.connectionIds.size() == (max + 1u - min))
				<< "Connection count and port size does not match for port " << port.name << " of group " << group.name;

			const auto map_port_type = [](auto type) {
				switch (type)
				{
					case Instance::PortType::Input: return Builder::PortType::Input;
					case Instance::PortType::Output: return Builder::PortType::Output;
					case Instance::PortType::Bidirectional: return Builder::PortType::Bidirectional;
					default: __builtin_unreachable();
				}
			};

			Builder::PortId portId = group.EmplacePort(port.name, map_port_type(port.type));
			Builder::Port& portRef = group.GetPort(portId);
			portRef.SetSize(port.size);
			portRef.SetSourceInfo(builder->AddSourceInfo(port.sourceInfo));
			portRef.SetConnections(port.connectionIds);
		}
	}

	// ------------------------------------------------------------------------
	// Convert all primitives to circuit elements
	// ------------------------------------------------------------------------

	// Prepare the primitive contexts
	std::vector<Primitives::PrimitiveContext> primitiveContexts;
	primitiveContexts.reserve(instance.gates.size());

	LOG(INFO) << "Converting primitives to unmapped and mapped circuit elements";

	for (size_t gateIndex = 0u; gateIndex < instance.gates.size(); ++gateIndex)
	{
		const Gate& gate = instance.gates[gateIndex];

		const auto primitive = gate.primitive;
		const size_t inputs = primitive->inputPorts.size();
		const size_t outputs = primitive->outputPorts.size();

		Primitives::PrimitiveContext& context = primitiveContexts.emplace_back(gate, gate.group, inputs, outputs);
		if (!primitive->ConvertToCircuit(*builder, configuration, context))
		{
			LOG(ERROR) << "Could not convert primitive " << gate.type << " to circuit";
			return std::unique_ptr<Builder::CircuitBuilder>();
		}

		for (size_t input = 0u; input < inputs; ++input)
		{
			auto [mappedNode, mappedPin] = context.mappedInputs[input];
			auto [unmappedNode, unmappedPin] = context.unmappedInputs[input];

			if (mappedNode != Builder::UNCONNECTED_MAPPED_PORT
				&& unmappedNode != Builder::UNCONNECTED_UNMAPPED_PORT)
			{
				builder->LinkMappedToUnmappedInputPin(mappedNode, unmappedNode, mappedPin, unmappedPin);
			}

			size_t connectionId = gate.inputs[input];
			if (connectionId == GATE_NO_CONNECTION)
			{
				continue;
			}

			auto const& connection = instance.connections[connectionId];
			if (mappedNode != Builder::UNCONNECTED_MAPPED_PORT)
			{
				auto& node = builder->GetMappedNode(mappedNode);
				node.SetInputConnectionId(mappedPin, connectionId);
				node.SetInputConnectionName(mappedPin, connection.name);
			}

			if (unmappedNode != Builder::UNCONNECTED_UNMAPPED_PORT)
			{
				auto& node = builder->GetUnmappedNode(unmappedNode);
				node.SetInputConnectionId(unmappedPin, connectionId);
				node.SetInputConnectionName(unmappedPin, connection.name);
			}
		}

		for (size_t output = 0u; output < outputs; ++output)
		{
			auto [mappedNode, mappedPin] = context.mappedOutputs[output];
			auto [unmappedNode, unmappedPin] = context.unmappedOutputs[output];

			if (mappedNode != Builder::UNCONNECTED_MAPPED_PORT
				&& unmappedNode != Builder::UNCONNECTED_UNMAPPED_PORT
				&& mappedPin == 0u)
			{
				builder->LinkMappedToUnmappedOutputPin(mappedNode, unmappedNode, unmappedPin);
			}

			size_t connectionId = gate.outputs[output];
			if (connectionId == GATE_NO_CONNECTION)
			{
				continue;
			}

			auto const& connection = instance.connections[connectionId];
			if (mappedNode != Builder::UNCONNECTED_MAPPED_PORT
				&& mappedPin == 0u)
			{
				auto& node = builder->GetMappedNode(mappedNode);
				node.SetOutputConnectionId(connectionId);
				node.SetOutputConnectionName(connection.name);
			}

			if (unmappedNode != Builder::UNCONNECTED_UNMAPPED_PORT)
			{
				auto& node = builder->GetUnmappedNode(unmappedNode);
				node.SetOutputConnectionId(unmappedPin, connectionId);
				node.SetOutputConnectionName(unmappedPin, connection.name);
			}
		}
	}

	// ------------------------------------------------------------------------
	// Connect all primitives in the circuit, which have been previously created.
	// Start from the inputs as one output can feed multiple inputs.
	// ------------------------------------------------------------------------

	LOG(INFO) << "Connecting all circuit elements created by primitives in the unmapped and mapped circuit";

	for (size_t gateIndex = 0u; gateIndex < instance.gates.size(); ++gateIndex)
	{
		auto const& gate = instance.gates[gateIndex];
		for (size_t input = 0u; input < gate.inputs.size(); ++input)
		{
			size_t gateInput = gate.inputs[input];
			if (gateInput == GATE_NO_CONNECTION)
			{
				continue;
			}

			auto& connection = instance.connections[gateInput];
			if (connection.drivingGates.size() == 0u)
			{
				LOG(WARNING) << "There are no driving gates for input " << input << " at " << gate.type << " gate " << gate.name << " in " << gate.sourceInfo.sourceLocation;
				continue;
			}
			else if (connection.drivingGates.size() > 1u)
			{
				LOG(ERROR) << "There is more than one driving gate for input " << input << " at " << gate.type << " gate " << gate.name << " in " << gate.sourceInfo.sourceLocation;
				for (size_t drivingGateId : connection.drivingGates)
				{
					const Gate& drivingGate = instance.gates[drivingGateId];
					std::string drivingPin = "unknown";
					if (auto it = std::find(drivingGate.outputs.cbegin(), drivingGate.outputs.cend(), gateInput); it != drivingGate.outputs.cend())
					{
						drivingPin = std::to_string(std::distance(drivingGate.outputs.cbegin(), it));
					}
					LOG(WARNING) << "Pin " << drivingPin << " of " << drivingGate.type << " gate " << drivingGate.name << " in " << gate.sourceInfo.sourceLocation
						<< " drives the connection";
				}
				return { };
			}

			size_t drivingGateIndex = connection.drivingGates[0u];
			auto const& drivingGate = instance.gates[drivingGateIndex];

			bool outputFound = false;
			for (size_t output = 0u; output < drivingGate.outputs.size(); ++output)
			{
				if (drivingGate.outputs[output] != gateInput)
				{
					continue;
				}

				DVLOG(5) << "The " << drivingGate.type << " gate " << drivingGate.name << " in " << gate.sourceInfo.sourceLocation
					<< " drives input pin " << input << " of " << gate.type << " gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
					<< " with output pin " << output;

				if (outputFound)
				{
					LOG(ERROR) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " which is driven by more than one output port of the driver gate";
					return { };
				}

				outputFound = true;

				Primitives::PrimitiveContext& sinkContext = primitiveContexts[gateIndex];
				Primitives::PrimitiveContext& sourceContext = primitiveContexts[drivingGateIndex];
				assert(sinkContext.unmappedInputs.size() == gate.inputs.size());
				assert(sinkContext.mappedInputs.size() == gate.inputs.size());
				assert(sourceContext.unmappedOutputs.size() == drivingGate.outputs.size());
				assert(sourceContext.mappedOutputs.size() == drivingGate.outputs.size());

				auto& [unmappedSinkNode, unmappedSinkPin] = sinkContext.unmappedInputs[input];
				auto& [unmappedSourceNode, unmappedSourcePin] = sourceContext.unmappedOutputs[output];
				if (unmappedSinkNode == Builder::UNCONNECTED_UNMAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " but not in the unmapped circuit";
				}
				else if (unmappedSourceNode == Builder::UNCONNECTED_UNMAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " but no driver in the unmapped circuit";
				}
				else
				{
					auto& sinkNode = builder->GetUnmappedNode(unmappedSinkNode);
					auto& sourceNode = builder->GetUnmappedNode(unmappedSourceNode);
					sourceNode.AddOutputNode(unmappedSourcePin, unmappedSinkNode);
					sinkNode.SetInputNode(unmappedSinkPin, unmappedSourceNode);
				}

				auto& [mappedSinkNode, mappedSinkPin] = sinkContext.mappedInputs[input];
				auto& [mappedSourceNode, mappedSourcePin] = sourceContext.mappedOutputs[output];
				if (mappedSinkNode == Builder::UNCONNECTED_MAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " but not in the mapped circuit";
				}
				else if (mappedSourceNode == Builder::UNCONNECTED_MAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " but no driver in the mapped circuit";
				}
				else if (mappedSourcePin != 0u)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " which is connected to an invalid output pin in the mapped circuit";
				}
				else
				{
					auto& sinkNode = builder->GetMappedNode(mappedSinkNode);
					auto& sourceNode = builder->GetMappedNode(mappedSourceNode);
					sourceNode.AddSuccessorNode(mappedSinkNode);
					sinkNode.SetInputNode(mappedSinkPin, mappedSourceNode);
				}
			}

			if (!outputFound)
			{
				LOG(ERROR) << "Found " << drivingGate.type << " gate " << drivingGate.name << " which is in the drivingGates list of connection "
					<< connection.name << ", but gate has no output pin which is driving the connection";
				return std::unique_ptr<Builder::CircuitBuilder>();
			}
		}
	}

	// ------------------------------------------------------------------------
	// Check the graph connectivity
	// ------------------------------------------------------------------------

	LOG(INFO) << "Checking graph connectivity in mapped and unmapped circuit";

	const auto Contains = [](auto& vector, auto element) {
		return std::find(vector.cbegin(), vector.cend(), element) != vector.cend();
	};

	for (size_t gateIndex = 0u; gateIndex < instance.gates.size(); ++gateIndex)
	{
		auto const& gate = instance.gates[gateIndex];

		// ------------------------------------------------------------------------
		// Check inputs
		// ------------------------------------------------------------------------

		for (size_t input = 0u; input < gate.inputs.size(); ++input)
		{
			size_t gateInput = gate.inputs[input];
			if (gateInput == GATE_NO_CONNECTION
				|| instance.connections[gateInput].drivingGates.size() == 0u)
			{
				Primitives::PrimitiveContext& sinkContext = primitiveContexts[gateIndex];
				assert(sinkContext.unmappedInputs.size() == gate.inputs.size());
				assert(sinkContext.mappedInputs.size() == gate.inputs.size());

				auto& [unmappedSinkNode, unmappedSinkPin] = sinkContext.unmappedInputs[input];
				auto& [mappedSinkNode, mappedSinkPin] = sinkContext.mappedInputs[input];
				if (unmappedSinkNode != Builder::UNCONNECTED_UNMAPPED_PORT
					&& builder->GetUnmappedNode(unmappedSinkNode).inputs[unmappedSinkPin] != Builder::UNCONNECTED_UNMAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " in the unmapped circuit while it shouldn't";
				}
				if (mappedSinkNode != Builder::UNCONNECTED_MAPPED_PORT
					&& builder->GetMappedNode(mappedSinkNode).inputs[mappedSinkPin] != Builder::UNCONNECTED_MAPPED_PORT)
				{
					LOG(FATAL) << "The gate " << gate.name << " in " << gate.sourceInfo.sourceLocation
						<< " has a connection at input " << input << " in the mapped circuit while it shouldn't";
				}

				continue;
			}

			auto& connection = instance.connections[gateInput];
			size_t drivingGateIndex = connection.drivingGates[0u];
			auto const& drivingGate = instance.gates[drivingGateIndex];

			bool outputFound = false;
			for (size_t output = 0u; output < drivingGate.outputs.size(); ++output)
			{
				if (drivingGate.outputs[output] != gateInput)
				{
					continue;
				}

				DVLOG(5) << "The " << drivingGate.type << " gate " << drivingGate.name << " drives input pin " << input
					<< " of " << gate.type << " gate " << gate.name << " with output pin " << output;

				if (outputFound)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input
						<< " which is driven by more than one output port of the driver gate";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}

				outputFound = true;

				Primitives::PrimitiveContext& sinkContext = primitiveContexts[gateIndex];
				Primitives::PrimitiveContext& sourceContext = primitiveContexts[drivingGateIndex];
				assert(sinkContext.unmappedInputs.size() == gate.inputs.size());
				assert(sinkContext.mappedInputs.size() == gate.inputs.size());
				assert(sourceContext.unmappedOutputs.size() == drivingGate.outputs.size());
				assert(sourceContext.mappedOutputs.size() == drivingGate.outputs.size());

				auto& [unmappedSinkNode, unmappedSinkPin] = sinkContext.unmappedInputs[input];
				auto& [unmappedSourceNode, unmappedSourcePin] = sourceContext.unmappedOutputs[output];
				if (unmappedSinkNode == Builder::UNCONNECTED_UNMAPPED_PORT
					|| builder->GetUnmappedNode(unmappedSinkNode).inputs[unmappedSinkPin] == Builder::UNCONNECTED_UNMAPPED_PORT)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but not in the unmapped circuit";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
				else if (unmappedSourceNode == Builder::UNCONNECTED_UNMAPPED_PORT
					|| !Contains(builder->GetUnmappedNode(unmappedSourceNode).outputs[unmappedSourcePin], unmappedSinkNode))
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but no driver in the unmapped circuit";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}

				auto& [mappedSinkNode, mappedSinkPin] = sinkContext.mappedInputs[input];
				auto& [mappedSourceNode, mappedSourcePin] = sourceContext.mappedOutputs[output];
				if (mappedSinkNode == Builder::UNCONNECTED_MAPPED_PORT
					|| builder->GetMappedNode(mappedSinkNode).inputs[mappedSinkPin] == Builder::UNCONNECTED_MAPPED_PORT)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but not in the mapped circuit";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
				else if (mappedSourcePin != 0u)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " which is connected to an invalid output pin in the mapped circuit";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
				else if (mappedSourceNode == Builder::UNCONNECTED_MAPPED_PORT
					|| !Contains(builder->GetMappedNode(mappedSourceNode).successors, mappedSinkNode))
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but no driver in the mapped circuit";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
			}

			if (!outputFound)
			{
				LOG(ERROR) << "Found " << drivingGate.type << " gate " << drivingGate.name << " which is in the drivingGates list of connection "
					<< connection.name << ", but gate has no output pin which is driving the connection";
				return std::unique_ptr<Builder::CircuitBuilder>();
			}
		}

		// ------------------------------------------------------------------------
		// Check outputs
		// ------------------------------------------------------------------------

		for (size_t output = 0u; output < gate.outputs.size(); ++output)
		{
			size_t gateOutput = gate.outputs[output];
			if (gateOutput == GATE_NO_CONNECTION)
			{
				Primitives::PrimitiveContext& sourceContext = primitiveContexts[gateIndex];
				assert(sourceContext.unmappedOutputs.size() == gate.outputs.size());
				assert(sourceContext.mappedOutputs.size() == gate.outputs.size());

				auto& [unmappedSourceNode, unmappedSourcePin] = sourceContext.unmappedOutputs[output];
				auto& [mappedSourceNode, mappedSourcePin] = sourceContext.mappedOutputs[output];
				if (unmappedSourceNode != Builder::UNCONNECTED_UNMAPPED_PORT
					&& builder->GetUnmappedNode(unmappedSourceNode).outputs[unmappedSourcePin].size() > 0)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at output " << output << " in the unmapped circuit while it shouldn't";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
				if (mappedSourceNode != Builder::UNCONNECTED_MAPPED_PORT
					&& builder->GetMappedNode(mappedSourceNode).successors.size() > 0)
				{
					LOG(ERROR) << "The gate " << gate.name << " has a connection at output " << output << " in the mapped circuit while it shouldn't";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}

				continue;
			}

			auto& connection = instance.connections[gateOutput];
			for (size_t drivenGateIndex : connection.drivenGates)
			{
				auto const& drivenGate = instance.gates[drivenGateIndex];

				bool inputFound = false;
				for (size_t input = 0u; input < drivenGate.inputs.size(); ++input)
				{
					if (drivenGate.inputs[input] != gateOutput)
					{
						continue;
					}

					DVLOG(5) << "The " << gate.type << " gate " << gate.name << " drives input pin " << input
						<< " of " << drivenGate.type << " gate " << drivenGate.name << " with output pin " << output;

					inputFound = true;

					Primitives::PrimitiveContext& sinkContext = primitiveContexts[drivenGateIndex];
					Primitives::PrimitiveContext& sourceContext = primitiveContexts[gateIndex];
					assert(sinkContext.unmappedInputs.size() == drivenGate.inputs.size());
					assert(sinkContext.mappedInputs.size() == drivenGate.inputs.size());
					assert(sourceContext.unmappedOutputs.size() == gate.outputs.size());
					assert(sourceContext.mappedOutputs.size() == gate.outputs.size());

					auto& [unmappedSinkNode, unmappedSinkPin] = sinkContext.unmappedInputs[input];
					auto& [unmappedSourceNode, unmappedSourcePin] = sourceContext.unmappedOutputs[output];
					if (unmappedSinkNode == Builder::UNCONNECTED_UNMAPPED_PORT
						|| builder->GetUnmappedNode(unmappedSinkNode).inputs[unmappedSinkPin] == Builder::UNCONNECTED_UNMAPPED_PORT)
					{
						LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but not in the unmapped circuit";
						return std::unique_ptr<Builder::CircuitBuilder>();
					}
					else if (unmappedSourceNode == Builder::UNCONNECTED_UNMAPPED_PORT
						|| !Contains(builder->GetUnmappedNode(unmappedSourceNode).outputs[unmappedSourcePin], unmappedSinkNode))
					{
						LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but no driver in the unmapped circuit";
						return std::unique_ptr<Builder::CircuitBuilder>();
					}

					auto& [mappedSinkNode, mappedSinkPin] = sinkContext.mappedInputs[input];
					auto& [mappedSourceNode, mappedSourcePin] = sourceContext.mappedOutputs[output];
					if (mappedSinkNode == Builder::UNCONNECTED_MAPPED_PORT
						|| builder->GetMappedNode(mappedSinkNode).inputs[mappedSinkPin] == Builder::UNCONNECTED_MAPPED_PORT)
					{
						LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but not in the mapped circuit";
						return std::unique_ptr<Builder::CircuitBuilder>();
					}
					else if (mappedSourcePin != 0u)
					{
						LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " which is connected to an invalid output pin in the mapped circuit";
						return std::unique_ptr<Builder::CircuitBuilder>();
					}
					else if (mappedSourceNode == Builder::UNCONNECTED_MAPPED_PORT
						|| !Contains(builder->GetMappedNode(mappedSourceNode).successors, mappedSinkNode))
					{
						LOG(ERROR) << "The gate " << gate.name << " has a connection at input " << input << " but no driver in the mapped circuit";
						return std::unique_ptr<Builder::CircuitBuilder>();
					}
				}

				if (!inputFound)
				{
					LOG(ERROR) << "Found " << drivenGate.type << " gate " << drivenGate.name << " which is in the drivenGates list of connection "
						<< connection.name << ", but gate has no input pin which is driven by the connection";
					return std::unique_ptr<Builder::CircuitBuilder>();
				}
			}
		}
	}

	return builder;
}

template bool ReportUnknownElements<AttributeCollection>(const AttributeCollection& elements, const std::vector<std::string>& known, const std::string& message);
template bool ReportUnknownElements<ParameterCollection>(const ParameterCollection& elements, const std::vector<std::string>& known, const std::string& message);
template bool ReportUnknownElements<ParameterMappingCollection>(const ParameterMappingCollection& elements, const std::vector<std::string>& known, const std::string& message);

};
};
};
