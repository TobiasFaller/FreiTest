#pragma once

#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <mutex>

namespace SolverProxy
{

class TimeoutHelper
{
public:
	TimeoutHelper(void);
	virtual ~TimeoutHelper(void);

	void StartTimeout(uint64_t timeoutInMillis);
	void StopTimeout(void);

protected:
	virtual void OnTimeout(void) = 0;

private:
	std::shared_ptr<std::promise<void>> _promise;
	std::shared_ptr<std::thread> _thread;
	std::recursive_mutex _mutex;
};

class CallbackTimeoutHelper: public TimeoutHelper
{
public:
	CallbackTimeoutHelper(std::function<void(void)> callback);

	void OnTimeout(void) override;

private:
	std::function<void(void)> _callback;
};

};
