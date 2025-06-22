#include "Tpg/LogicGenerator/Utility/ClearAssumptionsEncoder.hpp"


#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "Tpg/LogicGenerator/UnaryCounter.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"

using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
ClearAssumptionsEncoder<PinData>::ClearAssumptionsEncoder(void):
	ICircuitEncoder<PinData>()
{
}

template<typename PinData>
ClearAssumptionsEncoder<PinData>::~ClearAssumptionsEncoder(void) = default;

template<typename PinData>
std::string ClearAssumptionsEncoder<PinData>::GetName(void) const
{
	return "ClearAssumptionsEncoder";
}

template<typename PinData>
bool ClearAssumptionsEncoder<PinData>::PreEncodeCircuit(GeneratorContext<PinData>& context)
{
	auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(context.GetSolver());
	if (satSolver && context.IsIncremental())
	{
		// Remove assumptions for incremental mode as they are expected to be regenerated
		satSolver->ClearAssumptions();
	}

	return true;
}

#define FOR_PINDATA(PINDATA) template class ClearAssumptionsEncoder<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
