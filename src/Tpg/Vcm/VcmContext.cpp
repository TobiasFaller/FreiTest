#include "Tpg/Vcm/VcmContext.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

VcmContext::VcmContext(std::string target, std::string displayName):
	target(target),
	displayName(displayName),
	tags(),
	targetStartState(),
	vcmStartState(),
	vcmParameters({
		{ "is_simulation", false }
	})
{
}

VcmContext::VcmContext(const VcmContext& defaults, std::string target, std::string displayName):
	target(target),
	displayName(displayName),
	tags(defaults.tags),
	targetStartState(defaults.targetStartState),
	vcmStartState(defaults.vcmStartState),
	vcmParameters(defaults.vcmParameters)
{
}

VcmContext::~VcmContext(void) = default;

void VcmContext::SetTarget(std::string target)
{
	this->target = target;
}

const std::string& VcmContext::GetTarget(void) const
{
	return target;
}

void VcmContext::SetTags(std::vector<std::string> tags)
{
	this->tags = tags;
}

void VcmContext::AddTags(const std::vector<std::string>& tags)
{
	for (auto const& tag : tags)
	{
		this->tags.push_back(tag);
	}
}

void VcmContext::ClearTags(void)
{
	tags.clear();
}

const std::vector<std::string>& VcmContext::GetTags(void) const
{
	return tags;
}

void VcmContext::SetDisplayName(std::string displayName)
{
	this->displayName = displayName;
}

const std::string& VcmContext::GetDisplayName(void) const
{
	return displayName;
}

void VcmContext::SetTargetStartState(std::vector<Basic::Logic> state)
{
	this->targetStartState = state;
}

void VcmContext::SetVcmStartState(std::vector<Basic::Logic> state)
{
	this->vcmStartState = state;
}

const std::vector<Basic::Logic>& VcmContext::GetTargetStartState(void) const
{
	return targetStartState;
}

const std::vector<Basic::Logic>& VcmContext::GetVcmStartState(void) const
{
	return vcmStartState;
}

void VcmContext::SetVcmParameter(std::string name, VcmParameter value)
{
	vcmParameters.insert_or_assign(name, value);
}

void VcmContext::SetVcmParameters(std::map<std::string, VcmParameter> parameters)
{
	this->vcmParameters = parameters;
}

void VcmContext::AddVcmParameters(const std::map<std::string, VcmParameter>& parameters)
{
	for (auto const& [name, value] : parameters)
	{
		this->vcmParameters[name] = value;
	}
}

void VcmContext::ClearVcmParameters(void)
{
	this->vcmParameters.clear();
}

const std::map<std::string, VcmParameter>& VcmContext::GetVcmParameters(void) const
{
	return vcmParameters;
}

std::optional<VcmParameter> VcmContext::GetVcmParameter(const std::string& name) const
{
	if (auto it = vcmParameters.find(name); it != vcmParameters.end())
	{
		return { it->second };
	}
	else
	{
		return std::nullopt;
	}
}

Basic::Logic VcmContext::GetLogicValueForVcmParameter(const std::string& name, size_t index) const
{
	if (auto it = vcmParameters.find(name); it != vcmParameters.end())
	{
		auto result = GetVcmParameterValue(it->second, index);
		if (!result.has_value())
		{
			LOG(FATAL) << "The vcm parameter " << name << " bit "
				<< index << " could not be encoded.";
			__builtin_unreachable();
		}

		return result.value();
	}
	else
	{
		LOG(FATAL) << "The parameter " << name << " has not been found inside the parameter list.";
		__builtin_unreachable();
	}
}

};
};
};
