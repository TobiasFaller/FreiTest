#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Basic/Fault/Models/CellAwareFaultModel.hpp"
#include "Circuit/CellLibrary.hpp"
#include "Circuit/MappedCircuit.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/Container/LogicContainer.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/IContainerInterceptor.hpp"

namespace FreiTest
{
namespace Tpg
{

// Determines the effect logic output if no alternative
// was found in the UDFM for the input of the cell.
enum class CellAwareNoAlternativeEffect
{
	FixedOne,      // The output is always LOGIC_ONE
	FixedZero,     // The output is always LOGIC_ZERO
	FixedDontCare, // The output is always LOGIC_DONT_CARE
	FixedUnknown,  // The output is always LOGIC_UNKNOWN
	FaultFree,     // The output is equal to the fault-free gate output
	Unconstrained  // The output is independent of the inputs
};

template<typename PinData>
class CellAwareFaultEncoder:
	public ICircuitEncoder<PinData>,
	public IContainerInterceptor<PinData>
{
public:
	CellAwareFaultEncoder(std::shared_ptr<Fault::CellAwareFaultModel> faultModel);
	virtual ~CellAwareFaultEncoder(void);

	void SetEffectWhenNoAlternativeFound(CellAwareNoAlternativeEffect effect);
	const CellAwareNoAlternativeEffect& GetEffectWhenNoAlternativeFound(void) const;

	std::string GetName(void) const override;

	void Reset(void) override;
	bool PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId) override;
	bool PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId) override;
	bool PostEncodeCircuit(GeneratorContext<PinData>& context) override;
	const void* GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframe, size_t nodeId, Circuit::PortType portType, size_t portId) override;

	const std::vector<std::vector<LogicContainer01>>& GetStates() const;
	const std::vector<std::vector<LogicContainer01>>& GetNextStates() const;
	const std::vector<std::vector<std::vector<LogicContainer01>>>& GetTransitions() const;
	const std::vector<LogicContainer01>& GetNoTransitions() const;

private:
	using BadContainer = get_pin_data_container_t<PinData, BadTag>;
	using DiffContainer = get_pin_data_container_t<PinData, DiffTag>;

	std::shared_ptr<Fault::CellAwareFaultModel> _faultModel;
	CellAwareNoAlternativeEffect _noAlternativeEffect;

	std::vector<std::map<Circuit::MappedCircuit::NodeAndPort, BadContainer>> _badContainerForTimeframe;
	std::vector<std::map<Circuit::MappedCircuit::NodeAndPort, DiffContainer>> _diffContainerForTimeframe;
	std::vector<std::vector<LogicContainer01>> _states;
	std::vector<std::vector<LogicContainer01>> _nextStates;
	std::vector<std::vector<std::vector<LogicContainer01>>> _transitions;
	std::vector<LogicContainer01> _noTransitions;

};

};
};
