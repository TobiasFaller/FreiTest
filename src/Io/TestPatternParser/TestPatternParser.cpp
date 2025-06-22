#include "Io/TestPatternParser/TestPatternParser.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>
#include <utility>

#include "Circuit/CircuitEnvironment.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Pattern/TestPatternList.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Io/CircuitGuard/CircuitGuard.hpp"

using namespace FreiTest::Basic;

namespace FreiTest
{
namespace Io
{

static std::string ConvertInputCaptureToString(Pattern::InputCapture capture)
{
	switch (capture)
	{
		case Pattern::InputCapture::PrimaryInputsOnly:
			return "primary-inputs-only";
		case Pattern::InputCapture::SecondaryInputsOnly:
			return "secondary-inputs-only";
		case Pattern::InputCapture::PrimaryAndSecondaryInputs:
			return "primary-and-secondary";
		case Pattern::InputCapture::PrimaryAndInitialSecondaryInputs:
			return "primary-and-initial-secondary";
		default:
			return "unknown";
	}
}

static Pattern::InputCapture ConvertStringToInputCapture(std::string capture)
{
	if (capture == "primary-inputs-only")
	{
		return Pattern::InputCapture::PrimaryInputsOnly;
	}
	if (capture == "secondary-inputs-only")
	{
		return Pattern::InputCapture::SecondaryInputsOnly;
	}
	if (capture == "primary-and-secondary")
	{
		return Pattern::InputCapture::PrimaryAndSecondaryInputs;
	}
	if (capture == "primary-and-initial-secondary")
	{
		return Pattern::InputCapture::PrimaryAndInitialSecondaryInputs;
	}

	throw boost::property_tree::ptree_bad_data("Invalid input capture given", capture);
}

bool ExportPatterns(std::ostream& output, const TestPatternExchangeFormat& patterns)
{
	using ptree = boost::property_tree::ptree;

	const auto& circuit { patterns.GetCircuit() };
	const auto& mappedCircuit { circuit.GetMappedCircuit() };
	const auto& testPatterns { patterns.GetTestPatterns() };
	const auto capture { patterns.GetInputCapture() };

	ptree exportPatternsList;
	size_t testPatternIndex { 0u };
	for (const auto pattern : testPatterns)
	{
		Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
		Simulation::SimulationResult simulation(pattern->GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
		Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, *pattern, {}, simulation, simConfig);

		ptree timeframesItem;
		for (size_t timeframe = 0; timeframe < pattern->GetNumberOfTimeframes(); ++timeframe)
		{
			ptree timeframeItem;
			timeframeItem.put("index", timeframe);

			if (capture == Pattern::InputCapture::PrimaryInputsOnly
				|| capture == Pattern::InputCapture::PrimaryAndSecondaryInputs
				|| capture == Pattern::InputCapture::PrimaryAndInitialSecondaryInputs)
			{
				std::string inputs;
				for (auto const& input : mappedCircuit.GetPrimaryInputs())
				{
					Logic value = simulation[timeframe][input->GetNodeId()];
					inputs += static_cast<char>(value);
				}
				timeframeItem.put("primary_inputs", inputs);

				std::string outputs;
				for (auto const& output : mappedCircuit.GetPrimaryOutputs())
				{
					Logic value = simulation[timeframe][output->GetNodeId()];
					outputs += static_cast<char>(value);
				}
				timeframeItem.put("primary_outputs", outputs);
			}

			if (capture == Pattern::InputCapture::SecondaryInputsOnly
				|| capture == Pattern::InputCapture::PrimaryAndSecondaryInputs
				|| (capture == Pattern::InputCapture::PrimaryAndInitialSecondaryInputs && timeframe == 0))
			{
				std::string inputs;
				for (auto const& input : mappedCircuit.GetSecondaryInputs())
				{
					Logic value = simulation[timeframe][input->GetNodeId()];
					inputs += static_cast<char>(value);
				}
				timeframeItem.put("secondary_inputs", inputs);

				std::string outputs;
				for (auto const& output : mappedCircuit.GetSecondaryOutputs())
				{
					Logic value = simulation[timeframe][output->GetNodeId()];
					outputs += static_cast<char>(value);
				}
				timeframeItem.put("secondary_outputs", outputs);
			}

			timeframesItem.push_back(std::make_pair("", timeframeItem));
		}

		ptree testPatternItem;
		testPatternItem.put("index", testPatternIndex++);
		testPatternItem.put_child("timeframes", timeframesItem);

		exportPatternsList.push_back(std::make_pair("", testPatternItem));
	}

	ptree root;
	root.put_child("patterns", exportPatternsList);
	root.put("pattern_capture", ConvertInputCaptureToString(capture));
	root.put_child("circuit", CreateCircuitGuard(circuit));

	try
	{
		boost::property_tree::write_json(output, root);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not write json data: " << exception.what();
		return false;
	}

	return true;
}

std::optional<TestPatternExchangeFormat> ImportPatterns(std::istream& input, const Circuit::CircuitEnvironment& circuit)
{
	using ptree = boost::property_tree::ptree;

	const auto& mappedCircuit { circuit.GetMappedCircuit() };

	try
	{
		ptree root;
		boost::property_tree::read_json(input, root);
		if (!ValidateCircuitGuard(root.get_child("circuit"), circuit))
		{
			return std::nullopt;
		}

		auto capture { ConvertStringToInputCapture(root.get_child("pattern_capture").get_value<std::string>()) };

		Pattern::TestPatternList patterns;
		ptree& treePatterns = root.get_child("patterns");
		for (auto patIt = treePatterns.ordered_begin(); patIt != treePatterns.not_found(); ++patIt)
		{
			ptree& patternItem = patIt->second;

			if (patternItem.get_child("index").get_value<size_t>() != patterns.size())
			{
				LOG(ERROR) << "Pattern has invalid index";
				return std::nullopt;
			}

			ptree& timeframesList = patternItem.get_child("timeframes");

			size_t timeframeIndex = 0u;
			Pattern::TestPattern pattern(timeframesList.size(), mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs(), Logic::LOGIC_DONT_CARE);
			for (auto tfIt = timeframesList.ordered_begin(); tfIt != timeframesList.not_found(); ++tfIt, ++timeframeIndex)
			{
				ptree& timeframeItem = tfIt->second;
				if (timeframeIndex >= pattern.GetNumberOfTimeframes())
				{
					LOG(ERROR) << "The timeframe " << timeframeIndex << " is longer than expected";
					return std::nullopt;
				}

				if (auto primaryInputs = timeframeItem.find("primary_inputs"); primaryInputs != timeframeItem.not_found())
				{
					std::string inputs = primaryInputs->second.get_value<std::string>();
					if (inputs.size() != pattern.GetNumberOfPrimaryInputs())
					{
						LOG(ERROR) << "The timeframe " << timeframeIndex << " has " << inputs.size() << " inputs while the circuit has " << pattern.GetNumberOfPrimaryInputs();
						return std::nullopt;
					}

					size_t inputIndex = 0u;
					for (char input : inputs)
					{
						pattern.SetPrimaryInput(timeframeIndex, inputIndex, static_cast<Logic>(input));
						inputIndex++;
					}
				}

				if (auto secondaryInputs = timeframeItem.find("secondary_inputs"); secondaryInputs != timeframeItem.not_found())
				{
					std::string inputs = secondaryInputs->second.get_value<std::string>();
					if (inputs.size() != pattern.GetNumberOfSecondaryInputs())
					{
						LOG(ERROR) << "The timeframe " << timeframeIndex << " has " << inputs.size() << " inputs while the circuit has " << pattern.GetNumberOfSecondaryInputs();
						return std::nullopt;
					}

					size_t inputIndex = 0u;
					for (char input : inputs)
					{
						pattern.SetSecondaryInput(timeframeIndex, inputIndex, static_cast<Logic>(input));
						inputIndex++;
					}
				}
			}

			patterns.emplace_back(pattern);
		}

		return std::make_optional<TestPatternExchangeFormat>(circuit, patterns, capture);
	}
	catch (boost::property_tree::json_parser_error& exception)
	{
		LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
	}
	catch (boost::property_tree::ptree_bad_path& exception)
	{
		LOG(ERROR) << "A key does not exist in json file: " << exception.what();
	}
	catch (boost::property_tree::ptree_bad_data& exception)
	{
		LOG(ERROR) << "Invalid data value: " << exception.what();
	}

	return std::nullopt;
}

};
};
