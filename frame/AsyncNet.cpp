#include "AsyncNet.h"

#include <net/EventLoopThreadPool.h>

#include <assert.h>
#include <signal.h>

using namespace iak;

std::vector<AsyncNet::Functor> AsyncNet::s_pendingFunctors_;
std::mutex AsyncNet::s_mutex_;
EventLoopThreadPool* AsyncNet::s_loopThreadPool_ = NULL;
uint32_t AsyncNet::s_indexLoop_ = 0;

void AsyncNet::init(uint32_t count) {
	if (!s_loopThreadPool_) {
		s_loopThreadPool_ = new EventLoopThreadPool(count);
		s_loopThreadPool_->start();
		::signal(SIGPIPE, SIG_IGN);
	}
}

void AsyncNet::destroy() {
	delete s_loopThreadPool_;
}

EventLoop* AsyncNet::getEventLoop() {
	assert(s_loopThreadPool_);
	if (s_loopThreadPool_) {
		++s_indexLoop_;
		s_indexLoop_ %= s_loopThreadPool_->getCount();
		return s_loopThreadPool_->getLoop(s_indexLoop_);
	}
	return NULL;
}

void AsyncNet::dispatch() {
	std::vector<Functor> functors;
	{
		std::unique_lock<std::mutex> lock(s_mutex_);
		functors.swap(s_pendingFunctors_);
	}
	for (Functor functor : functors) {
		functor();
	}
}
