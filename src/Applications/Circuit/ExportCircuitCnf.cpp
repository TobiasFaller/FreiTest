#include "Applications/Circuit/ExportCircuitCnf.hpp"

#include <boost/format.hpp>
#include <boost/utility.hpp>

#include <algorithm>
#include <vector>
#include <string>
#include <regex>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tuple>

#include "Basic/Settings.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/LogicGenerator/Tagger/Generic/WholeCircuitTagger.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/DefaultLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/InputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/OutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialInputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialOutputLogicEncoder.hpp"
#include "Tpg/LogicGenerator/CircuitLogic/SequentialConnectionEncoder.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Sat/DimacsExportProxy.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace SolverProxy::Sat;

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

ExportCircuitCnf::ExportCircuitCnf(void) = default;
ExportCircuitCnf::~ExportCircuitCnf(void) = default;

void ExportCircuitCnf::Init(void)
{
}

bool ExportCircuitCnf::SetSetting(std::string key, std::string value)
{
	return false;
}

void ExportCircuitCnf::Run(void)
{
	const auto circuitName { this->circuit->GetName() };
	const auto& metaData { this->circuit->GetMetaData() };
	const auto& mappedCircuit { this->circuit->GetMappedCircuit() };

	// ------------------------------------------------------------------------
	// Circuit to CNF encoding
	// ------------------------------------------------------------------------

	using LogicContainer = LogicContainer01;
	using PinData = PinDataG<LogicContainer>;

	std::shared_ptr<SatSolverProxy> solverProxy { SatSolverProxy::CreateSatSolver(SatSolver::EXPORT_SAT_DIMACS) };
	auto logicGenerator { std::make_shared<Tpg::LogicGenerator<PinData>>(solverProxy, this->circuit) };
	auto& context { logicGenerator->GetContext() };
	context.SetIncremental(false);
	context.SetNumberOfTimeframes(1u);

	logicGenerator->template EmplaceModule<Tpg::WholeCircuitTagger<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::InputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::DefaultLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialInputLogicEncoder<PinData, GoodTag>>(LogicConstraint::ONLY_LOGIC_01X);
	logicGenerator->template EmplaceModule<Tpg::SequentialOutputLogicEncoder<PinData, GoodTag>>(Basic::MakeUnclockedSetResetFlipFlopModel());
	logicGenerator->template EmplaceModule<Tpg::SequentialConnectionEncoder<PinData, GoodTag>>();
	logicGenerator->template EmplaceModule<Tpg::OutputLogicEncoder<PinData, GoodTag>>();
	logicGenerator->GenerateCircuitLogic();

	// ------------------------------------------------------------------------
	// Annotated CNF export
	// ------------------------------------------------------------------------

	std::vector<std::string> comments;
	comments.push_back(boost::str(boost::format("Circuit: %s") % circuitName));
	comments.push_back(boost::str(boost::format("    Primary Inputs: %d (Normal Inputs)") % mappedCircuit.GetNumberOfPrimaryInputs()));
	comments.push_back(boost::str(boost::format("    Primary Outputs: %d (Normal Outputs)") % mappedCircuit.GetNumberOfPrimaryOutputs()));
	comments.push_back(boost::str(boost::format("    Sequential Elements: %d (Flip-Flops, Latches)") % mappedCircuit.GetNumberOfSecondaryInputs()));

	auto const timeframes { context.GetNumberOfTimeframesForTarget(GenerationTarget::PatternExtraction) };
	for (size_t timeframeIndex { 0u }; timeframeIndex < timeframes; timeframeIndex++)
	{
		auto const& timeframe { context.GetTimeframe(timeframeIndex) };

		comments.push_back(boost::str(boost::format("Timeframe: %d") % timeframeIndex));

		// Primary inputs and outputs
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumeratePrimaryInputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ inputNode, { Circuit::PortType::Output, 0u } }) };
			const auto container { timeframe.GetContainer<GoodTag>(context, inputNode->GetNodeId(), Circuit::PortType::Output, 0u) };
			comments.push_back(boost::str(boost::format("    PI %s: %d") % friendlyName % container.l0.GetSigned()));
		}
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumeratePrimaryOutputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			const auto container { timeframe.GetContainer<GoodTag>(context, outputNode->GetNodeId(), Circuit::PortType::Input, 0u) };
			comments.push_back(boost::str(boost::format("    PO %s: %d") % friendlyName % container.l0.GetSigned()));
		}

		// Secondary inputs and outputs (sequential elements)
		for (auto [inputIndex, inputNode] : mappedCircuit.EnumerateSecondaryInputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ inputNode->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }) };
			const auto container { timeframe.GetContainer<GoodTag>(context, inputNode->GetNodeId(), Circuit::PortType::Output, 0u) };
			comments.push_back(boost::str(boost::format("    SI %s: %d") % friendlyName % container.l0.GetSigned()));

		}
		for (auto [outputIndex, outputNode] : mappedCircuit.EnumerateSecondaryOutputs())
		{
			const auto friendlyName { metaData.GetFriendlyName({ outputNode, { Circuit::PortType::Input, 0u } }) };
			const auto container { timeframe.GetContainer<GoodTag>(context, outputNode->GetNodeId(), Circuit::PortType::Input, 0u) };
			comments.push_back(boost::str(boost::format("    SO %s: %d") % friendlyName % container.l0.GetSigned()));
		}

		for (auto [nodeId, node] : mappedCircuit.EnumerateNodes())
		{
			if (mappedCircuit.IsInput(nodeId) || mappedCircuit.IsOutput(nodeId))
			{
				continue;
			}

			const auto* group { metaData.GetGroup(node) };
			const auto* cell { group->GetParent() };
			if (cell == nullptr)
			{
				continue;
			}

			const auto& sourceInfo { cell->GetSourceInfo() };
			if (!sourceInfo.GetProperty<bool>("module-is-cell").value_or(false))
			{
				continue;
			}

			bool isCellOutput = false;
			std::string portName;
			for (auto port: cell->GetPorts())
			{
				const auto portType { port.GetSourceInfo().GetProperty<std::string>("port-type").value_or("") };
				if (portType != "output"
					|| port.GetConnections().size() != 1u
					|| port.GetConnections()[0u] == nullptr)
				{
					continue;
				}

				auto const& nodeAndPorts { port.GetConnections()[0]->GetMappedSources() };
				for (auto& nodeAndPort : nodeAndPorts)
				{
					if (nodeAndPort.node->GetNodeId() == nodeId)
					{
						isCellOutput = true;
						portName =  port.GetHierarchyName();
					}
				}
			}

			if (!isCellOutput)
			{
				continue;
			}

			const auto container { timeframe.GetContainer<GoodTag>(context, node->GetNodeId(), Circuit::PortType::Output, 0u) };
			comments.push_back(boost::str(boost::format("    GT %s: %d") % portName % container.l0.GetSigned()));
		}
	}

	auto dimacsExportProxy { std::dynamic_pointer_cast<DimacsExportProxy>(solverProxy) };
	dimacsExportProxy->SetFilename(Settings::GetInstance()->MapFileName("[DataExportDirectory]/" + circuitName + ".cnf"));
	dimacsExportProxy->SetFormat(DimacsExportProxy::Format::Dimacs);
	dimacsExportProxy->SetCompression(DimacsExportProxy::Compression::None);
	dimacsExportProxy->SetComments(comments);
	dimacsExportProxy->Solve();
}

};
};
};
