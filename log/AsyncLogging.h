#ifndef IAK_BASE_ASYNCLOGGING_H
#define IAK_BASE_ASYNCLOGGING_H

#include "AsyncLogger.h"

#include <base/CountDownLatch.h>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <vector>

namespace iak {
namespace log {

class AsyncLogging {

public:
	AsyncLogging();
	~AsyncLogging();
	// noncopyable
	AsyncLogging(const AsyncLogging&) = delete;
	AsyncLogging& operator=(const AsyncLogging&) = delete;

	void append(AsyncLoggerPtr&& asyncLoggerPtr, const char* logline, int len);

	void start() {
		running_ = true;
		thread_ = std::thread(&AsyncLogging::threadFunc, this);
		latch_.wait();
	}

	void stop() {
		running_ = false;
		cv_.notify_one();
		thread_.join();
	}

private:
	//AsyncLogger(const AsyncLogger&);  // ptr_container
	//void operator=(const AsyncLogger&);  // ptr_container

	//static const uint32_t kNanoSecondsPerSecond = 1e9;
	
	void threadFunc();
	void swapAsyncLoggersToWrite(std::vector<AsyncLoggerPtr>& asyncLoggersToWrite);

	volatile bool running_;
	std::thread thread_;
	base::CountDownLatch latch_;
	std::mutex mutex_;
	std::condition_variable cv_;

	std::vector<AsyncLoggerPtr> asyncLoggers_;
}; // end class AsyncLogging

} // end namespace log
} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGING_H
