#pragma once

#include <vector>

#include "Tpg/Encoder/LogicEncoder.hpp"
#include "Tpg/LogicGenerator/Timeframe.hpp"
#include "Tpg/LogicGenerator/UnaryCounter.hpp"
#include "Tpg/LogicGenerator/BinaryCounter.hpp"
#include "SolverProxy/ISolverProxy.hpp"

namespace FreiTest
{

namespace Circuit
{

class CircuitEnvironment;
class MappedCircuit;
class UnmappedCircuit;

};

namespace Tpg
{

template<typename PinData>
class IContainerInterceptor;

enum GenerationTarget
{
	CircuitTagging,
	CircuitEncoding,
	ConstraintEncoding,
	PatternExtraction
};

template<typename PinData>
class GeneratorContext
{

public:
	GeneratorContext(std::shared_ptr<SolverProxy::ISolverProxy> solverProxy, std::shared_ptr<Circuit::CircuitEnvironment> circuit);
	virtual ~GeneratorContext(void);

	const std::shared_ptr<Circuit::CircuitEnvironment> GetCircuitEnvironment(void) const;
	const Circuit::MappedCircuit& GetMappedCircuit(void) const;
	const Circuit::UnmappedCircuit& GetUnmappedCircuit(void) const;

	std::shared_ptr<SolverProxy::ISolverProxy> GetSolver(void);
	const std::shared_ptr<SolverProxy::ISolverProxy> GetSolver(void) const;

	LogicEncoder& GetEncoder(void);
	const LogicEncoder& GetEncoder(void) const;

	void SetNumberOfTimeframes(size_t timeframes);
	Timeframe<PinData>& GetTimeframe(size_t timeframe);
	const Timeframe<PinData>& GetTimeframe(size_t timeframe) const;

	size_t GetNumberOfTimeframesForTarget(GenerationTarget target) const;
	bool IsTimeframeIncludedForTarget(size_t timeframe, GenerationTarget target) const;
	void SetTimeframeCompletedForTarget(size_t timeframe, GenerationTarget target, bool completed);
	bool IsTimeframeCompletedForTarget(size_t timeframe, GenerationTarget target) const;

	void AddContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor);
	void RemoveContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor);
	bool HasContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor) const;
	std::vector<std::shared_ptr<IContainerInterceptor<PinData>>> const& GetContainerInterceptors(void) const;
	std::vector<std::shared_ptr<IContainerInterceptor<PinData>>>& GetContainerInterceptors(void);

	void SetIncremental(bool incremental);
	bool IsIncremental(void) const;

	UnaryCounter& GetUnaryCounter(void);
	const UnaryCounter& GetUnaryCounter(void) const;

	BinaryCounter& GetBinaryCounter(void);
	const BinaryCounter& GetBinaryCounter(void) const;

protected:
	std::shared_ptr<Circuit::CircuitEnvironment> _circuit;
	std::shared_ptr<SolverProxy::ISolverProxy> _solver;
	std::vector<std::shared_ptr<IContainerInterceptor<PinData>>> _interceptors;
	LogicEncoder _commonEncoder;
	UnaryCounter _unaryCounter;
	BinaryCounter _binaryCounter;

	bool _incremental;
	size_t _timeframes;
	std::vector<bool> _taggedTimeframes;
	std::vector<bool> _encodedTimeframes;
	std::vector<bool> _constrainedTimeframes;
	std::vector<Timeframe<PinData>> _storedTimeframes;

};

};
};
