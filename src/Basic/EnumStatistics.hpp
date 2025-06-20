#pragma once

#include <map>

namespace FreiTest
{
namespace Basic
{

template<typename Enum>
class EnumStatistics
{
public:
	EnumStatistics(void) = default;
	virtual ~EnumStatistics(void) = default;

	void Reset(void) { counts.clear(); }
	void Add(Enum value, size_t count = 1u) { counts[value] += count; }
	void Remove(Enum value, size_t count = 1u) { counts[value] -= count; }
	void Update(Enum previous, Enum current, size_t count = 1u) { counts[previous] -= count; counts[current] += count; }

	size_t Get(Enum value) const
	{
		if (auto it = counts.find(value); it != counts.end())
		{
			return it->second;
		}

		return 0u;
	}

private:
	std::map<Enum, size_t> counts;

};

}
}
