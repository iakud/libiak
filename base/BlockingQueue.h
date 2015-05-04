#ifndef IAK_BASE_BLOCKINGQUEUE_H
#define IAK_BASE_BLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"

#include <deque>
#include <assert.h>

namespace iak {

template<typename T>
class BlockingQueue {
public:
	BlockingQueue()
		: mutex_()
		, notEmpty_(mutex_)
		, queue_() {
	}
	// noncopyable
	BlockingQueue(const BlockingQueue&) = delete;
	BlockingQueue& operator=(const BlockingQueue&) = delete;

	void put(const T& x) {
		MutexGuard lock(mutex_);
		queue_.push_back(x);
		notEmpty_.signal();
	}

	T take() {
		MutexGuard lock(mutex_);
		while (queue_.empty()) {
			notEmpty_.wait();
		}
		assert(!queue_.empty());
		T front(queue_.front());
		queue_.pop_front();
		return front;
	}

	size_t size() const {
		MutexGuard lock(mutex_);
		return queue_.size();
	}

private:
	mutable Mutex mutex_;
	Condition notEmpty_;
	std::deque<T> queue_;
}; // end class BlockingQueue

} // end namespace iak

#endif  // IAK_BASE_BLOCKINGQUEUE_H
