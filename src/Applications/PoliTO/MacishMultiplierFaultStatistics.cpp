#include "Applications/PoliTO/MacishMultiplierFaultStatistics.hpp"

#include <cmath>
#include <tuple>

#include "Basic/Settings.hpp"
#include "Basic/Fault/Models/FaultFreeModel.hpp"
#include "Basic/Fault/Models/SingleStuckAtFaultModel.hpp"
#include "Basic/Fault/Lists/SingleStuckAtFaultList.hpp"
#include "Basic/Parallel.hpp"
#include "Circuit/DriverFinder.hpp"
#include "Simulation/CircuitSimulator.hpp"

namespace FreiTest
{
namespace Application
{
namespace PoliTO
{

static const std::string PREFIX = "Polito/MacishMultiplierFaultStatistics";

MacishMultiplierFaultStatistics::MacishMultiplierFaultStatistics():
    verbosity(Verbosity::ListRelevant)
{
}

MacishMultiplierFaultStatistics::~MacishMultiplierFaultStatistics() = default;

bool MacishMultiplierFaultStatistics::SetSetting(std::string key, std::string value)
{
    if (Settings::IsOption(key, "Verbosity", PREFIX))
    {
        return Settings::ParseEnum(value, verbosity, {
            { "ListAll", Verbosity::ListAll },
            { "ListRelevant", Verbosity::ListRelevant },
        });
    }

    return false;
}

void MacishMultiplierFaultStatistics::Init()
{
}

void MacishMultiplierFaultStatistics::Run()
{
    auto const& mappedCircuit { this->circuit->GetMappedCircuit() };
    auto const to_inputs = [&](auto const& ports) {
        std::vector<size_t> indices;
        for (auto& port : ports) {
            indices.push_back(mappedCircuit.GetPrimaryInputNumber(port));
        }
        return indices;
    };

    auto faultList { Fault::GenerateStuckAtFaultList(*this->circuit) };
    auto portADrivers { Circuit::GetDrivers(this->circuit->GetName() + "/A [0:1]", *this->circuit) };
    auto portBDrivers { Circuit::GetDrivers(this->circuit->GetName() + "/B [0:1]", *this->circuit) };
    auto portODrivers { Circuit::GetDrivers(this->circuit->GetName() + "/O [0:3]", *this->circuit) };
    auto portAInputs { to_inputs(portADrivers) };
    auto portBInputs { to_inputs(portBDrivers) };

	Parallel::ExecuteParallel(0u, faultList.size() + 1u, Parallel::Arena::PatternGeneration, Parallel::Order::Sequential, [&](size_t index) {
        std::vector<std::tuple<size_t, size_t, size_t, size_t, size_t, size_t, size_t, Pattern::TestPattern, Simulation::SimulationResult>> resultData;
        const constexpr size_t indexDifference = 5; (void) indexDifference;
        const constexpr size_t indexPropagation = 6; (void) indexPropagation;

        Simulation::SimulationConfig configuration { Simulation::MakeSimulationConfig(Basic::MakeUnclockedSetResetFlipFlopModel()) };
        Simulation::SimulationResult simulation { 1u, mappedCircuit.GetNumberOfNodes() };
        Simulation::SimulationResult simulationFF { 1u, mappedCircuit.GetNumberOfNodes() };

        size_t maxDifference { 0u };
        for (size_t inputs { 0u }; inputs < 16; inputs++)
        {
            Pattern::TestPattern pattern(1u, mappedCircuit.GetNumberOfPrimaryInputs(), mappedCircuit.GetNumberOfSecondaryInputs());
            pattern.SetPrimaryInput(0u, portAInputs[0], ((inputs >> 0u) & 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO);
            pattern.SetPrimaryInput(0u, portAInputs[1], ((inputs >> 1u) & 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO);
            pattern.SetPrimaryInput(0u, portBInputs[0], ((inputs >> 2u) & 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO);
            pattern.SetPrimaryInput(0u, portBInputs[1], ((inputs >> 3u) & 1u) ? Basic::Logic::LOGIC_ONE : Basic::Logic::LOGIC_ZERO);

            Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, pattern, { }, simulationFF, configuration);
            if (index == 0)
            {
                Simulation::SimulateTestPatternNaive<Fault::FaultFreeModel>(mappedCircuit, pattern, { }, simulation, configuration);
            }
            else
            {
                auto fault = std::make_shared<Fault::SingleStuckAtFault>(faultList[index - 1u]);
                Simulation::SimulateTestPatternNaive<Fault::SingleStuckAtFaultModel>(mappedCircuit, pattern, { fault }, simulation, configuration);
            }

            auto valueA { (inputs >> 0u) & 3u };
            auto valueB { (inputs >> 2u) & 3u };
            auto exactO { valueA * valueB };
            auto valueO { (((simulation.GetOutputLogic(portODrivers[0], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 0u)
                | (((simulation.GetOutputLogic(portODrivers[1], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 1u)
                | (((simulation.GetOutputLogic(portODrivers[2], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 2u)
                | (((simulation.GetOutputLogic(portODrivers[3], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 3u) };
            auto valueFFO { (((simulationFF.GetOutputLogic(portODrivers[0], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 0u)
                | (((simulationFF.GetOutputLogic(portODrivers[1], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 1u)
                | (((simulationFF.GetOutputLogic(portODrivers[2], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 2u)
                | (((simulationFF.GetOutputLogic(portODrivers[3], 0u) == Basic::Logic::LOGIC_ONE) ? 1ul : 0ul) << 3u) };
            auto propO { valueO ^ valueFFO };
            auto differenceO { std::max(exactO, valueO) - std::min(exactO, valueO) };
            maxDifference = std::max(maxDifference, differenceO);

            resultData.emplace_back(valueA, valueB, valueO, valueFFO, exactO, differenceO, propO, pattern, simulation);
        }

        std::sort(resultData.begin(), resultData.end(), [](auto const& first, auto const& second) -> bool {
            return std::get<indexDifference>(first) > std::get<indexDifference>(second) ||
                (std::get<indexDifference>(first) == std::get<indexDifference>(second) && std::get<indexPropagation>(first) > std::get<indexPropagation>(second));
        });

        auto resultLinear { resultData };
        resultLinear.erase(std::unique(resultLinear.begin(), resultLinear.end(), [](auto const& first, auto const& second) -> bool {
            return std::get<indexDifference>(first) == std::get<indexDifference>(second);
        }), resultLinear.end());

        if (auto linear = std::is_sorted(resultLinear.begin(), resultLinear.end(), [](auto const& first, auto const& second) -> bool {
            return std::get<indexPropagation>(first) > std::get<indexPropagation>(second);
        }); linear) {
            return;
        }

        auto const get_propagation = [](size_t prop) -> std::string {
            return std::to_string((prop >> 3u) & 1u)
                + std::to_string((prop >> 2u) & 1u)
                + std::to_string((prop >> 1u) & 1u)
                + std::to_string((prop >> 0u) & 1u);
        };

        if (index == 0)
        {
            LOG(INFO) << "Fault-Free:";
        }
        else
        {
            LOG(INFO) << "Fault " << to_string(faultList[index - 1u]) << ":";
        }

        if (verbosity == Verbosity::ListAll)
        {
            for (auto& [valueA, valueB, valueO, valueFFO, exactO, differenceO, propO, pattern, simulation] : resultData)
            {
                LOG(INFO) << "    D " << differenceO << " P " << get_propagation(propO) << " " << valueA << " x " << valueB << " = " << valueO << " (" << valueFFO << ")";
            }
        }
        else
        {
            for (auto& [valueA, valueB, valueO, valueFFO, exactO, differenceO, propO, pattern, simulation] : resultLinear)
            {
                LOG(INFO) << "    D " << differenceO << " P " << get_propagation(propO) << " " << valueA << " x " << valueB << " = " << valueO << " (" << valueFFO << ")";
            }
        }
    });
}

}
}
}