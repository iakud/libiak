#ifndef IAK_BASE_COUNTDOWNLATCH_H
#define IAK_BASE_COUNTDOWNLATCH_H

#include <mutex>
#include <condition_variable>

namespace iak {
namespace base {

class CountDownLatch {
public:
	explicit CountDownLatch(int count)
		: mutex_()
		, cv_()
		, count_(count) {
	}
	// noncopyable
	CountDownLatch(const CountDownLatch&) = delete;
	CountDownLatch& operator=(const CountDownLatch&) = delete;
	
	void wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (count_ > 0) {
			cv_.wait(lock);
		}
	}

	void countDown() {
		std::unique_lock<std::mutex> lock(mutex_);
		--count_;
		if (count_ == 0) {
			cv_.notify_all();
		}
	}

	int getCount() {
		std::unique_lock<std::mutex> lock(mutex_);
		return count_;
	}

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	int count_;
}; // end class CountDownLatch

} // end namespace base
} // end namespace iak

#endif // IAK_BASE_COUNTDOWNLATCH_H
