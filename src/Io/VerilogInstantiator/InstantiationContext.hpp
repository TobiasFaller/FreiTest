#pragma once

#include <limits>
#include <stack>
#include <vector>
#include <string>
#include <memory>

#include "Io/VerilogInstantiator/ModuleInstance.hpp"
#include "Io/VerilogInstantiator/VerilogModules.hpp"
#include "Io/VerilogSpiritParser/VerilogComponents.hpp"

namespace FreiTest
{
namespace Io
{
namespace Verilog
{
namespace Instance
{

constexpr size_t CONNECTION_INVALID_ID = std::numeric_limits<size_t>::max();
constexpr size_t GATE_INVALID_ID = std::numeric_limits<size_t>::max();
constexpr size_t GROUP_INVALID_ID = std::numeric_limits<size_t>::max();

struct InstantiationContext
{
    InstantiationContext(const Modules& modules);
	virtual ~InstantiationContext(void);

	const Modules& modules;

	// Reference to a group
    std::vector<size_t> currentHierarchy;

    std::vector<Connection> connections;
    std::vector<Gate> gates;
	std::vector<Group> groups;

    size_t constant0Id;
    size_t constant1Id;
    size_t constantXId;
    size_t constantUId;

	Group& GetCurrentGroup(void);
    std::string PrefixName(const std::string& name) const;
	std::string GetPrefix(void) const;
	std::string GetName(void) const;

	std::shared_ptr<Primitives::Primitive> FindPrimitive(std::string name, size_t gate);
    size_t AddGate(std::string name, std::shared_ptr<Primitives::Primitive> primitive, size_t inputCount, size_t outputCount);
	size_t AddConnection(std::string name);

};

class PrefixedScopeGuard
{
public:
    PrefixedScopeGuard(InstantiationContext& context, std::string name);
    PrefixedScopeGuard(InstantiationContext& context, size_t parent, std::string name);
    ~PrefixedScopeGuard(void);

private:
    InstantiationContext& context;

};

};
};
};
};
