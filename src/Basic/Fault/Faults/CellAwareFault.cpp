#include "Basic/Fault/Faults/CellAwareFault.hpp"

#include <string>
#include <set>
#include <map>
#include <vector>
#include <memory>

#include <boost/format.hpp>

#include "Basic/Logging.hpp"
#include "Basic/Logic.hpp"
#include "Circuit/CircuitMetaData.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

using NodeAndPort = FreiTest::Circuit::MappedCircuit::NodeAndPort;

namespace FreiTest
{
namespace Fault
{

static std::string to_string(const CellAwareFaultStateMachine::TransitionType& type);
static std::string to_string(const std::vector<CellAwareFaultStateMachine::Port>& ports);

CellAwareFaultStateMachine::CellAwareFaultStateMachine(std::vector<State> states):
	states(states),
	inputNodes(),
	outputNodes(),
	timeframes(0u),
	minInputNode(std::numeric_limits<size_t>::max()),
	maxInputNode(0u),
	minOutputNode(std::numeric_limits<size_t>::max()),
	maxOutputNode(0u)
{
	for (const auto& state : this->states)
	{
		for (const auto& transition : state.transitions)
		{
			for (const auto& input : transition.inputs)
			{
				inputNodes.push_back(input.nodeAndPort);
				minInputNode = std::min(minInputNode, input.nodeAndPort.node->GetNodeId());
				maxInputNode = std::max(maxInputNode, input.nodeAndPort.node->GetNodeId());
			}
			for (const auto& output : transition.outputs)
			{
				outputNodes.push_back(output.nodeAndPort);
				minOutputNode = std::min(minOutputNode, output.nodeAndPort.node->GetNodeId());
				maxOutputNode = std::max(maxOutputNode, output.nodeAndPort.node->GetNodeId());
			}
		}

		timeframes = std::max(timeframes, state.depth + 1u);
	}

	std::sort(inputNodes.begin(), inputNodes.end());
	std::sort(outputNodes.begin(), outputNodes.end());
	inputNodes.erase(std::unique(inputNodes.begin(), inputNodes.end()), inputNodes.end());
	outputNodes.erase(std::unique(outputNodes.begin(), outputNodes.end()), outputNodes.end());
}
CellAwareFaultStateMachine::~CellAwareFaultStateMachine(void) = default;

CellAwareFault::CellAwareFault(std::string name, Circuit::GroupMetaData const* cell, CellAwareFaultStateMachine stateMachine):
	name(name),
	cell(cell),
	stateMachine(stateMachine),
	sequential(false)
{
	// A cell-aware fault is sequential if there are sequential elements inside.
	// These kind of cells require special treatment during fault simulation
	// as the cell output node ids will be smaller the cell input node ids
	// and no simple linear circuit simulation is possible.
	//
	// This is independent of the cell aware fault having multiple timeframes.
	// Example: Combinational cells can have transition tests (two timeframe inputs).
	for (const auto* gate : cell->GetSubGroups())
	{
		for (const auto* node : gate->GetMappedNodes())
		{
			const auto type { node->GetCellType() };
			sequential |= (type == CellType::S_IN || type == CellType::S_OUT
			|| type == CellType::S_OUT_CLK || type == CellType::S_OUT_EN);
		}
	}
}
CellAwareFault::~CellAwareFault(void) = default;

std::string CellAwareFault::GetCellName(void) const
{
	return this->cell->GetHierarchyName();
}

std::string CellAwareFault::GetCellType(void) const
{
	return this->cell->GetSourceInfo().GetProperty<std::string>("cell-type").value_or("UNKNOWN");
}

std::optional<CellAwareFault> CreateCellAwareFaultForCell(const CellAwareFaultSpecification& specification, const Circuit::GroupMetaData* cell)
{
	const auto sourceInfo = cell->GetSourceInfo();
	ASSERT(sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
		<< "The provided group has to be a cell.";

	const auto name = sourceInfo.GetProperty<std::string>("module-name").value_or("");
	const auto type = sourceInfo.GetProperty<std::string>("module-type").value_or("");
	const auto portMap = CreateCellPortMapForCell(cell);

	const auto ports_to_string = [&](auto& ports) -> std::string {
		std::string result;
		for (const auto& [portName, nodeAndPort] : ports)
		{
			if (!result.empty()) result += ", ";
			result += portName + ": ";
			result += nodeAndPort.node->GetName() + " (" + to_string(nodeAndPort.node->GetCellType()) + ") ";
			result += (nodeAndPort.port.portType == Circuit::PortType::Input) ? "Input " : "Output ";
			result += std::to_string(nodeAndPort.port.portNumber);
		}
		return result;
	};
	DVLOG(6) << "Cell " << name << " with type " << type << " (" << ports_to_string(portMap) << ")";

	bool portsMapped = true;
	for (const auto& state : specification.GetStates())
	{
		for (const auto& transition : state.transitions)
		{
			for (const auto& input : transition.inputs)
			{
				auto found { portMap.find(input.name) != portMap.end() };
				if (!found)
				{
					LOG(WARNING) << "Was not able to find port \"" << input.name << "\" in cell \"" << name << "\"!";
				}
				portsMapped &= found;
			}
			for (const auto& output : transition.outputs)
			{
				auto found { portMap.find(output.name) != portMap.end() };
				if (!found)
				{
					LOG(WARNING) << "Was not able to find port \"" << output.name << "\" in cell \"" << name << "\"!";
				}
				portsMapped &= found;
			}
		}
	}
	if (!portsMapped)
	{
		LOG(WARNING) << "Port mapping did not work for fault \"" << specification.GetName()
			<< "\" for cell \"" << name << "\" of type \"" << type << "\"";
		return { };
	}

	// Check if all node / ports are unique on the cell outputs.
	// If not then constraints can not be applied correctly / checked.
	std::unordered_map<size_t, std::string> driverMap;
	for (auto const& [portName, nodeAndPort] : portMap)
	{
		auto const connectionId { nodeAndPort.node->GetPortConnectionId(nodeAndPort.port) };
		if (auto const [it, inserted] = driverMap.emplace(connectionId, portName); !inserted)
		{
			LOG(WARNING) << "Port mapping did not work for fault \"" << specification.GetName()
				<< "\" for cell \"" << name << "\" of type \"" << type << "\" as port \""
				<< portName << "\" and port \"" << it->second << "\" share the same connection "
				<< connectionId << "."
				<< " Connections need to be unique to correctly identify ports.";
			return { };
		}
	}

	using State = CellAwareFaultStateMachine::State;
	using Transition = CellAwareFaultStateMachine::Transition;
	using TransitionType = CellAwareFaultStateMachine::TransitionType;
	using Port = CellAwareFaultStateMachine::Port;

	const auto map_ports = [&](const auto& ports) -> auto {
		std::vector<Port> result;
		for (auto& port : ports)
		{
			result.push_back(Port {
				.nodeAndPort = portMap.find(port.name)->second,
				.constraint = port.constraint
			});
		}
		return result;
	};

	constexpr size_t STATE_EPSILON = 0u;
	constexpr size_t DEPTH_UNKNOWN = std::numeric_limits<size_t>::max();

	std::vector<State> states;
	for (const auto& state : specification.GetStates())
	{
		std::vector<Transition> transitions;
		for (const auto& transition : state.transitions) {
			transitions.push_back(Transition {
				.inputs = map_ports(transition.inputs),
				.outputs = map_ports(transition.outputs),
				.toState = transition.toState,
				.type = TransitionType::Primary,
				.accepting = transition.accepting,
			});
		}
		states.push_back(State {
			.transitions = transitions,
			.depth = DEPTH_UNKNOWN,
		});
	}
	ASSERT(states.size() > 0)
		<< "There has to be at least one state in the cell-aware model (epsilon)";

	// Create transitions from the states to the start of new sequences.
	for (size_t stateIndex { 1u }; stateIndex < states.size(); stateIndex++)
	{
		for (auto const& epsTrans : states[STATE_EPSILON].transitions)
		{
			// Create a transition from each state such that it is the start state for the state machine.
			// Existing transitions have priority and this is not considered a conflict.
			//
			// Example: Transition system without continuing transitions on the states.
			//   eps -+--> (A=0, B=0 -> C=X) 1
			//        +--> (A=1, B=0 -> C=X) 2
			//        +--> (A=0, B=0 -> C=X) 3 ---> 4 (A=1, B=0, C=1)
			//
			// Transitioning into any of these states would result in the system to get stuck.
			// Therefore, transitions equivalent to the ones from eps are inserted into each state.
			// The corner case arises then from state 3, where the conditions A=1, B=0 match the ones of alternative 2.
			// However, only the transition test creates a fault difference and has a differing fault effect.
			// The transition from 3 to 2 should not be generated and 3 to 4 is relevant instead.
			// => Therefore, transition type Secondary is used here to give it a secondary priority.
			states[stateIndex].transitions.push_back(Transition {
				.inputs = epsTrans.inputs,
				.outputs = epsTrans.outputs,
				.toState = epsTrans.toState,
				.type = TransitionType::Secondary,
				.accepting = epsTrans.accepting,
			});
		}
	}

	// Compute the depth of states such that the maximum path length
	// can be calculated via the maximum depth.
	std::vector<size_t> statesToVisit { 0u };
	for (size_t depth { 0u }; !statesToVisit.empty(); depth++)
	{
		std::vector<size_t> nextStates;
		for (size_t stateIndex : statesToVisit)
		{
			auto& state = states[stateIndex];
			if (state.depth == DEPTH_UNKNOWN)
			{
				state.depth = depth;
				for (auto& transition : state.transitions)
				{
					if (transition.toState != 0)
					{
						nextStates.push_back(transition.toState);
					}
				}
			}
		}
		statesToVisit = std::move(nextStates);
		nextStates.clear();
	}

	return { CellAwareFault(specification.GetName(), cell, { states }) };
}

std::unordered_map<std::string, Circuit::MappedCircuit::NodeAndPort> CreateCellPortMapForCell(const Circuit::GroupMetaData* cell)
{
	std::vector<const Circuit::MappedNode*> gates { };
	for (const auto* gate : cell->GetSubGroups())
	{
		gates.insert(gates.end(), gate->GetMappedNodes().begin(), gate->GetMappedNodes().end());
	}
	std::sort(gates.begin(), gates.end());
	gates.erase(std::unique(gates.begin(), gates.end()), gates.end());

	std::unordered_map<std::string, Circuit::MappedCircuit::NodeAndPort> portMap;
	for (auto port: cell->GetPorts())
	{
		const auto& portName = port.GetName();
		ASSERT(port.GetConnections().size() == 1)
			<< "Port \"" << portName << "\" in cell \"" << cell->GetHierarchyName()
			<< "\" has a bus with a size not equal to 1 as input / output port.";

		const auto portType = port.GetSourceInfo().GetProperty<std::string>("port-type").value_or("");
		ASSERT(portType == "input" || portType == "output")
			<< "Port \"" << portName << "\" in cell \"" << cell->GetHierarchyName()
			<< "\" has invalid type " << portType << ".";

		if (port.GetConnections()[0u] == nullptr)
		{
			continue;
		}

		const auto& ports = (portType == "input")
			? port.GetConnections()[0]->GetMappedSinks()
			: port.GetConnections()[0]->GetMappedSources();
		for (const auto& port : ports)
		{
			if (std::find(gates.begin(), gates.end(), port.node) != gates.end())
			{
				portMap.insert_or_assign(portName, port);
			}
		}
	}

	return portMap;
}

std::string to_string(const CellAwareFaultStateMachine& stateMachine)
{
	std::string result;
	for (size_t stateIndex { 0u }; stateIndex < stateMachine.GetStates().size(); stateIndex++)
	{
		const auto& state = stateMachine.GetState(stateIndex);
		result += boost::str(boost::format("\nState %d depth=%d") % stateIndex % state.depth);

		for (size_t transIndex { 0u }; transIndex < state.transitions.size(); transIndex++)
		{
			const auto& transition = state.transitions[transIndex];
			result += "\n    " + to_string(transition.type) + " to " + std::to_string(transition.toState);
			result += " (" + to_string(transition.inputs) + " -> " + to_string(transition.outputs) + ")";
			result += ", " + std::string(transition.accepting ? "accepting" : "not accepting");
		}
	}
	return result;
}

std::string to_string(const CellAwareFault& fault)
{
	return boost::str(boost::format("CA fault %s @ %s (%s)")
		% fault.GetName() % fault.GetCellName() % fault.GetCellType());
}

static std::string to_string(const CellAwareFaultStateMachine::TransitionType& type)
{
	switch (type)
	{
		case CellAwareFaultStateMachine::TransitionType::Primary: return "primary";
		case CellAwareFaultStateMachine::TransitionType::Secondary: return "secondary";
		default: __builtin_unreachable();
	}
}

static std::string to_string(const std::vector<CellAwareFaultStateMachine::Port>& ports)
{
	std::string result;
	for (const auto& port : ports)
	{
		if (result.size() != 0) result += ", ";
		result += std::to_string(port.nodeAndPort.node->GetNodeId());
		switch (port.nodeAndPort.port.portType)
		{
			case Circuit::PortType::Input: result += "/I"; break;
			case Circuit::PortType::Output: result += "/O"; break;
		}
		result += std::to_string(port.nodeAndPort.port.portNumber);
		result += "=" + to_string(port.constraint);
	}
	return result;
}

};
};
