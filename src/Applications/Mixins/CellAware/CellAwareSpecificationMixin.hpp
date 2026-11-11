#pragma once

#include <memory>

#include "Applications/BaseApplication.hpp"
#include "Basic/Fault/Specification/CellAwareSpecification.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class CellAwareSpecificationMixin:
	public virtual BaseApplication
{
public:
	enum class CellAwareFaultModelSource {
		None,
		Custom,
		UserDefinedFaultModel,
		StateMachineFaultModel
	};

	CellAwareSpecificationMixin(std::string configPrefix);
	virtual ~CellAwareSpecificationMixin(void);

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;
	void Run(void) override;

	void SetCellAwareModelSpecification(std::shared_ptr<Fault::CellAwareModelSpecification> specification);
	const std::shared_ptr<Fault::CellAwareModelSpecification>& GetCellAwareModelSpecification(void) const;

private:
	std::string configPrefix;
	std::string userDefinedFaultModelPath;
	std::string stateMachineFaultModelPath;

protected:
	CellAwareFaultModelSource cellAwareFaultModelSource;
	std::shared_ptr<Fault::CellAwareModelSpecification> cellAwareSpecification;
	std::shared_ptr<Io::Udfm::UdfmModel> userDefinedFaultModelSource;
	std::shared_ptr<Fault::CellAwareModelSpecification> stateMachineFaultModelSource;

};

};
};
};
