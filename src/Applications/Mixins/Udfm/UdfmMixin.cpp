#include "Applications/Mixins/Udfm/UdfmMixin.hpp"

#include "Basic/Settings.hpp"
#include "Basic/Logging.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/UserDefinedFaultModel/UdfmParser.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

UdfmMixin::UdfmMixin(std::string configPrefix):
	configPrefix(configPrefix),
	udfmFileName("UNDEFINED_SOURCE"),
    udfm()
{
}

UdfmMixin::~UdfmMixin(void) = default;

bool UdfmMixin::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "UdfmImportPath", configPrefix))
	{
		udfmFileName = value;
		return true;
	}
	return false;
}

void UdfmMixin::Init(void)
{
}

void UdfmMixin::Run(void)
{
    ASSERT(!this->udfmFileName.empty()) << "No UDFM file has been specified";
    FileHandle fileHandle(this->udfmFileName, true);
    this->udfm = Io::Udfm::ParseUdfm(fileHandle.GetStream());
    ASSERT(this->udfm) << "Could not load UDFM file \"" << this->udfmFileName
        << "\"! Make sure that the file exists and the name has been written correctly.";
}

void UdfmMixin::SetUdfm(std::shared_ptr<Io::Udfm::UdfmModel> udfm)
{
	this->udfm = udfm;
}

std::shared_ptr<Io::Udfm::UdfmModel>& UdfmMixin::GetUdfm(void)
{
	return this->udfm;
}

std::shared_ptr<Io::Udfm::UdfmModel> const& UdfmMixin::GetUdfm(void) const
{
	return this->udfm;
}

};
};
};
