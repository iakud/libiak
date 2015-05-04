#ifndef IAK_BASE_BLOCKINGQUEUE_H
#define IAK_BASE_BLOCKINGQUEUE_H

#include <mutex>
#include <condition_variable>
#include <deque>
#include <assert.h>

namespace iak {

template<typename T>
class BlockingQueue {
public:
	BlockingQueue()
		: mutex_()
		, cv_()
		, queue_() {
	}
	// noncopyable
	BlockingQueue(const BlockingQueue&) = delete;
	BlockingQueue& operator=(const BlockingQueue&) = delete;

	void put(const T& x) {
		std::unique_lock<std::mutex> lock(mutex_);
		queue_.push_back(x);
		cv_.notify_one();
	}

	T take() {
		std::unique_lock<std::mutex> lock(mutex_);
		while (queue_.empty()) {
			cv_.wait(lock);
		}
		assert(!queue_.empty());
		T front(queue_.front());
		queue_.pop_front();
		return front;
	}

	size_t size() const {
		std::unique_lock<std::mutex> lock(mutex_);
		return queue_.size();
	}

private:
	std::mutex mutex_;
	std::condition_variable cv_;
	std::deque<T> queue_;
}; // end class BlockingQueue

} // end namespace iak

#endif  // IAK_BASE_BLOCKINGQUEUE_H
