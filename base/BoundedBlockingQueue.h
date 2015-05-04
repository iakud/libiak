#ifndef IAKUD_BASE_BOUNDEDBLOCKINGQUEUE_H
#define IAKUD_BASE_BOUNDEDBLOCKINGQUEUE_H

#include "Condition.h"
#include "Mutex.h"

#include <deque>
#include <assert.h>

namespace iak {

template<typename T>
class BoundedBlockingQueue {

public:
	explicit BoundedBlockingQueue(size_t capacity)
		: mutex_()
		, notEmpty_(mutex_)
		, notFull_(mutex_)
		, capacity_(capacity)
		, queue_() {
	}
	// noncopyable
	BoundedBlockingQueue(const BoundedBlockingQueue&) = delete;
	BoundedBlockingQueue& operator=(const BoundedBlockingQueue&) = delete;

	void put(const T& x) {
		MutexGuard lock(mutex_);
		while (queue_.full()) {
			notFull_.wait();
		}
		assert(!queue_.full());
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
		notFull_.signal();
		return front;
	}

	bool empty() const {
		MutexGuard lock(mutex_);
		return queue_.empty();
	}

	bool full() const {
		MutexGuard lock(mutex_);
		return queue_.size() >= capacity_;
	}

	size_t size() const {
		MutexGuard lock(mutex_);
		return queue_.size();
	}

	size_t capacity() const {
		return capacity_;
	}

private:
	mutable Mutex mutex_;
	Condition notEmpty_;
	Condition notFull_;
	size_t capacity_;
	std::deque<T> queue_;
}; // end class BoundedBlockingQueue

} // end namespace iak

#endif  // IAK_BASE_BOUNDEDBLOCKINGQUEUE_H