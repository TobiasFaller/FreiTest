#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"

#include <cstddef>

#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Circuit;
using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename Tag>
SequentialOutputLogicEncoder<PinData, Tag>::SequentialOutputLogicEncoder(SequentialConfig sequentialConfig):
	_sequentialConfig(sequentialConfig)
{
}

template<typename PinData, typename Tag>
SequentialOutputLogicEncoder<PinData, Tag>::~SequentialOutputLogicEncoder(void) = default;

template<typename PinData, typename Tag>
std::string SequentialOutputLogicEncoder<PinData, Tag>::GetName(void) const
{
	return "SequentialOutputLogicEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool SequentialOutputLogicEncoder<PinData, Tag>::EncodeNode(GeneratorContext<PinData>& context, size_t timeframeId, size_t nodeId)
{
	const auto& circuit = context.GetMappedCircuit();
	auto& encoder = context.GetEncoder();
	auto& timeframe = context.GetTimeframe(timeframeId);

	if (!circuit.IsSecondaryOutput(nodeId))
	{
		return true;
	}
	if (!timeframe.template HasTag<Tag>(nodeId))
	{
		return true;
	}

	const auto* secondaryOutput = circuit.GetNode(nodeId);
	const auto* secondaryInput = circuit.GetSecondaryInputForSecondaryOutput(secondaryOutput);
	ASSERT(secondaryInput != nullptr) << "Found invalid secondary input";
	ASSERT(secondaryOutput != nullptr) << "Found invalid secondary output";
	ASSERT(secondaryOutput->GetNumberOfInputs() == 4) << "Found secondary output with unsupported number of inputs";

	auto get_input_container = [&](const MappedNode* node, size_t portId) -> auto {
		return timeframe.template GetContainer<Tag>(context, node->GetNodeId(), PortType::Input, portId);
	};
	auto get_internal_container = [&](const MappedNode* node) -> auto& {
		return timeframe.template GetInternalContainer<Tag>(context, node->GetNodeId());
	};
	auto encode_set_reset_model = [&](auto input, auto set, auto reset) -> auto {
		switch (_sequentialConfig.setResetModel)
		{
			case SetResetModel::None:
				return input;

			case SetResetModel::OnlySet:
				ASSERT(set.IsSet()) << "The model specifies the set port to be implemented but there is no set port present";
				return encoder.EncodeSetControl(input, set);

			case SetResetModel::OnlyReset:
				ASSERT(reset.IsSet()) << "The model specifies the reset port to be implemented but there is no set port present";
				return encoder.EncodeResetControl(input, reset);

			case SetResetModel::SetHasPriority:
				ASSERT(set.IsSet()) << "The model specifies the set port to be implemented but there is no set port present";
				ASSERT(reset.IsSet()) << "The model specifies the reset port to be implemented but there is no set port present";
				return encoder.EncodeSetControl(encoder.EncodeResetControl(input, reset), set);

			case SetResetModel::ResetHasPriority:
				ASSERT(set.IsSet()) << "The model specifies the set port to be implemented but there is no set port present";
				ASSERT(reset.IsSet()) << "The model specifies the reset port to be implemented but there is no set port present";
				return encoder.EncodeResetControl(encoder.EncodeSetControl(input, set), reset);

			default:
				Logging::Panic("Unknown set / reset model");
		}
	};
	auto get_cell_type_for_sequential_model = [&](void) -> CellType {
		switch(_sequentialConfig.sequentialModel)
		{
			case SequentialModel::Keep:
				return secondaryOutput->GetCellType();

			case SequentialModel::None:
				Logging::Panic("No sequential model was specified but sequential element was found in the circuit");

			case SequentialModel::Unclocked:
				return CellType::S_OUT;

			case SequentialModel::FlipFlop:
				return CellType::S_OUT_CLK;

			case SequentialModel::Latch:
				return CellType::S_OUT_EN;

			default:
				Logging::Panic("Invalid sequential model was selected");
		}
	};

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	switch (get_cell_type_for_sequential_model())
	{
		case CellType::S_OUT:
		{
			// Sequential element that updates the internal state independent of any clock.
			ASSERT(secondaryOutput->GetInput(0) != nullptr) << "Found sequential element with unconnected input pin";

			auto& outLogic = get_internal_container(secondaryOutput);
			const auto input = get_input_container(secondaryOutput, 0);
			outLogic = encode_set_reset_model(
				input,
				get_input_container(secondaryOutput, 2),
				get_input_container(secondaryOutput, 3)
			);
			break;
		}

		case CellType::S_OUT_CLK:
		{
			// Master-Slave Flip-Flop that updates the value according to the clock input.
			// The SAT implementation might assume a clock cycle for every second timeframe
			// and access the logic value of the previous timeframes.
			// This BMC encoding might not be possible without duplicating all
			// latches to hold the master and slave value at the same time as each timeframe
			// is only able to reference only the next timeframe.
			Logging::Panic("Not implemented yet!");
			break;
		}

		case CellType::S_OUT_EN:
		{
			// Latch that updates the value if the enable input is logic high.
			ASSERT(secondaryOutput->GetInput(0) != nullptr) << "Found latch with unconnected input pin";
			ASSERT(secondaryOutput->GetInput(1) == nullptr) << "Found latch without enable input";

			// The original starting state from the same timeframe.
			// In the case that a fault model is present DO NOT use the faulty value.
			auto& outLogic = get_internal_container(secondaryOutput);
			const auto state = get_internal_container(secondaryInput);
			const auto input = get_input_container(secondaryOutput, 0);
			const auto enable = get_input_container(secondaryOutput, 1);
			outLogic = encode_set_reset_model(
				encoder.EncodeMultiplexer(state, input, enable),
				get_input_container(secondaryOutput, 2),
				get_input_container(secondaryOutput, 3)
			);
			break;
		}

		default:
			Logging::Panic("Encountered invalid secondary output port type");
	}

	return true;
}

#define FOR_TAG(PINDATA, TAG) template class SequentialOutputLogicEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_ENCODINGS

};
};
