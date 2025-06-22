#include "Applications/Circuit/ExportCircuitStructuralDependence.hpp"

#include <boost/format.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>

#include "Circuit/MappedNode.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitStructuralDependence::ExportCircuitStructuralDependence(void) = default;
ExportCircuitStructuralDependence::~ExportCircuitStructuralDependence(void) = default;

void ExportCircuitStructuralDependence::Init(void)
{
}

bool ExportCircuitStructuralDependence::SetSetting(std::string key, std::string value)
{
	return false;
}

void ExportCircuitStructuralDependence::Run(void)
{
	auto const& metaData { this->circuit->GetMetaData() };
	auto const& mappedCircuit { this->circuit->GetMappedCircuit() };

	FileHandle in2outHandle("[DataExportDirectory]/inputs2outputs.txt", false);
	std::ofstream& inputs2outputs = in2outHandle.GetOutStream();

	FileHandle out2inHandle("[DataExportDirectory]/outputs2inputs.txt", false);
	std::ofstream& outputs2inputs = out2inHandle.GetOutStream();

	std::shared_ptr<SolverProxy::ISolverProxy> nullProxy;
	Tpg::GeneratorContext<Tpg::PinDataG<Tpg::LogicContainer01>> context { nullProxy, this->circuit };
	context.SetNumberOfTimeframes(1u);
	auto timeframe { context.GetTimeframe(0u) };

	// Inputs to outputs relevance
	for (auto [index, primaryInput] : mappedCircuit.EnumerateInputs()) {
		timeframe.Reset();

		std::vector<size_t> nodes { primaryInput->GetNodeId() };
		while (nodes.size() != 0) {
			timeframe.TagOutputCone<Tpg::GoodTag>(context, nodes);

			nodes.clear();
			for (auto [index, secondaryInput] : mappedCircuit.EnumerateSecondaryInputs()) {
				auto secondaryOutput = mappedCircuit.GetSecondaryOutputForSecondaryInput(secondaryInput);
				if (!timeframe.template HasTag<Tpg::GoodTag>(secondaryOutput->GetNodeId())) {
					continue;
				}
				if (timeframe.template HasTag<Tpg::GoodTag>(secondaryInput->GetNodeId())) {
					continue;
				}
				nodes.push_back(secondaryInput->GetNodeId());
			}
		}

		auto name { metaData.GetFriendlyName({ primaryInput, { Circuit::PortType::Output, 0u } }) };
		ASSERT(name.find("/") != std::string::npos) << "Primary input is missing circuit name";
		inputs2outputs << name.substr(name.find("/") + 1u) << std::endl;

		for (auto [index, primaryOutput] : mappedCircuit.EnumeratePrimaryOutputs()) {
			if (!timeframe.template HasTag<Tpg::GoodTag>(primaryOutput->GetNodeId())) {
				continue;
			}

			auto name { metaData.GetFriendlyName({ primaryOutput, { Circuit::PortType::Input, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Primary output is missing circuit name";
			inputs2outputs << "\t" << name.substr(name.find("/") + 1u) << std::endl;
		}
	}

	// Outputs to inputs relevance
	for (auto [index, primaryOutput] : mappedCircuit.EnumerateOutputs()) {
		timeframe.Reset();

		std::vector<size_t> nodes { primaryOutput->GetNodeId() };
		while (nodes.size() != 0) {
			timeframe.TagInputCone<Tpg::GoodTag>(context, nodes);

			nodes.clear();
			for (auto [index, secondaryOutput] : mappedCircuit.EnumerateSecondaryOutputs()) {
				auto secondaryInput = mappedCircuit.GetSecondaryInputForSecondaryOutput(secondaryOutput);
				if (!timeframe.template HasTag<Tpg::GoodTag>(secondaryInput->GetNodeId())) {
					continue;
				}
				if (timeframe.template HasTag<Tpg::GoodTag>(secondaryOutput->GetNodeId())) {
					continue;
				}
				nodes.push_back(secondaryOutput->GetNodeId());
			}
		}

		auto name { metaData.GetFriendlyName({ primaryOutput, { Circuit::PortType::Input, 0u } }) };
		ASSERT(name.find("/") != std::string::npos) << "Primary output is missing circuit name";
		outputs2inputs << name.substr(name.find("/") + 1u) << std::endl;

		for (auto [index, primaryInput] : mappedCircuit.EnumeratePrimaryInputs()) {
			if (!timeframe.template HasTag<Tpg::GoodTag>(primaryInput->GetNodeId())) {
				continue;
			}

			auto name { metaData.GetFriendlyName({ primaryInput, { Circuit::PortType::Output, 0u } }) };
			ASSERT(name.find("/") != std::string::npos) << "Primary input is missing circuit name";
			outputs2inputs << "\t" << name.substr(name.find("/") + 1u) << std::endl;
		}
	}
}

};
};
};
