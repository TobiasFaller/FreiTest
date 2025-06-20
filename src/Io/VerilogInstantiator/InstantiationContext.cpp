#include "Io/VerilogInstantiator/InstantiationContext.hpp"

#include <boost/format.hpp>

#include "Io/VerilogInstantiator/ModuleInstance.hpp"
#include "Io/VerilogInstantiator/Primitive.hpp"

using namespace std;

namespace FreiTest
{
namespace Io
{
namespace Verilog
{
namespace Instance
{

InstantiationContext::InstantiationContext(const Modules& modules):
	modules(modules),
	currentHierarchy(),
	connections(),
	gates(),
	groups(),
	constant0Id(CONNECTION_INVALID_ID),
	constant1Id(CONNECTION_INVALID_ID),
	constantXId(CONNECTION_INVALID_ID),
	constantUId(CONNECTION_INVALID_ID)
{
}

InstantiationContext::~InstantiationContext(void) = default;

Group& InstantiationContext::GetCurrentGroup(void)
{
	return groups[currentHierarchy.back()];
}

string InstantiationContext::PrefixName(const string& name) const
{
    if (currentHierarchy.empty())
    {
        return name;
    }

	std::string path = name;
	size_t group = currentHierarchy.back();
	do
	{
		path = groups[group].name + "/" + path;
		group = groups[group].parent;
	} while(group != GROUP_INVALID_ID);

	return path;
}

string InstantiationContext::GetPrefix(void) const
{
	if (currentHierarchy.empty())
	{
		return "{root}";
	}

	std::string path;
	size_t group = currentHierarchy.back();
	do
	{
		path = groups[group].name + (path.empty() ? "" : ("/" + path));
		group = groups[group].parent;
	} while(group != GROUP_INVALID_ID);

	return path;
}

string InstantiationContext::GetName(void) const
{
	if (currentHierarchy.empty())
    {
        return "{root}";
    }

	std::string path;
	size_t group = currentHierarchy.back();
	do
	{
		path = groups[group].name + "/" + path;
		group = groups[group].parent;
	} while(group != GROUP_INVALID_ID);

	return path;
}

std::shared_ptr<Primitives::Primitive> InstantiationContext::FindPrimitive(string name, size_t port)
{
    if (auto it = modules.primitives.get<Primitives::primitive_name_and_ports_tag>().find(std::make_pair(name, port));
		it != modules.primitives.get<Primitives::primitive_name_and_ports_tag>().end())
	{
		return it->second;
	}

	return std::shared_ptr<Primitives::Primitive>();
}

size_t InstantiationContext::AddGate(string name, std::shared_ptr<Primitives::Primitive> primitive, size_t inputCount, size_t outputCount)
{
	ASSERT(primitive->inputPorts.size() == inputCount) << "Input port count does not match the primitive definition";
	ASSERT(primitive->outputPorts.size() == outputCount) << "Output port count does not match the primitive definition";

    size_t index = gates.size();
    gates.emplace_back(name, primitive->name, primitive, inputCount, outputCount, currentHierarchy.back());
	GetCurrentGroup().gates.push_back(index);
    return index;
}

size_t InstantiationContext::AddConnection(string name)
{
	// Prefix name here as the connections will be used as output signals inside the CircuitBuilder.
	// However, the CircuitBuilder has no notion of the source group of which this wire was instantiated from.
    size_t index = connections.size();
    connections.emplace_back(PrefixName(name));
    return index;
}

PrefixedScopeGuard::PrefixedScopeGuard(InstantiationContext& context, std::string name):
	PrefixedScopeGuard(context, context.currentHierarchy.empty() ? GROUP_INVALID_ID : context.currentHierarchy.back(), name)
{
}

PrefixedScopeGuard::PrefixedScopeGuard(InstantiationContext& context, size_t parent, std::string name):
	context(context)
{
	const size_t groupIndex = context.groups.size();
	context.currentHierarchy.push_back(groupIndex);
	context.groups.emplace_back(name, parent);

	if (const bool hasParent = (parent != GROUP_INVALID_ID); hasParent)
	{
		context.groups[parent].groups.push_back(groupIndex);
	}
}

PrefixedScopeGuard::~PrefixedScopeGuard(void)
{
	context.currentHierarchy.pop_back();
}

};
};
};
};
