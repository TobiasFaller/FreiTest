#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include "Basic/Logic.hpp"

namespace FreiTest
{
namespace Helper
{

class StatePatcher
{
public:
	struct PatchValue
	{
		PatchValue(size_t flipFlop, Basic::Logic value);
		virtual ~PatchValue(void);

		size_t flipFlop;
		Basic::Logic value;
	};

	struct Patcher
	{
		Patcher(std::string name, std::vector<PatchValue> values);
		virtual ~Patcher(void);

		std::string name;
		std::vector<PatchValue> values;
	};

	StatePatcher(void);
	virtual ~StatePatcher(void);

	std::shared_ptr<Patcher> AddPatcher(std::string name, std::vector<PatchValue> patchValues);
	void ApplyPatches(std::vector<Basic::Logic>& state) const;

private:
	std::vector<std::shared_ptr<Patcher>> patchers = {};

};

};
};
