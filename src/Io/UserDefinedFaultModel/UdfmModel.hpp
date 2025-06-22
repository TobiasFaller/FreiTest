#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Io
{
namespace Udfm
{

using UdfmPortConstraint = std::vector<Basic::LogicConstraint>;
using UdfmPortMap = std::map<std::string, UdfmPortConstraint>;

class UdfmAlternative
{
public:
	UdfmAlternative(std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects);
	virtual ~UdfmAlternative(void);

	const std::string& GetTestType(void) const;
	const UdfmPortMap& GetTestConditions(void) const;
	const UdfmPortMap& GetTestEffects(void) const;

	const UdfmPortConstraint& GetTestConditionConstraints(const std::string& portName) const;
	const UdfmPortConstraint& GetTestEffectConstraints(const std::string& portName) const;

private:
	std::string testType;
	UdfmPortMap testConditions;
	UdfmPortMap testEffects;
};

using UdfmAlternativeList = std::vector<std::shared_ptr<UdfmAlternative>>;

class UdfmFault
{
public:
	UdfmFault(std::string cellName, std::string faultName, std::string faultCategory);
	virtual ~UdfmFault(void);

	std::shared_ptr<UdfmAlternative> AddAlternative(std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects);

	const std::string& GetCellName(void) const;
	const std::string& GetFaultName(void) const;
	const std::string& GetFaultCategory(void) const;
	const UdfmAlternativeList& GetAlternatives(void) const;
	bool HasAlternative(size_t index) const;
	std::shared_ptr<UdfmAlternative>& GetAlternative(size_t index);
	const std::shared_ptr<UdfmAlternative>& GetAlternative(size_t index) const;

	const std::string& GetAlternativeType(size_t index) const;
	const UdfmPortMap& GetAlternativeConditions(size_t index) const;
	const UdfmPortMap& GetAlternativeEffects(size_t index) const;

private:
	std::string cellName;
	std::string faultName;
	std::string faultCategory;
	UdfmAlternativeList alternatives;
};

using UdfmFaultMap = std::map<std::string, std::shared_ptr<UdfmFault>>;

class UdfmCell
{
public:
	UdfmCell(std::string cellName);
	virtual ~UdfmCell(void);

	std::shared_ptr<UdfmFault> AddFault(std::string faultName, std::string faultCategory);
	std::shared_ptr<UdfmAlternative> AddAlternative(std::string faultName, std::string faultCategory, std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects);

	const std::string& GetCellName() const;
	const UdfmFaultMap& GetFaults() const;
	const UdfmAlternativeList& GetAlternatives(const std::string& faultName) const;

	bool HasFault(const std::string& faultName) const;
	bool HasAlternative(const std::string& faultName, size_t index) const;

	std::shared_ptr<UdfmFault>& GetFault(const std::string& faultName);
	std::shared_ptr<UdfmAlternative>& GetAlternative(const std::string& faultName, size_t index);

	const std::shared_ptr<UdfmFault>& GetFault(const std::string& faultName) const;
	const std::shared_ptr<UdfmAlternative>& GetAlternative(const std::string& faultName, size_t index) const;

private:
	std::string cellName;
	UdfmFaultMap faults;
};

using UdfmCellMap = std::map<std::string, std::shared_ptr<UdfmCell>>;

class UdfmModel
{
public:
	UdfmModel(void);
	virtual ~UdfmModel(void);

	std::shared_ptr<UdfmCell> AddCell(std::string cellName);
	std::shared_ptr<UdfmFault> AddFault(std::string cellName, std::string faultName, std::string faultCategory);
	std::shared_ptr<UdfmAlternative> AddAlternative(std::string cellName, std::string faultName, std::string faultCategory, std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects);

	const UdfmCellMap& GetCells(void) const;
	const UdfmFaultMap& GetFaults(const std::string& cellName) const;
	const UdfmAlternativeList& GetAlternatives(const std::string& cellName, const std::string& faultName) const;

	bool HasCell(const std::string& cellName) const;
	bool HasFault(const std::string& cellName, const std::string& faultName) const;
	bool HasAlternative(const std::string& cellName, const std::string& faultName, size_t index) const;

	std::shared_ptr<UdfmCell>& GetCell(const std::string& cellName);
	std::shared_ptr<UdfmFault>& GetFault(const std::string& cellName, const std::string& faultName);
	std::shared_ptr<UdfmAlternative>& GetAlternative(const std::string& cellName, const std::string& faultName, size_t index);

	const std::shared_ptr<UdfmCell>& GetCell(const std::string& cellName) const;
	const std::shared_ptr<UdfmFault>& GetFault(const std::string& cellName, const std::string& faultName) const;
	const std::shared_ptr<UdfmAlternative>& GetAlternative(const std::string& cellName, const std::string& faultName, size_t index) const;

private:
	UdfmCellMap cells;
};

std::string to_string(const UdfmModel& udfmModel);
std::string to_string(const UdfmCell& udfmCell);
std::string to_string(const UdfmFault& udfmFault);
std::string to_string(const UdfmAlternative& udfmAlternative);
std::string to_string(const UdfmPortMap& udfmPortMap);

std::ostream& operator<<(std::ostream& stream, const UdfmModel& udfmModel);
std::ostream& operator<<(std::ostream& stream, const UdfmCell& udfmCell);
std::ostream& operator<<(std::ostream& stream, const UdfmFault& udfmFault);
std::ostream& operator<<(std::ostream& stream, const UdfmAlternative& udfmAlternative);
std::ostream& operator<<(std::ostream& stream, const UdfmPortMap& udfmPortMap);

std::string to_string(const std::shared_ptr<UdfmModel>& udfmModel);
std::string to_string(const std::shared_ptr<UdfmCell>& udfmCell);
std::string to_string(const std::shared_ptr<UdfmFault>& udfmFault);
std::string to_string(const std::shared_ptr<UdfmAlternative>& udfmAlternative);
std::string to_string(const std::shared_ptr<UdfmPortMap>& udfmPortMap);

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmModel>& udfmModel);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmCell>& udfmCell);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmFault>& udfmFault);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmAlternative>& udfmAlternative);
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmPortMap>& udfmPortMap);

std::string to_string(const std::unique_ptr<UdfmModel>& udfmModel);
std::string to_string(const std::unique_ptr<UdfmCell>& udfmCell);
std::string to_string(const std::unique_ptr<UdfmFault>& udfmFault);
std::string to_string(const std::unique_ptr<UdfmAlternative>& udfmAlternative);
std::string to_string(const std::unique_ptr<UdfmPortMap>& udfmPortMap);

std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmModel>& udfmModel);
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmCell>& udfmCell);
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmFault>& udfmFault);
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmAlternative>& udfmAlternative);
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmPortMap>& udfmPortMap);

};
};
};
