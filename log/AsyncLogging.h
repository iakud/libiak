#ifndef IAK_BASE_ASYNCLOGGING_H
#define IAK_BASE_ASYNCLOGGING_H

#include <base/CountDownLatch.h>
#include <base/Mutex.h>
#include <base/Thread.h>

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
		MutexGuard lock(mutex_);
		pendingFunctors_.push_back(functor);
		cond_.signal();
	}

	void start() {
		running_ = true;
		thread_.start();
		latch_.wait();
	}

	void stop() {
		running_ = false;
		cond_.signal();
		thread_.join();
	}

private:
	//AsyncLogger(const AsyncLogger&);  // ptr_container
	//void operator=(const AsyncLogger&);  // ptr_container

	//static const uint32_t kNanoSecondsPerSecond = 1e9;

	void threadFunc();

	volatile bool running_;

	Thread thread_;
	CountDownLatch latch_;
	Mutex mutex_;
	Condition cond_;

	std::vector<Functor> pendingFunctors_;
}; // end class AsyncLogging

} // end namespace iak

#endif  // IAK_BASE_ASYNCLOGGING_H
