#include "Tpg/LogicGenerator/GeneratorContext.hpp"

#include <vector>

#include "Basic/Logic.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Circuit/CircuitEnvironment.hpp"
#include "Circuit/UnmappedCircuit.hpp"
#include "SolverProxy/ISolverProxy.hpp"
#include "SolverProxy/Sat/SatSolverProxy.hpp"
#include "SolverProxy/Bmc/BmcSolverProxy.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"

using namespace std;
using namespace SolverProxy;
using namespace SolverProxy::Bmc;
using namespace SolverProxy::Sat;
using namespace FreiTest::Basic;
using namespace FreiTest::Circuit;

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
GeneratorContext<PinData>::GeneratorContext(std::shared_ptr<ISolverProxy> solverProxy, shared_ptr<CircuitEnvironment> circuit):
	_circuit(circuit),
	_solver(solverProxy),
	_interceptors(),
	_commonEncoder(solverProxy),
	_unaryCounter(),
	_binaryCounter(),
	_incremental(true),
	_timeframes(0u),
	_taggedTimeframes(),
	_encodedTimeframes(),
	_constrainedTimeframes(),
	_storedTimeframes()
{
}

template<typename PinData>
GeneratorContext<PinData>::~GeneratorContext(void)
{
}

template<typename PinData>
const shared_ptr<CircuitEnvironment> GeneratorContext<PinData>::GetCircuitEnvironment(void) const
{
	return _circuit;
}

template<typename PinData>
const MappedCircuit& GeneratorContext<PinData>::GetMappedCircuit(void) const
{
	return _circuit->GetMappedCircuit();
}

template<typename PinData>
const UnmappedCircuit& GeneratorContext<PinData>::GetUnmappedCircuit(void) const
{
	return _circuit->GetUnmappedCircuit();
}

template<typename PinData>
shared_ptr<ISolverProxy> GeneratorContext<PinData>::GetSolver(void)
{
	return _solver;
}

template<typename PinData>
const shared_ptr<ISolverProxy> GeneratorContext<PinData>::GetSolver(void) const
{
	return _solver;
}

template<typename PinData>
LogicEncoder& GeneratorContext<PinData>::GetEncoder(void)
{
	return _commonEncoder;
}

template<typename PinData>
const LogicEncoder& GeneratorContext<PinData>::GetEncoder(void) const
{
	return _commonEncoder;
}

template<typename PinData>
UnaryCounter& GeneratorContext<PinData>::GetUnaryCounter(void)
{
	return _unaryCounter;
}

template<typename PinData>
const UnaryCounter& GeneratorContext<PinData>::GetUnaryCounter(void) const
{
	return _unaryCounter;
}

template<typename PinData>
BinaryCounter& GeneratorContext<PinData>::GetBinaryCounter(void)
{
	return _binaryCounter;
}

template<typename PinData>
const BinaryCounter& GeneratorContext<PinData>::GetBinaryCounter(void) const
{
	return _binaryCounter;
}

template<typename PinData>
void GeneratorContext<PinData>::SetNumberOfTimeframes(size_t timeframes)
{
	ASSERT(timeframes > 0u) << "There is no such thing as no timeframes in a logic generator context.";

	// Limit the number of stored timeframes to 1 in case of a BMC solver
	const auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver);
	const auto newStoredTimeframes = (bmcSolver ? 1u : timeframes);
	const auto nodes = _circuit->GetMappedCircuit().GetNumberOfNodes();

	_taggedTimeframes.resize(newStoredTimeframes, false);
	_encodedTimeframes.resize(newStoredTimeframes, false);
	_constrainedTimeframes.resize(timeframes, false);

	_storedTimeframes.reserve(newStoredTimeframes);
	while (_storedTimeframes.size() > newStoredTimeframes)
	{
		_storedTimeframes.pop_back();
	}
	while (_storedTimeframes.size() < newStoredTimeframes)
	{
		_storedTimeframes.emplace_back(_storedTimeframes.size(), nodes);
	}

	_timeframes = timeframes;
}

template<typename PinData>
Timeframe<PinData>& GeneratorContext<PinData>::GetTimeframe(size_t timeframe)
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver);
	return _storedTimeframes[bmcSolver ? 0u : timeframe];
}

template<typename PinData>
const Timeframe<PinData>& GeneratorContext<PinData>::GetTimeframe(size_t timeframe) const
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver);
	return _storedTimeframes[bmcSolver ? 0u : timeframe];
}

template<typename PinData>
size_t GeneratorContext<PinData>::GetNumberOfTimeframesForTarget(GenerationTarget target) const
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver);

	switch (target)
	{
		case GenerationTarget::CircuitTagging:
		case GenerationTarget::CircuitEncoding:
			return bmcSolver ? 1u : _timeframes;
		case GenerationTarget::ConstraintEncoding:
			return _timeframes;
		case GenerationTarget::PatternExtraction:
			return bmcSolver ? bmcSolver->GetLastDepth() : _timeframes;
		default:
			Logging::Panic();
	}
}

template<typename PinData>
bool GeneratorContext<PinData>::IsTimeframeIncludedForTarget(size_t timeframe, GenerationTarget target) const
{
	auto bmcSolver = std::dynamic_pointer_cast<BmcSolverProxy>(_solver);

	switch (target)
	{
		case GenerationTarget::CircuitTagging:
		case GenerationTarget::CircuitEncoding:
			return bmcSolver ? (timeframe == 0u) : true;
		case GenerationTarget::ConstraintEncoding:
		case GenerationTarget::PatternExtraction:
			return true;
		default:
			Logging::Panic();
	}
}

template<typename PinData>
bool GeneratorContext<PinData>::IsTimeframeCompletedForTarget(size_t timeframe, GenerationTarget target) const
{
	switch (target)
	{
		case GenerationTarget::CircuitTagging:
			return _taggedTimeframes[timeframe];
		case GenerationTarget::CircuitEncoding:
			return _encodedTimeframes[timeframe];
		case GenerationTarget::ConstraintEncoding:
			return _constrainedTimeframes[timeframe];
		default:
			Logging::Panic();
	}
}

template<typename PinData>
void GeneratorContext<PinData>::SetTimeframeCompletedForTarget(size_t timeframe, GenerationTarget target, bool completed)
{
	switch (target)
	{
		case GenerationTarget::CircuitTagging:
			_taggedTimeframes[timeframe] = completed;
			break;
		case GenerationTarget::CircuitEncoding:
			_encodedTimeframes[timeframe] = completed;
			break;
		case GenerationTarget::ConstraintEncoding:
			_constrainedTimeframes[timeframe] = completed;
			break;
		default:
			Logging::Panic();
	}
}

template<typename PinData>
void GeneratorContext<PinData>::AddContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor)
{
	_interceptors.push_back(interceptor);
}

template<typename PinData>
void GeneratorContext<PinData>::RemoveContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor)
{
	if (auto it = std::find(_interceptors.begin(), _interceptors.end(), interceptor); it != _interceptors.end())
	{
		_interceptors.erase(it);
	}
}

template<typename PinData>
bool GeneratorContext<PinData>::HasContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor) const
{
	return (std::find(_interceptors.begin(), _interceptors.end(), interceptor) != _interceptors.end());
}

template<typename PinData>
std::vector<std::shared_ptr<IContainerInterceptor<PinData>>> const& GeneratorContext<PinData>::GetContainerInterceptors(void) const
{
	return _interceptors;
}

template<typename PinData>
std::vector<std::shared_ptr<IContainerInterceptor<PinData>>>& GeneratorContext<PinData>::GetContainerInterceptors(void)
{
	return _interceptors;
}

template<typename PinData>
void GeneratorContext<PinData>::SetIncremental(bool incremental)
{
	if (incremental)
	{
		auto satSolver = std::dynamic_pointer_cast<SatSolverProxy>(_solver);
		if (satSolver)
		{
			ASSERT(satSolver->IsIncrementalSupported()) << "The selected SAT-solver does not support incremental solving.";
		} else {
			Logging::Panic("Incremental mode is only supported when using a SAT-solver.");
		}
	}

	_incremental = incremental;
}

template<typename PinData>
bool GeneratorContext<PinData>::IsIncremental(void) const
{
	return _incremental;
}

#define FOR_PINDATA(PINDATA) template class GeneratorContext<PINDATA>;
INSTANTIATE_FOR_ALL_PINDATA

};
};
