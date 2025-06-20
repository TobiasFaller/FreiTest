#include "Basic/Parallel.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <execution>
#include <random>
#include <vector>
#include <type_traits>

#include <tbb/task_arena.h>
#include <tbb/task_group.h>

namespace FreiTest
{
namespace Parallel
{

template<std::size_t size, typename T, std::size_t... indexes>
constexpr auto make_array_n_impl(T && value, std::index_sequence<indexes...>) {
	// Use the comma operator to expand the variadic pack
	// Move the last element in if possible. Order of evaluation is well-defined
	// for aggregate initialization, so there is no risk of copy-after-move
	return std::array<std::decay_t<T>, size>{ (static_cast<void>(indexes), value)..., std::forward<T>(value) };
}

template<std::size_t size, typename T>
constexpr auto make_array_n(T && value) {
	return make_array_n_impl<size>(std::forward<T>(value), std::make_index_sequence<size - 1>{});
}

static std::array<tbb::task_arena, static_cast<size_t>(Arena::NUM_ITEMS)> arenas = make_array_n<static_cast<size_t>(Arena::NUM_ITEMS)>(tbb::task_arena {});
static tbb::task_arena& GetArena(Arena arena)
{
	return arenas[static_cast<size_t>(arena)];
}

void SetThreads(Arena arenaId, size_t threads)
{
	GetArena(arenaId).initialize((threads != 0u) ? static_cast<int>(threads) : tbb::task_arena::automatic);
}

size_t GetThreads(Arena arenaId)
{
	return GetArena(arenaId).max_concurrency();
}

void ExecuteParallelImpl(Arena arenaId, size_t begin, size_t end, Order order, std::function<void(size_t)> function)
{
	if (order == Order::Sequential || GetArena(arenaId).max_concurrency() == 1)
	{
		for (size_t index {begin}; index != end; ++index)
		{
			function(index);
		}
	}
	else
	{
		tbb::task_arena& arena { GetArena(arenaId) };
		tbb::task_group group {};

		// Manager schedules work for the task  group
		for (size_t index {begin}; index != end; ++index)
		{
			arena.execute([&, index=index]() {
				group.run([&, index=index]() {
					function(index);
				});
			});
		}

		// Manager can now join the task group
		arena.execute([&]() {
			group.wait();
		});
	}
};

void ExecuteParallelInBlocksImpl(Arena arenaId, size_t begin, size_t end, size_t blockSize, Order order, std::function<void(size_t, size_t)> function)
{
	if (order == Order::Sequential || GetArena(arenaId).max_concurrency() == 1)
	{
		for (size_t index {begin}; index < end; index += blockSize)
		{
			function(index, std::min(index + blockSize, end));
		}
	}
	else
	{
		tbb::task_arena& arena { GetArena(arenaId) };
		tbb::task_group group {};

		// Manager schedules work for the task  group
		for (size_t index {begin}; index < end; index += blockSize)
		{
			arena.execute([&, index=index, blockSize=blockSize, end=end]() {
				group.run([&, index=index, blockSize=blockSize, end=end]() {
					function(index, std::min(index + blockSize, end));
				});
			});
		}

		// Manager can now join the task group
		arena.execute([&]() {
			group.wait();
		});
	}
};

};
};
