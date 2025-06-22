#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Circuit/CellLibrary.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/ICircuitTagger.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/IContainerInterceptor.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename FaultModel>
class TransitionDelayFaultEncoder:
	public ICircuitEncoder<PinData>,
	public IContainerInterceptor<PinData>
{
public:
	TransitionDelayFaultEncoder(std::shared_ptr<FaultModel> faultModel);
	virtual ~TransitionDelayFaultEncoder(void);

	std::string GetName(void) const override;

	void Reset(void) override;
	bool PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframeId) override;
	const void* GetContainer(GeneratorContext<PinData>& context, NodeTag& tag, size_t timeframe, size_t nodeId, Circuit::PortType portType, size_t portId) override;

private:
	using BadContainer = get_pin_data_container_t<PinData, BadTag>;
	using DiffContainer = get_pin_data_container_t<PinData, DiffTag>;

	std::shared_ptr<FaultModel> _faultModel;
	std::vector<std::vector<BadContainer>> _badContainerForTimeframe;
	std::vector<std::vector<DiffContainer>> _diffContainerForTimeframe;
	std::vector<std::vector<Tpg::LogicContainer01>> _transitionInitialValidForTimeframe;
	std::vector<std::vector<Tpg::LogicContainer01>> _transitionInitialValueForTimeframe;

};

};
};
