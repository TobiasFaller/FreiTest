#pragma once

#include <memory>
#include <string>

namespace FreiTest
{
namespace Basic
{
class ApplicationStatistics;
};

namespace Circuit
{

class CircuitEnvironment;

};

namespace Application
{

class BaseApplication
{
public:
	BaseApplication(void);
	virtual ~BaseApplication(void);

	virtual void Init(void) = 0;
	virtual void PreInit(void);
	virtual void PostInit(void);

	virtual void Run(void) = 0;
	virtual void PreRun(void);
	virtual void PostRun(void);

	virtual bool SetSetting(std::string key, std::string value);
	virtual Basic::ApplicationStatistics GetStatistics(void);

	// Gets automatically set before calling application->Run();
	void SetCircuit(std::shared_ptr<Circuit::CircuitEnvironment> circuit) { this->circuit = circuit; };

	static std::unique_ptr<BaseApplication> Create(std::string application);

protected:
	std::shared_ptr<Circuit::CircuitEnvironment> circuit;

};

};
};
