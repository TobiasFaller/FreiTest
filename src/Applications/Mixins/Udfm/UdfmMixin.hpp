#pragma once

#include <memory>

#include "Applications/BaseApplication.hpp"
#include "Io/UserDefinedFaultModel/UdfmModel.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class UdfmMixin:
	public virtual BaseApplication
{
public:
	UdfmMixin(std::string configPrefix);
	virtual ~UdfmMixin(void);

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;
	void Run(void) override;

	void SetUdfm(std::shared_ptr<Io::Udfm::UdfmModel> udfm);
	std::shared_ptr<Io::Udfm::UdfmModel>& GetUdfm(void);
	std::shared_ptr<Io::Udfm::UdfmModel> const& GetUdfm(void) const;

private:
	std::string configPrefix;
	std::string udfmFileName;
	std::shared_ptr<Io::Udfm::UdfmModel> udfm;

};

};
};
};
