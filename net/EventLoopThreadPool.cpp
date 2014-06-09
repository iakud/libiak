#include "EventLoopThreadPool.h"
#include "EventLoop.h"

#include <base/Thread.h>

#include <algorithm>

using namespace iak;

EventLoopThreadPool::EventLoopThreadPool(uint32_t count)
	: count_(count) {
	for (int i=0; i<count; ++i) {
		EventLoop* loop = EventLoop::create();
		Thread* thread = new Thread(
				std::bind(&EventLoopThreadPool::threadFunc, this, loop));
		loops_.push_back(loop);
		threads_.push_back(thread);
	}
}

EventLoopThreadPool::~EventLoopThreadPool() {
	std::for_each(loops_.begin(), loops_.end(),
			std::bind(&EventLoop::quit, std::placeholders::_1));
	std::for_each(threads_.begin(), threads_.end(),
			std::bind(&Thread::join, std::placeholders::_1));
}

void EventLoopThreadPool::threadFunc(EventLoop* loop) {
	loop->loop();
}

void EventLoopThreadPool::start() {
	std::for_each(threads_.begin(), threads_.end(),
			std::bind(&Thread::start, std::placeholders::_1));
}