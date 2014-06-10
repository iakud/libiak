#include "AsyncNet.h"

#include <net/EventLoopThreadPool.h>

using namespace iak;

std::vector<Functor> AsyncNet::s_pendingFunctors_;
Mutex AsyncNet::s_mutex_;
EventLoopThreadPool* AsyncNet::s_loopThreadPool_ = NULL;
uint32_t AsyncNet::s_indexLoop_ = 0;

AsyncNet::init(uint32_t count) {
	if (!s_loopThreadPool_) {
		s_loopThreadPool_ = new EventLoopThreadPool(count);
	}
}

AsyncNet::destroy() {
	delete s_loopThreadPool_;
}

void AsyncNet::dispatch() {
	std::vector<Functor> functors;
	{
		MutexGuard lock(s_mutex_);
		functors.swap(s_pendingFunctors_);
	}
	for (Functor functor : functors) {
		functor();
	}
}