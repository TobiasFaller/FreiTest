#include "Applications/Mixins/Vcm/VcmMixin.hpp"

#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <cstdint>
#include <execution>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <vector>

#include "Basic/ApplicationStatistics.hpp"
#include "Basic/Logic.hpp"
#include "Basic/Logging.hpp"
#include "Basic/Parallel.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Helper/FileHandle.hpp"
#include "Helper/StringHelper.hpp"
#include "Io/VerilogImporter/VerilogConverter.hpp"
#include "Io/JsoncParser/JsonCParser.hpp"
#include "Simulation/CircuitSimulator.hpp"
#include "Tpg/Vcm/VcmCircuit.hpp"

using namespace FreiTest::Basic;
using namespace FreiTest::Tpg;
using namespace SolverProxy;

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

Mixin::VcmMixin::VcmMixin(std::string configPrefix):
	vcmCircuit(),
	vcmInputs(),
	vcmOutputs(),
	vcmEnable(VcmEnable::Disabled),
	vcmBaseDirectory("UNDEFINED"),
	vcmImportFilename("UNDEFINED"),
	vcmExportPreprocessedFilename(""),
	vcmExportProcessedFilename(""),
	vcmTopLevelModuleName("LAST_DEFINED"),
	vcmTags(),
	vcmParameters(),
	vcmConfiguration("NO_CONFIGURATION_SELECTED"),
	vcmConfigurations(),
	vcmConfigPrefix(configPrefix),
	vcmStatistics()
{
}

Mixin::VcmMixin::~VcmMixin(void) = default;

bool Mixin::VcmMixin::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "Vcm", vcmConfigPrefix))
	{
		return Settings::ParseEnum(value, vcmEnable, {
			{ "Enabled", VcmEnable::Enabled },
			{ "Disabled", VcmEnable::Disabled },
		});
	}
	if (Settings::IsOption(key, "VcmBaseDirectory", vcmConfigPrefix))
	{
		vcmBaseDirectory = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmImportFilename", vcmConfigPrefix))
	{
		vcmImportFilename = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmLibraryFilename", vcmConfigPrefix))
	{
		if (value == "")
			vcmLibraryFilenames.clear();
		else
			vcmLibraryFilenames.push_back(value);
		return true;
	}
	if (Settings::IsOption(key, "VcmExportPreprocessedFilename", vcmConfigPrefix))
	{
		vcmExportPreprocessedFilename = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmExportProcessedFilename", vcmConfigPrefix))
	{
		vcmExportProcessedFilename = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmTopLevelModuleName", vcmConfigPrefix))
	{
		vcmTopLevelModuleName = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmTag", vcmConfigPrefix))
	{
		if (value.empty())
		{
			vcmTags.clear();
		}
		else
		{
			vcmTags.emplace_back(value);
		}
		return true;
	}
	if (Settings::IsOption(key, "VcmParameter", vcmConfigPrefix))
	{
		if (value.empty())
		{
			vcmParameters.clear();
		}
		else
		{
			const auto index = value.find("=");
			if (index == std::string::npos)
			{
				return false;
			}

			const auto paramName { value.substr(0u, index) };
			const auto paramValue { value.substr(index + 1u) };
			auto paramVector { GetLogicValuesForString(paramValue) };

			// Convert to big endian for simplicity of access
			std::reverse(paramVector.begin(), paramVector.end());
			vcmParameters[paramName] = paramVector;
		}
		return true;
	}
	if (Settings::IsOption(key, "VcmConfiguration", vcmConfigPrefix))
	{
		vcmConfiguration = value;
		return true;
	}
	if (Settings::IsOption(key, "VcmConfigurationsFilename", vcmConfigPrefix))
	{
		FileHandle parametersFile { value, true };
		boost::property_tree::ptree root;

		try
		{
			boost::property_tree::read_jsonc(parametersFile.GetStream(), root);

			for (auto& [_, configuration] : root)
			{
				auto nameIt = configuration.find("name");
				auto parametersIt = configuration.find("parameters");
				auto tagsIt = configuration.find("tags");

				if (nameIt == configuration.not_found())
				{
					LOG(ERROR) << "Found entry without \"name\" in VCM parameter configuration file";
					return false;
				}
				if (parametersIt == configuration.not_found())
				{
					LOG(ERROR) << "Found entry without \"parameters\" in VCM parameter configuration file";
					return false;
				}
				if (tagsIt == configuration.not_found())
				{
					LOG(ERROR) << "Found entry without \"tags\" in VCM parameter configuration file";
					return false;
				}

				std::string name = nameIt->second.get_value<std::string>();
				std::map<std::string, Vcm::VcmParameter> parameters;
				std::vector<std::string> tags;

				for (auto& [_, parameter] : parametersIt->second)
				{
					const auto nameIt = parameter.find("name");
					const auto valueIt = parameter.find("value");
					const auto typeIt = parameter.find("type");

					if (nameIt == parameter.not_found())
					{
						LOG(ERROR) << "Found entry without \"name\" in VCM parameter configuration file.";
						return false;

					}
					if (valueIt == parameter.not_found())
					{
						LOG(ERROR) << "Found entry without \"value\" in VCM parameter configuration file.";
						return false;
					}
					if (typeIt == parameter.not_found())
					{
						LOG(ERROR) << "Found entry without \"type\" in VCM parameter configuration file.";
						return false;
					}

					const auto parameterType = typeIt->second.get_value<std::string>();
					const auto parameterName = nameIt->second.get_value<std::string>();
					if (parameterType == "binary")
					{
						auto values = GetLogicValuesForString(valueIt->second.get_value<std::string>());
						std::reverse(values.begin(), values.end());
						// Stored in big-endian order
						parameters[parameterName] = values;
					}
					else if (parameterType == "uint32")
					{
						const auto parameterValue = valueIt->second.get_value<std::string>();
						parameters[parameterName] = static_cast<uint32_t>(std::stoll(parameterValue, nullptr,
							StringHelper::StartsWith("0x", parameterValue) ? 16u : 10u));
					}
					else if (parameterType == "uint64")
					{
						const auto parameterValue = valueIt->second.get_value<std::string>();
						parameters[parameterName] = static_cast<uint64_t>(std::stoll(parameterValue, nullptr,
							StringHelper::StartsWith("0x", parameterValue) ? 16u : 10u));
					}
				}
				for (auto& [_, tag] : tagsIt->second)
				{
					tags.push_back(tag.get_value<std::string>());
				}

				vcmConfigurations.insert(std::make_pair(name, VcmConfiguration { name, parameters, tags }));
			}
		}
		catch (boost::property_tree::json_parser_error& exception)
		{
			LOG(ERROR) << "Could not parse json data in line " << exception.line() << ": " << exception.what();
			return false;
		}
		catch (boost::property_tree::ptree_bad_path& exception)
		{
			LOG(ERROR) << "A key does not exist in json file: " << exception.what();
			return false;
		}
		catch (boost::property_tree::ptree_bad_data& exception)
		{
			LOG(ERROR) << "Invalid data value: " << exception.what();
			return false;
		}

		return true;
	}

	return false;
}

void Mixin::VcmMixin::Init(void)
{
}

void Mixin::VcmMixin::Run(void)
{
	if (vcmEnable == VcmEnable::Enabled)
	{
		LoadVcmCircuit();
	}
}

Basic::ApplicationStatistics Mixin::VcmMixin::GetStatistics(void)
{
	return std::move(vcmStatistics);
}

void Mixin::VcmMixin::LoadVcmCircuit(void)
{
	LOG(INFO) << "Loading VCM module \"" << vcmImportFilename << "\"";

	// Copy settings and modify for VCM
	std::shared_ptr<Settings> vcmSettings = std::make_shared<Settings>(*Settings::GetInstance());
	vcmSettings->CircuitBaseDirectories = { ".", vcmBaseDirectory };
	vcmSettings->VerilogImportFilenames = { vcmImportFilename };
	vcmSettings->VerilogLibraryFilenames = vcmLibraryFilenames;
	vcmSettings->VerilogExportPreprocessedFilename = vcmExportPreprocessedFilename;
	vcmSettings->VerilogExportProcessedFilename = vcmExportProcessedFilename;
	vcmSettings->TopLevelModuleName = vcmTopLevelModuleName;

	Io::Verilog::VerilogConverter verilogConverter;
	vcmCircuit = verilogConverter.LoadCircuit(vcmSettings);
	if (!vcmCircuit)
	{
		LOG(FATAL) << "Failed to load VCM circuit " << vcmImportFilename;
		return;
	}

	vcmStatistics.Add("Vcm.Name", vcmCircuit->GetName(), "", "The name of the loaded circuit");
	vcmStatistics.Add("Vcm.Unmapped.Gates", vcmCircuit->GetUnmappedCircuit().GetNumberOfNodes(), "Gate(s)", "The number of gates in the unmapped circuit");
	vcmStatistics.Add("Vcm.Unmapped.PrimaryInputs", vcmCircuit->GetUnmappedCircuit().GetNumberOfPrimaryInputs(), "Input(s)", "The number of input wires in the unmapped circuit");
	vcmStatistics.Add("Vcm.Unmapped.PrimaryOutputs", vcmCircuit->GetUnmappedCircuit().GetNumberOfPrimaryOutputs(), "Output(s)", "The number of output wires in the unmapped circuit");
	vcmStatistics.Add("Vcm.Mapped.Gates", vcmCircuit->GetMappedCircuit().GetNumberOfNodes(), "Gate(s)", "The number of gates in the mapped circuit");
	vcmStatistics.Add("Vcm.Mapped.PrimaryInputs", vcmCircuit->GetMappedCircuit().GetNumberOfPrimaryInputs(), "Input(s)", "The number of input wires in the mapped circuit");
	vcmStatistics.Add("Vcm.Mapped.PrimaryOutputs", vcmCircuit->GetMappedCircuit().GetNumberOfPrimaryOutputs(), "Output(s)", "The number of output wires in the mapped circuit");
	vcmStatistics.Add("Vcm.Mapped.FlipFlops", vcmCircuit->GetMappedCircuit().GetNumberOfSecondaryInputs(), "Flip-Flop(s)", "The number of flip-flops in the mapped circuit");

	LOG(INFO) << "Loaded VCM module";

	const auto& mappedCircuit { vcmCircuit->GetMappedCircuit() };

	// Try to map the VCM inputs to the corresponding nodes of the circuit core
	vcmInputs.reserve(mappedCircuit.GetNumberOfPrimaryInputs());
	for (auto node : mappedCircuit.GetPrimaryInputs())
	{
		const auto vcmInput = Vcm::GetVcmInput(*vcmCircuit, node, *this->circuit);
		VLOG(3) << "Found VCM input " << vcmInput.vcmPortName << " of type " << to_string(vcmInput.vcmPortType);

		switch (vcmInput.vcmPortType)
		{
			case Vcm::VcmInputType::Good:
			case Vcm::VcmInputType::Bad:
			case Vcm::VcmInputType::Difference:
				ASSERT(vcmInput.targetNode != nullptr) << "Could not find node " << vcmInput.targetNodeName << " in target circuit.";
				VLOG(3) << "Target node is \"" << vcmInput.targetNodeName << "\"";
				break;
			case Vcm::VcmInputType::GoodValid:
			case Vcm::VcmInputType::BadValid:
			case Vcm::VcmInputType::Parameter:
			case Vcm::VcmInputType::ConstantZero:
			case Vcm::VcmInputType::ConstantOne:
			case Vcm::VcmInputType::ConstantDontCare:
			case Vcm::VcmInputType::ConstantUnknown:
			case Vcm::VcmInputType::FreeStatic:
			case Vcm::VcmInputType::FreeDynamic:
			case Vcm::VcmInputType::CounterInitial:
			case Vcm::VcmInputType::CounterBinary:
			case Vcm::VcmInputType::CounterUnary:
			case Vcm::VcmInputType::CounterTimeframe:
				break;
			default:
				LOG(FATAL) << "Invalid VCM input port type for port " << node->GetName();
				__builtin_unreachable();
		}

		vcmInputs.emplace_back(vcmInput);
	}

	// Detect the output port types of the VCM
	vcmOutputs.reserve(mappedCircuit.GetNumberOfPrimaryOutputs());
	for (auto [index, node] : mappedCircuit.EnumeratePrimaryOutputs())
	{
		const auto vcmOutput = Vcm::GetVcmOutput(*vcmCircuit, node);
		VLOG(3) << "Found VCM output " << vcmOutput.vcmPortName << " of type " << to_string(vcmOutput.vcmPortType);

		switch (vcmOutput.vcmPortType)
		{
		case Vcm::VcmOutputType::InitialConstraint:
		case Vcm::VcmOutputType::TransitionConstraint:
		case Vcm::VcmOutputType::TargetConstraint:
		case Vcm::VcmOutputType::Result:
			break;

		default:
			LOG(FATAL) << "Invalid VCM output port type for port " << node->GetName();
			__builtin_unreachable();
		}

		vcmOutputs.emplace_back(vcmOutput);
	}
}

Pattern::TestPattern Mixin::VcmMixin::GetVcmPatternForSimulationResult(const Vcm::VcmContext& context, const Simulation::SimulationResult& goodResult, const Simulation::SimulationResult& badResult) const
{
	const auto& mappedCircuit { vcmCircuit->GetMappedCircuit() };
	Pattern::TestPattern vcmPattern(goodResult.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfPrimaryInputs(),
		mappedCircuit.GetNumberOfSecondaryInputs(), Logic::LOGIC_DONT_CARE);

	ASSERT(context.GetVcmStartState().size() == mappedCircuit.GetNumberOfSecondaryInputs())
		<< "The VCM start state has invalid number of bits";
	vcmPattern.GetSecondaryInputs(0u) = context.GetVcmStartState();

	for (size_t timeframe = 0u; timeframe < vcmPattern.GetNumberOfTimeframes(); ++timeframe)
	{
		for (size_t index = 0u; index < mappedCircuit.GetNumberOfPrimaryInputs(); ++index)
		{
			const Logic inputValue = [&]() -> Logic {
				const auto& vcmInput = vcmInputs[index];
				const auto* circuitNode = vcmInput.targetNode;
				const auto get_difference_value = [](Logic value1, Logic value2) -> Logic {
					if ((value1 == Logic::LOGIC_ONE && value2 == Logic::LOGIC_ZERO)
						|| (value1 == Logic::LOGIC_ZERO && value2 == Logic::LOGIC_ONE))
					{
						return Logic::LOGIC_ONE;
					}

					return Logic::LOGIC_ZERO;
				};

				switch (vcmInput.vcmPortType)
				{
					case Vcm::VcmInputType::Good:
						return goodResult.GetOutputLogic(circuitNode, timeframe);
					case Vcm::VcmInputType::Bad:
						return badResult.GetOutputLogic(circuitNode, timeframe);
					case Vcm::VcmInputType::Difference:
						DVLOG(6) << "Output logic: " << goodResult.GetOutputLogic(circuitNode, timeframe)
							<< " (good) vs " << badResult.GetOutputLogic(circuitNode, timeframe) << " (bad)";
						return get_difference_value(
							goodResult.GetOutputLogic(circuitNode, timeframe),
							badResult.GetOutputLogic(circuitNode, timeframe)
						);
					case Vcm::VcmInputType::GoodValid:
						return IsValidLogic01(goodResult.GetOutputLogic(circuitNode, timeframe)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::BadValid:
						return IsValidLogic01(badResult.GetOutputLogic(circuitNode, timeframe)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::Parameter:
						return context.GetLogicValueForVcmParameter(vcmInput.parameterName, vcmInput.parameterIndex);
					case Vcm::VcmInputType::ConstantZero:
						return Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::ConstantOne:
						return Logic::LOGIC_ONE;
					case Vcm::VcmInputType::ConstantDontCare:
						return Logic::LOGIC_DONT_CARE;
					case Vcm::VcmInputType::ConstantUnknown:
						return Logic::LOGIC_UNKNOWN;
					// REMARK: The free literal-inputs are replaced with logic unknown as they are only
					// functional during the ATPG process and can not be recovered by this method.
					// So don't even pretend them to have any sensible values.
					case Vcm::VcmInputType::FreeStatic:
						return Logic::LOGIC_UNKNOWN;
					case Vcm::VcmInputType::FreeDynamic:
						return Logic::LOGIC_UNKNOWN;
					case Vcm::VcmInputType::CounterInitial:
						return (timeframe == 0u) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::CounterBinary:
						return (timeframe & (1u << vcmInput.vcmPortElementIndex)) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::CounterUnary:
						return (timeframe >= vcmInput.vcmPortElementIndex) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;
					case Vcm::VcmInputType::CounterTimeframe:
						return (timeframe == vcmInput.vcmPortElementIndex) ? Logic::LOGIC_ONE : Logic::LOGIC_ZERO;

					default:
						Logging::Panic("Unimplemented input port type");
				}
			}();

			DVLOG(6) << "VCM Primary Input " << index << " (" << vcmInputs[index].vcmPortName
				<< ", type=" << to_string(vcmInputs[index].vcmPortType) << ") to "
				<< to_string(inputValue) << " in timeframe " << std::to_string(timeframe);
			vcmPattern.SetPrimaryInput(timeframe, index, inputValue);
		}
	}

	return vcmPattern;
}

bool Mixin::VcmMixin::SimulateVcmPattern(const Vcm::VcmContext& context, Pattern::TestPattern& vcmPattern) const
{
	const auto& mappedCircuit { vcmCircuit->GetMappedCircuit() };

	Simulation::SimulationConfig simConfig { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
	Simulation::SimulationResult vcmResult(vcmPattern.GetNumberOfTimeframes(), mappedCircuit.GetNumberOfNodes());
	Simulation::SimulateTestPatternEventDriven<Fault::FaultFreeModel>(mappedCircuit, vcmPattern, {}, vcmResult, simConfig);

	for (size_t timeframe = 0u; timeframe < vcmResult.GetNumberOfTimeframes(); ++timeframe)
	{
		bool targetReached = true;
		bool constraintsFulfilled = true;

		for (auto& vcmInput : vcmInputs)
		{
			Logic value = vcmResult.GetOutputLogic(vcmInput.vcmPort, timeframe);
			switch (vcmInput.vcmPortType)
			{
			case Vcm::VcmInputType::Good:
			case Vcm::VcmInputType::Bad:
			case Vcm::VcmInputType::Difference:
			case Vcm::VcmInputType::GoodValid:
			case Vcm::VcmInputType::BadValid:
			case Vcm::VcmInputType::Parameter:
			case Vcm::VcmInputType::FreeStatic:
			case Vcm::VcmInputType::FreeDynamic:
			case Vcm::VcmInputType::CounterInitial:
			case Vcm::VcmInputType::CounterBinary:
			case Vcm::VcmInputType::CounterUnary:
			case Vcm::VcmInputType::CounterTimeframe:
				DVLOG(6) << "VCM: " << to_string(vcmInput.vcmPortType) << " input " << vcmInput.vcmPort->GetName()
					<< ((value == Logic::LOGIC_ONE) ? " is true" : " is false")
					<< " in timeframe " << timeframe;
				break;

			default:
				break;
			}
		}

		for (auto& vcmOutput : vcmOutputs)
		{
			auto value = vcmResult.GetOutputLogic(vcmOutput.vcmPort, timeframe);
			switch (vcmOutput.vcmPortType)
			{
			case Vcm::VcmOutputType::InitialConstraint:
				constraintsFulfilled &= (timeframe != 0u) | (value == Logic::LOGIC_ONE);
				DVLOG(6) << "VCM: Initial Constraint " << vcmOutput.vcmPort->GetName()
					<< (((timeframe != 0u) | (value == Logic::LOGIC_ONE)) ? " is satisfied" : " is unsatisfied")
					<< " in timeframe " << timeframe;
				break;

			case Vcm::VcmOutputType::TransitionConstraint:
				constraintsFulfilled &= (value == Logic::LOGIC_ONE);
				DVLOG(6) << "VCM: Constraint " << vcmOutput.vcmPort->GetName()
					<< ((value == Logic::LOGIC_ONE) ? " is satisfied" : " is unsatisfied")
					<< " in timeframe " << timeframe;
				break;

			case Vcm::VcmOutputType::TargetConstraint:
				targetReached &= (value == Logic::LOGIC_ONE);
				DVLOG(6) << "VCM: Target Constraint " << vcmOutput.vcmPort->GetName()
					<< ((value == Logic::LOGIC_ONE) ? " is satisfied" : " is unsatisfied")
					<< " in timeframe " << timeframe;
				break;

			case Vcm::VcmOutputType::Result:
				DVLOG(6) << "VCM: Result " << vcmOutput.vcmPort->GetName()
						<< ((value == Logic::LOGIC_ONE) ? " is one" : " is zero")
						<< " in timeframe " << timeframe;
				break;

			default:
				break;
			}
		}

		if (!constraintsFulfilled)
		{
			DVLOG(3) << "VCM: Not all constraints are satisfied in timeframe " << timeframe;
			return false;
		}

		if (targetReached)
		{
			DVLOG(3) << "VCM: All target constraints are satisfied in timeframe " << timeframe;
			return true;
		}
	}

	return false;
}

VcmConfiguration::VcmConfiguration(std::string name):
	name(name),
	parameters(),
	tags()
{
}

VcmConfiguration::VcmConfiguration(std::string name, std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter> parameters, std::vector<std::string> tags):
	name(name),
	parameters(parameters),
	tags(tags)
{
}

VcmConfiguration::~VcmConfiguration(void) = default;

const std::string& VcmConfiguration::GetName(void) const
{
	return name;
}

const std::map<std::string, FreiTest::Tpg::Vcm::VcmParameter>& VcmConfiguration::GetParameters(void) const
{
	return parameters;
}

const std::vector<std::string>& VcmConfiguration::GetTags(void) const
{
	return tags;
}

};
};
};
