#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

#include <cstdint>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

    std::string GetAttributeName(const Attribute& attribute)
    {
        return attribute.name;
    }

	std::string GetPortName(const Port& port)
    {
        return port.name;
    }

	std::string GetParameterName(const Parameter& parameter)
    {
        return parameter.name;
    }

	std::string GetWireName(const Wire& wire)
    {
        return wire.name;
    }

	std::string GetInstantiationName(const Instantiation& instantiation)
    {
        return instantiation.name;
    }

	std::string GetParameterMappingName(const ParameterMapping& mapping)
    {
        return mapping.name;
    }

	std::string GetPortMappingName(const PortMapping& mapping)
    {
        return mapping.name;
    }

	std::string GetModuleName(const Module& module)
    {
        return module.name;
    }

    VectorSize::VectorSize(void):
        top(0),
        bottom(0),
        unknown(true)
    {
    }

    VectorSize::VectorSize(uint64_t top, uint64_t bottom):
        top(top),
        bottom(bottom),
        unknown(false)
    {
    }

    uint64_t VectorSize::size(void) const
    {
        return std::max(top, bottom) + 1u - std::min(top, bottom);
    }

    uint64_t VectorSize::min(void) const
    {
        return std::min(top, bottom);
    }

    uint64_t VectorSize::max(void) const
    {
        return std::max(top, bottom);
    }

    bool VectorSize::operator==(const VectorSize& other) const
    {
        return top == other.top
            && bottom == other.bottom;
    }

	std::ostream& operator<<(std::ostream& out, const VectorSize& size)
	{
		return out << "VectorSize[unknown: " << size.unknown << ", top: " << size.top << ", bottom: " << size.bottom << "]";
	}

	std::string to_string(const VectorSize& size)
	{
		std::ostringstream stream;
		stream << size;
		return stream.str();
	}

    VectorInitializer::VectorInitializer(void):
        value()
    {
    }

    VectorInitializer::VectorInitializer(uint64_t bits, uint64_t value):
        value()
    {
		for (size_t bit = 0u; bit < bits; ++bit)
		{
			this->value.push_back(((value >> bit) & 1u) ? VectorBit::BIT_1 : VectorBit::BIT_0);
		}
    }

    VectorInitializer::VectorInitializer(std::vector<VectorBit> value):
        value(value)
    {
    }

	size_t VectorInitializer::size(void) const
	{
		return value.size();
	}

	VectorBit VectorInitializer::operator[](size_t index) const
	{
		return value[index];
	}

    bool VectorInitializer::operator==(const VectorInitializer& other) const
    {
        return value == other.value;
    }

	std::ostream& operator<<(std::ostream& out, const VectorInitializer& init)
	{
		out << "VectorInitializer[" << init.value.size() << "'b ";
		for (size_t bit = init.value.size(); bit > 0u; --bit)
		{
			out << to_string(init.value[bit - 1u]);
		}
		return out << "]";
	}

	std::string to_string(const VectorInitializer& init)
	{
		std::ostringstream stream;
		stream << init;
		return stream.str();
	}

	VectorPlaceholder::VectorPlaceholder(void)
	{
	}

	bool VectorPlaceholder::operator==(const VectorPlaceholder& other) const
	{
		return true;
	}

	std::ostream& operator<<(std::ostream& out, const VectorPlaceholder& init)
	{
		return out << "VectorPlaceholder";
	}

	std::string to_string(const VectorPlaceholder& init)
	{
		std::ostringstream stream;
		stream << init;
		return stream.str();
	}

	std::ostream& operator<<(std::ostream& out, const VectorBit& bit)
	{
		switch (bit)
		{
			case VectorBit::BIT_0:
				return out << "0";
			case VectorBit::BIT_1:
				return out << "1";
			case VectorBit::BIT_X:
				return out << "X";
			case VectorBit::BIT_U:
				return out << "U";
			default:
				return out << "-";
		}
	}

	std::string to_string(const VectorBit& bit)
	{
		std::ostringstream stream;
		stream << bit;
		return stream.str();
	}

    Attribute::Attribute(void):
        name(""),
        value("")
    {
    }

    Attribute::Attribute(std::string name, std::string value):
        name(name),
        value(value)
    {
    }

    Attribute::Attribute(std::string name, uint64_t value):
        name(name),
        value(value)
    {
    }

    Attribute::Attribute(std::string name, VectorInitializer value):
        name(name),
        value(value)
    {
    }

    bool Attribute::operator==(const Attribute& other) const
    {
        return name == other.name
            && value == other.value;
    }

	std::ostream& operator<<(std::ostream& out, const Attribute& attribute)
	{
		return out << "Attribute[name: " << attribute.name << ", value: "
                << attribute.value << "]";
	}

	std::string to_string(const Attribute& attribute)
	{
		std::ostringstream stream;
		stream << attribute;
		return stream.str();
	}

    Port::Port(void):
        name(),
        type(PortType::PORT_UNKNOWN),
        size(0, 0),
        attributes()
    {
    }

	Port::Port(std::string name, PortType type, VectorSize size):
        name(name),
        type(type),
        size(size),
        attributes()
    {
    }

    bool Port::operator==(const Port& other) const
    {
        return name == other.name
            && type == other.type
            && size == other.size;
    }

	std::ostream& operator<<(std::ostream& out, const Port& port)
	{
		return out << "Port[name: " << port.name << ", type: " << port.type << ", size: " << port.size << "]";
	}

	std::string to_string(const Port& port)
	{
		std::ostringstream stream;
		stream << port;
		return stream.str();
	}

    Parameter::Parameter(void):
        name(),
        value()
    {
    }

    Parameter::Parameter(std::string name, int64_t value):
        name(name),
        value(value)
    {
    }

    Parameter::Parameter(std::string name, VectorInitializer value):
        name(name),
        value(value)
    {
    }

    bool Parameter::operator==(const Parameter& other) const
    {
        return name == other.name
            && value == other.value;
    }

    std::ostream& operator<<(std::ostream& out, const Parameter& parameter)
	{
		return out << "Parameter[name: " << parameter.name << ", value: " << parameter.value << "]";
	}

	std::string to_string(const Parameter& parameter)
	{
		std::ostringstream stream;
		stream << parameter;
		return stream.str();
	}

    Wire::Wire(void):
        name(),
        size({ 0, 0 }),
        attributes()
    {
    }

    Wire::Wire(std::string name, VectorSize size):
        name(name),
        size(size),
        attributes()
    {
    }

    bool Wire::operator==(const Wire& other) const
    {
        return name == other.name
            && size == other.size;
    }

    std::ostream& operator<<(std::ostream& out, const Wire& wire)
    {
        return out << "Wire[name: " << wire.name << ", size: " << wire.size << "]";
    }

	std::string to_string(const Wire& wire)
	{
		std::ostringstream stream;
		stream << wire;
		return stream.str();
	}

    Assignment::Assignment(void):
        source(),
        target(),
        attributes()
    {
    }

    Assignment::Assignment(Bus source, Bus target):
        source(source),
        target(target),
        attributes()
    {
    }

    bool Assignment::operator==(const Assignment& other) const
    {
        return source == other.source
            && target == other.target;
    }

    std::ostream& operator<<(std::ostream& out, const Assignment& assignment)
    {
        return out << "Assignment[source: " << assignment.source
                << ", target: " << assignment.target << "]";
    }

	std::string to_string(const Assignment& assignment)
	{
		std::ostringstream stream;
		stream << assignment;
		return stream.str();
	}

    WireReference::WireReference(void):
        name(),
        size({ 0, 0 })
    {
    }

    WireReference::WireReference(std::string name, VectorSize size):
        name(name),
        size(size)
    {
    }

    WireReference::WireReference(std::string name, uint64_t index):
        name(name),
        size({ index, index })
    {
    }

    bool WireReference::operator==(const WireReference& other) const
    {
        return name == other.name
            && size == other.size;
    }

    std::ostream& operator<<(std::ostream& out, const WireReference& wire)
    {
        return out << "Wire[name: " << wire.name << ", size: " << wire.size << "]";
    }

	std::string to_string(const WireReference& wire)
	{
		std::ostringstream stream;
		stream << wire;
		return stream.str();
	}

    Bus::Bus(void):
        sources()
    {
    }

    Bus::Bus(std::vector<BusValue> sources):
        sources(sources)
    {
    }

    bool Bus::operator==(const Bus& other) const
    {
        return sources == other.sources;
    }

    std::ostream& operator<<(std::ostream& out, const Bus& bus)
    {
        out << "Bus[sources: [";

        size_t index = 0;
        for (auto it = bus.sources.cbegin(); it != bus.sources.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        return out << "]]";
    }

	std::string to_string(const Bus& bus)
	{
		std::ostringstream stream;
		stream << bus;
		return stream.str();
	}

    ParameterMapping::ParameterMapping():
        name(),
        value()
    {
    }

    ParameterMapping::ParameterMapping(std::string name, ParameterValue value):
        name(name),
        value(value)
    {
    }

    ParameterMapping& ParameterMapping::operator=(const VectorInitializer& other)
    {
        value = other;
        return *this;
    }

    ParameterMapping& ParameterMapping::operator=(const int64_t& other)
    {
        value = other;
        return *this;
    }

    bool ParameterMapping::operator==(const ParameterMapping& other) const
    {
        return name == other.name
            && value == other.value;
    }

    std::ostream& operator<<(std::ostream& out, const ParameterMapping& mapping)
    {
        return out << "ParameterMapping[name: " << mapping.name << ", value: " << mapping.value << "]";
    }

	std::string to_string(const ParameterMapping& mapping)
	{
		std::ostringstream stream;
		stream << mapping;
		return stream.str();
	}

    PortMapping::PortMapping():
        name(),
        bus()
    {
    }

    PortMapping::PortMapping(std::string name, Bus bus):
        name(name),
        bus(bus)
    {
    }

    bool PortMapping::operator==(const PortMapping& other) const
    {
        return name == other.name
            && bus == other.bus;
    }

    std::ostream& operator<<(std::ostream& out, const PortMapping& mapping)
    {
        return out << "PortMapping[name: " << mapping.name << ", bus: " << mapping.bus << "]";
    }

	std::string to_string(const PortMapping& mapping)
	{
		std::ostringstream stream;
		stream << mapping;
		return stream.str();
	}

    Instantiation::Instantiation():
        name(),
        type(),
        parameter(),
        ports(),
        attributes()
    {
    }

    Instantiation::Instantiation(std::string name, std::string type):
        name(name),
        type(type),
        parameter(),
        ports(),
        attributes()
    {
    }

    Instantiation::Instantiation(std::string name, std::string type, ParameterMappingList parameter, PortMappingList ports):
        name(name),
        type(type),
        parameter(),
        ports(),
        attributes()
    {
        for (ParameterMapping mapping : parameter)
        {
            this->parameter.push_back(std::move(mapping));
        }
        for (PortMapping mapping : ports)
        {
            this->ports.push_back(std::move(mapping));
        }
    }

    bool Instantiation::operator==(const Instantiation& other) const
    {
        return name == other.name
            && type == other.type
            && parameter == other.parameter
            && ports == other.ports
            && attributes == other.attributes;
    }

    std::ostream& operator<<(std::ostream& out, const Instantiation& instantiation)
    {
        out << "Instantiation[name: " << instantiation.name
                << ", type: " << instantiation.type
                << ", parameter: [";

        size_t index = 0;
        for (auto it = instantiation.parameter.cbegin();
                it != instantiation.parameter.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        out << "], ports: [";

        index = 0;
        for (auto it = instantiation.ports.cbegin();
                it != instantiation.ports.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        return out << "]]";
    }

	std::string to_string(const Instantiation& instantiation)
	{
		std::ostringstream stream;
		stream << instantiation;
		return stream.str();
	}

    Module::Module(void):
        name(),
        libraryModule(false),
        ports(),
        wires(),
        parameter(),
        assignments(),
        instantiations(),
        attributes()
    {
    }

    Module::Module(std::string name):
        name(name),
        libraryModule(false),
        ports(),
        wires(),
        parameter(),
        assignments(),
        instantiations(),
        attributes()
    {
    }

    bool Module::operator==(const Module& other) const
    {
        return name == other.name
            && ports == other.ports
            && wires == other.wires
            && parameter == other.parameter
            && assignments == other.assignments
            && instantiations == other.instantiations
            && attributes == other.attributes;
    }

	std::ostream& operator<<(std::ostream& out, const Module& module)
	{
		out << "Module[name: " << module.name << ", ports: [";

        size_t index = 0;
        for (auto it = module.ports.cbegin(); it != module.ports.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        return out << "]]";
	}

	std::string to_string(const Module& module)
	{
		std::ostringstream stream;
		stream << module;
		return stream.str();
	}

	std::ostream& operator<<(std::ostream& out, const PortType& type)
	{
        switch (type)
        {
            case PortType::PORT_IN:
                return out << "Input";
            case PortType::PORT_INOUT:
                return out << "In-/output";
            case PortType::PORT_OUT:
                return out << "Output";
            case PortType::PORT_UNKNOWN:
            default:
                return out << "Unknown";
        }
	}

    std::ostream& operator<<(std::ostream& out, const std::vector<ParameterMapping>& list)
    {
        out << "ParameterMapping: ";

        size_t index = 0;
        for (auto it = list.cbegin(); it != list.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        return out;
    }

    std::ostream& operator<<(std::ostream& out, const std::vector<PortMapping>& list)
    {
        out << "PortMapping: ";

        size_t index = 0;
        for (auto it = list.cbegin(); it != list.cend(); it++)
        {
            if (index != 0) out << ", ";
            out << *it;
            index++;
        }

        return out;
    }

	std::string to_string(const PortType& type)
	{
		std::ostringstream stream;
		stream << type;
		return stream.str();
	}

	std::string to_string(const std::vector<ParameterMapping>& list)
	{
		std::ostringstream stream;
		stream << list;
		return stream.str();
	}

	std::string to_string(const std::vector<PortMapping>& list)
	{
		std::ostringstream stream;
		stream << list;
		return stream.str();
	}

};
};
};
