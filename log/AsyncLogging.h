#ifndef IAK_BASE_ASYNCLOGGING_H
#define IAK_BASE_ASYNCLOGGING_H

#include <base/CountDownLatch.h>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <vector>
#include <string>

namespace iak {

class AsyncLogging {

public:
	typedef std::function<void()> Functor;

public:
	AsyncLogging();
	~AsyncLogging() {
		if (running_) {
			stop();
		}
	}
	// noncopyable
	AsyncLogging(const AsyncLogging&) = delete;
	AsyncLogging& operator=(const AsyncLogging&) = delete;

	void append(Functor&& functor) {
		std::unique_lock<std::mutex> lock(mutex_);
		pendingFunctors_.push_back(functor);
		cv_.notify_one();
	}

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

	volatile bool running_;
	std::thread thread_;
	CountDownLatch latch_;
	std::mutex mutex_;
	std::condition_variable cv_;

	std::vector<Functor> pendingFunctors_;
}; // end class AsyncLogging

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGING_H
