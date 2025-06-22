#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Basic/Logic.hpp"
#include "Tpg/LogicGenerator/ICircuitEncoder.hpp"
#include "Tpg/LogicGenerator/LogicGenerator.hpp"
#include "Tpg/Vcm/VcmTypes.hpp"

namespace FreiTest
{
namespace Tpg
{

template<typename PinData, typename TargetPinData>
class VcmInputLogicEncoder: public ICircuitEncoder<PinData>
{
	using LogicContainer = get_pin_data_container_t<PinData, GoodTag>;

public:
	VcmInputLogicEncoder(std::shared_ptr<LogicGenerator<TargetPinData>> targetGenerator, std::vector<Vcm::VcmInput> vcmInputs);
	VcmInputLogicEncoder(std::shared_ptr<LogicGenerator<TargetPinData>> targetGenerator, std::vector<Vcm::VcmInput> vcmInputs, std::map<std::string, Vcm::VcmParameter> parameters);
	virtual ~VcmInputLogicEncoder(void);

	std::string GetName(void) const override;
	void Reset(void) override;
	bool PreEncodeTimeframe(GeneratorContext<PinData>& context, size_t timeframe) override;

	std::map<std::string, Vcm::VcmParameter>& GetParameters(void);
	std::map<std::string, Vcm::VcmParameter> const& GetParameters(void) const;
	template<typename T> void SetParameter(std::string name, T value);
	template<typename T> bool HasParameter(std::string name) const;
	template<typename T> std::optional<T> GetParameter(std::string name) const;

	enum class InputFilter { All, Miter, Parameter, Constant, Free, Counter };
	std::vector<std::tuple<Vcm::VcmInput, std::vector<LogicContainer>>> GetEncodedInputs(InputFilter type) const;

private:
	std::shared_ptr<LogicGenerator<TargetPinData>> _targetGenerator;
	std::vector<Vcm::VcmInput> _vcmInputs;
	std::map<std::string, Vcm::VcmParameter> _parameters;
	std::vector<std::vector<LogicContainer>> _encodedInputs;

};

template<typename PinData, typename TargetPinData>
template<typename T>
void VcmInputLogicEncoder<PinData, TargetPinData>::SetParameter(std::string name, T value)
{
	_parameters.emplace(name, value);
}

template<typename PinData, typename TargetPinData>
template<typename T>
bool VcmInputLogicEncoder<PinData, TargetPinData>::HasParameter(std::string name) const
{
	return GetParameter<T>(name).has_value();
}

template<typename PinData, typename TargetPinData>
template<typename T>
std::optional<T> VcmInputLogicEncoder<PinData, TargetPinData>::GetParameter(std::string name) const
{
	if (auto it = _parameters.find(name); it != _parameters.end())
	{
		try
		{
			return { std::get<T>(it->second) };
		}
		catch (const std::bad_variant_access& e)
		{
			throw std::runtime_error("The parameter could not be cast to the given type!");
		}
	}

	return std::nullopt;
}

};
};
