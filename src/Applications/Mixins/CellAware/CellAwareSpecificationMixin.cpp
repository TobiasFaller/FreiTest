#include "Applications/Mixins/CellAware/CellAwareSpecificationMixin.hpp"

#include "Basic/Settings.hpp"
#include "Basic/Logging.hpp"
#include "Helper/FileHandle.hpp"
#include "Io/UserDefinedFaultModel/UdfmParser.hpp"
#include "Io/UserDefinedFaultModel/UdfmToSpecification.hpp"
#include "Io/StateMachineFaultModel/StateMachineFaultModelParser.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

CellAwareSpecificationMixin::CellAwareSpecificationMixin(std::string configPrefix):
	configPrefix(configPrefix),
	userDefinedFaultModelPath(),
	stateMachineFaultModelPath(),
	cellAwareFaultModelSource(CellAwareFaultModelSource::None),
    cellAwareSpecification(),
	userDefinedFaultModelSource(),
	stateMachineFaultModelSource()
{
}

CellAwareSpecificationMixin::~CellAwareSpecificationMixin(void) = default;

bool CellAwareSpecificationMixin::SetSetting(std::string key, std::string value)
{
	if (Settings::IsOption(key, "CellAwareFaultModelSource", configPrefix))
	{
		return Settings::ParseEnum(value, cellAwareFaultModelSource, {
			{ "UserDefinedFaultModel", CellAwareFaultModelSource::UserDefinedFaultModel },
			{ "StateMachineFaultModel", CellAwareFaultModelSource::StateMachineFaultModel },
		});
	}
	if (Settings::IsOption(key, "UserDefinedFaultModelPath", configPrefix))
	{
		userDefinedFaultModelPath = value;
		return true;
	}
	if (Settings::IsOption(key, "StateMachineFaultModelPath", configPrefix))
	{
		stateMachineFaultModelPath = value;
		return true;
	}
	return false;
}

void CellAwareSpecificationMixin::Init(void)
{
}

void CellAwareSpecificationMixin::Run(void)
{
	switch (cellAwareFaultModelSource)
	{
		case CellAwareFaultModelSource::None:
			LOG(FATAL) << "No cell aware fault specification has been specified (User Defined Fault Model / State Machine Fault Model)";
			break;

		case CellAwareFaultModelSource::Custom:
			break;

		case CellAwareFaultModelSource::UserDefinedFaultModel:
		{
			FileHandle fileHandle(this->userDefinedFaultModelPath, true);
			this->userDefinedFaultModelSource = Io::Udfm::Parse(fileHandle.GetStream());
			ASSERT(this->userDefinedFaultModelSource)
				<< "Could not load user defined fault model from \"" << this->userDefinedFaultModelPath << "\"!"
				<< " Make sure that the file exists and the name has been written correctly.";
			this->cellAwareSpecification = std::make_shared<Fault::CellAwareModelSpecification>(
				Io::Udfm::ConvertToSpecification(*this->userDefinedFaultModelSource));
			break;
		}

		case CellAwareFaultModelSource::StateMachineFaultModel:
		{
			FileHandle fileHandle(this->stateMachineFaultModelPath, true);
			this->stateMachineFaultModelSource = Io::StateMachineFaultModel::Parse(fileHandle.GetStream());
			ASSERT(this->stateMachineFaultModelSource)
				<< "Could not load state machine fault model from \"" << this->stateMachineFaultModelPath << "\"!"
				<< " Make sure that the file exists and the name has been written correctly.";
			this->cellAwareSpecification = this->stateMachineFaultModelSource;
			break;
		}
	}
}

void CellAwareSpecificationMixin::SetCellAwareModelSpecification(std::shared_ptr<Fault::CellAwareModelSpecification> specification)
{
	this->cellAwareSpecification = specification;
}

const std::shared_ptr<Fault::CellAwareModelSpecification>& CellAwareSpecificationMixin::GetCellAwareModelSpecification(void) const
{
	return this->cellAwareSpecification;
}

};
};
};
