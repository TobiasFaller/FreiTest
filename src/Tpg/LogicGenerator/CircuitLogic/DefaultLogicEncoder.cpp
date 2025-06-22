#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"

#include <boost/core/demangle.hpp>

#include <type_traits>

#include "Basic/Logging.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CellLibrary.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;
using namespace SolverProxy::Bmc;

namespace FreiTest
{
namespace Tpg
{

template<typename>
struct is_std_vector: std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T, A>>: std::true_type {};

template<typename T, typename... U>
constexpr bool is_std_vector_v = is_std_vector<T>::value;

template<typename LogicContainer>
LogicContainer EncodeConstant(LogicEncoder& encoder, CellType type);

template<typename LogicContainer, typename... InputLogicContainer>
LogicContainer EncodeCombinatorialGate(LogicEncoder& encoder, CellCategory type, InputLogicContainer... inputs);

template<typename PinData, typename Tag>
DefaultLogicEncoder<PinData, Tag>::DefaultLogicEncoder(LogicConstraint constraint):
	ICircuitEncoder<PinData>(),
	_constraint(constraint)
{
}

template<typename PinData, typename Tag>
DefaultLogicEncoder<PinData, Tag>::~DefaultLogicEncoder(void) = default;

template<typename PinData, typename Tag>
std::string DefaultLogicEncoder<PinData, Tag>::GetName(void) const
{
	return "DefaultLogicEncoder<" + std::string(get_tag_name_v<Tag>) + ">";
}

template<typename PinData, typename Tag>
bool DefaultLogicEncoder<PinData, Tag>::EncodeNode(GeneratorContext<PinData>& context, size_t timeframeId, size_t nodeId)
{
	using LogicContainer = get_pin_data_container_t<PinData, Tag>;

	const MappedCircuit& circuit = context.GetMappedCircuit();
	Timeframe<PinData>& timeframe = context.GetTimeframe(timeframeId);
	LogicEncoder& encoder = context.GetEncoder();

	if (!timeframe.template HasTag<Tag>(nodeId))
	{
		return true;
	}

	const auto* node = circuit.GetNode(nodeId);
	switch(node->GetCellCategory())
	{
		case MAIN_CONSTANT:
		case MAIN_BUF:
		case MAIN_INV:
		case MAIN_XOR:
		case MAIN_AND:
		case MAIN_OR:
		case MAIN_XNOR:
		case MAIN_NAND:
		case MAIN_NOR:
		case MAIN_BUFIF:
		case MAIN_NOTIF:
		case MAIN_MUX:
		 	// Encode the circuit node
			break;

		case MAIN_IN:
		case MAIN_OUT:
			// Do not encode as other modules are responsible for that
			return true;

		case MAIN_UNKNOWN:
			Logging::Panic("Found unknown gate in circuit");
			__builtin_unreachable();
			break;

		default:
			Logging::Panic("Encountered node with unsupported type \"" + to_string(node->GetCellCategory()) + "\"");
			__builtin_unreachable();
			break;
	}

	DVLOG(3) << "Encoding node " << nodeId << " with type "
		<< to_string(node->GetCellCategory())
		<< " / " << to_string(node->GetCellType())
		<< " and tag " << get_tag_name_v<Tag> << ".";

	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(context.GetSolver());
	if (bmcSolver)
	{
		bmcSolver->SetTargetClauseType(ClauseType::Transition);
		bmcSolver->SetTargetVariableType(VariableType::Auxiliary);
	}

	const size_t inputCount = node->GetNumberOfInputs();
	const auto get_input_container = [&](size_t portId) -> auto {
		auto container = timeframe.template GetContainer<Tag>(context, nodeId, PortType::Input, portId);
		if (!container.IsSet())
		{
			LOG(FATAL) << "Found node with unconnected input pin " << std::to_string(portId) << ".";
		}
		return container;
	};

	auto& outLogic = timeframe.template GetInternalContainer<Tag>(context, nodeId);

	switch(node->GetCellCategory())
	{
		case MAIN_CONSTANT:
			if (__builtin_expect(inputCount, 0u) != 0u)
			{
				Logging::Panic("Found constant with input pins");
			}

			outLogic = EncodeConstant<LogicContainer>(encoder, node->GetCellType());
			break;

		case MAIN_BUF:
		case MAIN_INV:
		case MAIN_XOR:
		case MAIN_AND:
		case MAIN_OR:
		case MAIN_XNOR:
		case MAIN_NAND:
		case MAIN_NOR:
			switch (__builtin_expect(inputCount, 2u))
			{
				case 0u:
					Logging::Panic("Found combinatorial gate with 0 inputs");
					break;

				case 1u:
					outLogic = EncodeCombinatorialGate<LogicContainer>(
						encoder, node->GetCellCategory(),
						get_input_container(0u)
					);
					break;

				case 2u:
					outLogic = EncodeCombinatorialGate<LogicContainer>(
						encoder, node->GetCellCategory(),
						get_input_container(0u),
						get_input_container(1u)
					);
					break;

				default:
				{
					std::vector<LogicContainer> inputs { };
					inputs.reserve(node->GetNumberOfInputs());
					for (size_t input { 0u }; input < node->GetNumberOfInputs(); input++)
					{
						inputs.push_back(get_input_container(input));
					}

					outLogic = EncodeCombinatorialGate<LogicContainer>(
						encoder, node->GetCellCategory(),
						inputs
					);
					break;
				}
			}
			break;

		case MAIN_BUFIF:
		case MAIN_NOTIF:
		{
			if (__builtin_expect(inputCount, 2u) != 2u)
			{
				Logging::Panic("Found driver gate which has not exactly two inputs.");
				break;
			}

			LogicContainer inputIn = get_input_container(0u);
			LogicContainer inputEn = get_input_container(1u);
			outLogic = encoder.EncodeTristateBuffer<LogicContainer>(
				(node->GetCellCategory() == CellCategory::MAIN_BUFIF) ? inputIn : -inputIn,
				(node->GetCellType() == CellType::BUFIF1 || node->GetCellType() == CellType::NOTIF1) ? inputEn : -inputEn
			);
			break;
		}

		case MAIN_MUX:
		{
			if (__builtin_expect(inputCount, 3u) != 3u)
			{
				Logging::Panic("Found multiplexer gate which has not exactly three inputs.");
				break;
			}

			LogicContainer inputIn1 = get_input_container(0u);
			LogicContainer inputIn2 = get_input_container(1u);
			LogicContainer inputSel = get_input_container(2u);
			outLogic = encoder.EncodeMultiplexer<LogicContainer>(inputIn1, inputIn2, inputSel);
			break;
		}

		default:
			Logging::Panic("Encountered invalid state");
	}

	return true;
}

template<typename LogicContainer>
LogicContainer EncodeConstant(LogicEncoder& encoder, CellType type)
{
	switch (type)
	{
		case CellType::PRESET_0:
			return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
		case CellType::PRESET_1:
			return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_ONE);
		case CellType::PRESET_X:
			if constexpr (is_01_encoding_v<LogicContainer>)
			{
				VLOG(3) << "Encountered PRESET_X constant gate, encoding it as LOGIC_ZERO as the container does not support LOGIC_DONT_CARE";
				return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
			}
			else if constexpr (is_01X_encoding_v<LogicContainer> || is_U01X_encoding_v<LogicContainer>)
			{
				return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
			}

			Logging::Panic("Encountered invalid constant value \"" + to_string(type)
				+ "\" for logic container " + boost::core::demangle(typeid(LogicContainer).name()));
			break;

		case CellType::UNDEFTYPE:
			if constexpr (is_01_encoding_v<LogicContainer>)
			{
				VLOG(3) << "Encountered UNDEFTYPE constant gate, encoding it as LOGIC_ZERO as the container does not support LOGIC_UNKNOWN";
				return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_ZERO);
			}
			else if constexpr (is_01X_encoding_v<LogicContainer>)
			{
				VLOG(3) << "Encountered UNDEFTYPE constant gate, encoding it as LOGIC_DONT_CARE as the container does not support LOGIC_UNKNOWN";
				return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_DONT_CARE);
			}
			else if constexpr (is_U01X_encoding_v<LogicContainer>)
			{
				return encoder.NewLogicContainer<LogicContainer>(Logic::LOGIC_UNKNOWN);
			}

			Logging::Panic("Encountered invalid constant value \"" + to_string(type)
				+ "\" for logic container " + boost::core::demangle(typeid(LogicContainer).name()));
			break;

		default:
			Logging::Panic("Encountered invalid constant value");
	}
}

template<typename LogicContainer, typename... InputLogicContainer>
LogicContainer EncodeCombinatorialGate(LogicEncoder& encoder, CellCategory type, InputLogicContainer... inputs)
{
	if constexpr (sizeof...(inputs) == 1 && !is_std_vector_v<InputLogicContainer...>)
	{
		switch (type)
		{
			case MAIN_BUF:
			case MAIN_XOR:
			case MAIN_OR:
			case MAIN_AND:
				return encoder.EncodeBuffer<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_INV:
			case MAIN_XNOR:
			case MAIN_NOR:
			case MAIN_NAND:
				return encoder.EncodeInverter<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			default:
				Logging::Panic("Encountered invalid node type");
		}
	}
	else
	{
		switch (type)
		{
			case MAIN_BUF:
				Logging::Panic("Encountered buffer with more than one input");

			case MAIN_INV:
				Logging::Panic("Encountered inverter with more than one input");

			case MAIN_XOR:
				return encoder.EncodeXor<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_OR:
				return encoder.EncodeOr<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_AND:
				return encoder.EncodeAnd<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_XNOR:
				return encoder.EncodeXnor<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_NOR:
				return encoder.EncodeNor<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			case MAIN_NAND:
				return encoder.EncodeNand<LogicContainer>(std::forward<InputLogicContainer>(inputs)...);

			default:
				Logging::Panic("Encountered invalid node type");
		}
	}
}

#define FOR_TAG(PINDATA, TAG) template class DefaultLogicEncoder<PINDATA, TAG>;
INSTANTIATE_FOR_ALL_ENCODINGS

};
};
