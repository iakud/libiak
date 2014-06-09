#ifndef IAK_NET_EVENTLOOPTHREADPOOL_H
#define IAK_NET_EVENTLOOPTHREADPOOL_H

#include <base/NonCopyable.h>

#include <vector>
#include <memory>

namespace iak {

class EventLoop;
class Thread;

class EventLoopThreadPool : public NonCopyable {
public:
	EventLoopThreadPool(uint32_t count);
	~EventLoopThreadPool();

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
	std::vector<Thread*> threads_;
}; // end class EventLoopThreadPool

} // end namespace iak

#endif // IAK_NET_EVENTLOOPTHREADPOOL_H
