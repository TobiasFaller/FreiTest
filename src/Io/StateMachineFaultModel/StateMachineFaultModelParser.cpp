#include "Io/StateMachineFaultModel/StateMachineFaultModelParser.hpp"

#include "Basic/Logging.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"

namespace FreiTest
{
namespace Io
{
namespace StateMachineFaultModel
{

std::unique_ptr<Fault::CellAwareModelSpecification> Parse(std::istream& input)
{
	boost::property_tree::ptree root;
	try
	{
		boost::property_tree::read_jsonc(input, root);

		std::vector<Fault::CellAwareCellSpecification> cells;
		for (auto& [cellName, cell] : root)
		{
			std::vector<Fault::CellAwareFaultSpecification> faults;
			for (auto& [faultName, fault] : cell)
			{
				std::vector<Fault::CellAwareFaultSpecification::State> states;
				for (auto& [_key, state] : fault)
				{
					auto transitionsIt = state.find("transitions");
					if (transitionsIt == state.not_found())
					{
						LOG(ERROR) << "Found state without \"transitions\"";
						return { };
					}

					std::vector<Fault::CellAwareFaultSpecification::Transition> transitions;
					for (auto& [_key, transition] : transitionsIt->second)
					{
						auto toStateIt = transition.find("to-state");
						auto inputsIt = transition.find("inputs");
						auto outputsIt = transition.find("outputs");
						auto propagationsIt = transition.find("propagations");
						if (toStateIt == transition.not_found())
						{
							LOG(ERROR) << "Found transition without \"to-state\"";
							return { };
						}
						if (inputsIt == transition.not_found())
						{
							LOG(ERROR) << "Found transition without \"inputs\"";
							return { };
						}
						if (outputsIt == transition.not_found())
						{
							LOG(ERROR) << "Found transition without \"outputs\"";
							return { };
						}

						auto toState { toStateIt->second.get_value<size_t>() };
						auto accepting { propagationsIt != transition.not_found() && propagationsIt->second.size() > 0 };

						std::vector<Fault::CellAwareFaultSpecification::Port> inputs;
						for (auto& [port, constraint] : inputsIt->second)
						{
							inputs.push_back({
								.name = port,
								.constraint = Basic::GetLogicConstraintForString(
									constraint.get_value<std::string>())
							});
						}
						std::vector<Fault::CellAwareFaultSpecification::Port> outputs;
						for (auto& [port, constraint] : outputsIt->second)
						{
							outputs.push_back({
								.name = port,
								.constraint = Basic::GetLogicConstraintForString(
									constraint.get_value<std::string>())
							});
						}

						transitions.push_back({
							.inputs = inputs,
							.outputs = outputs,
							.toState = toState,
							.accepting = accepting,
						});
					}

					states.push_back({
						.transitions = transitions,
					});
				}

				faults.emplace_back(faultName, states);
			}

			cells.emplace_back(cellName, faults);
		}

		return std::make_unique<Fault::CellAwareModelSpecification>(cells);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
		return { };
	}
	catch (boost::property_tree::ptree_bad_path& exception)
	{
		LOG(ERROR) << "A key does not exist in json file: " << exception.what();
		return { };
	}
	catch (boost::property_tree::ptree_bad_data& exception)
	{
		LOG(ERROR) << "Invalid data value: " << exception.what();
		return { };
	}

	Logging::Panic("Unreachable code has been reached");
}

};
};
};
