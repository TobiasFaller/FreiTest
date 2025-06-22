#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"

namespace FreiTest
{
namespace Tpg
{

enum class CellAwareSensitizationTimeframe
{
	Any,
	Last
};

template<typename PinData>
class CellAwareSensitizationConstraintEncoder:
	public IConstraintEncoder<PinData>
{
public:
	CellAwareSensitizationConstraintEncoder(std::shared_ptr<Fault::CellAwareFaultModel> faultModel, CellAwareSensitizationTimeframe targetTimeframe = CellAwareSensitizationTimeframe::Any);
	virtual ~CellAwareSensitizationConstraintEncoder(void);

	void SetAllowEmptySensitization(bool enable);
	bool IsAllowEmptySensitization(void) const;

	std::string GetName(void) const override;
	void Reset(void) override;
	bool ConstrainTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool PostConstrainCircuit(GeneratorContext<PinData>& context) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

	const std::vector<std::vector<LogicContainer01>>& GetStates() const;
	const std::vector<std::vector<LogicContainer01>>& GetNextStates() const;
	const std::vector<std::vector<std::vector<LogicContainer01>>>& GetTransitions() const;
	const std::vector<LogicContainer01>& GetSensitizations() const;

protected:
	std::shared_ptr<Fault::CellAwareFaultModel> _faultModel;
	CellAwareSensitizationTimeframe _targetTimeframe;
	bool _allowEmptySensitization;

	std::vector<std::vector<LogicContainer01>> _states;
	std::vector<std::vector<LogicContainer01>> _nextStates;
	std::vector<std::vector<std::vector<LogicContainer01>>> _transitions;
	std::vector<Tpg::LogicContainer01> _sensitizations;

};

};
};
