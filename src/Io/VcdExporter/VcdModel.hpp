#pragma once

#include <ctime>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Io
{
namespace Vcd
{

class VcdModelBuilder;

struct Timeframe
{
	std::map<std::string, std::vector<FreiTest::Basic::Logic>> wireValues;
	std::map<std::string, std::vector<FreiTest::Basic::Logic>> registerValues;
};

enum class ModuleTypes
{
	beginM, forkM, functionM, moduleM, taskM
};

enum class VariableTypes
{
	eventV, integerV, parameterV, realV, realtimeV, regV,
	supply0V, supply1V, timeV, triV, triandV, triorV, triregV,
	tri0V, tri1V, wandV, wireV, worV
};

class Variable
{
public:
	Variable(std::string name, std::string reference, size_t size, VariableTypes type):
		type(type),
		size(size),
		name(name),
		reference(reference)
	{}

	const VariableTypes& GetType() const { return type; }
	const size_t& GetSize() const { return size; }
	const std::string& GetName() const { return name; }
	const std::string& GetReference() const { return reference; }

private:
	friend VcdModelBuilder;

	VariableTypes type;
	size_t size;
	std::string name;
	std::string reference;
};

class Scope
{
public:
	Scope(std::string name, ModuleTypes type):
		name(name),
		type(type),
		scopes(),
		variables()
	{}

	const std::string& GetName() const { return name; }
	const ModuleTypes& GetType() const { return type; }
	const std::vector<std::shared_ptr<Scope>>& GetScopes() const { return scopes; }
	const std::vector<std::shared_ptr<Variable>>& GetVariables() const { return variables; }

	std::shared_ptr<Variable>& CreateVariable(std::string name, std::string ref, size_t size, VariableTypes type)
	{
		return variables.emplace_back(std::make_shared<Variable>(name, ref, size, type));
	}

private:
	friend VcdModelBuilder;

	std::string name;
	ModuleTypes type;
	std::vector<std::shared_ptr<Scope>> scopes;
	std::vector<std::shared_ptr<Variable>> variables;

};

class VcdHeader
{
public:
	VcdHeader() = default;
	VcdHeader(std::string version, int timelapse, time_t time):
		version(version),
		timelapse(timelapse),
		timestamp((time == 0) ? std::time(0) : time)
	{
	}

	const std::string& GetVersion() const { return version; }
	const int& GetTimelapse() const { return timelapse; }
	const time_t& GetTimestamp() const { return timestamp; }

private:
	friend VcdModelBuilder;

	std::string version;
	int timelapse;
	time_t timestamp;

};

class VcdData
{
public:
	VcdData() = default;

	const std::vector<Timeframe>& GetTimeFrames() const { return timeframes; }

	void AddTimeFrame(Timeframe timeframe){ timeframes.emplace_back(timeframe); }

private:
	friend VcdModelBuilder;

	std::vector<Timeframe> timeframes;
};

class VcdVariable
{
public:
	VcdVariable() = default;
	VcdVariable(std::shared_ptr<Scope> scope):
		scope(scope)
	{}

	const std::shared_ptr<Scope> GetScope() const { return scope; }

private:
	friend VcdModelBuilder;

	std::shared_ptr<Scope> scope;

};

class VcdModel
{
public:
	VcdModel() = default;
	VcdModel(VcdHeader header, VcdData data, VcdVariable variable):
		header(header),
		data(data),
		variable(variable)
	{}

	const VcdHeader& GetHeader() const { return header; }
	const VcdData& GetData() const { return data; }
	const VcdVariable& GetVariable() const { return variable; }

private:
	friend VcdModelBuilder;

	VcdHeader header;
	VcdData data;
	VcdVariable variable;

};

};
};
};
