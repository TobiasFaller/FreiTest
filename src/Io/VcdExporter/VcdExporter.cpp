#include "Io/VcdExporter/VcdExporter.hpp"

#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Io/VcdExporter/VcdModel.hpp"
#include "Helper/FileHandle.hpp"

#include "Helper/StringHelper.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include <time.h>
#include <chrono>
#include <ctime>

namespace FreiTest
{
namespace Io
{
namespace Vcd
{

std::string to_string(const VariableTypes& type);
std::string to_string(const ModuleTypes& type);

void WriteHeader(std::ostream& output, const VcdModel& model);
void WriteVariable(std::ostream& output, const VcdModel& model);
void WriteData(std::ostream& output, const VcdModel& model);
void WriteVar(std::ostream& output, const Variable& var);
void WriteScope(std::ostream& output, const std::shared_ptr<Scope>& scope);

bool ExportVcd(const VcdModel& model, const std::string& fileName)
{
	FileHandle handle(fileName , false);
	std::ofstream& output = handle.GetOutStream();

	if (!output.is_open())
	{
		return false;
	}

	WriteHeader(output, model);
	WriteVariable(output, model);
	WriteData(output, model);
	return true;
}

void WriteHeader(std::ostream& outputStream, const VcdModel& model)
{
	auto const& header = model.GetHeader();

	outputStream << "$version" << std::endl;
	outputStream << "\t" << header.GetVersion() << std::endl;
	outputStream << "$end" << std::endl;

	outputStream << "$date" << std::endl;
	outputStream << "\t" << ctime(&(header.GetTimestamp()));
	outputStream << "$end" << std::endl;

	outputStream << "$timescale "<< header.GetTimelapse() << "ns $end" << std::endl;
	outputStream << std::endl;
}

void WriteVariable(std::ostream& outputStream, const VcdModel& model)
{
	WriteScope(outputStream, model.GetVariable().GetScope());
	outputStream << "$enddefinitions $end" << std::endl;
}

void WriteData(std::ostream& outputStream, const VcdModel& model)
{
	const std::vector<Timeframe>& timeFrames = model.GetData().GetTimeFrames();

	const auto write_values = [&](auto& values, const std::string& symbol) {
		outputStream << std::endl;
		if (values.size() > 1) outputStream << "b";
		for (auto value : values) {
			if (value == Basic::Logic::LOGIC_INVALID) value = Basic::Logic::LOGIC_DONT_CARE;
			outputStream << to_string(value);
		}
		if (values.size() > 1) outputStream << " ";
		outputStream << symbol;
	};

	size_t timeIndex = 0;
	for (const Timeframe& timeframe : timeFrames) {
		outputStream << std::endl << "#" << timeIndex;
		timeIndex += 5u;

		for (auto [wireSymbol, wireValue] : timeframe.wireValues) {
			write_values(wireValue, wireSymbol);
		}
		for (auto [registerSymbol, registerValue] : timeframe.registerValues) {
			write_values(registerValue, registerSymbol);
		}
	}
	outputStream << std::endl << "#" << timeIndex << std::endl;
}

// -----------------------------------------------------------------------
// ------------------------- Helper functions ----------------------------
// -----------------------------------------------------------------------
void WriteVar(std::ostream& outputStream, const Variable& var)
{
	outputStream << "$var " << to_string(var.GetType()) << " ";
	outputStream << var.GetSize() << " " << var.GetReference() << " ";
	outputStream << var.GetName() << " $end" << std::endl;
}

void WriteScope(std::ostream& outputStream, const std::shared_ptr<Scope>& scope)
{
	outputStream << std::endl << "$scope " << to_string(scope->GetType()) << " ";
	outputStream << scope->GetName() << " $end" << std::endl;

	for (auto variable : scope->GetVariables())
	{
		WriteVar(outputStream, *variable);
	}

	for (auto childScope : scope->GetScopes())
	{
		WriteScope(outputStream, childScope);
	}

	outputStream << "$upscope $end" << std::endl;
	return;
}

std::string to_string(const VariableTypes& type)
{
	switch (type)
	{
		case VariableTypes::eventV: return "event";
		case VariableTypes::integerV: return "integer";
		case VariableTypes::parameterV: return "parameter";
		case VariableTypes::realV: return "real";
		case VariableTypes::wireV: return "wire";
		case VariableTypes::tri0V: return "tri0";
		case VariableTypes::tri1V: return "tri1";
		case VariableTypes::regV: return "reg";
		case VariableTypes::worV: return "wor";
		default: __builtin_unreachable();
	}
}

std::string to_string(const ModuleTypes& type)
{
	switch (type)
	{
		case ModuleTypes::beginM: return "begin";
		case ModuleTypes::forkM: return "fork";
		case ModuleTypes::functionM: return "function";
		case ModuleTypes::moduleM: return "module";
		case ModuleTypes::taskM: return "task";
		default: __builtin_unreachable();
	}
}

};
};
};


