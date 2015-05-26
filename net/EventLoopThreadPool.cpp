#include "EventLoopThreadPool.h"
#include "EventLoop.h"

#include <algorithm>

using namespace iak::net;

EventLoopThreadPool::EventLoopThreadPool(uint32_t count)
	: count_(count) {
	for (uint32_t i=0; i<count; ++i) {
		EventLoop* loop = new EventLoop();
		loops_.push_back(loop);
	}
}

EventLoopThreadPool::~EventLoopThreadPool() {
	std::for_each(loops_.begin(), loops_.end(),
			std::bind(&EventLoop::quit, std::placeholders::_1));
	
	for (std::thread& t : threads_) {
		t.join();
	}
	for (EventLoop* loop : loops_) {
		delete loop;
	}
}

void EventLoopThreadPool::threadFunc(EventLoop* loop) {
	loop->loop();
}

void EventLoopThreadPool::start() {
	for (uint32_t i = 0; i < count_; ++i) {
		EventLoop* loop = loops_[i];
		threads_.push_back(std::thread(&EventLoopThreadPool::threadFunc, this, loop));
	}
}
