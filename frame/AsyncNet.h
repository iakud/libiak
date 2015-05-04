#ifndef IAK_FRAME_ASYNCNET_H
#define IAK_FRAME_ASYNCNET_H

#include <mutex>
#include <vector>
#include <functional>

#include <stdint.h>

namespace iak {

class EventLoop;
class EventLoopThreadPool;

class AsyncNet {
public:
	static void init(uint32_t count);
	static void destroy();

	typedef std::function<void()> Functor;

	static void put(Functor&& functor) {
		std::unique_lock<std::mutex> lock(s_mutex_);
		s_pendingFunctors_.push_back(functor);
	}

	// run in main thread, per loop
	static void dispatch();

	static EventLoop* getEventLoop();

	static EventLoopThreadPool* getEventLoopThreadPool() {
		return s_loopThreadPool_;
	}

private:
	static std::vector<Functor> s_pendingFunctors_;
	static std::mutex s_mutex_;
	static EventLoopThreadPool* s_loopThreadPool_;
	static uint32_t s_indexLoop_;
}; // end class AsyncNet

} // end namespace iak

#endif // IAK_FRAME_ASYNCNET_H
