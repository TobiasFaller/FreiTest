#include "Io/VerilogInstantiator/ModuleInstance.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

using namespace std;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

namespace Instance
{

Port::Port(std::string name, std::pair<size_t, size_t> size, PortType type, SourceInformation sourceInfo, std::vector<size_t> connectionIds):
	name(name),
	size(size),
	type(type),
	sourceInfo(sourceInfo),
	connectionIds(connectionIds)
{
}

Wire::Wire(std::string name, std::pair<size_t, size_t> size, SourceInformation sourceInfo, std::vector<size_t> connectionIds):
	name(name),
	size(size),
	sourceInfo(sourceInfo),
	connectionIds(connectionIds)
{
}

Group::Group(string name, size_t parent):
	name(name),
	parent(parent),
	sourceInfo(),
	groups(),
	gates(),
	wires(),
	ports()
{
}

Gate::Gate(string name, string type, std::shared_ptr<Primitives::Primitive> primitive, size_t inputCount, size_t outputCount, size_t group):
    name(name),
    type(type),
	primitive(primitive),
    inputs(inputCount, GATE_NO_CONNECTION),
    outputs(outputCount, GATE_NO_CONNECTION),
	group(group)
{
}

Connection::Connection(string name):
    name(name),
	unused(false),
    drivingGates(),
    drivenGates()
{
}

ModuleInstance::ModuleInstance(string name, std::vector<Gate> gates, std::vector<Connection> connections, std::vector<Group> groups):
    name(name),
    gates(gates),
    connections(connections),
	groups(groups)
{
}

std::string to_string(const Gate& gate)
{
    std::string result = "Gate[Name: " + gate.name + ", Type: " + gate.type + " Inputs: [";
    for (size_t input = 0u; input < gate.inputs.size(); ++input)
    {
        if (input != 0u)
        {
            result += ", ";
        }
        result += gate.inputs[input];
    }
    result += "], Outputs: [";
    for (size_t output = 0u; output < gate.outputs.size(); ++output)
    {
        if (output != 0u)
        {
           result += ", ";
        }
        result += gate.outputs[output];
    }
    result += "]]";
	return result;
}

std::string to_string(const Connection& connection)
{
	std::string result = "Connection[Name: " + connection.name + ", Driving: [";
    for (size_t gate = 0u; gate < connection.drivingGates.size(); ++gate)
    {
        if (gate != 0u)
        {
            result += ", ";
        }
        result += connection.drivingGates[gate];
    }
    result += "], Driven: [";
    for (size_t gate = 0u; gate < connection.drivenGates.size(); ++gate)
    {
        if (gate != 0u)
        {
            result += ", ";
        }
        result += connection.drivenGates[gate];
    }
    result += "]]";
    return result;
}

ostream& operator<<(ostream& os, const Gate& gate)
{
    os << to_string(gate);
    return os;
}

ostream& operator<<(ostream& os, const Connection& connection)
{
    os << to_string(connection);
    return os;
}

};
};
};
};
