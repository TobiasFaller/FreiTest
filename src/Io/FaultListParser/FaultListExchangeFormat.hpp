#pragma once

#include <vector>

#include "Circuit/CircuitEnvironment.hpp"

namespace FreiTest
{
namespace Io
{

template<typename FaultList>
class FaultListExchangeFormat
{
public:
	inline FaultListExchangeFormat(const Circuit::CircuitEnvironment& circuit, const FaultList& faults):
		_circuit(circuit), _faults(faults)
	{
	}
	inline FaultListExchangeFormat(const FaultListExchangeFormat& other) = default;
	inline FaultListExchangeFormat(FaultListExchangeFormat&& other) = default;
	inline virtual ~FaultListExchangeFormat(void) = default;

	inline FaultListExchangeFormat& operator=(const FaultListExchangeFormat& other) = default;
	inline FaultListExchangeFormat& operator=(FaultListExchangeFormat&& other) = default;

	inline const Circuit::CircuitEnvironment& GetCircuit(void) const { return _circuit; };
	inline const FaultList& GetFaults(void) const { return _faults; };

	inline size_t GetNumberOfFaults(void) const { return _faults.size(); };
	inline const std::shared_ptr<typename FaultList::fault_type> GetFault(size_t index) const
	{
		assert(index < _faults.size());
		return std::get<0>(_faults[index]);
	}
	inline const std::shared_ptr<typename FaultList::metadata_type> GetMetadata(size_t index) const
	{
		assert(index < _faults.size());
		return std::get<1>(_faults[index]);
	}

	inline std::shared_ptr<typename FaultList::fault_type> GetFault(size_t index)
	{
		assert(index < _faults.size());
		return std::get<0>(_faults[index]);
	}
	inline std::shared_ptr<typename FaultList::metadata_type> GetMetadata(size_t index)
	{
		assert(index < _faults.size());
		return std::get<1>(_faults[index]);
	}

private:
	const Circuit::CircuitEnvironment& _circuit;
	FaultList _faults;

};

};
};
