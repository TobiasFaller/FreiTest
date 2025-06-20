#include "Basic/Statistic/Sampling.hpp"

namespace FreiTest
{
namespace Statistic
{

bool IsSampleUsed(size_t elementIndex, size_t listSize, size_t sampleCount)
{
	if (__builtin_expect((sampleCount == 0u) || (sampleCount >= listSize), false))
	{
		return true;
	}

	const float sampleDistance = static_cast<float>(listSize) / sampleCount;
	const size_t nearestSample = static_cast<size_t>(static_cast<size_t>((elementIndex + 1u) / sampleDistance) * sampleDistance);
	return __builtin_expect(elementIndex == nearestSample, false);
}

};
};
