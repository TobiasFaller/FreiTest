#pragma once

#include <memory>
#include <vector>

#include "Circuit/CircuitEnvironment.hpp"
#include "Tpg/LogicGenerator/GeneratorContext.hpp"
#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"
#include "Tpg/LogicGenerator/IContainerInterceptor.hpp"
#include "SolverProxy/ISolverProxy.hpp"

namespace FreiTest
{
namespace Tpg
{

/*!
 * \brief This logic generator combines multiple generator modules into one generator.
 *
 * The circuit generation and circuit tagging is delegated to the generator modules.
 * A new module can be added to this generator by calling the AddCircuitTagger,
 * AddCircuitEncoder or AddConstraintEncoder methods.
 */
template<typename PinData>
class LogicGenerator
{
public:
	LogicGenerator(std::shared_ptr<SolverProxy::ISolverProxy> solver, std::shared_ptr<Circuit::CircuitEnvironment> circuit);
	virtual ~LogicGenerator(void);

	void Reset(void);

	bool GenerateCircuitLogic(void);
	GeneratorContext<PinData>& GetContext(void);
	const GeneratorContext<PinData>& GetContext(void) const;

	void AddCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> circuitTagger);
	void RemoveCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> circuitTagger);
	bool HasCircuitTagger(std::shared_ptr<ICircuitTagger<PinData>> circuitTagger) const;
	std::vector<std::shared_ptr<ICircuitTagger<PinData>>> const& GetCircuitTaggers(void) const;
	std::vector<std::shared_ptr<ICircuitTagger<PinData>>>& GetCircuitTaggers(void);

	void AddCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder);
	void RemoveCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder);
	bool HasCircuitEncoder(std::shared_ptr<ICircuitEncoder<PinData>> circuitEncoder) const;
	std::vector<std::shared_ptr<ICircuitEncoder<PinData>>> const& GetCircuitEncoders(void) const;
	std::vector<std::shared_ptr<ICircuitEncoder<PinData>>>& GetCircuitEncoders(void);

	void AddConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder);
	void RemoveConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder);
	bool HasConstraintEncoder(std::shared_ptr<IConstraintEncoder<PinData>> constraintEncoder) const;
	std::vector<std::shared_ptr<IConstraintEncoder<PinData>>> const& GetContraintEncoder(void) const;
	std::vector<std::shared_ptr<IConstraintEncoder<PinData>>>& GetContraintEncoder(void);

	void AddContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor);
	void RemoveContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor);
	bool HasContainerInterceptor(std::shared_ptr<IContainerInterceptor<PinData>> interceptor) const;
	std::vector<std::shared_ptr<IContainerInterceptor<PinData>>> const& GetContainerInterceptors(void) const;
	std::vector<std::shared_ptr<IContainerInterceptor<PinData>>>& GetContainerInterceptors(void);

	template<typename FaultModel, typename... Arguments> std::shared_ptr<FaultModel> EmplaceModule(Arguments... arguments)
	{
		auto model = std::make_shared<FaultModel>(std::forward<Arguments>(arguments)...);
		if constexpr (is_circuit_tagger_v<FaultModel>)
		{
			AddCircuitTagger(model);
		}
		if constexpr (is_circuit_encoder_v<FaultModel>)
		{
			AddCircuitEncoder(model);
		}
		if constexpr (is_constraint_encoder_v<FaultModel>)
		{
			AddConstraintEncoder(model);
		}
		if constexpr (is_container_interceptor_v<FaultModel>)
		{
			AddContainerInterceptor(model);
		}
		return model;
	}

protected:

	bool TagCircuit(void);
	bool PreTagCircuit(void);
	bool PostTagCircuit(void);

	bool TagTimeframe(size_t timeframe, TaggingDirection direction);
	bool PreTagTimeframe(size_t timeframe, TaggingDirection direction);
	bool PostTagTimeframe(size_t timeframe, TaggingDirection direction);

	bool EncodeCircuit(void);
	bool PreEncodeCircuit(void);
	bool PostEncodeCircuit(void);

	bool EncodeTimeframe(size_t timeframe);
	bool PreEncodeTimeframe(size_t timeframe);
	bool PostEncodeTimeframe(size_t timeframe);

	bool EncodeNode(size_t timeframe, size_t node);
	bool PreEncodeNode(size_t timeframe, size_t node);
	bool PostEncodeNode(size_t timeframe, size_t node);

	bool ConstrainCircuit(void);
	bool PreConstrainCircuit(void);
	bool PostConstrainCircuit(void);

	bool ConstrainTimeframe(size_t timeframe);
	bool PreConstrainTimeframe(size_t timeframe);
	bool PostConstrainTimeframe(size_t timeframe);

	GeneratorContext<PinData> _context;
	std::vector<std::shared_ptr<ICircuitTagger<PinData>>> _circuitTaggers;
	std::vector<std::shared_ptr<ICircuitEncoder<PinData>>> _circuitEncoders;
	std::vector<std::shared_ptr<IConstraintEncoder<PinData>>> _constraintEncoders;

};

};
};
