#include "Applications/Mixins/InitialStateMixin.hpp"
#include "Basic/Settings.hpp"
#include "Basic/Logging.hpp"
#include "Helper/FileHandle.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include <boost/algorithm/string.hpp>

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

InitialStateMixin::InitialStateMixin(std::string configPrefix):
	initStateEnable(ApplyInitialState::Disabled),
	initStateType(InitialStateType::DontCare),
	configPrefix(configPrefix),
	initialStateFilename("UNDEFINED_SOURCE"),
	initialState()
{
}

InitialStateMixin::~InitialStateMixin(void) = default;

bool InitialStateMixin::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "ApplyInitialState", configPrefix))
	{
		return Settings::ParseEnum(value, initStateEnable, {
			{"Enabled", ApplyInitialState::Enabled},
			{"Disabled", ApplyInitialState::Disabled}
		});
	}
	if (Settings::IsOption(key, "InitialStateType", configPrefix))
	{
		return Settings::ParseEnum(value, initStateType, {
			{"Zero", InitialStateType::Zero},
			{"One", InitialStateType::One},
			{"DontCare", InitialStateType::DontCare},
			{"File", InitialStateType::File}
		});
	}
	if (Settings::IsOption(key, "InitialStateFile", configPrefix))
	{
		initialStateFilename = value;
		return true;
	}
	return false;
}


void InitialStateMixin::Init(void)
{
}

void InitialStateMixin::Run(void)
{
	if (initStateEnable == ApplyInitialState::Enabled)
	{
		InitializeInitialState();
		LOG(INFO) << "Generated Initial State";
	}

}

void InitialStateMixin::InitializeInitialState(void)
{
	auto numberOfFlipFlops { this->circuit->GetMappedCircuit().GetNumberOfSecondaryInputs() };
	auto metaData = this->circuit->GetMetaData();
	switch (initStateType)
	{
		case InitialStateType::Zero:
			initialState = std::vector<Basic::Logic>(numberOfFlipFlops, Basic::Logic::LOGIC_ZERO);
			LOG(INFO) << "Loaded initial state as LOGIC_ZERO";
			break;
		case InitialStateType::One:
			initialState = std::vector<Basic::Logic>(numberOfFlipFlops, Basic::Logic::LOGIC_ONE);
			LOG(INFO) << "Loaded initial state as LOGIC_ONE";
			break;
		case InitialStateType::DontCare:
			initialState = std::vector<Basic::Logic>(numberOfFlipFlops, Basic::Logic::LOGIC_DONT_CARE);
			LOG(INFO) << "Loaded initial state as LOGIC_DONT_CARE";
			break;

		case InitialStateType::File:
		{

			ASSERT(!initialStateFilename.empty()) << "Please provide a file with an initial state";
			FileHandle initialStateFile { initialStateFilename, true };

			auto& initialStateStream = initialStateFile.GetStream();
			ASSERT(initialStateStream.good()) << "Could not initial state file";

			size_t secondaryInputCounter = 0u;
			while (!initialStateStream.eof() && initialStateStream.good())
			{
				std::string line;
				std::getline(initialStateStream, line);
				boost::trim(line);

				if (line.empty() || line[0u] == '#')
				{
					continue;
				}

				std::vector<std::string> tokenizedLine;
				boost::split(tokenizedLine, line, boost::is_any_of("="));

				ASSERT(tokenizedLine.size() == 2) << "Something is wrong in the format of the file. Should be \"name = logic\" ";

				auto parsedName = tokenizedLine[0];
				auto parsedValue = tokenizedLine[1];

				boost::trim(parsedName);
				boost::trim(parsedValue);
				boost::to_upper(parsedValue);

				ASSERT(parsedName.rfind("/") != std::string::npos) << "Secondary input is missing wire name";
				parsedName = parsedName.substr(0, parsedName.rfind("/"));

				ASSERT(secondaryInputCounter < numberOfFlipFlops) << "More bits are specified than exist in the circuit";
				auto input = this->circuit->GetMappedCircuit().GetSecondaryInput(secondaryInputCounter);
				auto circuitGivenName { metaData.GetFriendlyName({ input->GetSuccessor(0u), { Circuit::PortType::Output, 0u } }) };

				ASSERT(circuitGivenName.find("/") != std::string::npos) << "Secondary input is missing circuit name";
				circuitGivenName = circuitGivenName.substr(circuitGivenName.find("/") + 1u);

				ASSERT(circuitGivenName.rfind("/") != std::string::npos) << "Secondary input is missing wire name";
				circuitGivenName = circuitGivenName.substr(0, circuitGivenName.rfind("/"));

				if (parsedName != circuitGivenName)
					LOG(FATAL) << " Parsed secondary input (" << parsedName
						<< ") does not match the corresponding one from the mapped circuit (" << circuitGivenName << ")";

				initialState.push_back(Basic::GetLogicForString(parsedValue));
				secondaryInputCounter += 1;
			}

			ASSERT(initialState.size() == numberOfFlipFlops) << "Less bits have been specified than are required";
			LOG(INFO) << "Loaded initial state from file " << initialStateFilename;
			break;
		}

		default:
			Logging::Panic("Initial state source not implemented");
	}
}

std::vector<Basic::Logic> InitialStateMixin::GetInitialState(void) const
{
	ASSERT(initialState.size() == this->circuit->GetMappedCircuit().GetNumberOfSecondaryInputs())
		<< "The initial state size differs from the number of secondary inputs of the circuit";

	ASSERT(std::all_of(initialState.begin(), initialState.end(), [](Basic::Logic logicValue) {
		return logicValue != Basic::Logic::LOGIC_INVALID;})) << "Found INVALID LOGIC in the initial state: " << to_string(initialState);

	return initialState;
}

};
};
};
