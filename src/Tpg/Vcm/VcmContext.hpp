#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Basic/Logic.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{
namespace Vcm
{

class VcmContext
{
public:
    VcmContext(std::string target, std::string displayName);
    VcmContext(const VcmContext& defaults, std::string target, std::string displayName);
    virtual ~VcmContext(void);

    void SetTarget(std::string target);
	void SetTags(std::vector<std::string> tags);
	void AddTags(const std::vector<std::string>& tags);
    void ClearTags(void);
    void SetDisplayName(std::string target);

    const std::string& GetTarget(void) const;
	const std::vector<std::string>& GetTags(void) const;
    const std::string& GetDisplayName(void) const;

    void SetTargetStartState(std::vector<Basic::Logic> state);
    void SetVcmStartState(std::vector<Basic::Logic> state);
    const std::vector<Basic::Logic>& GetTargetStartState(void) const;
    const std::vector<Basic::Logic>& GetVcmStartState(void) const;

    void SetVcmParameter(std::string name, VcmParameter value);
    void SetVcmParameters(std::map<std::string, VcmParameter> parameters);
    void AddVcmParameters(const std::map<std::string, VcmParameter>& parameters);
    void ClearVcmParameters(void);
    const std::map<std::string, VcmParameter>& GetVcmParameters(void) const;
	std::optional<VcmParameter> GetVcmParameter(const std::string& name) const;
    Basic::Logic GetLogicValueForVcmParameter(const std::string& name, size_t index) const;

private:
    std::string target;
    std::string displayName;

	std::vector<std::string> tags;
    std::vector<Basic::Logic> targetStartState;
    std::vector<Basic::Logic> vcmStartState;
    std::map<std::string, VcmParameter> vcmParameters;

};

};
};
};
