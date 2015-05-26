#ifndef IAKUD_BASE_BOUNDEDBLOCKINGQUEUE_H
#define IAKUD_BASE_BOUNDEDBLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <assert.h>

namespace iak {
namespace base {

template<typename T>
class BoundedBlockingQueue {

public:
	explicit BoundedBlockingQueue(size_t capacity)
		: mutex_()
		, cvne_()
		, cvnf_()
		, capacity_(capacity)
		, queue_() {
	}
	// noncopyable
	BoundedBlockingQueue(const BoundedBlockingQueue&) = delete;
	BoundedBlockingQueue& operator=(const BoundedBlockingQueue&) = delete;

	void put(const T& x) {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.full()) {
			cvnf_.wait(lock);
		}
		assert(!queue_.full());
		queue_.push_back(x);
		cvne_.notify_one();
	}

	T take() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty()) {
			cvne_.wait();
		}
		assert(!queue_.empty());
		T front(queue_.front());
		queue_.pop_front();
		cvnf_.notify_one();
		return front;
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.empty();
	}

	bool full() {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size() >= capacity_;
	}

	size_t size() {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size();
	}

	size_t capacity() const {
		return capacity_;
	}

private:
	std::mutex mutex_;
	std::condition_variable cvne_; // not empty
	std::condition_variable cvnf_; // not full
	size_t capacity_;
	std::deque<T> queue_;
}; // end class BoundedBlockingQueue

} // end namespace base
} // end namespace iak

#endif  // IAK_BASE_BOUNDEDBLOCKINGQUEUE_H