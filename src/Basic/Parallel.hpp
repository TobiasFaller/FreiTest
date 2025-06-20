#pragma once

#include <cstdint>
#include <functional>

namespace FreiTest
{
namespace Parallel
{

enum class Order
{
	Sequential,
	Parallel
};

enum class Arena {
	General,
	PatternGeneration,
	FaultSimulation,

	NUM_ITEMS
};

void SetThreads(Arena arena, size_t threads);
size_t GetThreads(Arena arena);

void ExecuteParallelImpl(Arena arena, size_t begin, size_t end, Order order, std::function<void(size_t)> function);
void ExecuteParallelInBlocksImpl(Arena arena, size_t begin, size_t end, size_t blockSize, Order order, std::function<void(size_t, size_t)> function);

template<typename Func>
void ExecuteParallel(size_t begin, size_t end, Arena arena, Order order, Func function)
{
	ExecuteParallelImpl(arena, begin, end, order,
		[&function](size_t index) { function(index); });
}

template<typename Func>
void ExecuteParallelInBlocks(size_t begin, size_t end, size_t blockSize, Arena arena, Order order, Func function)
{
	ExecuteParallelInBlocksImpl(arena, begin, end, blockSize, order,
		[&function](size_t begin, size_t end) { function(begin, end); });
}

};
};
