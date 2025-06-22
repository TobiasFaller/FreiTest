#pragma once

#include <memory>
#include <string>
#include <vector>
#include <tuple>

#include "Circuit/MappedCircuit.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

class TransitionTarget
{
public:
	TransitionTarget(Circuit::MappedCircuit::NodeAndPort target, size_t toggleCount);
	virtual ~TransitionTarget(void);

	const Circuit::MappedCircuit::NodeAndPort& GetTarget(void) const;
	const size_t& GetTransitionCount() const;

private:
	Circuit::MappedCircuit::NodeAndPort target;
	size_t toggleCount;

};

template<typename PinData>
class MultiTransitionConstraintEncoder: public IConstraintEncoder<PinData>
{
public:

	MultiTransitionConstraintEncoder(Circuit::MappedCircuit::NodeAndPort target, size_t toggleCount = 2u);
	MultiTransitionConstraintEncoder(std::vector<TransitionTarget> targets);
	virtual ~MultiTransitionConstraintEncoder(void);

	std::string GetName(void) const override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

private:
	std::vector<TransitionTarget> targets;
	std::vector<std::vector<std::vector<Tpg::LogicContainer01>>> toggleCountLiterals;

};

};
};
