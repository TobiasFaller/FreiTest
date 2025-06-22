#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

#include "Basic/Logging.hpp"
#include "Helper/StringHelper.hpp"

namespace FreiTest
{
namespace Io
{
namespace Udfm
{

UdfmAlternative::UdfmAlternative(std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects):
	testType(testType),
	testConditions(testConditions),
	testEffects(testEffects)
{
}
UdfmAlternative::~UdfmAlternative(void) = default;

const std::string& UdfmAlternative::GetTestType(void) const
{
	return testType;
}
const UdfmPortMap& UdfmAlternative::GetTestConditions(void) const
{
	return testConditions;
}
const UdfmPortMap& UdfmAlternative::GetTestEffects(void) const
{
	return testEffects;
}
const UdfmPortConstraint& UdfmAlternative::GetTestConditionConstraints(const std::string& portName) const
{
	ASSERT(testConditions.find(portName) != testConditions.end()) << "Pin name " << portName << " not found in conditions";
	return testConditions.at(portName);
}
const UdfmPortConstraint& UdfmAlternative::GetTestEffectConstraints(const std::string& portName) const
{
	ASSERT(testEffects.find(portName) != testEffects.end()) << "Pin name " << portName << " not found in effects";
	return testEffects.at(portName);
}

UdfmFault::UdfmFault(std::string cellName, std::string faultName, std::string faultCategory):
	cellName(cellName),
	faultName(faultName),
	faultCategory(faultCategory),
	alternatives()
{
}
UdfmFault::~UdfmFault(void) = default;

std::shared_ptr<UdfmAlternative> UdfmFault::AddAlternative(std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects)
{
	return alternatives.emplace_back(std::make_shared<UdfmAlternative>(testType, testConditions, testEffects));
}
const std::string& UdfmFault::GetCellName(void) const
{
	return cellName;
}
const std::string& UdfmFault::GetFaultName(void) const
{
	return faultName;
}
const std::string& UdfmFault::GetFaultCategory(void) const
{
	return faultCategory;
}
const UdfmAlternativeList& UdfmFault::GetAlternatives(void) const
{
	return alternatives;
}
bool UdfmFault::HasAlternative(size_t index) const
{
	return (index < alternatives.size());
}
std::shared_ptr<UdfmAlternative>& UdfmFault::GetAlternative(size_t index)
{
	return alternatives[index];
}
const std::shared_ptr<UdfmAlternative>& UdfmFault::GetAlternative(size_t index) const
{
	return alternatives[index];
}

const std::string& UdfmFault::GetAlternativeType(size_t index) const
{
	return alternatives[index]->GetTestType();
}
const UdfmPortMap& UdfmFault::GetAlternativeConditions(size_t index) const
{
	return alternatives[index]->GetTestConditions();
}
const UdfmPortMap& UdfmFault::GetAlternativeEffects(size_t index) const
{
	return alternatives[index]->GetTestEffects();
}

UdfmCell::UdfmCell(std::string cellName):
	cellName(cellName),
	faults()
{
}
UdfmCell::~UdfmCell(void) = default;

std::shared_ptr<UdfmFault> UdfmCell::AddFault(std::string faultName, std::string faultCategory)
{
	if (faults.find(faultName) == faults.end())
	{
		faults[faultName] = std::make_shared<UdfmFault>(cellName, faultName, faultCategory);
	}

	return faults[faultName];
}
std::shared_ptr<UdfmAlternative> UdfmCell::AddAlternative(std::string faultName, std::string faultCategory, std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects)
{
	auto fault = AddFault(faultName, faultCategory);
	return fault->AddAlternative(testType, testConditions, testEffects);
}

const std::string& UdfmCell::GetCellName() const
{
	return cellName;
}
const UdfmFaultMap& UdfmCell::GetFaults() const
{
	return faults;
}
const UdfmAlternativeList& UdfmCell::GetAlternatives(const std::string& faultName) const
{
	ASSERT(faults.find(faultName) != faults.end()) << "Fault " << faultName << " has not been found in UDFM";
	return faults.at(faultName)->GetAlternatives();
}
bool UdfmCell::HasFault(const std::string& faultName) const
{
	return faults.find(faultName) != faults.end();
}
bool UdfmCell::HasAlternative(const std::string& faultName, size_t index) const
{
	return HasFault(faultName) && faults.at(faultName)->HasAlternative(index);
}
std::shared_ptr<UdfmFault>& UdfmCell::GetFault(const std::string& faultName)
{
	ASSERT(faults.find(faultName) != faults.end()) << "Fault " << faultName << " has not been found in UDFM";
	return faults.at(faultName);
}
std::shared_ptr<UdfmAlternative>& UdfmCell::GetAlternative(const std::string& faultName, size_t index)
{
	ASSERT(faults.find(faultName) != faults.end()) << "Fault " << faultName << " has not been found in UDFM";
	ASSERT(index < faults.at(faultName)->GetAlternatives().size()) << "Alternative " << index << " exceeds alternatives for fault " << faultName << " has not been found in UDFM";
	return faults.at(faultName)->GetAlternative(index);
}
const std::shared_ptr<UdfmFault>& UdfmCell::GetFault(const std::string& faultName) const
{
	ASSERT(faults.find(faultName) != faults.end()) << "Fault " << faultName << " has not been found in UDFM";
	return faults.at(faultName);
}
const std::shared_ptr<UdfmAlternative>& UdfmCell::GetAlternative(const std::string& faultName, size_t index) const
{
	ASSERT(faults.find(faultName) != faults.end()) << "Fault " << faultName << " has not been found in UDFM";
	ASSERT(index < faults.at(faultName)->GetAlternatives().size()) << "Alternative " << index << " exceeds alternatives for fault " << faultName << " has not been found in UDFM";
	return faults.at(faultName)->GetAlternative(index);
}

UdfmModel::UdfmModel(void) = default;
UdfmModel::~UdfmModel(void) = default;

std::shared_ptr<UdfmCell> UdfmModel::AddCell(std::string cellName)
{
	DVLOG(4) << "Adding UDFM cell " << cellName;

	if (cells.find(cellName) == cells.end())
	{
		cells[cellName] = std::make_shared<UdfmCell>(cellName);
	}

	return cells[cellName];
}

std::shared_ptr<UdfmFault> UdfmModel::AddFault(std::string cellName, std::string faultName, std::string faultCategory)
{
	DVLOG(4) << "Adding UDFM fault: cell " << cellName << ", fault " << faultName;

	auto cell = AddCell(cellName);
	return cell->AddFault(faultName, faultCategory);
}

std::shared_ptr<UdfmAlternative> UdfmModel::AddAlternative(std::string cellName, std::string faultName, std::string faultCategory, std::string testType, UdfmPortMap testConditions, UdfmPortMap testEffects)
{
	DVLOG(4) << "Adding UDFM test alternative: cell " << cellName
	<< ", fault: " << faultName
	<< ", category: " << faultCategory
	<< ", type: " << testType
	<< ", conditions: " << to_string(testConditions)
	<< ", effects: " << to_string(testEffects);

	auto cell = AddCell(cellName);
	return cell->AddAlternative(faultName, faultCategory, testType, testConditions, testEffects);
}

const UdfmCellMap& UdfmModel::GetCells(void) const
{
	return cells;
}
const UdfmFaultMap& UdfmModel::GetFaults(const std::string& cellName) const
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetFaults();
}
const UdfmAlternativeList& UdfmModel::GetAlternatives(const std::string& cellName, const std::string& faultName) const
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetAlternatives(faultName);
}
bool UdfmModel::HasCell(const std::string& cellName) const
{
	return cells.find(cellName) != cells.end();
}
bool UdfmModel::HasFault(const std::string& cellName, const std::string& faultName) const
{
	return HasCell(cellName) && cells.at(cellName)->HasFault(faultName);
}
bool UdfmModel::HasAlternative(const std::string& cellName, const std::string& faultName, size_t index) const
{
	return HasCell(cellName) && cells.at(cellName)->HasAlternative(faultName, index);
}
std::shared_ptr<UdfmCell>& UdfmModel::GetCell(const std::string& cellName)
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName);
}
std::shared_ptr<UdfmFault>& UdfmModel::GetFault(const std::string& cellName, const std::string& faultName)
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetFault(faultName);
}
std::shared_ptr<UdfmAlternative>& UdfmModel::GetAlternative(const std::string& cellName, const std::string& faultName, size_t index)
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetAlternative(faultName, index);
}
const std::shared_ptr<UdfmCell>& UdfmModel::GetCell(const std::string& cellName) const
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName);
}
const std::shared_ptr<UdfmFault>& UdfmModel::GetFault(const std::string& cellName, const std::string& faultName) const
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetFault(faultName);
}
const std::shared_ptr<UdfmAlternative>& UdfmModel::GetAlternative(const std::string& cellName, const std::string& faultName, size_t index) const
{
	ASSERT(cells.find(cellName) != cells.end()) << "Cell " << cellName << " has not been found in UDFM";
	return cells.at(cellName)->GetAlternative(faultName, index);
}

std::string to_string(const UdfmAlternative& alternative)
{
	return "Alternative with Type: " + alternative.GetTestType()
		+ "; Effect(s): " + to_string(alternative.GetTestEffects())
		+ "; Condition(s): " + to_string(alternative.GetTestConditions());
}
std::string to_string(const UdfmFault& fault)
{
	std::string result = "Fault " + fault.GetFaultName() + " (" + fault.GetFaultCategory() + ")" + ":\n";
	size_t index = 0u;
	for (auto& alternative : fault.GetAlternatives())
	{
		if (index != 0) result += "\n";
		result += "    " + std::to_string(index) + ": " + to_string(*alternative);
		index++;
	}

	return result;
}
std::string to_string(const UdfmCell& cell)
{
	std::string result = "Cell " + cell.GetCellName() + ":\n";
	size_t index = 0;
	for (const auto& [faultName, fault] : cell.GetFaults())
	{
		if (index != 0) result += "\n";
		result += "  " + to_string(*fault) + "\n";
		index++;
	}
	return result;
}
std::string to_string(const UdfmModel& udfm)
{
	std::string result;
	for (auto& [cellName, cell] : udfm.GetCells())
	{
		result += to_string(*cell);
	}
	return result;
}
std::string to_string(const UdfmPortMap& portMap)
{
	std::string result;
	size_t index = 0;
	for (const auto& [portName, portValue] : portMap)
	{
		if (index != 0) result += ", ";
		result += portName + "=" + to_string(portValue);
		index++;
	}
	return result;
}

std::ostream& operator<<(std::ostream& stream, const UdfmModel& udfmModel)
{
	stream << to_string(udfmModel);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const UdfmCell& udfmCell)
{
	stream << to_string(udfmCell);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const UdfmFault& udfmFault)
{
	stream << to_string(udfmFault);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const UdfmAlternative& udfmAlternative)
{
	stream << to_string(udfmAlternative);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const UdfmPortMap& udfmPortMap)
{
	stream << to_string(udfmPortMap);
	return stream;
}

std::string to_string(const std::shared_ptr<UdfmModel>& udfmModel) { return udfmModel ? to_string(*udfmModel) : "None"; }
std::string to_string(const std::shared_ptr<UdfmCell>& udfmCell) { return udfmCell ? to_string(*udfmCell) : "None"; }
std::string to_string(const std::shared_ptr<UdfmFault>& udfmFault) { return udfmFault ? to_string(*udfmFault) : "None"; }
std::string to_string(const std::shared_ptr<UdfmAlternative>& udfmAlternative) { return udfmAlternative ? to_string(*udfmAlternative) : "None"; }
std::string to_string(const std::shared_ptr<UdfmPortMap>& udfmPortMap) { return udfmPortMap ? to_string(*udfmPortMap) : "None"; }

std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmModel>& udfmModel) { return stream << to_string(udfmModel); }
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmCell>& udfmCell) { return stream << to_string(udfmCell); }
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmFault>& udfmFault) { return stream << to_string(udfmFault); }
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmAlternative>& udfmAlternative) { return stream << to_string(udfmAlternative); }
std::ostream& operator<<(std::ostream& stream, const std::shared_ptr<UdfmPortMap>& udfmPortMap) { return stream << to_string(udfmPortMap); }

std::string to_string(const std::unique_ptr<UdfmModel>& udfmModel) { return udfmModel ? to_string(*udfmModel) : "None"; }
std::string to_string(const std::unique_ptr<UdfmCell>& udfmCell) { return udfmCell ? to_string(*udfmCell) : "None"; }
std::string to_string(const std::unique_ptr<UdfmFault>& udfmFault) { return udfmFault ? to_string(*udfmFault) : "None"; }
std::string to_string(const std::unique_ptr<UdfmAlternative>& udfmAlternative) { return udfmAlternative ? to_string(*udfmAlternative) : "None"; }
std::string to_string(const std::unique_ptr<UdfmPortMap>& udfmPortMap) { return udfmPortMap ? to_string(*udfmPortMap) : "None"; }

std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmModel>& udfmModel) { return stream << to_string(udfmModel); }
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmCell>& udfmCell) { return stream << to_string(udfmCell); }
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmFault>& udfmFault) { return stream << to_string(udfmFault); }
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmAlternative>& udfmAlternative) { return stream << to_string(udfmAlternative); }
std::ostream& operator<<(std::ostream& stream, const std::unique_ptr<UdfmPortMap>& udfmPortMap) { return stream << to_string(udfmPortMap); }

};
};
};
