#pragma once

#include <map>
#include <string>
#include <vector>
#include <tuple>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/PinData.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/IConstraintEncoder.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData>
class VcmOutputLogicEncoder:
	public ICircuitEncoder<PinData>,
	public IConstraintEncoder<PinData>
{
	using LogicContainer = get_pin_data_container_t<PinData, GoodTag>;

public:
	VcmOutputLogicEncoder(std::vector<Vcm::VcmOutput> vcmOutputs);
	virtual ~VcmOutputLogicEncoder(void);

	std::string GetName(void) const override;
	void Reset(void) override;
	bool PostEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;
	bool ConstrainCircuit(GeneratorContext<PinData>& context) override;

	enum class OutputFilter { All, Constraint, Result };
	std::vector<std::tuple<Vcm::VcmOutput, std::vector<LogicContainer>>> GetEncodedOutputs(OutputFilter filterType) const;

private:
	std::vector<Vcm::VcmOutput> _vcmOutputs;
	std::vector<std::vector<LogicContainer>> _encodedOutputs;

};

};
};
