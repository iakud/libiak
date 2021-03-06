#ifndef IAK_NET_EVENTLOOPTHREADPOOL_H
#define IAK_NET_EVENTLOOPTHREADPOOL_H

#include <thread>
#include <vector>
#include <memory>

namespace iak {
namespace net {

class EventLoop;

class EventLoopThreadPool {
public:
	EventLoopThreadPool(uint32_t count);
	~EventLoopThreadPool();
	// noncopyable
	EventLoopThreadPool(const EventLoopThreadPool&) = delete;
	EventLoopThreadPool& operator=(const EventLoopThreadPool&) = delete;

	uint32_t getCount() {
		return count_;
	}

	EventLoop* getLoop(uint32_t index) {
		if (index < count_) {
			return loops_[index];
		}
		return NULL;
	}

	void start();

private:
	void threadFunc(EventLoop*);

	const uint32_t count_;
	std::vector<EventLoop*> loops_;
	std::vector<std::thread> threads_;
}; // end class EventLoopThreadPool

} // end namespace net
} // end namespace iak

#endif // IAK_NET_EVENTLOOPTHREADPOOL_H
