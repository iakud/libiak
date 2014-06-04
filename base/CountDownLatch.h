#ifndef IAK_BASE_COUNTDOWNLATCH_H
#define IAK_BASE_COUNTDOWNLATCH_H

#include "NonCopyable.h"
#include "Condition.h"
#include "Mutex.h"

namespace iak {

class CountDownLatch : public NonCopyable {
public:
	explicit CountDownLatch(int count)
		: mutex_()
		, condition_(mutex_)
		, count_(count) {
	}
	
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

	int count() const {
		MutexGuard lock(mutex_);
		return count_;
	}

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
}; // end class CountDownLatch

} // end namespace iak

#endif // IAK_BASE_COUNTDOWNLATCH_H
