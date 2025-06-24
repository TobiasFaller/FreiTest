#pragma once

#include "Applications/BaseApplication.hpp"
#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Application
{
namespace Mixin
{

class InitialStateMixin:
	public virtual BaseApplication
{
public:
	enum class ApplyInitialState {Enabled, Disabled};
	enum class InitialStateType {Zero, One, DontCare, File};

	InitialStateMixin(std::string configPrefix);
	virtual ~InitialStateMixin(void);

	bool SetSetting(std::string key, std::string value) override;
	void Init(void) override;
	void Run(void) override;
	std::vector<Basic::Logic> GetInitialState(void) const;

	ApplyInitialState initStateEnable;
	InitialStateType initStateType;

private:
	std::string configPrefix;
	std::string initialStateFilename;
	std::vector<Basic::Logic> initialState;

	void InitializeInitialState(void);

};

};
};
};
