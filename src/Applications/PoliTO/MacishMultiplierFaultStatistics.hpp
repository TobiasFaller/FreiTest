#pragma once

#include "Applications/BaseApplication.hpp"

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

class MacishMultiplierFaultStatistics: public BaseApplication
{
public:
	MacishMultiplierFaultStatistics();
	virtual ~MacishMultiplierFaultStatistics();

	void Run();
	void Init();
	bool SetSetting(std::string key, std::string value);

private:
	enum class Verbosity { ListAll, ListRelevant };

	Verbosity verbosity;

};

};
};
};
