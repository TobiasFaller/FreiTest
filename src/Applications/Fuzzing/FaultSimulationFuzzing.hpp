#pragma once

#include <atomic>

#include "Applications/Scale4Edge/TestPatternGeneration/Base/AtpgBase.hpp"

namespace FreiTest
{
namespace Application
{
namespace Fuzzing
{

template<typename FaultModel, typename FaultList>
class FaultSimulationFuzzing:
	public Scale4Edge::AtpgBase<FaultModel, FaultList>
{
public:
    FaultSimulationFuzzing(void);
    virtual ~FaultSimulationFuzzing(void);

    bool SetSetting(std::string key, std::string value) override;
    void Init(void) override;
    void Run(void) override;

private:
    void ReportIfInequal(const Simulation::SimulationResult& resultA, const Simulation::SimulationResult& resultB, const std::string& nameA, const std::string& nameB);

    size_t seed;
    size_t timeframes;
    size_t iterations;
    std::atomic<bool> stopped;

};

};
};
};
