#pragma once

#include "Applications/BaseApplication.hpp"

#include <vector>

#include "Basic/Logic.hpp"
#include "Basic/Pattern/TestPattern.hpp"
#include "Circuit/MappedNode.hpp"

namespace FreiTest
{
namespace Application
{
namespace Fuzzing
{

class StimSatFuzzing : public BaseApplication
{
public:
    StimSatFuzzing(void);
    virtual ~StimSatFuzzing(void);

    bool SetSetting(std::string key, std::string value) override;
    void Init(void) override;
    void Run(void) override;

private:
    std::string stimuliPath;
    Pattern::TestPattern stimuli;

	std::vector<const Circuit::MappedNode*> clockInputs;
    std::vector<const Circuit::MappedNode*> resetInputs;
    std::vector<const Circuit::MappedNode*> primaryInputs;
    std::vector<const Circuit::MappedNode*> primaryOutputs;
    std::vector<const Circuit::MappedNode*> secondaryInputs;
    std::vector<const Circuit::MappedNode*> secondaryOutputs;

};

};
};
};
