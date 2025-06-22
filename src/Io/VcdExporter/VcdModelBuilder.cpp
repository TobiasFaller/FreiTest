#include "Io/VcdExporter/VcdModelBuilder.hpp"

#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Io/VcdExporter/VcdModel.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace FreiTest::Simulation;

namespace FreiTest
{
namespace Io
{
namespace Vcd
{

VcdModelBuilder::VcdModelBuilder():
	model(),
	defaultTimeframe(),
	nextReference({ 0 }),
	scopes(),
	references(),
	connectionIdToBusRefAndIndex()
{}

VcdModelBuilder::~VcdModelBuilder() = default;

std::string VcdModelBuilder::CreateReference()
{
	std::string currentValue;
	currentValue.reserve(nextReference.size());
	for(auto character : nextReference)
	{
		currentValue += static_cast<char>(character + 33);
	}

	for (size_t i { 0u }; ; i++)
	{
		if (i >= nextReference.size())
		{
			nextReference.push_back(0);
			break;
		}

		nextReference[i] += 1;
		if (nextReference[i] <= 93)
		{
			break;
		}

		nextReference[i] = 0;
	}

	return currentValue;
}

std::string VcdModelBuilder::GetOrCreateReference(size_t connectionId)
{
	if (references.find(connectionId) == references.end())
	{
		references[connectionId] = CreateReference();
	}

	return references[connectionId];
}

std::shared_ptr<Scope> VcdModelBuilder::CreateScope(const CircuitEnvironment& circuit, const GroupMetaData* group)
{
	if (group == nullptr)
	{
		group = circuit.GetMetaData().GetRoot();
	}

	auto sourceInfo = group->GetSourceInfo();
	auto name = sourceInfo.GetProperty<std::string>("module-name").value_or("");
	auto type = sourceInfo.GetProperty<std::string>("module-type").value_or("");

	if (type == "input" || type == "output" || type == "inout" || type == "assignment"
		|| type == "tieX" || type == "tie0" || type == "tie1" || type == "tieU")
	{
		return { };
	}

	auto scope { std::make_shared<Scope>(name, ModuleTypes::moduleM) };

	for (auto& wire: group->GetWires())
	{
		const auto wireName = wire.GetName();
		const auto size = wire.GetSize().GetSize();

		// Bus => ConnectionID to Bus
		std::string bus = "";
		if (size > 1) {
			// Bus muss zusätzlich hinzugefügt werden
			bus = CreateReference();
			scope->CreateVariable(wireName, bus, size, VariableTypes::wireV);
			this->defaultTimeframe.wireValues[bus].resize(size, Logic::LOGIC_DONT_CARE);
		}

		for (auto index : wire.GetSize().GetIndicesBottomToTop())
		{
			const auto connection = wire.GetConnectionForIndex(index);
			if (connection == nullptr)
			{
				continue;
			}

			auto ref = GetOrCreateReference(connection->GetConnectionId());
			this->defaultTimeframe.wireValues[ref].resize(1, Logic::LOGIC_DONT_CARE);

			auto name = wireName + ((size == 1) ? "" : (" [" + std::to_string(index) + "]"));
			scope->CreateVariable(name, ref, 1, VariableTypes::wireV);
			if (size > 1)
			{
				connectionIdToBusRefAndIndex[connection->GetConnectionId()] = std::make_pair(bus, index - wire.GetSize().GetMin());
			}
		}
	}

	for (auto& port: group->GetPorts())
	{
		const auto portName = port.GetName();
		const auto size = port.GetSize().GetSize();

		std::string bus = "";
		if (size > 1) {
			// Bus muss zusätzlich hinzugefügt werden
			bus = CreateReference();

			scope->CreateVariable(portName, bus, size, VariableTypes::wireV);
			this->defaultTimeframe.wireValues[bus].resize(size, Logic::LOGIC_DONT_CARE);
		}

		for (auto index : port.GetSize().GetIndicesBottomToTop())
		{
			const auto connection = port.GetConnectionForIndex(index);
			if (connection == nullptr)
			{
				continue;
			}

			auto ref = GetOrCreateReference(connection->GetConnectionId());
			this->defaultTimeframe.wireValues[ref].resize(1, Logic::LOGIC_DONT_CARE);

			auto name = (size == 1) ?  portName : (portName + " [" + std::to_string(index) + "]");
			scope->CreateVariable(name, ref, 1, VariableTypes::wireV);

			if (size > 1)
			{
				connectionIdToBusRefAndIndex[connection->GetConnectionId()] = std::make_pair( bus, index - port.GetSize().GetMin());
			}
		}
	}

	scopes.emplace(scope->name, scope);
	for (auto& subGroup: group->GetSubGroups())
	{
		auto subScope = CreateScope(circuit, subGroup);
		if (subScope != nullptr)
		{
			scope->scopes.push_back(subScope);
		}
	}

	return scope;
}

// -------------------------------------------------------------------
// ------------------------- Build VcdModel --------------------------
// -------------------------------------------------------------------

void VcdModelBuilder::Initialize(const CircuitEnvironment& circuit)
{
	nextReference = { 0 };

	defaultTimeframe.registerValues.clear();
	defaultTimeframe.wireValues.clear();
	scopes.clear();
	references.clear();
	connectionIdToBusRefAndIndex.clear();

	this->model = VcdModel();
	this->model.variable = VcdVariable(CreateScope(circuit));
}

VcdModel VcdModelBuilder::BuildVcdModel(const SimulationResult& simulationResult,
	const CircuitEnvironment& circuit, std::string version, int timelapse, time_t time) const
{
	// REMARK: Work on a copy of the model to support multi-threading
	VcdModel model = this->model;
	BuildVcdHeader(model, version, timelapse, time);
	BuildVcdData(model, simulationResult, circuit);
	return model;
}

void VcdModelBuilder::BuildVcdHeader(VcdModel& model, std::string version, int timelapse, time_t time) const
{
	model.header = VcdHeader(version, timelapse, time);
}

void VcdModelBuilder::BuildVcdData(VcdModel& model, const SimulationResult& sim, const CircuitEnvironment& circuit) const
{
	const auto& mappedCircuit { circuit.GetMappedCircuit() };

	for (size_t timeId { 0u }; timeId < sim.GetNumberOfTimeframes(); timeId++)
	{
		Timeframe currentTimeframe = this->defaultTimeframe;

		for (const auto& mappedNode : mappedCircuit.GetNodes())
		{
			if (mappedNode->GetNumberOfSuccessors() == 0u)
			{
				continue;
			}

			const auto connectionId = mappedNode->GetOutputConnectionId();
			if (connectionId == MappedCircuit::NO_CONNECTION)
			{
				continue;
			}

			const auto logicValue = sim.GetOutputLogic(mappedNode, timeId);

			if (auto it = references.find(connectionId); it != references.end())
			{
				currentTimeframe.wireValues[it->second][0] = logicValue;
			}

			if (auto it = connectionIdToBusRefAndIndex.find(connectionId); it != connectionIdToBusRefAndIndex.end())
			{
				const auto [busRef, index] = it->second;
				currentTimeframe.wireValues[busRef][index] = logicValue;
			}
		}

		model.data.AddTimeFrame(currentTimeframe);
	}
}

};
};
};
