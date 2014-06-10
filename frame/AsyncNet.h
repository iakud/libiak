#ifndef IAK_FRAME_ASYNCNET_H
#define IAK_FRAME_ASYNCNET_H

#include <base/Mutex.h>

#include <vector>

namespace iak {

class EventLoopThreadPool;

class AsyncNet {
public:
	static void init(uint32_t count);
	static void destroy();

	typedef std::function<void()> Functor;

	static void put(Functor&& functor) {
		MutexGuard lock(mutex_);
		s_pendingFunctors_.push_back(functor);
	}

	// run in main thread, per loop
	static void dispatch();

	static EventLoop* getEventLoop() {
		assert(s_loopThreadPool_);
		if (s_loopThreadPool_) {
			++s_indexLoop_;
			s_indexLoop_ %= s_loopThreadPool_->getCount();
			return s_loopThreadPool_->getLoop(s_indexLoop_);
		}
		return NULL;
	}

	static EventLoopThreadPool* getEventLoopThreadPool() {
		return s_loopThreadPool_;
	}

private:
	static std::vector<Functor> s_pendingFunctors_;
	static Mutex s_mutex_;
	static EventLoopThreadPool* s_loopThreadPool_;
	static uint32_t s_indexLoop_;
}; // end class AsyncNet

} // end namespace iak

#endif // IAK_FRAME_ASYNCNET_H