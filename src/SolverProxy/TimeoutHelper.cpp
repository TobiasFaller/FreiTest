#include "SolverProxy/TimeoutHelper.hpp"

#include <cassert>
#include <chrono>
#include <future>
#include <thread>

namespace SolverProxy
{

TimeoutHelper::TimeoutHelper(void):
	_promise(std::make_shared<std::promise<void>>()),
	_thread(),
	_mutex()
{
}

TimeoutHelper::~TimeoutHelper(void)
{
	StopTimeout();
}

void TimeoutHelper::StartTimeout(uint64_t timeoutInMillis)
{
	std::lock_guard guard { _mutex };

	// Make sure to copy the shared_ptr to guarantee no invalid dereference for the future
	_thread = std::make_shared<std::thread>([this, promise = _promise, timeout = timeoutInMillis]() -> void {
		std::future<void> future = promise->get_future();
		if (future.wait_for(std::chrono::milliseconds(timeout)) == std::future_status::timeout)
		{
			OnTimeout();
		}
	});
}

void TimeoutHelper::StopTimeout(void)
{
	std::lock_guard guard { _mutex };

	try {
		_promise->set_value();
	} catch (const std::future_error& error) {
		// Ignore promise_already_satisfied exception
		assert(error.code() == std::future_errc::promise_already_satisfied);
	}

	if (_thread && _thread->joinable()) {
		_thread->join();
	}
}

CallbackTimeoutHelper::CallbackTimeoutHelper(std::function<void(void)> callback):
	_callback(callback)
{
}

void CallbackTimeoutHelper::OnTimeout(void)
{
	_callback();
}

};
