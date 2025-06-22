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

enum class NetSensitizationTimeframe
{
	Any,
	Last
};

template<typename PinData>
class NetSensitizationConstraintEncoder: public IConstraintEncoder<PinData>
{
public:
	NetSensitizationConstraintEncoder(Circuit::MappedCircuit::NodeAndPort target, Basic::LogicConstraint constraint, NetSensitizationTimeframe targetTimeframe);
	virtual ~NetSensitizationConstraintEncoder(void);

	std::string GetName(void) const override;
	void Reset(void) override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;


protected:
	Circuit::MappedCircuit::NodeAndPort _target;
	Basic::LogicConstraint _logicConstraint;
	NetSensitizationTimeframe _targetTimeframe;
	std::vector<Tpg::LogicContainer01> _constraints;

};

};
};
