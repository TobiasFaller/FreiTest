#include "Io/CircuitGuard/CircuitGuard.hpp"

namespace FreiTest
{
namespace Io
{

bool ValidateCircuitGuard(const boost::property_tree::ptree& root, const Circuit::CircuitEnvironment& circuit)
{
	using Ptree = boost::property_tree::ptree;

	const auto& mappedCircuit { circuit.GetMappedCircuit() };
	const auto& metaData { circuit.GetMetaData() };

	std::string circuitName = root.get_child("name").get_value<std::string>();
	if (circuit.GetName() != circuitName)
	{
		LOG(ERROR) << "The circuit name does not match";
		return false;
	}

	size_t primaryInputIndex { 0u };
	const Ptree& circuitPrimaryInputs { root.get_child("primary_inputs") };
	for (auto it = circuitPrimaryInputs.ordered_begin(); it != circuitPrimaryInputs.not_found(); ++it, ++primaryInputIndex)
	{
		const Ptree& inputItem = it->second;

		if (inputItem.get_child("index").get_value<size_t>() != primaryInputIndex)
		{
			LOG(ERROR) << "Primary input has invalid index " << inputItem.get_child("index").get_value<size_t>()
				<< " while it should be " << primaryInputIndex;
			return false;
		}

		if (primaryInputIndex >= mappedCircuit.GetNumberOfPrimaryInputs())
		{
			LOG(ERROR) << "Primary input has has index " << primaryInputIndex << " while circuit has only " << mappedCircuit.GetNumberOfPrimaryInputs() << " inputs";
			return false;
		}

		if (auto expected = metaData.GetFriendlyName(
				{ mappedCircuit.GetPrimaryInput(primaryInputIndex), { Circuit::PortType::Output, 0u } }
			); inputItem.get_child("name").get_value<std::string>() != expected)
		{
			LOG(ERROR) << "Primary input has invalid name \"" << inputItem.get_child("name").get_value<std::string>()
				<< "\" while it is named \"" << expected << "\" in the circuit";
			return false;
		}
	}

	size_t primaryOutputIndex { 0u };
	const Ptree& circuitPrimaryOutputs { root.get_child("primary_outputs") };
	for (auto it = circuitPrimaryOutputs.ordered_begin(); it != circuitPrimaryOutputs.not_found(); ++it, ++primaryOutputIndex)
	{
		const Ptree& outputItem = it->second;

		if (outputItem.get_child("index").get_value<size_t>() != primaryOutputIndex)
		{
			LOG(ERROR) << "Primary output has invalid index " << outputItem.get_child("index").get_value<size_t>()
				<< " while it should be " << primaryOutputIndex;
			return false;
		}

		if (primaryOutputIndex >= mappedCircuit.GetNumberOfPrimaryOutputs())
		{
			LOG(ERROR) << "Primary output has has index " << primaryOutputIndex << " while circuit has only " << mappedCircuit.GetNumberOfPrimaryOutputs() << " outputs";
			return false;
		}

		if (auto expected = metaData.GetFriendlyName(
				{ mappedCircuit.GetPrimaryOutput(primaryOutputIndex), { Circuit::PortType::Input, 0u } }
			); outputItem.get_child("name").get_value<std::string>() != expected)
		{
			LOG(ERROR) << "Primary output has invalid name \"" << outputItem.get_child("name").get_value<std::string>()
				<< "\" while it is named \"" << expected << "\" in the circuit";
			return false;
		}
	}

	size_t secondaryInputIndex { 0u };
	const Ptree& circuitSecondaryInputs { root.get_child("secondary_inputs") };
	for (auto it = circuitSecondaryInputs.ordered_begin(); it != circuitSecondaryInputs.not_found(); ++it, ++secondaryInputIndex)
	{
		const Ptree& inputItem { it->second };

		if (inputItem.get_child("index").get_value<size_t>() != secondaryInputIndex)
		{
			LOG(ERROR) << "Secondary input has invalid index " << inputItem.get_child("index").get_value<size_t>()
				<< " while it should be " << secondaryInputIndex;
			return false;
		}

		if (secondaryInputIndex >= mappedCircuit.GetNumberOfSecondaryInputs())
		{
			LOG(ERROR) << "Secondary input has has index " << secondaryInputIndex << " while circuit has only " << mappedCircuit.GetNumberOfSecondaryInputs() << " inputs";
			return false;
		}

		if (auto expected = metaData.GetFriendlyName(
				{ mappedCircuit.GetSecondaryInput(secondaryInputIndex)->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }
			); inputItem.get_child("name").get_value<std::string>() != expected)
		{
			LOG(ERROR) << "Secondary input has invalid name \"" << inputItem.get_child("name").get_value<std::string>()
				<< "\" while it is named \"" << expected << "\" in the circuit";
			return false;
		}
	}

	size_t secondaryOutputIndex { 0u };
	const Ptree& circuitSecondaryOutputs { root.get_child("secondary_outputs") };
	for (auto it = circuitSecondaryOutputs.ordered_begin(); it != circuitSecondaryOutputs.not_found(); ++it, ++secondaryOutputIndex)
	{
		const Ptree& outputItem { it->second };

		if (outputItem.get_child("index").get_value<size_t>() != secondaryOutputIndex)
		{
			LOG(ERROR) << "Secondary output has invalid index " << outputItem.get_child("index").get_value<size_t>()
				<< " while it should be " << secondaryOutputIndex;
			return false;
		}

		if (secondaryOutputIndex >= mappedCircuit.GetNumberOfSecondaryOutputs())
		{
			LOG(ERROR) << "Secondary output has has index " << secondaryOutputIndex << " while circuit has only " << mappedCircuit.GetNumberOfSecondaryOutputs() << " outputs";
			return false;
		}

		if (auto expected = metaData.GetFriendlyName(
				{ mappedCircuit.GetSecondaryOutput(secondaryOutputIndex), { Circuit::PortType::Input, 0u } }
			); outputItem.get_child("name").get_value<std::string>() != expected)
		{
			LOG(ERROR) << "Secondary output has invalid name \"" << outputItem.get_child("name").get_value<std::string>()
				<< "\" while it is named \"" << expected << "\" in the circuit";
			return false;
		}
	}

	return true;
}

boost::property_tree::ptree CreateCircuitGuard(const Circuit::CircuitEnvironment& circuit)
{
	using Ptree = boost::property_tree::ptree;

	const auto& mappedCircuit { circuit.GetMappedCircuit() };
	const auto& metaData { circuit.GetMetaData() };

	Ptree circuitItem;
	circuitItem.put("name", circuit.GetName());

	Ptree primaryInputsItem;
	for (auto [inputIndex, inputNode] : mappedCircuit.EnumeratePrimaryInputs())
	{
		Ptree item;
		item.put("index", inputIndex);
		item.put("name", metaData.GetFriendlyName({ inputNode, { Circuit::PortType::Output, 0u } }));
		primaryInputsItem.push_back(std::make_pair("", item));
	}
	circuitItem.put_child("primary_inputs", primaryInputsItem);

	Ptree primaryOutputsItem;
	for (auto [outputIndex, outputNode] : mappedCircuit.EnumeratePrimaryOutputs())
	{
		Ptree item;
		item.put("index", outputIndex);
		item.put("name", metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }));
		primaryOutputsItem.push_back(std::make_pair("", item));
	}
	circuitItem.put_child("primary_outputs", primaryOutputsItem);

	Ptree secondaryInputsItem;
	for (auto [inputIndex, inputNode] : mappedCircuit.EnumerateSecondaryInputs())
	{
		Ptree item;
		item.put("index", inputIndex);
		item.put("name", metaData.GetFriendlyName({ inputNode->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }));
		secondaryInputsItem.push_back(std::make_pair("", item));
	}
	circuitItem.put_child("secondary_inputs", secondaryInputsItem);

	Ptree secondaryOutputsItem;
	for (auto [outputIndex, outputNode] : mappedCircuit.EnumerateSecondaryOutputs())
	{
		Ptree item;
		item.put("index", outputIndex);
		item.put("name", metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }));
		secondaryOutputsItem.push_back(std::make_pair("", item));
	}
	circuitItem.put_child("secondary_outputs", secondaryOutputsItem);

	return circuitItem;
}

};
};
