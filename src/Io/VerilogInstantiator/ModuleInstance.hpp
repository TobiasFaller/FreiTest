#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include <limits>
#include <utility>

#include "Circuit/SourceInformation.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{

namespace Primitives
{

struct Primitive;

};
namespace Instance
{

constexpr size_t GATE_NO_CONNECTION = std::numeric_limits<size_t>::max();
constexpr size_t GATE_UNUSED_CONNECTION = std::numeric_limits<size_t>::max() - 1u;

enum class PortType
{
	Input,
	Output,
	Bidirectional
};

struct Port
{
	Port(std::string name, std::pair<size_t, size_t> size, PortType type, SourceInformation sourceInfo, std::vector<size_t> connectionIds);

	std::string name;
	std::pair<size_t, size_t> size;
	PortType type;
	SourceInformation sourceInfo;
	std::vector<size_t> connectionIds;
};

struct Wire
{
	Wire(std::string name, std::pair<size_t, size_t> size, SourceInformation sourceInfo, std::vector<size_t> connectionIds);

	std::string name;
	std::pair<size_t, size_t> size;
	SourceInformation sourceInfo;
	std::vector<size_t> connectionIds;
};

struct Group
{
	Group(std::string name, size_t parent);

	std::string name;
	size_t parent;
	SourceInformation sourceInfo;
	std::vector<size_t> groups;
	std::vector<size_t> gates;
	std::vector<Wire> wires;
	std::vector<Port> ports;
};

struct Gate
{
    Gate(std::string name, std::string type, std::shared_ptr<Primitives::Primitive> primitive, size_t inputCount, size_t outputCount, size_t group);

    std::string name;
    std::string type;
	std::shared_ptr<Primitives::Primitive> primitive;

    // IDs of Connections for each port or GATE_NO_CONNECTION
    std::vector<size_t> inputs;
    std::vector<size_t> outputs;
	SourceInformation sourceInfo;
	size_t group;
};

struct Connection
{
    Connection(std::string name);

    std::string name;
	bool unused;
    std::vector<size_t> drivingGates;
    std::vector<size_t> drivenGates;
};

struct ModuleInstance {
    ModuleInstance(std::string name, std::vector<Gate> gates, std::vector<Connection> connections, std::vector<Group> groups);

    std::string name;
    std::vector<Gate> gates;
    std::vector<Connection> connections;
	std::vector<Group> groups;
};

std::string to_string(const Gate& gate);
std::string to_string(const Connection& connection);

std::ostream& operator<<(std::ostream& os, const Gate& gate);
std::ostream& operator<<(std::ostream& os, const Connection& connection);

};
};
};
};
