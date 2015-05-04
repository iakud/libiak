#ifndef IAK_BASE_COUNTDOWNLATCH_H
#define IAK_BASE_COUNTDOWNLATCH_H

#include "Condition.h"
#include "Mutex.h"

namespace iak {

class CountDownLatch {
public:
	explicit CountDownLatch(int count)
		: mutex_()
		, condition_(mutex_)
		, count_(count) {
	}
	// noncopyable
	CountDownLatch(const CountDownLatch&) = delete;
	CountDownLatch& operator=(const CountDownLatch&) = delete;
	
	void wait() {
		MutexGuard lock(mutex_);
		while (count_ > 0) {
			condition_.wait();
		}
	}

	void countDown() {
		MutexGuard lock(mutex_);
		--count_;
		if (count_ == 0) {
			condition_.broadcast();
		}
	}

	int getCount() const {
		MutexGuard lock(mutex_);
		return count_;
	}

private:
	mutable Mutex mutex_;
	Condition condition_;
	int count_;
}; // end class CountDownLatch

} // end namespace iak

#endif // IAK_BASE_COUNTDOWNLATCH_H
