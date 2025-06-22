#pragma once

#include <string>
#include <ostream>

#include "Applications/BaseApplication.hpp"
#include "Applications/Mixins/Vcm/VcmMixin.hpp"
#include "Circuit/CircuitMetaData.hpp"
#include "Circuit/MappedNode.hpp"

namespace FreiTest
{
namespace Application
{
namespace ExportCircuit
{

enum class LogicalDependency
{
	StructuralIndependent            = 0,
	StructuralDependent              = 1,

	ConstantPositve                  = 2,
	ConstantNegative                 = 4,

	FunctionalIndependent            = 0,
	FunctionalDependent              = 8,

	FunctionalPositive               = 16,
	FunctionalNegative               = 32,
	FunctionalBoth                   = 48,

	FunctionalPositiveForcesPositive = 64,
	FunctionalPositiveForcesNegative = 128,
	FunctionalNegativeForcesPositive = 256,
	FunctionalNegativeForcesNegative = 512,
	FunctionalInverted               = 384,
	FunctionalEquivalent             = 576
};

enum class LogicalConstant
{
	NotConstant                      = 0,
	ConstantPositive                 = 1,
	ConstantNegative                 = 2
};

class ExportCircuitLogicalDependence:
	public virtual BaseApplication,
	public virtual Mixin::VcmMixin
{
public:
	ExportCircuitLogicalDependence(void);
	virtual ~ExportCircuitLogicalDependence(void);

	void Init(void) override;
	void Run(void) override;
	bool SetSetting(std::string key, std::string value) override;

private:
	enum class Analysis { Enabled, Disabled };

	void ComputeStructuralDependence();
	void ComputeFunctionalDependence();
	void ExportData() const;
	std::string GetName(const Circuit::MappedNode* node) const;
	bool IsExcluded(const Circuit::MappedNode* node) const;

	std::string portInclude;
	std::string portExclude;
	std::vector<bool> inputsIncluded;
	std::vector<bool> outputsIncluded;
	std::vector<LogicalConstant> inputsConstant;
	std::vector<LogicalConstant> outputsConstant;

	Analysis enableInputsToInputs;
	Analysis enableInputsToOutputs;
	Analysis enableOutputsToInputs;
	Analysis enableOutputsToOutputs;
	std::vector<std::vector<LogicalDependency>> inputsToInputs;
	std::vector<std::vector<LogicalDependency>> inputsToOutputs;
	std::vector<std::vector<LogicalDependency>> outputsToInputs;
	std::vector<std::vector<LogicalDependency>> outputsToOutputs;

};

};
};
};

